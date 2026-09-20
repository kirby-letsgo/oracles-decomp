#!/usr/bin/env python3
"""Turn switch statements whose case labels are per-game addresses into if-chains.

usage: tools/switch_to_if.py [FILE...]

After tools/symbolize.py, jump-table dispatch reads `case SYM(label):` / `case b_+N:`, which are
not constant expressions. Each such switch becomes

    do { uint16_t jt_ = (subject);
      if (jt_ == A) { ... }
      else if (jt_ == B || jt_ == C) { ... }
      else { default body }
    } while (0);

The do/while keeps every `break;` in a case body meaning exactly what it meant in the switch.
"""
import glob, os, re, sys

FALL_N = 0
CASE = re.compile(r'^(\s*)(?:case (.+?)|(default)):\s*(.*)$')


def match_brace(t, i):
    """index just past the brace that closes the one at t[i]."""
    depth = 0
    while i < len(t):
        if t[i] == '{': depth += 1
        elif t[i] == '}':
            depth -= 1
            if depth == 0: return i + 1
        i += 1
    raise ValueError('unbalanced braces')


def subject_end(t, i):
    """t[i] == '(' of `switch (`; return index past the matching ')'."""
    depth = 0
    while i < len(t):
        if t[i] == '(': depth += 1
        elif t[i] == ')':
            depth -= 1
            if depth == 0: return i + 1
        i += 1
    raise ValueError('unbalanced parens')


def convert_body(body):
    """[(labels or None for default, body_lines)] in source order; None if the body is not plain cases."""
    lines = body.split('\n')
    sections, cur_labels, cur_body, depth, in_default = [], None, [], 0, False

    def flush():
        if cur_labels is not None or in_default: sections.append((cur_labels, cur_body))

    for line in lines:
        m = CASE.match(line) if depth == 0 else None
        if m:
            rest = m.group(4)
            if m.group(3):
                flush(); cur_labels, in_default = None, True
            elif cur_labels is not None and not in_default and not ''.join(cur_body).strip():
                cur_labels = cur_labels + [m.group(2)]
            else:
                flush(); cur_labels, in_default = [m.group(2)], False
            cur_body = [rest] if rest.strip() else []
            depth += rest.count('{') - rest.count('}')
            continue
        if cur_labels is None and not in_default:
            if line.strip().startswith('//'): sections.append(('comment', line)); continue
            if line.strip(): return None
            continue
        cur_body.append(line)
        depth += line.count('{') - line.count('}')
    flush()
    return sections


def convert_one(t, start):
    """Convert the switch statement beginning at t[start] (pointing at 'switch'). Returns new text."""
    p = t.index('(', start)
    e = subject_end(t, p)
    subject = t[p + 1:e - 1]
    b = t.index('{', e)
    end = match_brace(t, b)
    body = t[b + 1:end - 1]
    if re.search(r'[^\n]\s*(case [^:]+:|default:)', body):     # labels not at line starts: one-line switch
        body = re.sub(r'\s*(case [^:\n]+:|default:)', r'\n\1', body)
    if body.startswith('\n'): body = body[1:]
    labels = re.findall(r'^\s*case ([^:]+):', body, re.M)
    if not labels or all(re.fullmatch(r'\s*(0x[0-9a-f]+|\d+)\s*', l) for l in labels): return None
    sections = convert_body(body)
    if sections is None: return None
    line_start = t.rfind('\n', 0, start) + 1
    indent = t[line_start:start]
    if indent.strip(): indent = re.match(r'\s*', indent).group(0)
    out = [f'do {{ uint16_t jt_ = ({subject.strip()});']
    first = True
    TERMINATOR = re.compile(r'(return;|break;|goto \w+;|continue;|HANDOFF(_UP)?\([^;]*\);|RET(_TAKEN|I)?\([^;]*\);|\})\s*$')
    pending_label = None
    prefix = [b for l, b in sections if l == 'comment']
    sections = [s for s in sections if s[0] != 'comment']
    out.extend(prefix)
    for k, (labels, blines) in enumerate(sections):
        code_lines = [l.split('//')[0].rstrip() for l in blines]
        last = '\n'.join(code_lines).rstrip()
        falls = k + 1 < len(sections) and not TERMINATOR.search(last)
        if falls and not last.strip() and labels is not None:
            # empty body before another label: same as stacking those labels onto the next section
            nxt_labels, nxt_body = sections[k + 1]
            sections[k + 1] = (None if nxt_labels is None else labels + nxt_labels, nxt_body)
            if nxt_labels is None: print(f'note: labels {labels} folded into default', file=sys.stderr)
            continue
        cond = None if labels is None else ' || '.join(f'jt_ == {l.strip()}' for l in labels)
        head = 'else' if cond is None else ('if' if first else 'else if') + f' ({cond})'
        first = False
        body = list(blines)
        while body and not body[-1].strip(): body.pop()
        if pending_label:
            body.insert(0, f'{indent}    {pending_label}:;')
            pending_label = None
        if falls:
            global FALL_N
            FALL_N += 1
            pending_label = f'jt_fall{FALL_N}'
            body.append(f'{indent}    goto {pending_label};')
        text = '\n'.join(body).rstrip()
        if '\n' not in text.strip():
            code, _, comment = text.strip().partition('//')
            out.append(f'{indent}  {head} {{ {code.strip()} }}' + (f' //{comment}' if comment else ''))
        else:
            out.append(f'{indent}  {head} {{')
            out.extend(body)
            out.append(f'{indent}  }}')
    out.append(f'{indent}}} while (0);')
    return t[:start] + '\n'.join(out) + t[end:]


def convert_file(path):
    t = open(path, errors='replace').read()
    orig = t
    n = 0
    while True:
        # innermost-first: pick the last `switch (` whose body holds no other switch
        cands = [m.start() for m in re.finditer(r'\bswitch\s*\(', t)]
        done = True
        for s in reversed(cands):
            new = convert_one(t, s)
            if new is not None:
                t = new; n += 1; done = False
                break
        if done: break
    if t != orig: open(path, 'w').write(t)
    return n


def main():
    files = sys.argv[1:] or [f for f in sorted(glob.glob('src/game/**/*.c', recursive=True)) if not os.path.basename(f).startswith('gen_')]
    total = sum(convert_file(f) for f in files)
    print(f'{total} switches converted in {len(files)} files')


if __name__ == '__main__':
    main()
