#!/usr/bin/env python3
"""Keep the audit findings that sit in C no movie runs.

usage: tools/filter_uncovered.py BINARY PROFDATA[,PROFDATA...] < audit_output

BINARY is an oracles-run built with -fprofile-instr-generate -fcoverage-mapping, and each
PROFDATA the llvm-profdata merge of the movies run on it (one per game). A function counts as
run when any profile ran it. Lines of the form `path:line: ...` (every audit here prints that)
are passed through when the line falls inside a function no profile ran, and dropped otherwise;
other lines pass unchanged. The audits' review lists are long; findings in code the gates never
execute are the ones only the audits can catch, so they are read first.
"""
import collections, json, os, re, subprocess, sys

binary, profiles = sys.argv[1], sys.argv[2].split(',')
root = os.getcwd() + '/'
ran = collections.defaultdict(bool)
spans = collections.defaultdict(list)     # path -> [(first line, last line, function)]
for prof in profiles:
    out = subprocess.run(['xcrun', 'llvm-cov', 'export', binary, f'-instr-profile={prof}'], capture_output=True, text=True).stdout
    for f in json.loads(out)['data'][0]['functions']:
        name, path = f['name'], f['filenames'][0].replace(root, '')
        if f['count']: ran[(path, name)] = True
        if prof == profiles[0] and f['regions']:
            spans[path].append((min(r[0] for r in f['regions']), max(r[2] for r in f['regions']), name))
kept = dropped = 0
for line in sys.stdin:
    m = re.match(r'([^:\s]+\.c):(\d+):', line)
    if not m: sys.stdout.write(line); continue
    path, ln = m.group(1), int(m.group(2))
    owner = next((n for lo, hi, n in spans.get(path, []) if lo <= ln <= hi), None)
    if owner is not None and ran[(path, owner)]: dropped += 1; continue
    kept += 1; sys.stdout.write(line)
print(f'filter_uncovered: {kept} findings in never-run code kept, {dropped} in code the movies run dropped', file=sys.stderr)
