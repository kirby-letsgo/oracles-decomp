#!/usr/bin/env python3
"""Development seed generator with a source-audited optional D1 chest preset.

Logic format and limitations: tools/randomizer/README.md.
"""
from collections import Counter
from hashlib import sha256
import argparse
import json
from pathlib import Path
import re

ALGORITHM = 'oracles-forward-fill-v1'


class GenerationError(ValueError):
    pass


def require(ok, message):
    if not ok:
        raise GenerationError(message)


def number(value, high):
    return type(value) is int and 0 <= value <= high


def treasure_counts(game):
    # Reuse the host's audited table instead of maintaining a second validity list.
    source = Path(__file__).resolve().parents[2] / 'src/mods/treasures.c'
    match = re.search(r'unsigned char ' + game + r'\[\] = \{([^}]+)\}', source.read_text())
    require(match is not None, 'cannot read host treasure table')
    return [int(n) for n in re.findall(r'\d+', match.group(1))]


def validate(world):
    require(isinstance(world, dict), 'world must be an object')
    require(world.get('format') == 1, 'world format must be 1')
    require(world.get('game') in ('ages', 'seasons'), 'game must be ages or seasons')
    counts = treasure_counts(world['game'])
    def valid_treasure(tid, subid):
        return number(tid, len(counts) - 1) and number(subid, 255) and subid < counts[tid]
    items = world.get('items', {})
    require(isinstance(items, dict) and items, 'items must be a nonempty object')
    for name, item in items.items():
        require(isinstance(name, str) and re.fullmatch(r'[a-z0-9-]+', name), 'invalid item name')
        require(isinstance(item, dict), f'invalid item: {name}')
        require(valid_treasure(item.get('id'), item.get('subid')), f'invalid treasure: {name}')
    regions = world.get('regions', [])
    require(isinstance(regions, list) and all(isinstance(r, str) for r in regions)
            and len(regions) == len(set(regions)), 'regions must be distinct strings')
    require(world.get('start') in regions, 'start must name a region')

    def requirements(r):
        require(isinstance(r, list), 'requirements must be a list of alternative item-count objects')
        for clause in r:
            require(isinstance(clause, dict), 'each requirement alternative must be an object')
            for item, count in clause.items():
                require(item in items and type(count) is int and count > 0, 'invalid requirement item/count')

    requirements(world.get('goal', []))
    require(world.get('goal'), 'goal must have at least one alternative')
    start_items = world.get('starting_items', {})
    require(isinstance(start_items, dict), 'starting_items must be an object')
    for item, count in start_items.items():
        require(item in items and type(count) is int and count > 0, 'invalid starting item/count')
    edges = world.get('edges', [])
    require(isinstance(edges, list), 'edges must be a list')
    for edge in edges:
        require(isinstance(edge, dict) and edge.get('from') in regions and edge.get('to') in regions,
                'edge must connect known regions')
        requirements(edge.get('requires', [{}]))
    locations = world.get('locations', [])
    require(isinstance(locations, list) and 0 < len(locations) <= 512, 'expected 1..512 locations')
    names, targets = set(), set()
    for loc in locations:
        require(isinstance(loc, dict), 'location must be an object')
        name = loc.get('name')
        require(isinstance(name, str) and name and name not in names, 'duplicate or missing location name')
        names.add(name)
        require(loc.get('region') in regions, f'unknown region: {name}')
        requirements(loc.get('requires', [{}]))
        target = loc.get('target', [])
        require(isinstance(target, list) and len(target) == 4
                and all(number(v, 7 if i == 0 else 255) for i, v in enumerate(target)),
                f'invalid swap target: {name}')
        require(valid_treasure(target[2], target[3]), f'unsupported source treasure: {name}')
        require(tuple(target) not in targets, f'ambiguous swap target: {name}')
        targets.add(tuple(target))
    pool = world.get('pool', [])
    require(isinstance(pool, list) and len(pool) == len(locations), 'one pool item is required per location')
    require(all(isinstance(item, str) and item in items for item in pool), 'unknown pool item')


def allows(requirements, inventory):
    return any(all(inventory[item] >= n for item, n in clause.items()) for clause in requirements)


def reachable(world, inventory):
    regions = {world['start']}
    while True:
        added = {e['to'] for e in world.get('edges', []) if e['from'] in regions
                 and allows(e.get('requires', [{}]), inventory)} - regions
        if not added:
            break
        regions.update(added)
    return [loc['name'] for loc in world['locations'] if loc['region'] in regions
            and allows(loc.get('requires', [{}]), inventory)]


def verify(world, placement):
    """Independent sphere replay; never trusts the generator's simulated inventory."""
    require(set(placement) == {loc['name'] for loc in world['locations']}, 'placement must cover every location')
    require(Counter(placement.values()) == Counter(world['pool']), 'placement changed the item pool')
    inventory = Counter(world.get('starting_items', {}))
    checked, spheres = set(), []
    while True:
        sphere = sorted(set(reachable(world, inventory)) - checked)
        if not sphere:
            break
        spheres.append(sphere)
        checked.update(sphere)
        inventory.update(placement[name] for name in sphere)
    require(len(checked) == len(placement), 'placement contains unreachable checks')
    require(allows(world['goal'], inventory), 'placement cannot reach the goal')
    return spheres


