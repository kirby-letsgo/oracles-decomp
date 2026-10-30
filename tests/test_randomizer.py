"""Synthetic progression graphs, not extracted game locations or ROM fixtures."""
import copy
import json
import itertools
import random
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/randomizer'))
from generate import GenerationError, generate, package, validate, verify
from hashlib import sha256
from seasons import chest_catalog, dungeon_chests, d1_optional


def world():
    return {
        'format': 1, 'game': 'seasons', 'start': 'start',
        'regions': ['start', 'forest', 'summit'],
        'edges': [
            {'from': 'start', 'to': 'forest', 'requires': [{'sword': 1}]},
            {'from': 'forest', 'to': 'summit', 'requires': [{'feather': 1}]},
        ],
        'items': {'sword': {'id': 5, 'subid': 0}, 'feather': {'id': 23, 'subid': 0},
                  'rupees': {'id': 40, 'subid': 1}},
        'pool': ['sword', 'feather', 'rupees', 'rupees'],
        'locations': [
            {'name': 'start-a', 'region': 'start', 'target': [0, 0, 40, 1]},
            {'name': 'start-b', 'region': 'start', 'target': [0, 1, 40, 1]},
            {'name': 'forest-check', 'region': 'forest', 'target': [0, 2, 40, 1]},
            {'name': 'summit-check', 'region': 'summit', 'target': [0, 3, 40, 1]},
        ],
        'goal': [{'sword': 1, 'feather': 1}],
    }


