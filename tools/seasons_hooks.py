#!/usr/bin/env python3
"""Write src/hooks/generated_seasons.txt: the hooks that are safe to run under Oracle of Seasons.

usage: tools/seasons_hooks.py AGES.gbc ages.sym SEASONS.gbc seasons.sym

A hook is eligible when its routine is instruction-identical in Seasons (tools/routine_equiv.py)
and every C function it calls directly (a `foo_hook(gb)` tail call or a static helper in the same
file, transitively) is eligible too; CALL_C targets are checked against the hook table at run
time, so they do not matter here. The Seasons (bank, address) of each eligible hook comes from
seasons.sym, pairing multi-copy labels by bank order. Then run
tools/gen_hooks.py seasons.sym src/hooks/generated_seasons.txt src/hooks/table_seasons.h.
"""
import glob, os, re, subprocess, sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from symfiles import rom_labels, seasons_code

FUNC = re.compile(r'^(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?(\w+)\([^)]*\)\s*\{')
CALL = re.compile(r'\b(\w+)\(gb[,)]')


def main():
    ages_rom, ages_sym, seasons_rom, seasons_sym = sys.argv[1:5]
    tsv = '/tmp/routine_equiv.tsv'
    subprocess.run([sys.executable, 'tools/routine_equiv.py', ages_rom, ages_sym, seasons_rom, seasons_sym, '--tsv', tsv], check=True, stdout=subprocess.DEVNULL)
    verdict = {}
    for l in open(tsv):
        n, v, *_ = l.rstrip('\n').split('\t')
        verdict[n] = v
    # JT_ONLY: identical apart from jump-table entries; fine when every jump table the C
    # dispatches in that routine's functions falls back to the interpreter
    safe_jt = set()
    for path in glob.glob('src/game/**/*.c', recursive=True):
        if os.path.basename(path).startswith('gen_'): continue
        text = open(path, errors='replace').read()
        for fm in re.finditer(r'\n(?:static )?(?:void|uint16_t|uint8_t|bool|int|unsigned) \*?\w+\([^)]*\)\s*\{(.*?)\n\}', text, re.S):
            body = fm.group(1)
            bm = re.search(r'\bBASE\((\w+)\)', body)
            if not bm: continue
            name = bm.group(1).replace('__', '@')
            if 'jump_table' in body and not re.search(r'HANDOFF\(|hook_continue\(|hook_handoff\(', body): safe_jt.add(name + '!'); continue
            safe_jt.add(name)
    for n in list(verdict):
        if verdict[n] == 'JT_ONLY':
            base = re.sub(r'_b[0-9a-f]{2}$', '', n)
            verdict[n] = 'IDENTICAL' if (base in safe_jt or n in safe_jt) and (base + '!') not in safe_jt and (n + '!') not in safe_jt else 'DIFFERENT'
    if os.path.exists('src/hooks/seasons_ok.txt'):
        for l in open('src/hooks/seasons_ok.txt'):
            n = l.strip()
            if n and verdict.get(n) == 'SAME_SHAPE': verdict[n] = 'IDENTICAL'
    # hand-checked routines: per-game offsets and edited C (tools/ofsmap.py), or C that already
    # tells the games apart (seasons_ok_manual.txt, whatever routine_equiv says); a jump table
    # without an interpreter fallback still disqualifies (a Seasons-only entry would be lost)
    for path in ('src/hooks/ofs_routines.txt', 'src/hooks/seasons_ok_manual.txt'):
        if not os.path.exists(path): continue
        for l in open(path):
            n = l.split('#')[0].split()
            n = n[0] if n else ''
            if not n: continue
            if (n + '!') in safe_jt: print(f'{path}: {n} dispatches a jump table without a fallback, left out'); continue
            verdict[n] = verdict[n.replace('@', '__')] = 'IDENTICAL'
            if path.endswith('ofs_routines.txt'):      # the edited C covers the routine's @locals too
                for loc in [k for k in verdict if k.startswith((n + '@', n + '__'))]:
                    loc = loc.replace('__', '@')
                    if (loc + '!') in safe_jt: continue
                    verdict[loc] = verdict[loc.replace('@', '__')] = 'IDENTICAL'

    # C call graph: function name -> callees, per file (static helpers are file-local)
    callees, burns, local_calls, tails, ages_only_funcs, raw_offs = {}, {}, {}, {}, set(), {}
    EXEC = re.compile(r'\b(?:CYCT?[0-9a-f]*|burn_rom|RET|RET_TAKEN|RETI|I|push_effect|BASE)\(([^;]*)')
    for path in sorted(glob.glob('src/game/**/*.c', recursive=True)):
        if os.path.basename(path).startswith('gen_') or os.path.basename(path) == 'syms.c': continue
        cur = None
        src = open(path, errors='replace').read().split('\n')
        for line, scode in zip(src, seasons_code(src)):
            m = FUNC.match(line)
            if m:
                cur = (path, m.group(1)); callees[cur] = set(); burns[cur] = set(); local_calls[cur] = set(); tails[cur] = set(); raw_offs[cur] = set()
                line = line[m.end():]
            elif line.startswith('}'): cur = None; continue
            if cur is None: continue
            code = scode[m.end():] if m else scode
            if 'AGES_ONLY()' in code: ages_only_funcs.add(cur)
            if m and code.count('}') > code.count('{'): oneliner = True
            else: oneliner = False
            guarded = set(re.findall(r'hook_(?:enabled_at|is)\(gb, SYM\((\w+)\)', code)) | set(re.findall(r'\bTAIL\((\w+)\)', code))
            for c in CALL.findall(code):
                if c.endswith('_hook') and c[:-5] in guarded: continue
                callees[cur].add(c)
            # a TAIL target need not be identical (the interpreter runs it) but must exist in Seasons
            for lab in re.findall(r'\bTAIL\((\w+)\)', code): tails[cur].add(lab)
            # CALL_L runs the call target's C unconditionally: that target's body must match too
            for off, lab, loff in re.findall(r'\bCALL_L(?:_CC)?\((?:b_\+(\d+)|\(SYM\((\w+)\) \+ (\d+)\)), \w+,', code):
                local_calls[cur].add((lab or None, int(off or loff)))
            for args in EXEC.findall(code):
                for lab in re.findall(r'\bSYM\((\w+)\)', args): burns[cur].add(lab)
                nm = re.match(r'\s*b_\+(\d+)\b', args)      # the range start (an end may be the next label)
                if nm: raw_offs[cur].add(int(nm.group(1)))
                bm = re.match(r'\s*(\w+)\)', args)
                if bm and 'BASE(' in code: burns[cur].add(bm.group(1))
            if oneliner: cur = None
    for key in ages_only_funcs:      # a function that returns at once under Seasons: nothing it does counts
        callees[key], burns[key], local_calls[key], tails[key] = set(), set(), set(), set()
    by_name = defaultdict(list)
    for (path, fn) in callees: by_name[fn].append(path)

    def routine_of(fn):
        base = fn[:-5] if fn.endswith('_hook') else None
        if base is None: return None
        base = base.replace('__', '@').split('@')[0]
        return base

    def hook_verdict(fn):
        base = fn[:-5]
        for cand in (base, base.replace('__', '@').split('@')[0], re.sub(r'_b[0-9a-f]{2}$', '', base.replace('__', '@').split('@')[0])):
            if cand in verdict: return verdict[cand]
        return None

    seasons_names = set(rom_labels(seasons_sym))
    from symfiles import pair_instances
    from routine_equiv import Game, mask_jumptables, reconcile
    pairs = pair_instances(ages_rom, ages_sym, seasons_rom, seasons_sym)
    A, S = Game(ages_rom, ages_sym), Game(seasons_rom, seasons_sym)
    ages_labels = rom_labels(ages_sym)
    unpairable = re.compile(r'^(_label_[0-9a-f]{2}_\d+|label_[0-9a-f]{2}_\d+)')

    local_cache = {}

    def local_verdict(name):
        """A @local that routine_equiv did not rate on its own (reached by `call`, so not part of
        its parent's flow-followed body): compare its body directly."""
        if name in local_cache: return local_cache[name]
        v = 'IDENTICAL'
        for a in ages_labels.get(name, []):
            s_ = pairs.get((name,) + a)
            if s_ is None: v = 'AGES_ONLY'; break
            try:
                an, ash = A.normalized(a[0], a[1]); sn, ssh = S.normalized(s_[0], s_[1])
                if an != sn and ash == ssh: an, sn = reconcile(an, sn, A, S)
                if an != sn: v = 'DIFFERENT'; break
            except Exception: v = 'DIFFERENT'; break
        local_cache[name] = v
        return v

    def label_verdict(sid):
        name = re.sub(r'_b[0-9a-f]{2}$', '', sid).replace('__', '@')
        if unpairable.match(name): return local_verdict(name) if name in ages_labels else 'AGES_ONLY'
        if name not in seasons_names: return 'AGES_ONLY'
        if name in verdict: return verdict[name]
        if name.replace('@', '__') in verdict: return verdict[name.replace('@', '__')]
        if '@' in name and name in ages_labels: return local_verdict(name)
        return verdict.get(name.split('@')[0])


    def local_call_ok(base, off):
        """The bodies at the call target of the `call` at base+off are identical in both games."""
        for a in ages_labels.get(base, []):
            s_ = pairs.get((base,) + a)
            if s_ is None: return False
            for g, (bank, addr) in ((A, a), (S, s_)):
                if g.rd(bank, addr + off) not in (0xcd, 0xc4, 0xcc, 0xd4, 0xdc): return False
            ta = A.rd(a[0], a[1] + off + 1) | (A.rd(a[0], a[1] + off + 2) << 8)
            ts = S.rd(s_[0], s_[1] + off + 1) | (S.rd(s_[0], s_[1] + off + 2) << 8)
            try:
                an, ash = A.normalized(a[0], ta); sn, ssh = S.normalized(s_[0], ts)
                if an != sn and ash == ssh: an, sn = reconcile(an, sn, A, S)
                if mask_jumptables(an) != mask_jumptables(sn): return False
            except Exception: return False
        return bool(ages_labels.get(base))

    real_hooks = set(l.split()[1] for l in open('src/hooks/generated.txt') if len(l.split()) >= 2)
    # a routine rewritten by hand for Seasons (src/game/seasons/) is Seasons' own; the shared C stays Ages-only for it
    rewritten_seasons = set()
    if os.path.exists('src/hooks/rewritten_seasons.txt'):
        for l in open('src/hooks/rewritten_seasons.txt'):
            n = l.split('#')[0].strip()
            if n: rewritten_seasons.add(n.replace('@', '__') + '_hook')
    hand_checked = set()
    for path in ('src/hooks/ofs_routines.txt', 'src/hooks/seasons_ok_manual.txt'):
        if os.path.exists(path):
            for l in open(path):
                n = l.split('#')[0].split()
                if n: hand_checked.add(n[0].split(':')[0].replace('@', '__'))
    eligible, why = {}, {}
    for key, fn in ((k, k[1]) for k in callees):
        # a hook entry needs its routine identical; a helper (even one named *_hook) is judged by what it burns
        ok = hook_verdict(fn) == 'IDENTICAL' if fn in real_hooks else True
        if not ok: why[key] = f'routine {hook_verdict(fn)}'
        for lab in burns[key]:
            v = label_verdict(lab)
            if v is not None and v != 'IDENTICAL': ok = False; why.setdefault(key, f'burns {lab} ({v})')
        for lab in tails[key]:
            if label_verdict(lab) == 'AGES_ONLY': ok = False; why.setdefault(key, f'TAIL({lab}) has no Seasons address')
        base_labels = [l for l in burns[key] if l in ages_labels]
        # burns past the routine's flow-followed body land in a call-only @local (or another
        # routine): that code must be identical too
        if len(base_labels) == 1 and base_labels[0] not in hand_checked and raw_offs[key]:
            base = base_labels[0]
            for a in ages_labels[base]:
                try: body_addrs = set(x[0] for x in A.body(a[0], a[1]))
                except Exception: body_addrs = set()
                for n in sorted(raw_offs[key]):
                    if a[1] + n in body_addrs or a[1] + n >= 0x8000: continue
                    addr = a[1] + n
                    while addr > a[1] and (a[0] if addr >= 0x4000 else 0, addr) not in A.by_addr: addr -= 1
                    cover = A.by_addr.get((a[0] if addr >= 0x4000 else 0, addr))
                    if not cover or cover == base: continue
                    v = label_verdict(cover.replace('@', '__'))
                    if v is not None and v != 'IDENTICAL': ok = False; why.setdefault(key, f'burns {base}+{n} in {cover} ({v})'); break
                    # b_+N must land on the same instruction in Seasons: the covering label has to sit
                    # at the same distance from the base in both games
                    sb_ = pairs.get((base,) + a); ca = [x for x in ages_labels.get(cover, []) if x[0] == a[0] or x[0] == 0]
                    if sb_ and ca:
                        sc = pairs.get((cover,) + ca[0])
                        if sc is None or (sc[1] - sb_[1]) != (ca[0][1] - a[1]):
                            ok = False; why.setdefault(key, f'burns {base}+{n} in {cover}, which sits at another offset in Seasons'); break
        for lab, off in local_calls[key]:
            base = lab or (base_labels[0] if len(base_labels) == 1 else None)
            if base is None or not local_call_ok(base, off): ok = False; why.setdefault(key, f'CALL_L target at {base}+{off} differs')
        eligible[key] = ok
    changed = True
    while changed:
        changed = False
        for key, cs in callees.items():
            if not eligible[key]: continue
            path, fn = key
            for c in cs:
                if (path, c) in callees: ok = eligible[(path, c)]
                elif c.endswith('_hook'):
                    ok = any(eligible.get((p, c), False) for p in by_name.get(c, [])) if by_name.get(c) else False
                else: ok = True   # engine/core helpers (mem_rd, alu_*, push_effect...) are game-neutral
                if not ok:
                    eligible[key] = False; changed = True; why[key] = f'calls {c}'; break
    if '--why' in sys.argv:
        # identical routines that are still out, with the reason (a callee's own reason is chased)
        def root(key, seen=()):
            r = why.get(key, '?')
            m = re.match(r'calls (\w+)', r)
            if m and key not in seen:
                for p in by_name.get(m.group(1), []) or [key[0]]:
                    k2 = (p, m.group(1))
                    if k2 in why: return f'{r} <- ' + root(k2, seen + (key,))
            return r
        for key in sorted(callees):
            fn = key[1]
            if fn.endswith('_hook') and not eligible[key] and hook_verdict(fn) == 'IDENTICAL':
                print(f'{fn}: {root(key)}')

    ages, seasons = ages_labels, rom_labels(seasons_sym)
    rows, skipped = [], defaultdict(int)
    for l in open('src/hooks/generated.txt'):
        p = l.split()
        if len(p) < 2: continue
        bank, addr = int(p[0][:2], 16), int(p[0][3:], 16)
        fn, flags = p[1], (p[2] if len(p) > 2 else '-')
        if not fn.endswith('_hook'): skipped['not a hook'] += 1; continue
        if not any(eligible.get((path, fn), False) for path in by_name.get(fn, [])):
            skipped['not eligible'] += 1
            if '--why' in sys.argv and not by_name.get(fn): print(f'{fn}: no C function of that name')
            elif '--why' in sys.argv and hook_verdict(fn) != 'IDENTICAL' and not any(why.get((path, fn)) for path in by_name.get(fn, [])): print(f'{fn}: routine {hook_verdict(fn)}, no other reason')
            continue
        name = [n for n, inst in ages.items() if (bank, addr) in inst]
        name = [n for n in name if n.replace('@', '__') in fn] or name
        if not name: skipped['no label'] += 1; continue
        name = name[0]
        parent = name.split('@')[0]
        s = pairs.get((name, bank, addr))
        if s is None:
            skipped['no seasons label'] += 1
            if '--why' in sys.argv: print(f'{fn}: no Seasons pair for {name} at {bank:02x}:{addr:04x}')
            continue
        if fn in rewritten_seasons: skipped['rewritten for Seasons'] += 1; continue
        rows.append((s[0], s[1], fn, flags))
    rows.sort()
    with open('src/hooks/generated_seasons.txt', 'w') as f:
        for b, a, fn, fl in rows: f.write(f'{b:02x}:{a:04x} {fn} {fl}\n')
    print(f'{len(rows)} hooks eligible for Seasons; skipped: {dict(skipped)}')


if __name__ == '__main__':
    main()
