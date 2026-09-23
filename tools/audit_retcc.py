#!/usr/bin/env python3
"""Find a conditional `ret` whose not-taken case burns nothing: after
`if (...) { RET_TAKEN(b_+N); return; }` the next burn must start at b_+N (the `ret cc` costs
2 M-cycles when it falls through). usage: tools/audit_retcc.py [--fix]"""
import glob, re, sys
BURN = re.compile(r'\b(?:CYCT?|CALL_C|CALL_C_CC|CALL_ROM|CALL_ROM_CC|CALL_L|CALL_L_CC|RET|RET_TAKEN|CALL|I|push_effect|bank_push|bank_pop|switch_bank)\((?:gb, )?b_\+(?:OE?\(|S\()?(\d+)')
IF_RET = re.compile(r'if \([^{};]*\)\s*\{\s*RET_TAKEN\(b_\+(?:O\(|S\()?(\d+)\)?\);\s*return;\s*\}', re.S)
hits = []
fix = '--fix' in sys.argv
for p in sorted(glob.glob('src/game/**/*.c', recursive=True)):
    if '/seasons/gen_' in p or p.endswith(('syms.c', 'ofs.c')): continue
    text = open(p, errors='replace').read()
    code = re.sub(r'//[^\n]*', '', text)
    for m in IF_RET.finditer(code):
        n = int(m.group(1)); rest = code[m.end():m.end() + 600]
        stop = re.search(r'\belse\b|\breturn\b|\bgoto\b|^\s*\w+:|\n\}', rest, re.M)
        b = BURN.search(rest)
        if re.match(r'\s*else\b', rest): continue
        if b and (not stop or b.start() < stop.start()) and int(b.group(1)) != n:
            hits.append(f'{p}:{code.count(chr(10), 0, m.end()) + 1}: ret-cc at b_+{n} falls through to a burn at b_+{b.group(1)}')
            if fix:
                # the brace ends at the same spot in the uncommented text only if no comment precedes it on its line
                line_no = code.count(chr(10), 0, m.end())
                lines = text.split(chr(10)); tl = lines[line_no]
                k = tl.rfind('return; }')
                if k < 0: k = tl.rfind('}') - len('return; ')
                k = tl.index('}', k) + 1 if k >= 0 else -1
                if k > 0: lines[line_no] = tl[:k] + f' CYC(b_+{n}, b_+{n + 1});' + tl[k:]
                text = chr(10).join(lines); open(p, 'w').write(text); code = re.sub(r'//[^\n]*', '', text)
print('\n'.join(hits)); print(f'{len(hits)} missing not-taken ret burns')
