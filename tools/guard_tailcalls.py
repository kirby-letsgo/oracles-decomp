#!/usr/bin/env python3
"""Guard direct tail calls into other routines' hooks with hook_is().

usage: tools/guard_tailcalls.py [--apply] [FILE...]

A C hook that ends with `other_hook(gb); return;` (a `jp`) or dispatches a jump table with
`if (jt_ == SYM(x)) { x_hook(gb); return; }` runs the callee's C whether or not that routine is
hooked in the running game. Under Seasons that is wrong whenever the callee is not identical, and
it makes the caller ineligible. The guarded forms

    TAIL(x);
    if (jt_ == SYM(x) && hook_is(gb, SYM(x), x_hook)) { x_hook(gb); return; }

(TAIL(x) is the first form as a macro in game.h: the interpreter runs x until it returns to the
address on top of the emulated stack) fall back to the interpreter at the callee's address
instead, which is what the ROM does. Ages is unchanged: every routine is hooked there. Only
calls whose callee name has a symbol and is hooked in one of the tables are guarded; jump-table
chains with no interpreter fallback are left alone.
"""
import glob, os, re, sys

PLAIN = re.compile(r'^(\s*)((?:CYCT?\([^;]*\); )?)(\w+)_hook\(gb\);\s*return;(.*)$')
CHAIN = re.compile(r'if \(jt_ == (SYM\((\w+)\)|b_\+\d+)\) \{ (\w+)_hook\(gb\); return; \}')
FUNC_START = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?\w+\([^)]*\)\s*\{')


def sym_ids():
    return set(re.findall(r'^\s+S_(\w+),', open('src/game/syms.h').read(), re.M))


def hooked():
    """Callees some game's table hooks. A TAIL to a routine hooked in neither game always falls back
    to its ROM address (the native builds have no code there), so a direct call to its C stays."""
    out = set()
    for path in ('src/hooks/generated.txt', 'src/hooks/generated_seasons.txt'):
        if os.path.exists(path):
            out.update(p[1][:-5] for p in (l.split() for l in open(path)) if len(p) >= 2 and p[1].endswith('_hook'))
    return out


def guard_file(path, syms, apply):
    text0 = open(path, errors='replace').read()
    lines = text0.split('\n')
    # jump-table chains without an interpreter fallback: leave their branches alone
    unsafe = set()
    for m in re.finditer(r'do \{ uint16_t jt_ = .*?\n(.*?)\n\s*\} while \(0\);', text0, re.S):
        after = text0[m.end():m.end() + 200].split('\n')[1:3]     # a fallback right after the chain counts too
        if not re.search(r'else \{[^}]*(HANDOFF|hook_continue|hook_handoff)', m.group(1), re.S) and not any(re.search(r'HANDOFF|hook_continue|hook_handoff', l) for l in after):
            a = text0.count('\n', 0, m.start()); b = text0.count('\n', 0, m.end())
            unsafe.update(range(a, b + 1))
    out, changed = [], 0
    func_start = None
    need_sp0 = set()
    skip = False
    for i, line in enumerate(lines):
        if skip: skip = False; continue
        code, sep, comment = line.partition('//')
        if FUNC_START.match(line): func_start = len(out)
        if line == '}': func_start = None
        m2 = re.match(r'^(\s*)(\w+)_hook\(gb\);\s*$', code)
        if m2 and i + 1 < len(lines) and re.match(r'^\s*return;\s*$', lines[i + 1].split('//')[0]) and m2.group(2) in syms and func_start is not None:
            indent, callee = m2.groups()
            out.append(f'{indent}TAIL({callee});' + (sep + comment if sep else ''))
            changed += 1; skip = True
            continue
        m3 = re.match(r'^(\s*)((?:CYCT?\([^;]*\); )?)(\w+)_hook\(gb\);\s*$', code)
        if m3 and i + 1 < len(lines) and lines[i + 1] == '}' and m3.group(3) in syms and func_start is not None and not lines[func_start].startswith('static '):
            indent, pre, callee = m3.groups()
            out.append(f'{indent}{pre}TAIL({callee});' + (sep + comment if sep else ''))
            changed += 1
            continue
        m = PLAIN.match(code)
        if m and m.group(3) in syms and not re.search(r'hook_enabled_at|hook_is', code) and func_start is not None:
            indent, pre, callee, rest = m.groups()
            new = f'{indent}{pre}TAIL({callee});{rest}'
            out.append(new + (sep + comment if sep else '')); changed += 1
            continue
        new = code if i in unsafe else CHAIN.sub(lambda mm: mm.group(0) if mm.group(2) is None or mm.group(2) != mm.group(3) else
                        f'if (jt_ == {mm.group(1)} && hook_is(gb, SYM({mm.group(2)}), {mm.group(3)}_hook)) {{ {mm.group(3)}_hook(gb); return; }}', code)
        if new != code: changed += 1
        out.append(new + (sep + comment if sep else ''))
    text = '\n'.join(out)
    if apply and changed: open(path, 'w').write(text)
    return changed


def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    if apply: args.remove('--apply')
    syms = sym_ids() & hooked()
    files = args or [f for f in sorted(glob.glob('src/game/**/*.c', recursive=True))
                     if not os.path.basename(f).startswith('gen_') and os.path.basename(f) not in ('syms.c', 'kernel.c', 'cyc.c', 'ram_code.c')]
    total = sum(guard_file(f, syms, apply) for f in files)
    print(f'{total} tail calls guarded in {len(files)} files')


if __name__ == '__main__':
    main()