def generate(world, seed, max_nodes=100000):
    validate(world)
    require(isinstance(seed, str) and 0 < len(seed) <= 128, 'seed must be 1..128 characters')
    require(type(max_nodes) is int and max_nodes > 0, 'max_nodes must be positive')
    # Canonical ordering and SHA-256 avoid Python hash/random implementation dependence.
    canonical = json.dumps(world, sort_keys=True, separators=(',', ':'))
    fingerprint = sha256(canonical.encode()).hexdigest()
    prefix = ALGORITHM + '\0' + fingerprint + '\0' + seed
    def order(values, step):
        return sorted(values, key=lambda v: sha256((prefix + '\0' + step + '\0' + v).encode()).digest())

    placement = {}
    pool = Counter(world['pool'])
    inventory = Counter(world.get('starting_items', {}))
    failed = set()
    nodes = 0

    def fill():
        nonlocal nodes
        nodes += 1
        require(nodes <= max_nodes, 'search budget exhausted; no seed exported')
        if len(placement) == len(world['locations']):
            return allows(world['goal'], inventory)
        state = (tuple(sorted(placement)), tuple(sorted((k, v) for k, v in pool.items() if v)))
        if state in failed:
            return False
        available = set(reachable(world, inventory)) - placement.keys()
        # Every remaining choice is locked: backtrack instead of exporting a broken seed.
        if not available:
            failed.add(state)
            return False
        depth = str(len(placement))
        location = order(available, 'location:' + depth)[0]
        for item in order([k for k, v in pool.items() if v], 'item:' + depth):
            placement[location] = item
            pool[item] -= 1
            inventory[item] += 1
            if fill():
                return True
            inventory[item] -= 1
            pool[item] += 1
            del placement[location]
        failed.add(state)
        return False

    require(fill(), 'no completable placement exists under this logic')
    spheres = verify(world, placement)
    return {'algorithm': ALGORITHM, 'seed': seed, 'world_sha256': fingerprint,
            'game': world['game'], 'preset': world.get('preset'), 'scope': world.get('scope', 'Custom world logic'),
            'placements': dict(sorted(placement.items())), 'spheres': spheres}


def package(world, result, destination):
    validate(world)
    canonical = json.dumps(world, sort_keys=True, separators=(',', ':'))
    require(result['world_sha256'] == sha256(canonical.encode()).hexdigest(), 'world changed since generation')
    spheres = verify(world, result['placements'])
    require(result['algorithm'] == ALGORITHM and result['game'] == world['game'], 'incompatible seed result')
    # Fail before writing anything if the output already exists: never replace a played seed.
    destination = Path(destination)
    identity = sha256(json.dumps(result, sort_keys=True).encode()).hexdigest()[:12]
    mod_id = 'randomizer-' + world['game'] + '-' + identity
    require(destination.name == mod_id, f'output folder must be named {mod_id}')
    lines = ['-- Generated by ' + ALGORITHM + '. Do not change placements mid-playthrough.']
    for loc in sorted(world['locations'], key=lambda x: x['name']):
        item = world['items'][result['placements'][loc['name']]]
        args = loc['target'] + [item['id'], item['subid']]
        lines.append('oracles.swap_treasure(' + ', '.join(str(v) for v in args) + ')')
    lines.append('oracles.log("randomizer ' + identity + ' ready")')
    destination.mkdir(parents=True, exist_ok=False)
    is_d1 = world.get('preset') == 'seasons-d1-optional-v1'
    title = 'D1 Chest Shuffle' if is_d1 else 'Randomizer'
    description = ('Shuffles only D1 map, compass and Gasha chests. All progression stays vanilla. Unlinked Seasons only.'
                   if is_d1 else 'Development seed. Use only with its matching logic and a fresh save.')
    (destination / 'mod.ini').write_text(
        f'id={mod_id}\nname={title} {identity}\ndescription={description}\n'
        f'version=0.1\nauthor=Oracles\napi=2\ngames={world["game"]}\nchanges_game=yes\n')
    (destination / 'main.lua').write_text('\n'.join(lines) + '\n')
    (destination / 'README.txt').write_text(
        title + ' - development preview\n\n' + description + '\n\n'
        'Install this entire folder under your Oracles app data mods folder.\n'
        'Use a fresh test save and only one randomizer mod at a time.\n'
        'Keep this same seed folder installed for the entire run.\n'
        'This build does not enforce seed/save pairing.\n'
        'See spoiler.json for placements and world.json for scope and exclusions.\n'
        'Full-game progression randomization is not implemented.\n')
    spoiler = dict(result, spheres=spheres)
    (destination / 'world.json').write_text(json.dumps(world, indent=2) + '\n')
    (destination / 'spoiler.json').write_text(json.dumps(spoiler, indent=2) + '\n')
    return destination


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('world', type=Path, nargs='?')
    parser.add_argument('--preset', choices=['seasons-d1-optional'])
    parser.add_argument('--seed', required=True)
    parser.add_argument('--output', type=Path, required=True, help='parent directory for the generated mod')
    args = parser.parse_args()
    try:
        require(bool(args.world) != bool(args.preset), 'provide a world file or --preset, not both')
        if args.preset:
            from seasons import d1_optional
            world = d1_optional()
        else:
            world = json.loads(args.world.read_text())
        result = generate(world, args.seed)
        identity = sha256(json.dumps(result, sort_keys=True).encode()).hexdigest()[:12]
        path = package(world, result, args.output / f'randomizer-{world["game"]}-{identity}')
    except (GenerationError, OSError, ValueError, KeyError, TypeError) as exc:
        parser.exit(1, f'randomizer: {exc}\n')
    print(path)


if __name__ == '__main__':
    main()
