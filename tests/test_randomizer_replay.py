"""Generate the source-audited D1 preset and replay it through the actual native engine."""
from pathlib import Path
import sys
import tempfile
from hashlib import sha256
import json
import subprocess

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/randomizer'))
from generate import generate, package
from seasons import d1_optional

world = d1_optional()
result = generate(world, 'first')
assert result['placements']['d1-map-chest'] == 'd1-gasha-chest'
assert result['placements']['d1-gasha-chest'] == 'd1-map-chest'
identity = sha256(json.dumps(result, sort_keys=True).encode()).hexdigest()[:12]
with tempfile.TemporaryDirectory(prefix='oracles-randomizer-replay-') as tmp:
    package(world, result, Path(tmp) / ('randomizer-seasons-' + identity))
    replay = subprocess.run([sys.argv[1], tmp], timeout=300)
    sys.exit(replay.returncode)
