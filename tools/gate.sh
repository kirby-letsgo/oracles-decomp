#!/bin/bash
# The full gate for a batch, from build-quirk: the Seasons playthrough and the whole Ages movie
# under --verify-shadow --ref-check, ctest, and the native TAS tests, in parallel.
# usage: tools/gate.sh TAG   (logs in /tmp/TAG{S,A,ct,nt}.log, summary on stdout)
cd "$(dirname "$0")/.."
T=${1:-gate}
cp ./build-quirk/oracles-run /tmp/oracles-run-$T
S="roms/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc"
A="roms/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc"
(/tmp/oracles-run-$T --rom "$S" --boot roms/cgb_boot.bin --init-ram tas/gbhawk-wram0.txt --tas tas/seasons-play.inputs --frames 265064 --verify-shadow --ref-check tas/seasons-play.ref > /tmp/${T}S.log 2>&1; echo "seasons exit $?" >> /tmp/${T}S.log) &
(/tmp/oracles-run-$T --rom "$A" --boot roms/cgb_boot.bin --init-ram tas/gbhawk-wram0.txt --tas tas/ages-consoleverified.inputs --frames 289518 --verify-shadow --ref-check tas/ages.ref > /tmp/${T}A.log 2>&1; echo "ages exit $?" >> /tmp/${T}A.log) &
(ctest --test-dir build-quirk 2>&1 | grep -E "tests passed|Failed|\*\*\*" > /tmp/${T}ct.log) &
(./build-quirk/test_native_tas > /tmp/${T}nt.log 2>&1) &
wait
for f in /tmp/${T}S.log /tmp/${T}A.log; do
  echo "== $f"
  grep -E "exit|shadow:|verify failures|mismatch at" "$f"
  grep MISMATCH "$f" | awk '{print $3,$5}' | sort | uniq -c | sort -rn | head -12
done
cat /tmp/${T}ct.log
grep -v "^ok" /tmp/${T}nt.log | head -3
tail -2 /tmp/${T}nt.log