class RandomizerTests(unittest.TestCase):
    def test_seed_replays_and_many_seeds_are_completable(self):
        w = world()
        outcomes = set()
        for seed in map(str, range(200)):
            result = generate(w, seed)
            self.assertEqual(result, generate(copy.deepcopy(w), seed))
            self.assertEqual(result['spheres'], verify(w, result['placements']))
            outcomes.add(tuple(result['placements'].items()))
        self.assertGreater(len(outcomes), 1)

    def test_independent_verifier_rejects_self_locked_sword(self):
        with self.assertRaisesRegex(GenerationError, 'unreachable'):
            verify(world(), {'start-a': 'rupees', 'start-b': 'feather',
                             'forest-check': 'sword', 'summit-check': 'rupees'})

    def test_impossible_world_and_budget_fail(self):
        w = world()
        for loc in w['locations']:
            loc['requires'] = [{'sword': 1}]
        with self.assertRaisesRegex(GenerationError, 'no completable'):
            generate(w, 'locked')
        with self.assertRaisesRegex(GenerationError, 'budget exhausted'):
            generate(world(), 'budget', max_nodes=1)

    def test_starting_inventory_alternative_routes_and_counts(self):
        w = world()
        w['starting_items'] = {'sword': 1}
        w['edges'][0]['requires'] = [{'sword': 2}, {'rupees': 1}]
        w['goal'] = [{'sword': 2, 'feather': 1}]
        for seed in map(str, range(50)):
            result = generate(w, seed)
            verify(w, result['placements'])

    def test_goal_and_all_checks_must_be_reachable(self):
        w = world()
        w['goal'] = [{'sword': 3}]
        with self.assertRaises(GenerationError):
            generate(w, 'goal')
        w = world()
        w['edges'][1]['requires'] = []  # unreachable even if the goal is met
        with self.assertRaises(GenerationError):
            generate(w, 'unreachable')

    def test_search_matches_exhaustive_small_worlds(self):
        rng = random.Random(42)
        for case in range(40):
            w = world()
            for loc in w['locations']:
                loc['requires'] = rng.choice([[{}], [{'sword': 1}], [{'feather': 1}],
                                              [{'sword': 1}, {'feather': 1}], []])
            names = [loc['name'] for loc in w['locations']]
            exists = False
            for items in set(itertools.permutations(w['pool'])):
                try:
                    verify(w, dict(zip(names, items)))
                    exists = True
                    break
                except GenerationError:
                    pass
            if exists:
                verify(w, generate(w, str(case))['placements'])
            else:
                with self.assertRaisesRegex(GenerationError, 'no completable'):
                    generate(w, str(case))

    def test_invalid_worlds_are_rejected(self):
        mutations = [
            lambda w: w['locations'][1].update(target=w['locations'][0]['target']),
            lambda w: w['locations'][0].update(target=[8, 0, 40, 1]),
            lambda w: w['items']['sword'].update(subid=255),
            lambda w: w['items']['sword'].update(id=64),  # essences unsupported
            lambda w: w['pool'].pop(),
            lambda w: w['pool'].__setitem__(0, 'missing'),
            lambda w: w['edges'][0].update(requires=[{'typo': 1}]),
            lambda w: w.update(game='unknown'),
        ]
        for mutate in mutations:
            w = world()
            mutate(w)
            with self.assertRaises(GenerationError):
                validate(w)

    def test_export_calls_match_verified_placements_and_cannot_overwrite(self):
        w = world()
        r = generate(w, 'export')
        identity = sha256(json.dumps(r, sort_keys=True).encode()).hexdigest()[:12]
        with tempfile.TemporaryDirectory() as tmp:
            dest = Path(tmp) / ('randomizer-seasons-' + identity)
            package(w, r, dest)
            self.assertIn('id=' + dest.name, (dest / 'mod.ini').read_text())
            self.assertIn('changes_game=yes', (dest / 'mod.ini').read_text())
            lua = (dest / 'main.lua').read_text()
            self.assertEqual(lua.count('oracles.swap_treasure('), 4)
            for loc in w['locations']:
                item = w['items'][r['placements'][loc['name']]]
                args = loc['target'] + [item['id'], item['subid']]
                self.assertIn('oracles.swap_treasure(' + ', '.join(map(str, args)) + ')', lua)
            self.assertEqual(json.loads((dest / 'spoiler.json').read_text()), r)
            with self.assertRaises(FileExistsError):
                package(w, r, dest)
            w['goal'] = [{'rupees': 2}]
            with self.assertRaisesRegex(GenerationError, 'world changed'):
                package(w, r, Path(tmp) / 'changed')

    def test_seasons_catalog_and_d1_scope(self):
        catalog = chest_catalog()
        self.assertEqual(len(catalog), 108)
        # Group 2 and 3 deliberately share a table, not two separate lists.
        aliases = [[c['target'][1:] for c in catalog if c['target'][0] == g] for g in (2, 3)]
        self.assertEqual(aliases[0], aliases[1])
        d1 = dungeon_chests(1)
        self.assertEqual(len(d1), 7)
        w = d1_optional()
        self.assertEqual(len(w['locations']), 3)
        self.assertEqual({c['target'][1] for c in w['locations']}, {0x0d, 0x0f, 0x19})
        self.assertEqual({c['target'][2] for c in w['excluded']}, {3, 0x30, 0x31, 0x2d})
        self.assertTrue(all(c['source'] and c['symbol'] for c in w['locations']))
        outcomes = set()
        for seed in map(str, range(60)):
            result = generate(w, seed)
            verify(w, result['placements'])
            outcomes.add(tuple(result['placements'].values()))
        self.assertEqual(len(outcomes), 6)  # all three-item permutations, never progression items

    def test_cli_exports_and_rejects_invalid_seed_without_output(self):
        script = Path(__file__).resolve().parents[1] / 'tools/randomizer/generate.py'
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / 'world.json'
            source.write_text(json.dumps(world()))
            output = Path(tmp) / 'mods'
            result = subprocess.run([sys.executable, str(script), str(source), '--seed', 'cli',
                                     '--output', str(output)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue((Path(result.stdout.strip()) / 'main.lua').is_file())
            invalid = Path(tmp) / 'invalid'
            result = subprocess.run([sys.executable, str(script), str(source), '--seed', '',
                                     '--output', str(invalid)], capture_output=True, text=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertFalse(invalid.exists())


if __name__ == '__main__':
    unittest.main()
