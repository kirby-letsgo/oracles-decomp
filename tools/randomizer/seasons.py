"""Source-audited Seasons presets. This module contains symbols/rules, not ROM bytes."""
from pathlib import Path
import re
from generate import GenerationError, require, validate

DISASM = Path(__file__).resolve().parents[2] / 'ref/oracles-disasm'


def treasures(root=DISASM):
    """Resolve m_TreasureSubid symbols to the same id/subid pairs the assembler uses."""
    result, tid, subid = {}, None, 0
    path = root / 'data/seasons/treasureObjectData.s'
    for lineno, line in enumerate(path.read_text().splitlines(), 1):
        top = re.search(r'/\* \$([0-9a-f]+) \*/', line)
        label = re.match(r'treasureObjectData([0-9a-f]{2}):', line)
        if top or label:
            tid, subid = int((top or label).group(1), 16), 0
        symbol = re.search(r'm_TreasureSubid\s+.*?,\s*(TREASURE_OBJECT_\w+)', line)
        if symbol:
            require(tid is not None, f'treasure without an index at {lineno}')
            result[symbol.group(1)] = (tid, subid)
            subid += 1
    return result


def chest_catalog(root=DISASM):
    path = root / 'data/seasons/chestData.s'
    lines = path.read_text().splitlines()
    symbols = treasures(root)
    result = []
    for start, line in enumerate(lines):
        label = re.fullmatch(r'chestGroup([0-7])Data:', line.strip())
        if not label:
            continue
        group = int(label.group(1))
        # Consecutive group labels are aliases; follow each pointer until its $ff terminator.
        for at in range(start + 1, len(lines)):
            text = lines[at].split(';')[0].strip()
            if re.fullmatch(r'\.db\s+\$ff', text):
                break
            match = re.fullmatch(r'm_ChestData\s+\$([0-9a-f]+),\s*\$([0-9a-f]+),\s*(TREASURE_OBJECT_\w+)', text)
            if not match:
                continue
            position, room, symbol = match.groups()
            require(symbol in symbols, f'unknown chest treasure {symbol}')
            tid, subid = symbols[symbol]
            result.append({'target': [group, int(room, 16), tid, subid],
                           'position': int(position, 16), 'symbol': symbol,
                           'source': f'data/seasons/chestData.s:{at + 1}'})
        else:
            raise GenerationError(f'chest group {group} has no terminator')
    targets = [tuple(c['target']) for c in result]
    require(len(targets) == len(set(targets)), 'chests have ambiguous swap targets')
    return result


def dungeon_chests(number, root=DISASM):
    source = (root / 'data/seasons/dungeonLayouts.s').read_text()
    match = re.search(rf'^dungeon{number:02x}Layout:\n(.*?)(?=^dungeon\w+Layout:|\Z)', source, re.M | re.S)
    require(match is not None, f'missing dungeon layout {number}')
    rooms = {int(n, 16) for line in match.group(1).splitlines() if line.strip().startswith('.db')
             for n in re.findall(r'\$([0-9a-f]{2})', line)} - {0}
    data = (root / 'data/seasons/dungeonData.s').read_text()
    group = re.search(rf'dungeonData{number:02x}:\s+m_DungeonData >wGroup([0-7])RoomFlags', data)
    require(group is not None, f'missing dungeon metadata {number}')
    return [c for c in chest_catalog(root) if c['target'][0] == int(group.group(1)) and c['target'][1] in rooms]


def d1_optional(root=DISASM):
    # These rewards do not gate movement, combat, doors or the boss. Keep every
    # progression-affecting reward vanilla; do not infer room routes from map adjacency.
    selected = {
        'TREASURE_OBJECT_GASHA_SEED_01': 'd1-gasha-chest',
        'TREASURE_OBJECT_COMPASS_02': 'd1-compass-chest',
        'TREASURE_OBJECT_MAP_02': 'd1-map-chest',
    }
    chests = dungeon_chests(1, root)
    locations, items, pool, excluded = [], {}, [], []
    for chest in chests:
        symbol = chest['symbol']
        if symbol not in selected:
            excluded.append({'target': chest['target'], 'symbol': symbol, 'source': chest['source'],
                             'reason': 'kept vanilla in the optional-reward preset'})
            continue
        name = selected[symbol]
        require(name not in items, f'expected one D1 chest for {symbol}')
        items[name] = {'id': chest['target'][2], 'subid': chest['target'][3]}
        pool.append(name)
        locations.append({'name': name, 'region': 'd1-vanilla-route', 'target': chest['target'],
                          'source': chest['source'], 'symbol': symbol})
    require(len(locations) == len(selected), 'D1 optional chest set changed; audit the preset')
    world = {
        'format': 1, 'game': 'seasons', 'preset': 'seasons-d1-optional-v1',
        'scope': 'Unlinked Seasons; three optional D1 chests only. All progression stays vanilla.',
        'regions': ['d1-vanilla-route'], 'start': 'd1-vanilla-route',
        'items': items, 'pool': sorted(pool), 'locations': sorted(locations, key=lambda c: c['name']),
        'goal': [{item: 1 for item in sorted(items)}],
        'excluded': excluded,
        'logic_note': 'Reachability here means obtainable along the unchanged vanilla D1 route, not reachable at game start. '
                      'No randomized reward is a requirement for any door, enemy, movement or boss.',
        'limitations': ['Not a full-game randomizer.', 'Not audited for linked games.',
                        'Vanilla sword, seed satchel, keys, bombs, boss and essence are unchanged.'],
    }
    validate(world)
    return world
