# Progress

Updated 2026-09-15. Newest entries at the top of each section.

## Where things stand

- Milestone 3 (readable C engine) started 2026-09-09: plan in
  `docs/plans/2026-09-09-m3-readable-engine.md`. Goal: the disassembly's `code/` tree (55,673
  lines) as readable C, one file per disassembly file, named RAM, real parameters, verified per
  routine against the transliteration. Progress: 4,619 routine hooks rewritten across eighteen code
  banks; bank 0 is fully readable C, gates green on the whole movie after each batch. Whole-movie
  `--verify-hooks-continue` runs passed on the batch 23 build (49.5M hook calls, 0 failures)
  and the batch 24 build (45.1M calls, 0 failures). Every plain routine in bank 0 is now
  rewritten. Batch 78 closed the last reporting gaps: 25 executable entry points became readable
  hooks, `pushDirectionData` was removed from the routine registry because it is data, and the only
  remaining non-hook row is the already-handwritten RAM routine `wRamFunction`. Regeneration now
  deletes the empty `gen_bank00.c`. Phase 3 started with batch 26 doing the thread state
  helpers, the serial handlers, the timer interrupt and the four interrupt vectors, batch 27 the
  VBlank handler and its function queue, batch 28 the LCD interrupt family, batch 29 the boot
  chain and the main loop/thread switcher itself, batch 30 the six thread entry points
  (`textThreadStart`, `fileSelectThreadStart`, `thread_1b10`, `introThreadStart`,
  `paletteFadeThreadStart`, `mainThreadStart`). Batch 31 finished the RAM-resident code
  (`hramOamDmaFunction`, `wMusicReadFunction`, `wRamFunction`), and batch 32 began phase 4 with
  object-loading, part-dispatch, and animation-queue routines in banks 12, 11, and 4; batch 34
  completed the remaining simple object-data opcode handlers. Batch 38 completed phase 4: every
  planned object-system source routine is now readable C and verified. Batch 39 opened phase 5
  with room initialization, room-layout drawing, script conditionals, and text-buffer setup;
  batch 40 added the first scripting state/control cluster, textbox mapping, and cutscene helpers;
  batch 41 added room-specific leaf handlers and script state/spawn/property commands; batch 42
  added tileset transfer and vine helpers, script arithmetic/text helpers, and textbox/cutscene
  leaf routines; batch 43 added room-GFX post-load handlers, script text/memory commands, and
  textbox map save/DMA/scroll helpers; batch 44 added tile replacements, script flag/jump
  commands, and the textbox character-display and heart-piece paths; batch 45 added room tile
  state handlers, script room/item commands, textbox option navigation, and text-stack helpers;
  batch 46 continued with Jabu/overworld tile replacements, script tile/call commands, and
  textbox bank-reading, display-speed, and numeric-substitution paths; batch 47 finished the
  textbox source and moved that lane into Twinrova cutscene helpers while continuing vine and
  script condition handlers; batch 48 added ten more room tile-state handlers, completed the
  remaining straightforward scripting commands, and continued the intro/Twinrova cutscene state
  paths; batch 49 completed `roomSpecificTileChanges.s`, added underwater-wave maintenance and
  the small Ages cutscene wrappers, and continued the title/riding-horse intro states; batch 50
  added the safe pirate-ship update path, finished the small cutscene wrappers and `cutscenes2.s`
  leaves, and completed the remaining riding-horse intro states; batch 51 added linked-game and
  endgame cutscene leaf helpers and continued the temple-intro state path; batch 52 added the
  remaining linked-game spawn helpers, endgame tile/inventory helpers, and temple/pre-title
  cinematic states; batch 53 finished the linked-game source's safe routines, added the remaining
  bank-3 temple camera/bar helpers, and opened bank 10's two large cutscene state machines; batch
  54 continued bank 10 and added endgame object-GFX and miscellaneous cutscene helpers; batch 55
  added deeper endgame, Maku/Nayru/intro, and bank-10 black-tower state helpers; batch 56 added
  endgame-state, miscellaneous cutscene, graphics-copy, and final bank-10 cutscene helpers; batch
  57 continued the endgame and three room-change cutscenes and finished `ages/cutscenes.s`; batch
  58 added deeper endgame/Ambi/Jabu states and the Twinrova dispatcher and room setup; batch 59
  added the credits root, clean-seas states, and both Twinrova cutscene-body dispatchers; batch 60
  added the next clean-seas/endgame states, the remaining miscellaneous cutscene dispatcher
  states, and bank-1 transition/cutscene helpers; batch 61 added ten more clean-seas/endgame
  states, the intro dispatcher roots, and bank-1 room-transition helpers; batch 62 added the
  forest-scrambler/eye-puzzle warp paths, the Capcom/title-screen intro states, and the next
  endgame stage-0/stage-1 entries; batch 63 completed the top-level bank-3 cutscene source,
  added the bank-4 warp resolver cluster, and continued the endgame stage-1 states; batch 64
  finished the scripting source's remaining roots, added room-GFX drawing and pirate-room
  dispatchers, and continued the stage-1/endgame-20 states; batch 65 added the remaining core
  room/tileset dispatchers, five safe textbox roots, and seven more endgame-20 entries; batch 66
  added the next six endgame-20 entries, the inventory-text first pass, and eight bank-1 warp and
  timewarp-solidity routines; batch 67 added nine late endgame-20 entries, ten bank-1 cutscene
  dispatcher/state entries, and eight bank-3 game-secret generation and validation routines;
  batch 68 completed the endgame-20 state list, opened the endgame-0f dispatcher, added seed-tree
  refill and toggle-block helpers, and expanded the bank-3 secret encoder/decoder path; batch 69
  added ten bank-4 VBlank copy handlers, the last eligible bank-1 tail routine, and the remaining
  bank-3 secret data loading, ID, and size helpers; batch 70 completed the first twenty bank-4
  VBlank copy handlers, added the dungeon-map item/cursor drawing cluster, and restored the
  endgame-09 root dispatchers; batch 71 added the next ten VBlank copies, the bank-2 map arrow,
  room-visited, warp-site/tree helpers, and the bank-1 toggle-block cutscene state machine; batch
  72 completed the bank-4 VBlank copy list, added tile substitutions, palette-fade handlers, and
  more bank-2 map helpers; batch 73 added the dungeon floor-list, scroll renderer, room-tile
  selector, and a late miscellaneous-cutscene graphics/effects cluster; batch 74 added the
  remaining safe palette-thread handlers and the full fading/color-mixing loops; batch 75 added
  the bank-2 minimap popup-selection and dungeon-map scrolling/menu-rendering cluster; batch 76
  added the safe forest-fairy and completed-fairy dispatcher/state paths; batch 77 added all safe
  Nayru-singing state entries while retaining the two room-loading states in generated C; batch
  79 added the bank-2 gale-seed menu, map-menu state flow, minimap room selection, dungeon key and
  floor bookkeeping, and room-text resolver cluster; batch 80 added the complete safe pregame-intro
  root and its thirteen-state dispatcher; batch 81 added the safe Black Tower explanation and
  completion states plus their shared graphics dispatcher; batch 82 began the final late-cutscene
  pass, and batches 83–86 completed the safe dungeon, timewarp, Black Tower, Flame of Sorrow, and
  Zelda-kidnapped state clusters. Batch 87 added the eighteen independently hookable standard-text
  states and exits; the thread-starting textbox root remains generated. Batch 88 completed the
  text-option and inventory-text state machines and removed their cursor-delay data label from the
  executable routine registry. Batch 89 added bank 1's row/VBlank support, palette-transition,
  following-Link, screen-reload, and object-enable helpers. Batch 90 completed a bank 2 file-select
  and text-input rendering/input cluster and removed twenty-two associated sprite/table data rows
  from the executable routine registry. Batch 91 added the bank 2 status/menu bodies, common-GFX
  loader, equipped-item and treasure-tile renderers, and the complete heart-display path.
  Batch 92 added bank 1's room-transition state machine, camera-position calculation, screen
  shake, scroll-register update, and room-darkening paths.
  Batch 93 completed the horizontal/vertical scrolling-transition state machine and its camera,
  Link-position, follower-reset, and transition-finalization helpers.
  Batch 94 completed the remaining vertical-scroll path and added object-category clearing,
  compass-sound selection, and shocked-Link animation handling.
  Batch 95 added the fall-through-floor warp, cutscene 17, wave-scroll initialization, and the
  safe leaves around cutscene 15's generated thread-switching core.
  Batch 96 added the bank-2 inventory-menu dispatcher, initialisation, item-text selection,
  equipping, and subscreen input paths.
  Batch 97 added the complete bank-2 ring-appraisal dispatcher, initialization, prompt,
  payment, appraisal, refund, redraw, and delayed-exit paths.
  Batch 98 added the safe bank-2 secret-list input, scrolling, cursor, text-selection, bulk-name,
  and secret-data lookup paths while retaining the thread-starting initialization in generated C.
  Batch 99 added the bank-2 ring-list selection, ring text, ring-box insertion/search, page-scroll,
  and cursor-movement state machine through its direction-offset table.
  Batch 100 completed the ring-menu rendering and selection helpers. Batch 101 added the Game Link
  menu state machine, Link/file rendering, save/quit input and sprite helpers, and fake-reset states.
  Batch 102 added the remaining safe file-select mode 1/3/5 state and text-speed menu paths.
  Batch 103 added inventory state dispatchers, directional input, ring-box traversal, and submenu
  cursor rendering. Batch 104 added the seed-and-harp submenu renderer, equipped-ring marker,
  stored-item grid, and subscreen-1 treasure/ring drawing paths; four adjacent sprite/position
  tables were removed from the executable routine registry. Batch 105 added the subscreen-2
  treasure and heart-piece renderer, tilemap rectangle/display helpers, and inventory/harp sprite
  rendering; two adjacent data rows were removed from the executable routine registry. Batch 106
  added the bank-4 toggle-block, underwater Jabu, shutter, chest, switch, and single-tile
  substitution paths; five embedded tile-data rows left the executable routine registry.
  Batch 107 added the remaining file-select mode helpers, blank submenu sprites, the dungeon-map
  tilemap and compass paths, and textbox initialization; five adjacent data rows left the
  executable routine registry.
  Batch 108 added bank-2 item/status/ring rendering helpers and bank-4 tile-write, changed-tile,
  interleaved-tile, and VBlank queue paths; the adjacent VRAM-address table left the executable
  routine registry.
  Batch 109 added bank-1 dungeon-layout loading, active-room discovery and minimap updates, plus
  bank-2 file and child-name entry state paths with explicit handoff to the deferred text-input
  engine.
  Batch 110 added bank-1 room-lighting, music, warp-position, and companion cleanup helpers,
  bank-2 text-input graphics setup, and bank-4 room-tile generation and replacement paths;
  removing the remaining embedded data rows made bank 4 fully readable and deleted its generated
  C file.
  Batch 111 added the bank-2 erase-file, file-select secret, and in-game secret-entry dispatchers
  and state paths, preserving their RST jump-table frames and deferred-input handoffs.
  Batch 112 completed bank 2's text-input navigation and selection state machine; its adjacent
  sound-effects table was removed from the executable registry.
  Batch 113 added the bank-2 menu transition/load dispatcher, save-and-quit and secret-list roots,
  and the file-select error/input wait tails.
  Batch 114 added the bank-1 palette roots and safe cutscene-15 dispatchers plus bank-2 text-copy
  and file-select bootstrap paths.
  Batch 115 completed bank 2's remaining normal DMG guard, status-bar, menu-exit, and save-reset
  roots; only the deliberately corrupted garbage tail remains generated in that bank.
  Batch 116 reproduced that twelve-entry corrupted tail literally, making bank 2 fully readable
  and deleting its generated C file.
  Batch 117 added bank 1's game-logic and initialization roots, standard-game-state dispatcher,
  room-loading continuations, and the remaining room-transition cutscene roots; bank 1 now has
  289 of 297 entries readable.
  Batch 118 completed bank 1's final real routines, removed three embedded data labels from the
  executable registry, and promoted the real `$34ad` Bank-0 continuation reached by corrupted
  code. Banks 0 and 1 are now both fully readable, and `gen_bank01.c` is deleted.
  Batch 119 completed banks 0C and 12, removed five companion-script bytecode labels from the
  executable registry, and added bank 3's remaining fairy-room and palace room-load paths with
  their thread continuations.
  Batch 120 completed the Nayru-singing, Black Tower explanation, and Nayru-to-Maku-Tree
  cutscene roots, promoted every executable nested dispatcher and thread-resumption point, and
  removed two generated rows that were actually inline data; bank 3 is now 570/631.
  Batch 121 completed the Black Tower completion, turn-to-stone, and Twinrova-reveal cutscene
  roots and state dispatchers, promoting eleven hidden thread-resumption points; bank 3 is now
  594/640. Review also established that a callable RST dispatcher must use `hook_continue` for
  its dynamic fallback so the enclosing C caller resumes after the interpreted state returns.
  Batch 122 completed the Timewarp, Ambi's Passage, Jabu-Jabu opening, linked-game, and Black
  Tower escape roots, promoted three thread-resumption points, and removed one data table plus
  three corrupted garbage labels from the executable registry; bank 3 is now 616/636.
  Batch 123 completed the startup initializer, intro-cinematic and endgame-20 roots, and the
  disable-LCD room loader, promoted seven thread-resumption points, and removed the ROM source
  copy of the HRAM OAM-DMA routine from the executable registry. Bank 3 is now fully readable and
  `gen_bank03.c` is deleted.
  Batch 124 completed the textbox thread dispatcher and the final two Phase-5 bank-10 cutscene
  roots, removing one text table and three internal-only bank-10 labels from the executable
  registry. Phase 5 is complete with 2,607 readable hooks out of 12,641.
  Batch 125 opened phase 6 with the complete bank-7 save-file verification helper layer and
  bank-3F object-GFX lookup/bookkeeping helpers, including all nine generated local entry points.
  The project now has 2,634 readable hooks out of 12,641.
  Batch 126 completed bank 7's file-management dispatcher and save/load/create/erase roots,
  continued bank-3F graphics allocation and interaction-data lookup, and added isolated treasure
  helpers from banks 3F and 16. Four zero-caller internal labels were absorbed into their readable
  parents, leaving 2,653 readable hooks out of 12,637.
  Batch 127 added the bank-6 parent-item, interactable-tile, and breakable-tile helpers, the first
  bank-7 collision effects, and the bank-3F ring inventory, treasure-display, and gasha-maturity
  paths. Three internal/data rows disappeared, leaving 2,680 readable hooks out of 12,634.
  Batch 128 completed the item-drop decision paths, added ten more bank-7 collision effects, and
  added palette refresh, weapon graphics, and the per-object tile-index-base helper. Sixteen
  item-drop table rows and seven internal labels left the executable registry, leaving 2,696
  readable hooks out of 12,611.
  Batch 129 added thirteen more collision effects, all twelve simple treasure-parameter modes,
  and eight interactable-tile helpers. Eight parent-owned tile locals disappeared, leaving 2,729
  readable hooks out of 12,603.
  Batch 130 completed `giveTreasure_body`, added twelve more collision effects and eight more
  interactable-tile handlers, and removed sixteen parent-owned data/internal rows. The project now
  has 2,757 readable hooks out of 12,587.
  Batch 131 completed `interactableTiles.s`, confirmed `treasureAndDrops.s` already had no generated
  routines left, added twelve more collision effects, and opened the frame-resuming bank-3F
  graphics roots with three stable continuations. The project now has 2,777 readable hooks out of
  12,587.
  Batch 132 completed `collisionEffects.s`'s numbered handlers and `parentItemUsage.s`, and added
  the next three bank-3F graphics roots with their frame-resume continuations. Eighteen
  parent-owned item locals, four graphics locals, and one collision data row left the registry;
  the project now has 2,802 readable hooks out of 12,564.
  Batch 133 completed `collisionEffects.s` and `loadGraphics.s`, added the first bank-7 common-item
  animation helpers and ten bank-6 item-parent bit/input helpers. Eight data/internal rows left the
  registry; the project now has 2,831 readable hooks out of 12,556.
  Batch 134 added the bank-7 common-item throwing, hazard, offset, and clink helpers, bank-6 child
  item allocation and Link-state tests, and the bank-3F monkey jump and disappearance helpers.
  Sixteen parent-owned locals and two sprite-data rows left the registry; the project now has 2,858
  readable hooks out of 12,538.
  Batch 135 completed `commonCode1.s`, added the remaining pre-minecart item-parent common helpers,
  and rewrote eight monkey disappearance/state roots. Twenty-six parent-owned locals disappeared;
  the project now has 2,878 readable hooks out of 12,512.
  Batch 136 added both bounded minecart collision helpers, eight seed state/animation helpers, and
  six more monkey dispatcher/state roots. Twenty-five parent-owned locals disappeared; the project
  now has 2,894 readable hooks out of 12,487.
  Batch 137 added the complete bank-6 minecart state body, completed the remaining Ages seed
  movement and gale-warp helpers, and added eight bank-3F rabbit dispatcher/state roots. Thirty-five
  parent-owned local rows disappeared; the project now has 2,908 readable hooks out of 12,452.
  Batch 138 added nine bank-5 special-object and tile-type helpers, completed the bank-7 bomb and
  boomerang clusters, and finished the real bank-3F rabbit roots with one durable post-graphics
  continuation. Sixty parent-owned local rows disappeared; the project now has 2,933 readable
  hooks out of 12,392.
  Batch 139 added the next ten bank-5 companion/collision helpers, the bank-7 sword-tile, damage,
  bomb-throw, and shared-bounce helpers, and the bank-3F rabbit-spawn/Tuni Nut state cluster with
  one durable continuation. Twenty-nine parent-owned local rows disappeared; the project now has
  2,959 readable hooks out of 12,363.
  Batch 140 added the remaining bank-5 companion mounting, hazard, and dismount helpers, the
  bank-7 sword, shovel, punch, sword-beam, dust, ore, and magnet-ball item roots, and the bank-3F
  monkey interaction root with its durable post-graphics continuation. Forty-nine parent-owned
  local rows disappeared; the project now has 2,978 readable hooks out of 12,314.
  Batch 141 added ten more bank-5 companion position, respawn, cliff, mounting, and spawn helpers,
  ten bank-7 item post-update and animation helpers, and the final six bank-3F executable entries.
  Twelve parent-owned helper/data rows disappeared; bank 3F is now fully readable and its generated
  C file is deleted. The project now has 3,004 readable hooks out of 12,302.
  Batch 142 added ten bank-5 companion animation and terrain helpers, ten bank-7 item post-update,
  bracelet, throwing, and slingshot roots, and ten bank-6 sword, shield, flute, seed, and shovel
  parent-item roots. Sixty-two parent-owned helper/data rows disappeared; the project now has 3,034
  readable hooks out of 12,240.
  Batch 143 added the last companion and early warp helpers in bank 5, ten bank-7 Bombchu movement
  helpers, and ten more bank-6 boomerang, bomb, bracelet, feather, switch-hook, cane, and slingshot
  parent-item roots. Thirty-four parent-owned local rows disappeared; the project now has 3,064
  readable hooks out of 12,206.
  Batch 144 added ten bank-5 warp-transition and Link-direction roots, ten bank-7 Bombchu,
  switch-hook, and item roots, and ten bank-6 Link/companion cutscene helpers. Thirty-four
  parent-owned local rows disappeared; the project now has 3,094 readable hooks out of 12,172.
  Batch 145 added seven bank-5 Link-state and warp roots, eight bank-6 Link cutscene helpers, and
  four bank-7 companion-attack item roots. Forty-eight parent-owned local rows disappeared; the
  project now has 3,113 readable hooks out of 12,124.
  Batch 146 added ten bank-5 Link-state and warp roots, ten bank-6 Link cutscene state machines,
  and the final seven bank-7 item roots. One hundred forty-nine parent-owned local/data rows
  disappeared; the project now has 3,140 readable hooks out of 11,975.
  Batch 147 added the remaining bank-7 collision roots, ten bank-5 Link states, and ten bank-6
  companion/raft/breakable-tile roots. One hundred sixteen parent-owned rows and five file-data
  labels disappeared; bank 7 is now fully readable and its generated C file is deleted. The
  project now has 3,165 readable hooks out of 11,854.
  Batch 148 added the complete Maple and Ricky state-machine clusters, the final bank-6 Link
  cutscene root, and five bank-5 Link movement/wall helpers. Fifteen parent-owned local rows
  disappeared; bank 6 is now fully readable and its generated C file is deleted. The project now
  has 3,191 readable hooks out of 11,813.
  Batch 149 added ten Maple collection/drop helpers, ten Ricky state-5/state-8 movement and hazard
  helpers, and ten bank-16 serial packet helpers. Thirty-six parent-owned local/data rows
  disappeared; the project now has 3,221 readable hooks out of 11,777.
  Batch 150 added the remaining Maple encounter states, ten Ricky cutscene states/helpers, and ten
  bank-16 serial control/dispatcher roots. Twenty-four parent-owned local/data rows disappeared;
  the project now has 3,251 readable hooks out of 11,753.
  Batch 151 completed all remaining bank-16 serial/game-link roots and the changing-floor pattern
  loader. Three parent-owned local rows disappeared; bank 16 is now fully readable and its
  generated C file is deleted. The project now has 3,282 readable hooks out of 11,750.
  Batch 152 completed the remaining Maple and Ricky generated helpers and opened the Moosh state
  machine. Twenty-eight parent-owned local rows disappeared; the project now has 3,312 readable
  hooks out of 11,722.
  Batch 153 completed Moosh, opened Dimitri through its held/movement substates, and made transformed
  Link readable. Forty parent-owned local rows disappeared; the project now has 3,340 readable
  hooks out of 11,682.
  Batch 154 completed every remaining Dimitri state/helper and the link-riding-animal state machine.
  Eighteen parent-owned local rows disappeared; the project now has 3,365 readable hooks out of
  11,664.
  Batch 155 added Link's overworld swimming, diving, velocity, jumping, animation, speed, state,
  and position helpers plus the companion mounting-completion helper. The callable mermaid-suit
  velocity continuation at bank 5 `$58f7` became a durable hook, and thirty-six generated local
  rows disappeared; the project now has 3,396 readable hooks out of 11,628.
  Batch 156 completed bank 5 with Link's remaining sidescrolling movement, cliff, hole, position,
  state, and immobilization helpers. Thirty-two generated local/data rows disappeared; bank 5 is
  now fully readable and its generated C file is deleted. The project now has 3,409 readable hooks
  out of 11,596.
  Batch 157 opened bank 8 with Impa's cutscene state, movement, animation, stone, and screen-exit
  helpers. Twenty generated rows disappeared; the project now has 3,432 readable hooks out of
  11,576.
  The merged parallel bank-8 work brought that bank to 53 readable hooks out of 678. Batch 159
  opened bank 9 with bear, bird, ghost-Veran, NPC game-progress, and worker helpers. One hundred
  twelve generated local/data rows disappeared; the project now has 3,481 readable hooks out of
  11,264.
  Batch 160 continued bank 9 with shopkeeper, intro-sprite, sword, rabbit, explosion, and
  Z-oscillation helpers. Forty generated local rows disappeared; the project now has 3,511
  readable hooks out of 11,224.
  Batch 161 added shop-item, soldier, and seasons-fairy interaction state helpers. Thirty-seven
  generated local rows disappeared; the project now has 3,528 readable hooks out of 11,187.
  The merged completed bank-8 branch makes every remaining bank-8 entry readable and removes its
  generated file; the project now has 3,684 readable hooks out of 10,718. Batch 162 continued
  bank 9 with the shop-item purchase state and the palace-soldier substate/escort helpers; 25
  parent-owned generated rows were absorbed, leaving bank 9 at 83/721 and the project at 3,701/
  10,693.
  Batch 162 opened the bank-10 sweep (a separate branch/session from banks 8-9) with the shared
  enemy-common knockback, hazard, and hole/delete helpers from `commonCode.s`. Four generated
  local rows (the shared checkHazardsCommon sub-blocks and the fallingInHole center-of-hole
  helper) disappeared with their now-readable parents; the project now has 3,699 readable hooks
  out of 10,714. Batch 163 continued with the velocity/adjacent-walls helper chain (including the
  RST $10 add-A-to-HL vector and the two-check tile-collision local); the project now has 3,714
  readable hooks out of 10,707. Batch 164 finished the bounce/spawn/counter helper cluster; the
  project now has 3,726 readable hooks out of 10,705. Batch 165 finished the remaining `ecom_*`
  helpers (angle, position, kill, gale-seed, and fall-to-ground), completing the whole
  `commonCode.s` file for bank 10; the project now has 3,747 readable hooks out of 10,701.
  Batch 166 opened `commonBossCode.s` with the boss-enemy shared helpers (`enemyBoss_dead`,
  `spawnShadow`, `initializeRoom`/`initializeRoomWithoutExtraGfx`, `beginMiniboss`/`beginBoss`);
  the project now has 3,753 readable hooks out of 10,700. Batch 167 added the first two
  `object_code/common/interactions/` files, `eraOrSeasonInfo.c` and `ringHelpBook.c`; the project
  now has 3,755 readable hooks out of 10,690. Batch 168 ported the whole
  `object_code/ages/enemies/kingMoblinMinionMain.s` (new file `kingMoblinMinionMain.c`), the
  King Moblin minion's 10 states plus its shared animate tail, including a local RST $18
  add-double-index vector and two private per-subid data tables mis-decoded as code; the project
  now has 3,766 readable hooks out of 10,684. Merging that work with the bank-9 batch gives the
  project 3,783 readable hooks out of 10,659. Batch 163 continued bank 9 with the forest-fairy
  subid-0 dispatcher/states, two small NPC dispatchers, and the five soldier tail labels; the
  project is now 3,799/10,639, with bank 9 at 99/701. Batch 164 completed the remaining safe
  Forest Fairy state and NPC entries, bringing the project to 3,817/10,631 with bank 9 at 117/693.
  Batch 165 added the Past Old Lady root and collapsed pre-existing bank-9 Ghost Veran, bear, and
  bird hook dispatches into their readable source helpers, bringing the project to 3,818/10,623
  with bank 9 at 118/685.
  Batch 166 completed the Ambi interaction dispatcher and its state/subid/script helpers, bringing
  the project to 3,831/10,597 with bank 9 at 131/659.
  Batch 167 completed the Subrosian and Impa NPC interaction dispatchers, their state/script paths,
  and their private return/lookup tails, bringing the project to 3,854/10,566 with bank 9 at
  154/628.
  Batch 168 completed the Tokay dispatcher, thief, participant, item/accessory, script, and
  stink-bag-cutscene paths, bringing the project to 3,886/10,524 with bank 9 at 186/586.
  Batch 169 completed the interaction-3F dispatcher and all of its subid/state/script tails,
  bringing the project to 3,887/10,504 with bank 9 at 187/566.
  Batch 170 completed interactions 51–54 and their private dispatcher-reachable state/script
  paths, bringing the project to 3,891/10,479 with bank 9 at 191/541.
  Batch 171 completed the interaction-60 treasure dispatcher and its private state-machine/helper
  paths, bringing the project to 3,892/10,440 with bank 9 at 192/502.
  Batch 172 completed the Mamamu Dog subid state machines and movement/target helpers, bringing
  the project to 3,902/10,415 with bank 9 at 202/477.
  Batch 173 completed interaction 55's dispatcher and private script/graphics path, bringing the
  project to 3,903/10,410 with bank 9 at 203/472. Interactions 58–5C remain generated after the
  combined batch exposed two separate full-replay divergences; interaction 58's first divergence
  is a stack/control-flow difference during its subid initialization.
  Batch 174 completed interaction 61's lever state machine and private helper paths, bringing the
  project to 3,904/10,387 with bank 9 at 204/449.
  Phase 0 done: `tools/gen_ram.py` (1,810 named RAM labels), `src/hooks/rewritten.txt` and
  `<name>_hook` shims in the generator, `--report` readiness reports, `tools/lint_game.py`,
  `setCpuToDoubleSpeed` hand-written (the last interpreter use that was there by design).
  Phase 1 batches 1 to 3: memory, math, flags, RNG, LCD/VRAM setup, object counters,
  visibility, room flags, table searches, consumable counters, object positions.
  Three rules came out of it, all in the porting notes: cycles are burned from the ROM's
  instruction stream (`CYC`, `src/game/cyc.c`) so interrupt dispatch keeps its instruction
  boundary; memory accesses follow the burn of their instruction; the state hash and verify
  skip the thread stacks and the sound driver's timing-phase state, `tas/ages.ref` re-recorded
  from the interpreter, frame hashes checked on `--no-hooks` only.
- Milestone 2 done at commit `cc4602c`: every labelled routine runs as generated C (13,235 hook
  entries); the interpreter executes 7,314 instructions in the whole movie; verify mode, whole
  movie state and frame hashes, and all suites green.
- Milestone 1 done: the core plays the entire console-verified movie (289,518 frames, about 80
  minutes) in sync with GBHawk; reference hashes in `tas/`.
- The playable build works: `cmake --build build-sdl && ./build-sdl/oracles "roms/<ages>.gbc"`.

## How to test

    cmake -S . -B build -G Ninja && cmake --build build
    ctest --test-dir build                      # Blargg, cgb-acid2, unit tests, first 20k TAS frames
    TAS_FRAMES=289518 ctest --test-dir build -R tas   # whole movie against tas/ages.ref (3 min)
    ./build/oracles-run --rom "roms/<ages>.gbc" --boot roms/cgb_boot.bin \
        --init-ram tas/gbhawk-wram0.txt --tas tas/ages-consoleverified.inputs --probe

`--probe` prints each new (group, room) the game reaches with the frame number. `--dump FILE`
writes the same per-frame key bytes and 60-frame WRAM hashes as the GBHawk Lua dump, so
`tools/compare_dumps.py` can diff a run against the reference emulator.

## Timing rules matched so far (all verified against SameBoy and GBHawk with micro test ROMs)

- Speed switch: 32771 M-cycles from the `stop` fetch to the next instruction (SameBoy and GBHawk
  agree), DIV reset at entry, PPU skips 12 dots, timer keeps running.
- GDMA/HDMA: 16 M-cycles per 16 bytes in double speed. HBlank DMA requested while the LCD is off
  transfers one block immediately (not yet implemented, the game never does this).
- LCD enable: first line is 452 dots and starts in mode 0 (no OAM scan).
- Every other line: STAT keeps the previous line's mode 0 and LYC flag for 2 dots, mode 2 runs
  from dot 2 to 82, mode 3 from 82, mode 0 from 82 + mode-3 length (172 + SCX%8 + window + object penalties).
- LY reads 0 during line 153 after its first 4 dots.
- VBlank and LYC interrupt lines rise at dot 0 of the line, two dots before STAT shows it; leaving
  HALT for an interrupt costs one extra M-cycle.
- Object penalties: 6 dots per object plus a per-block scroll penalty; none on the first line after
  LCD enable and none for X >= 168 (30 micro-test variants agree with SameBoy and GBHawk).
- Interrupt dispatch from HALT and from running code: 5 M-cycles, identical to hardware.
- Boot: the real CGB boot ROM with GBHawk's 13-byte GBA patch, WRAM/HRAM initialised from a GBHawk
  frame-0 snapshot (`tas/gbhawk-wram0.txt`).

## Open problem

- Verify mode reworked (2026-09-09, evening): only routine-level entries are verified, nested
  verification samples the first 4 calls then every 256th, and interrupt dispatch is suppressed
  on both sides of a verification. 3,000 frames take 8 s again (1,500 did not finish in seven
  minutes before).
- Milestone 2: every routine the disassembly labels as code is hooked: 13,235 entries (routine
  labels, local labels, and synthetic entries after thread switches, after calls into
  thread-switching routines, and at cross-routine jump targets) generated by `tools/transliterate.py` into `src/game/gen_bankXX.c`, plus
  `src/game/ram_code.c` for the one piece of dynamic RAM code. On the first 20k frames the
  interpreter executes 1,114 instructions where it executed 51.6M before today; the whole movie
  interprets 7,314 instructions in total (`PCHIST` profile). Gates: whole-movie replay with state and frame hashes,
  routine diff (`--verify-hooks-continue`), all suites. Still interpreted: `stop` routines,
  routines the generator skips for illegal opcodes (data mislabelled as code), and the
  instruction sequences of unlabelled code the profile has not reached yet. The C is a
  cycle-annotated transliteration (gotos for jumps); the idiomatic rewrite comes after 100
  percent, as the design says. Order was `docs/plans/2026-09-08-m2-routine-porting.md`, then the
  profile.
- LCD transition timestamps now match GBHawk with a constant 1 M-cycle offset over the first
  12,000 frames (was drifting by up to 94).

## Fixed today

- Entry points for local labels, after `ld sp`, after calls into thread-switching routines and
  at cross-routine jump targets; hand-off continues inline until the frame's own return; hooks
  for the interrupt vectors and for code running from HRAM/WRAM (`src/hooks/extra.sym`);
  `tas/ages.frames` re-recorded (it predated the exact-cycle sampling that `ages.ref` uses).
- Kernel batch (halt/ei/reti in C): the dispatcher pushed a stale PC for a second interrupt
  serviced at the same boundary; the generator followed a bank-switching `jp` into bank 0 bytes;
  verify mode hung on thread switches inside interpreter call-outs. All three recorded in the
  porting notes.
- Long C routines (cutscene loops) ran past the 16-entry sample ring and dropped frames from the
  frame-hash stream, which showed up as a frame-hash mismatch at frame 9877 with identical
  screenshots. Frame samples are now delivered through `gb->frame_cb`, from `gb_run_frame` and
  from `gb_tick` when the ring is full; the headless runner and the TAS test consume samples in
  the callback.
- The generator followed `call cc` targets as reachable code, decoding switchable-bank callees as
  bank 0 bytes; `updateAllObjects` had silently dropped out of the ported set because of it.
- `tools/bisect_hooks.py` bisects a replay failure to one routine with `HOOK_ONLY`.
- Cartridge SRAM must start filled with 0xFF (GBHawk, and most flash carts). Ours started zeroed,
  so the save-file checksum check took a different branch in each of the six save-slot scans and
  cost 16 M-cycles more per scan. That was the entire "drift per loading window".
- GDMA costs one extra M-cycle per transfer on top of 16 per block (double speed).
- LYC STAT interrupt line now rises at dot 0 of the line (was 2 dots late), which shifted the
  game's HBlank poll loop by one 7-cycle iteration on every mid-frame register update.
- STOP stall was 1 M-cycle too long (32770 instead of 32769); the old micro test counted base
  cycles across the switch.

## Tooling

- `tools/lockstep.c`: runs our core and SameBoy on the same inputs, diffs WRAM per frame.
- `tools/make_timing_roms.py`: generates the micro test ROMs in `tests/timing/`.
- `tests/timing/*.c`: harnesses that print per-cycle PPU/interrupt timelines from our core.
- BizHawk runs headless in Docker (see `docs/porting-notes.md` once written up) with Lua scripts in
  the scratchpad that dump memory or PC timestamps per frame.

## Done

- 2026-09-21: Seasons hooks 3,002 to 3,212 and the SAME_SHAPE list closed (313 of 315 resolved;
  `cutscene11` calls routines that only Seasons names, `func_39_400c` is garbage code).
  `routine_equiv` resolves a bank-0 routine's cross-bank references through the bank it switched
  to (`ld a,N` + `hRomBank`/`$2222`, `ld e,N` + `interBankCall`), and an unresolved `$xxxx` on one
  side matches the other side's label when this game has that label at the address
  (`peer_sym`): 3,113 to 3,180 IDENTICAL. `gameconst` finds constants burned by helpers
  (`bank_push(gb, b_+N, bank)`, shared bodies), takes a same-valued constant on faith, and merges
  `src/hooks/seasons_ok_manual.txt` (hand-checked routines whose GV lives in a shared helper).
  `TAIL_GV(ages, seasons)` for a `jp` whose target differs per game (`mapMenu_drawSprites`);
  a Seasons-only label goes into `syms_used.txt` with `-` for the Ages address. Selector rules
  added after the verify caught them: a `@local` reached only by `call` is compared on its own
  (`loadRoomLayout@loadLayoutData` differs, so `loadRoomLayout` is out); a `TAIL` target must
  exist in Seasons (`showText` tail-calls `_label_00_203`, unpairable, so it is out).
  `guard_tailcalls` now guards a trailing `x_hook(gb);` before `}` (1,019 more sites). Latent
  Ages bugs fixed on the way: a conditional `ret` burned twice (four `ecom_updateAnimationFromAngle`
  copies, raft, timewarp, twinrova, credits; `tools/audit_burns.py`), and 14 range ends spelled
  from a label that only follows the range in Ages (`tools/audit_cyc.py --fix`, e.g.
  `parseGivenObjectData_hl` burning into `objectDataOpcodeSizes`), four `GV(GV(..))` leftovers.
  `tools/verify_trace.py` turns a BURNLOG/PCTRACE/VERIFYLOG run into the first differing
  instruction of a cycle mismatch. Gates: Ages ctest 8/8, whole movie, verify 30k; Seasons demo
  and playthrough, `VERIFY_ALL` on the playthrough 0 failures apart from the lcdInterrupt skew;
  lint 0.

- 2026-09-21 (early): Seasons hooks 2,406 to 2,863. `tools/guard_tailcalls.py` turned 4,082
  direct tail calls (`x_hook(gb); return;` for a `jp`, and the `jt_ ==` jump-table branches that
  have an interpreter fallback) into `TAIL(x)` / `hook_enabled_at` guards, so a routine can be
  hooked in Seasons even when the routine it jumps to is not; `tools/seasons_hooks.py` accepts
  the guarded calls and pairs label copies by content (`symfiles.pair_instances`; `_label_XX_N`
  never pair, `func_XX_YYYY` do). The first version keyed the pairing by Ages address and
  several labels share one (`dungeonLayoutDataStart`, `enemyCode4d`, `linkState01`,
  `itemCode1e`, `tilesetLayoutGroup33` came out wrong), which broke the playthrough at frame
  32760 whatever the hook table said; keyed by name now. Bisecting that was misleading until the
  committed table was retested on the new sources (a ddmin over hook subsets is only sound when
  the empty set passes). `VERIFY_ALL` then showed `enemyCode09` three cycles short: `call z`
  written as `CALL_C` instead of `CALL_C_CC`; `tools/audit_calls.py` checks every call site's
  opcode against the ROM and fixed 12 such sites (octorok, rope, spark, ghini, bubble, blade
  trap, vire, patch), all latent in Ages. Debug aids added: `VERIFY_ONLY=name`, `BURNLOG=1`
  (one line per burned instruction, `src/game/cyc.c`), `WRAMDUMP_AT` now dumps VRAM, OAM, IO,
  palettes and banks. Gates: Ages ctest 8/8, whole movie clean, Seasons demo and playthrough
  clean, `VERIFY_ALL` on the playthrough 0 failures apart from the lcdInterrupt skew, lint 0.

- 2026-09-20 (night): milestone 5 plan step 1 item 7b, batch 7 (class C), commit `d185ce0` on
  `worktree-m5-step0` on top of main `e93deeb` (merged, generated files regenerated with
  `transliterate.py`, `gen_hooks.py`, `seasons_hooks.py`, `gen_syms.py`; tree matched main byte
  for byte afterwards). Two mechanisms were leaving code in the interpreter: a `CALL_C` whose
  target is a local label with a C body but no hook entry falls through to `asm_call`, and the
  `jp hl` dispatchers ended in `hook_handoff` (longjmp), which discarded their callers' loops.
  `CALL_L`/`CALL_L_CC` in `game.h` are `CALL_C` without the hook-table test; a script rewrote
  the 159 such sites (list produced statically from the C, the symbol file and `table.h`).
  `updateEnemy`, `updateInteraction`, `jpHl`, `jpBc` and the item dispatcher now use `HANDOFF`.
  Gates as before, all green; Seasons hooks 2,406 unchanged (2,857 after the merge with the guarded-tail-call table: `seasons_hooks.py` now compares the call target's body for every `CALL_L` site and drops the six whose local differs in Seasons, `updateItems`' item trampoline among them). Interpreted instructions per movie
  3,896,076 to 3,801,186. Of the remainder, 3.75M is the six thread loops and about 45k is
  loop tails after a thread switch inside an interaction (about 870 occurrences per movie in
  each of `updateInteractions`, `checkSpawnTimeportalInteraction`, `_updateEnemiesIfStateIsZero`
  and `_updateInteractionsIfStateIsZero`); both are class B and end with fibers. The RST vectors
  (587) and a long tail of one-off routine tails make up the rest. Class A and class C are done.

- 2026-09-20 (late): milestone 5 plan step 1 item 7b, batches 2 to 6, worktree branch
  `worktree-m5-step0` (commits `6a092d6`, `e9804b0`, `4a6528f`, `6c1a76b`, `badecee` on top of
  main `ed9f340`, merged with main first and generated files regenerated, never merged by hand).
  Every live `CALL_ROM`-to-local site is now a static C helper called directly: the tileset and
  room layout locals, the cliff and heart-ring locals, item post dispatch, the inventory text
  control codes, `handleTextControlCode` and its sixteen handlers (whose interpreter hand-off had
  been unwinding the callers' loops into the interpreter too), the engine-file jump-table locals
  (warp tile check, map popup, inventory and ring menu states, secret text, tileset override,
  object allocation, Symmetry Village flags), the interaction, item and item-parent locals, and
  the 32-way Tokay subid initialiser. What stays interpreted on purpose: `func_7f55` and the
  `fake_*` bodies in `bank02End.c`/`bank3fEnd.c` (garbage-region code calling into the middle of
  other routines). Interpreted instructions per movie: 6,830,645 after batch 1 to 3,896,076;
  96% of the remainder is the six thread loops (class B, deferred by request), the rest is
  class C (`interactionCode60`, `updateItems`, `updateInteractions`,
  `checkSpawnTimeportalInteraction`, the RST vectors they run) at about 140k. Gates after every
  batch: `--verify-hooks-continue` 30k frames 0 failures, whole movie clean against `ages.ref`
  and `ages-hooked.frames`, `ctest` 8/8 with `seasons_demo_matches_reference` and
  `seasons_play_matches_reference`, Seasons playthrough `--verify-hooks-continue` showing only the
  known 1-cycle `lcdInterrupt` LYC-write skew, `lint_game.py` 0. Seasons hook count: 1,953 before,
  1,952 after batch 4 and unchanged since: `handleTextControlCode_hook` now tail-calls
  `retrieveTextCharacter_hook` (SAME_SHAPE) and `textControlCodeC_1/2/6_hook` (not eligible)
  directly, so `tools/seasons_hooks.py` drops it and Seasons runs it interpreted as before. If
  that one hook matters, the direct tail calls could be guarded with `hook_enabled_at` and the
  selector taught to accept the guard; that is a `tools/` change. New labels went through
  `src/hooks/syms_used.txt` and `tools/gen_syms.py` (33 added). Lessons recorded in the inventory
  doc: an `.ifdef ROM_AGES` preamble can be guarded by a label-equality test when the tail is
  identical; a per-game bank immediate needs a shared label in that bank (`SYMBANK`); a
  `CALL_C` cannot sit in a non-void helper (its failure path is a bare `return`); a helper that
  the assembly reaches by `call` and by fallthrough gets the `ret_effect` in the callee and a
  `push_effect` at the call site.

- 2026-09-20 (late): Seasons per-game constants. `tools/gameconst.py` makes the SAME_SHAPE
  routines (same instructions, some constant differs) run under Seasons by evaluating each C
  site for both games and wrapping the wrong ones as `GV(ages, seasons)` (`GVW`/`GVH` for a named
  RAM byte); it also audits every identical routine's constants and restored 36 literals the
  symbolizer had turned into label+offset, only right for Ages. 291 of 382 resolved; the rest are
  a different callee or variable and are hand work. Found on the way: the readable style repeats
  a constant in helper arguments and the MBC write (fixed 7+3 sites), and the enemy wall code's
  `capSpeed` path burned the fall-through `ret c` (latent in Ages, all four bank copies).
  `VERIFY_ALL=1` verifies every nested call. Merged Fable's item 7b batches 2-6 (interpreted
  instructions per movie 6.8M to 3.9M; the tool is idempotent on their code now). 2,406 hooks run
  under Seasons; the playthrough verifies clean apart from the lcdInterrupt LYC skew; Ages full
  replay passes. Commits `57ff81d`, `8085e5e` and this one.

- 2026-09-20 (evening): milestone 5 spec and plan written to
  `~/Projects/agent-docs/oracles-decomp/{specs,plans}/2026-09-20-m5-native-engine*.md`, adopted
  with three review conditions (separate worktree, `lockstep_native` before the first native
  boot, inline data in jump-table routines handled before assets v1). Step 0 of the plan done in
  worktree `.claude/worktrees/m5-step0` (uncommitted): VBlank delivery-context counters in
  `hooks.c`/`cpu.c`, `oracles-ref` CMake target, `tas/ages-hooked.frames` (hooked build's frame
  hashes, the native renderer's reference), `tools/pc_sites.py`. Inventory in
  `~/Projects/agent-docs/oracles-decomp/specs/2026-09-20-m5-step0-inventory.md`. Findings: the
  interpreter still executes 87.9M instructions per movie (303 per frame) at 1,979 addresses in
  111 routines, 72% of them in `drawAllSpritesUnconditionally@drawObject` because
  `CALL_ROM_CC(b_+132, ROM_drawAllSprites_drawObject)` calls an unhooked local label; 32 static
  `CALL_ROM`-to-local sites account for 95%, the six thread loop bodies re-entered after a yield
  for 3.7%, and about 90 routines' unlisted jump-table locals for the rest. 9,359 VBlanks (3.3%
  of frames) are delivered mid-logic, so the movie has lag frames and the native engine keeps
  the faithful clock. Only seven `gb->` field kinds are used outside the register file. Next:
  plan step 1 item 7b, the 32 local blocks as static C, starting with `@drawObject`.

- 2026-09-20 (night): milestone 5 plan step 1 item 7b, first batch, worktree `m5-step0`
  (uncommitted on top of `75221a4`): the four largest interpreter-executed local blocks
  rewritten as static C helpers called directly. `drawAllSpritesUnconditionally@drawObject`
  (`draw_object`, `bank0.c`, the OAM data bank is `SYMBANK(itemOamData4cf40)` because
  `BASE_OAM_DATA_BANK` is $13 in Ages and $12 in Seasons), `func_4553@getLinkWalkingAnimation`
  (`specialObjectAnimationsAndDamage.c`; the Ages-only underwater preamble is guarded by
  `SYM(func_4553__getLinkWalkingAnimation) != SYM(func_4553__notUnderwater)`, since the labels
  coincide in Seasons and the tail is instruction-identical), `retrieveTextCharacter@func_18fd`
  (`retrieve_text_character_tile`, byte-identical in both games), and
  `updateSpecialObjects@updateSpecialObject` (`update_special_object`, an if-chain over the
  `rst_jumpTable` targets with `HANDOFF` for the Seasons-only entry). Gates: `--verify-hooks-
  continue` 30k frames 0 failures after each block; whole movie against `ages.ref` and
  `ages-hooked.frames` clean; `ctest` 8/8 with the Ages 20k and Seasons demo; `lint_game.py` 0.
  Interpreted instructions per movie: 87,944,533 to 6,830,645 (92% removed); the remaining
  6.8M are 49% `mainThreadStart`'s loop after yields, then `loadTilesetLayout`,
  `checkLinkJumpingOffCliff`, `loadRoomLayout`, `updateItemsPost`, `paletteFadeThreadStart`.
  Seasons `--verify-hooks-continue` (30k no-input frames) reports 17 `lcdInterrupt_hook` IF-register
  (io off=15) mismatches on this build and 34 on the main tree's `build/oracles-run`, so that
  class predates this batch and is unrelated to it. Four labels added to
  `src/hooks/syms_used.txt` (`itemOamData4cf40`, `func_4553__getLinkWalkingAnimation`,
  `func_4553__notUnderwater`, `updateSpecialObjects__updateSpecialObject`), `syms.*` regenerated.

- 2026-09-20: Oracle of Seasons, first day. The Seasons ROM boots and plays on the same C: after
  building `seasons.sym` (`make seasons` in the disasm; the user's dump differs from the build
  only in padding fill and the header checksum), `tools/routine_equiv.py` showed that almost
  every routine lives at a different (bank, address) in Seasons even when byte-identical, so
  `tools/symbolize.py` rewrote all 271k hard-coded addresses in `src/game/` to per-game symbols
  (`BASE(label)` + `b_+offset`, `SYM(label)`, `RAMSYM()` for the 290 RAM names that move,
  `switch_to_if.py` for the 667 jump-table switches; the Ages replay proved it behaviour-neutral),
  `tools/seasons_hooks.py` picks the hooks that are instruction-identical and only burn or call
  identical code, and `hooks.c` selects the hook and symbol tables from the ROM title. Bugs found
  on the way, all by the Seasons runs: per-file bank anchors (a shared routine's copy can sit in a
  different bank per game), `game.h`'s `ROM_*` defines, 2-digit `hram_wr` offsets and loop-bound
  addresses left literal, `CALL_C` targets voting for a function's base, `hook_enabled_at`
  ignoring the bank, `_label_XX_N` names that mean different code per game, and one latent Ages
  bug (`assignRandomPositionToEnemy` never burned its `ret`; the Ages movie never reaches it).
  Verification without a console movie: the 2012 VBA-rr TAS desyncs at file select even with the
  boot offset corrected (no boot ROM, different power-on RAM), so the SDL app got `--record`
  (boots like the headless runner, keyed by the core's frame counter, resumes) and the user
  recorded a 33,337-frame playthrough; `tests/test_tas.c` now replays it and an input-free
  30k-frame demo run against the interpreter's hashes. 1,953 hooks run under Seasons; the
  playthrough ends on the recorded state hash. Commits `bd7ce5a` `287113e` `f7a211f` `e9579e9`
  `df583ea`; `src/game/` was reorganised to mirror `object_code/{common,ages,seasons}/` earlier
  in the day (`e447dd0`).

- 2026-09-20: milestone 3 phase 6 batch 344, bank 15: **CLOSED OUT -- the last remaining large
  bank.** Four parallel agent clusters (A/B/C/D, ~90 routines each) ported all 360 remaining
  routines across 60+ new files, every one ending in the literal `15` per a blanket naming rule
  imposed up front after a spot-check found most NPC names already had files for other banks.

  Fixed a wide sweep of cross-bank collisions this wave: `shootingGallery_removeAllTargets`
  needed a `_b15` suffix (vs bank 08's copy); `writeFlagsTocddb`/`giveRingAToLink`/
  `goron_setLinkPositionAndDirection`/`checkEssenceNotObtained` needed `_hook`-suffixed forward
  declarations added across 12+ files spanning all 4 clusters plus the pre-existing `goron.c`
  (a bank-09 file with an embedded bank-0x15 section from an earlier batch); two same-file
  collisions (`oldMan15.c`, `miscPuzzles15.c`) needed manual recovery of routines a merge had
  silently dropped.

  Registering the full wave regressed the TAS replay twice. Both times, `HOOK_SKIP`-based
  bisection turned out to be fundamentally unreliable: `hooks_init()`'s skip filtering only
  controls which entry's index gets written into the scan-start shortcut, not whether that entry
  stays in the `hooks[]` array -- `lookup()` scans forward past a "skipped" hook to the next
  bank match regardless, so isolating one candidate by skipping it does nothing whenever any
  other bank shares its raw address, which is nearly universal in the 0x4000-0x7fff switchable
  range. Pivoted to **registration-presence bisection** instead (editing `rewritten.txt` directly
  and regenerating the pipeline, which changes real compiled code paths rather than a runtime
  lookup table) and found four real bugs this way:

  - `beginJump_hook` (shootingGallery15.c) and `impa_restoreNormalSpriteSheet_hook` (impa15.c)
    both used bare `OBJ_*` field offsets instead of `INTERACTION_BASE + OBJ_*`. Once this pattern
    was recognized, a sweep of every bank-15 file for the same mistake turned up 29 more
    instances across 9 files (`bipin15`, `blossom15`, `child15`, `doorController15`, `farore15`,
    `movingPlatform15`, `npcFacingHelpers15`, `oldMan15`, `vasu15`) -- one of which
    (`doorController15.c`) also had a wrong field name, not just a missing offset. All 31 sites
    confirmed individually against `--report` disasm output before fixing, and fixed in one pass.
  - `interaction6b_isLinkAtScreenEdge_hook` (interaction6b15.c) had an inverted `jr nc` branch --
    the classic flag-polarity bug, still the single most common mistake this whole project.

  Full bare-symbol audit: 478 non-alias bare `ages.sym` symbols, 105 are data tables and script
  bodies (confirmed by name pattern -- `*Script`, `*Script_body`, `*ObjectDataTable`, layout-half
  tables), 373 real routines -- matching `table.h`'s distinct hooked-address count for bank 0x15
  exactly (100% rewritten).

  Full gate passed: both builds clean, `lint_game.py` 0 problems, both ctest suites 8/8, full
  289,518-frame TAS reference replay passed (372s). **`gen_bank15.c` is gone -- zero
  `gen_bankXX.c` files remain anywhere in the codebase.** Corrected the stale 514/13 to the
  audited 373/373, and recomputed the top-level totals fresh from the current bank list rather
  than incrementing them: **totalHooks 6,641, rewrittenTotal 6,641** -- every hooked routine in
  the entire ROM is now hand-written, readable C.

- 2026-09-19: milestone 3 phase 6 batch 343, bank 11: audit correction plus one real dead-code
  fix. `docs/bank-map.html`'s 650/649 was stale, same pattern as every other bank audited this
  session -- the real total is 227, confirmed two ways: `table.h`'s distinct hooked-address
  count for bank 0x11, and a full bare-symbol audit (255 `ages.sym` bare symbols; 24 are data
  tables -- collision tables, offset tables, generic `table_XXXX` names, confirmed via
  garbage-opcode decode; 1, `func_11_7f64`, is genuinely dead code).

  `func_11_7f64` lives in `code/ages/garbage/bank11End.s` -- an explicitly-named garbage source
  file -- and the `--report` output confirms `callers: 0`. This is the exact same fake-padding
  pattern already confirmed for bank 0E's `fake1`/`fake2` (never added to `ported.txt` there).
  Unlike those, `func_11_7f64` had been sitting in `ported.txt` since much earlier in the
  project despite being unreachable garbage -- which is why `gen_bank11.c` never fully
  disappeared even though every real routine in the bank was already hand-written. Removed it
  from `ported.txt` (matching the established "never register confirmed dead/data content"
  precedent) rather than hand-translating fake code that nothing ever calls; `gen_bank11.c`
  vanished entirely as a result.

  Full gate passed: both builds clean, both ctest suites 8/8, full 289,518-frame TAS reference
  replay passed (374s). Corrected 650/649 to 227/227.

  **This correction, combined with the identical ones already made this session for banks
  0A/0D/0E/0F/39, also exposed that the top-level `totalHooks`/`rewrittenTotal` counters in
  `docs/bank-map.html` had themselves drifted out of sync with the sum of the per-bank figures
  shown in the grid** -- the running counters were being manually incremented batch-by-batch
  (`previous + delta`) rather than recomputed from the authoritative per-bank list, so small
  errors accumulated silently over many edits, the same class of staleness this whole run of
  corrections has been about. Recomputed both directly from the current 23-bank list rather
  than trusting the incremental counters: **totalHooks 6,782, rewrittenTotal 6,281** (the
  previous incrementally-tracked values, 9,646 and 6,281 respectively before this batch's
  banks-0A-through-39 corrections, undercounted actual progress by hundreds of routines in one
  direction and overcounted the total in the other). Going forward, these two fields should be
  recomputed from the bank list after every batch rather than incremented, to prevent the same
  drift recurring.

- 2026-09-19: milestone 3 phase 6 batch 342, bank 39: **CLOSED OUT.** This is the game's
  sound/music engine (`code/audio.s`, one cohesive source file), a different kind of bank from
  every enemy bank done so far. Of 389 bare `ages.sym` symbols, 340 are pure data -- `snd*`
  sound-effect definitions and `mus*` music-track pointers, bytecode consumed by the engine
  rather than executed as CPU instructions (confirmed via garbage-opcode decode) -- plus one
  deliberate crash stub (`nonExistentFunction`, literally commented "Call this if you want your
  game to crash" in the source). Only 49 bare symbols were real code, and all 49 were already
  identified and sitting in `ported.txt` from a much earlier pass, just never rewritten (still
  100% interpreted going into this batch).

  Two parallel agents split the 49 top-level routines (~123 addressable entry points before
  rewriting, once `@`-local sub-states are counted) into `audioEngine.c` (init/update/stop/
  silence family, 62 routines) and `audioEnginePlaySound.c` (channel-command dispatch plus the
  complex `playSound` state machine, 61 routines). Both chose to inline every `@`-local as a
  `goto` within its parent function rather than mechanically duplicating tails the way the
  generated interpreter does -- cleaner and matches this project's readable-C style better.

  Both agents' self-review caught an unusually high number of real bugs before handoff, using
  custom ground-truth address-coverage scripts cross-checked against the mechanically-accurate
  generated code (`gen_bank39.c`) rather than relying on visual inspection alone: missing
  untaken-branch cycle charges after several `if (cond) { CYCT(...); ...; return; }` blocks,
  one structurally-inverted function (`stopSfx_hook` had inverted flag polarity combined with
  the taken-cost macro on the wrong branch), several target-vs-physical-address CYC bugs,
  missing cycle charges for unconditional `jp` tail-calls found via a full address-coverage
  diff, and one genuine internal-call-needs-`push_effect` instance
  (`getChannelVolume__affectedByMusicVolume`, reached both by fallthrough and by a real `call`
  from `standardCmdChannel6`). Independent verification found only one false-positive flag per
  file (a `jp hl` 1-byte instruction the checker script's regex misparsed), zero real bugs
  missed -- a strong sign the self-review scripts these two agents wrote were unusually
  effective compared to most prior batches.

  One registration mistake was caught before it reached the build: an initial pass registered
  all 123 addressable entry points (matching the pre-rewrite generated-code table.h), when only
  the 49 top-level names needed registering since neither agent created separate hook functions
  for the `@`-locals -- `lint_game.py` caught it immediately (74, then 21, then 2 problems across
  three correction passes as prefix-based removal missed some). The last 2 (`initSound`,
  `playSound`) needed their `_b39` bank suffix restored after an overly-aggressive suffix-strip,
  since they collide with identically-named routines in other banks.

  Full gate passed: both builds clean, both ctest suites 8/8, full 289,518-frame TAS reference
  replay passed (379s). `gen_bank39.c` is gone. Final bare-symbol audit: 389 `ages.sym` bare
  symbols for bank 0x39; 340 are pure data or the one crash stub; the remaining 49 distinct
  hooked addresses in `table.h` match `rewritten.txt` exactly. `docs/bank-map.html` bank 39
  corrected from 123/0 (stale) to 49/49. Project total is 6,281/9,646.

- 2026-09-19: milestone 3 phase 6 batch 341, bank 0f: **CLOSED OUT.** Four parallel agents split
  bank 0f's ~419 non-`ecom_*` routines by address range and ported 17 new files: cluster A
  (5 files: the shared `enemyBoss_*` boss-common-code plus giantGhini, swoop, subterror,
  armosWarrior), cluster B (4 files: smasher, vireEnemy, anglerFish, blueStalfos), cluster C
  (5 files: pumpkinHead, headThwomp, shadowHag, eyesoar, smog), cluster D (4 files: smogEnemy,
  octogon, plasmarine, kingMoblin). Then mechanically sed-ported the shared `ecom_*` cluster (63
  routines) from the already-twice-fixed bank-0x0d copy in `enemyCommonCode.c`.

  Two agents correctly self-disambiguated real filename collisions before writing anything
  destructive: `vireEnemy.c` (bank 0f's real overworld `vire` enemy, `common/enemies/vire.s`)
  avoided the pre-existing, unrelated bank-0x0b `vire.c` (`ages/interactions/vire.s`); and
  `enemyCommonBossCode_b0f.c` avoided the pre-existing bank-0x10 `enemyCommonBossCode.c` (both
  are instances of the same shared `commonBossCode.s`, exactly analogous to the `ecom_*`
  cluster). One same-file race still happened despite this -- two clusters both independently
  targeted `smog.c` -- and was self-detected via the disk-change notification and resolved by
  renaming to `smogEnemy.c`, the same pattern as batch 337's `buzzblob.c` and batch 339's
  `pumpkinHead.c` incidents. One routine, `enemyCode74`, fell exactly on the cluster A/B address
  boundary; both agents correctly noticed it looked like "the other cluster's" and deferred it,
  so neither ported it -- caught at the build-link stage (`_enemyCode74_hook` undefined) and
  ported directly by the coordinating session.

  Independent verification beyond each agent's self-review found substantially more than usual:
  - **61 real CYC-boundary bugs, all from one cluster's agent, across all 4 of its files**: every
    `jr` instruction (conditional and unconditional) used its jump *target* address as the
    cycle-burn endpoint instead of the physical `from+2` -- the single most common bug class this
    whole project has repeatedly flagged, and the agent's own self-review explicitly claimed to
    have checked and found clean. The naive "next-listed-instruction" checker script produces
    both false positives (near jump-table boundaries) and false negatives (misclassifying
    `ld bc,$nnnn` as a 2-byte `ld b,$nn`, and `ld a,(nnnn)` as `ldh`'s 2-byte form) -- rewrote it
    to compute physical instruction length directly from the disassembled mnemonic text instead
    of address-diffing, which cleanly separated the 61 real bugs from a handful of script
    artifacts. All 61 auto-fixed once correctly identified.
  - A wrong-bank-suffix sweep: cluster D used bare `ecom_*_b0f`/`enemyBoss_*_b0f` (8 call sites)
    instead of the `_hook`-suffixed form `gen_hooks.py` actually produces once registered --
    fixed, matching the identical bug class from batch 339.
  - 5 alias-winner corrections in `rewritten.txt` (`octogon_subid0AboveWater_stateF`/
    `octogon_subid0_pauseMovement` removed as losing aliases of the already-registered
    `stateA`/`stateB` winners; `enemyBoss_*` bare names removed and re-registered with the
    correct `_b0f` bank suffix since they're cross-bank shared, not bank-0f-exclusive).

  **The full TAS replay then failed at frame 42120, and hunting it down exposed a real
  limitation in the hook-isolation debugging technique itself** (used successfully in batch 338
  to find the bank-0d `push_effect` bug): `src/hooks/hooks.c`'s `HOOK_SKIP`/`HOOK_ONLY` env-var
  filtering only controls which hook's table index claims `first_at[addr]` during
  `hooks_init()`, but `lookup()`'s forward scan still walks *every* bank's entry sharing that
  raw 16-bit address regardless of an individual entry's own skip status -- it only filters by
  `bank == gb->rom_bank` at lookup time. Since ROM addresses 0x4000-0x7fff repeat identically
  across every bank, isolating or skipping a hook by name has **no effect whenever another
  bank's hook shares its raw address and stays enabled** -- true for roughly 80% of bank 0x0f's
  476 addresses. This produced two confirmed false leads: `collisionEffect32_hook` (a long
  pre-existing, untouched bank-0x07 routine that happens to share address 0x4625 with
  `giantGhini_gotoState9_hook`) and `enemyBoss_initializeRoom_b0f_hook` (later proven, via a
  full sed-normalized file diff, to be 100% byte-identical to its already-shipped bank-0x10
  sibling -- not buggy at all, just address-colliding with bank 0x01's
  `screenTransitionState5Substate2__state3_hook`). A collision-aware re-bisection (computing and
  also skipping every other-bank hook sharing each candidate's address before testing) correctly
  localized two real, unrelated wrong-literal bugs:
  - `enemyBoss_initializeRoom_b0f_hook` wrote the enemy ID into `wScreenVariables` (0xcd00)
    instead of `wEnemyIDToLoadExtraGfx` (0xcc1d) -- a straightforward wrong-constant slip,
    corrupting a shared screen-flags byte that a sibling function reads 5 lines later to decide
    whether to force Link to walk into the room. The identical bug exists verbatim in the
    already-shipped bank-0x10 copy (`ramrock_state0`'s call path apparently never exercises a
    visibly different outcome there) -- fixed both, matching batch 338's precedent of fixing a
    shared dormant bug in both bank copies at once.
  - `giantGhini_gotoState9_hook` wrote the raw hex digits from a `SPEED_c0` label (0xc0) instead
    of the label's actual disassembled value (0x1e) -- this project's naming convention labels
    the *value* 0x1e as `SPEED_c0` (confirmed identical in ~20 other already-shipped files), and
    the wrong literal set Giant Ghini's charge speed roughly 6x too fast the instant it
    transitions out of its miniboss-intro sequence.

  Full gate passed after both fixes: both builds clean, both ctest suites 8/8, full 289,518-frame
  TAS reference replay passed (369s, independently re-verified). Final bare-symbol audit: 496
  `ages.sym` bare symbols for bank 0x0f; 17 are pure data tables (`*Vals`/`*Offsets`/
  `*Boundaries`/`*Boxes` naming, confirmed via garbage-opcode decode); 3 were genuine
  previously-missed real-code gaps (`octogon_subid0AboveWater_stateA`/`stateB`,
  `smog_state8_subid2`) caught by the pre-wave audit and ported by cluster C/D; the remaining
  476 distinct hooked addresses in `table.h` match `rewritten.txt` exactly. `docs/bank-map.html`
  bank 0f corrected from 0/810 (stale) to 476/476. **All 16 banks from 0x00 through 0x0f are now
  fully readable C.** Project total is 6,232/9,646.

- 2026-09-19: milestone 3 phase 6 batch 340, bank 0a: audit-only correction, no new routines.
  `docs/bank-map.html`'s 928/172 was stale (an earlier session's closure was never re-audited),
  same pattern found and fixed today for banks 0b/0c/0d/0e. `gen_bank0a.c` is already gone,
  confirming the bank was in fact fully closed already: of 211 `ages.sym` bare symbols, 9 are
  unregistered -- 6 are data tables, 2 (`interaction6b_subid0b`/`0c`) are harmless losing aliases
  of the already-hooked `interaction6b_subid0a`, and one (`companionScript_companionBarrierText`)
  is real code with `callers: 0` in the `--report` output -- genuinely dead, unreachable code,
  confirmed not worth porting. 195 distinct hooked addresses in `table.h` match exactly. Corrected
  928/172 to 195/195. Project total is 5,756/9,646.

- 2026-09-19: milestone 3 phase 6 batch 339, bank 0e: **CLOSED OUT.** Four parallel agents split
  bank 0e's ~506 non-`ecom_*` enemy/dispatcher routines by address range and ported 44 new files:
  cluster A (9 files, 115 routines: tektite, stalfos, keese, babyCucco, zol, floormaster, cucco,
  butterfly, greatFairy-the-enemy), cluster B (7 files, 132 routines: fireKeese, waterTektite,
  swordEnemies, peahat, wizzrobe, crows, gel), cluster C (20 files, 130 routines plus correctly
  identifying `orbMovementScript` as data, not code), cluster D (7 files, 113 routines plus
  correctly identifying all 15 `movingSidescrollPlatformScript_subid00`-`0e` as data-table byte
  streams that happen to decode as syntactically-valid-but-meaningless SM83 instructions -- not
  the "illegal opcode" case transliterate.py auto-skips, confirmed against the human disassembly's
  own `.db`/movement-script macros). Then mechanically sed-ported the shared `ecom_*` cluster (63
  routines) from the already-twice-fixed bank-0x0d copy in `enemyCommonCode.c`.

  Independently re-verified every cluster with the CYC-boundary/flag-polarity cross-check script
  before registering anything, beyond each agent's own self-review (which itself caught ~130+ real
  bugs across the four clusters, mostly the usual target-vs-physical-end CYC class). Found and
  fixed 5 more real bugs this pass:
  - Two double-charged-cycle bugs (`veranPossessionBoss.c`, `vineSprout.c`): a `CYC()` span was
    written to include a following conditional `ret`'s own byte, so its untaken cost got burned
    unconditionally by the span *and* its taken cost got burned again by `RET_TAKEN` when the
    branch was actually taken -- same failure family as bank 0d's missing-push bug (a real ROM
    instruction's cost silently double-counted or dropped), different mechanism.
  - A wrong-callee-name bug: two files (from different clusters) called a nonexistent
    `markEnemyAsKilledInRoom_b0e_hook` instead of the real shared fixed-bank
    `markEnemyAsKilledInRoom_b00_hook` (confirmed via `gen_bank0e.c`'s own already-generated call
    and other already-correct files like `candle.c`/`wallmaster.c`).
  - A cluster-D-wide naming convention bug: 75 call sites used bare `ecom_*_b0e` instead of the
    `_b0e_hook` suffix that `gen_hooks.py` actually produces once a routine is registered in
    `rewritten.txt` -- cluster D's agent matched the *pre-registration* generated-name convention
    instead of anticipating the post-registration one that clusters A/B/C correctly used.

  **Two file-name collisions, one already committed.** Cluster A's `greatFairy.c` (the bank-0e
  enemy) silently overwrote bank 0b's pre-existing interaction file of the same name from batch
  332 (`interactionCoded5_hook`, `greatFairy_initialize_hook`, etc.) -- both banks' human
  disassembly sources are independently named `greatFairy.s` in different directories, so the
  project's own "name the file after the .s file" convention collided. This had already been
  captured in a real commit (`bank 9 & 0b`) before it was noticed. Recovered the original content
  from an earlier `git stash` index commit (the working commit history didn't have it uncommitted
  at all, since the batch-332 work was staged but never committed) and renamed the enemy file to
  `greatFairyEnemy.c`. Separately, cluster C's `objectMovementScript.c` re-ported
  `objectLoadMovementScript_body`/`objectRunMovementScript_body` -- bank 0e's only 2 pre-existing
  rewritten routines before this batch -- overwriting them with a fresh (also correct) translation;
  benign since both are valid translations of the same ROM bytes, so the new version was kept.

  Also caught and corrected a self-inflicted registration bug before it reached lint: initially
  bank-suffixed all 490 non-`ecom_*` routine names in `rewritten.txt` (matching the `ecom_*`
  cluster's convention), but none of bank 0e's own enemy routines collide with any other bank's
  bare symbol names, so they needed bare (unsuffixed) registration to match what all four agents
  actually wrote in their `_hook` function names -- `lint_game.py` caught this immediately (490
  problems) since `transliterate.py --out` correctly refused to treat the wrongly-suffixed names
  as satisfied. Separately, 5 of the "490" turned out to be losing aliases
  (`ambiGuard_stateE`, `enemyCode49`/`4a`/`4c`/`5f`) that leaked into a cluster's assigned list
  because the original address-sorted split wasn't deduplicated by alias group first -- removed
  from `rewritten.txt` (their winners `ambiGuard_stateC`/`enemyCode3d`/`enemyCode41`/`enemyCode4d`
  were correctly registered).

  Full gate passed after all fixes: both builds clean, both ctest suites 8/8, full 289,518-frame
  TAS reference replay passed (373s). Final bare-symbol audit: 588 `ages.sym` bare symbols for
  bank 0x0e; 35 are pure data tables, movement-script bytecode, or garbage padding (confirmed via
  garbage-opcode decode and cross-reference against the human disassembly's data macros); the
  remaining 548 distinct hooked addresses in `table.h` match `rewritten.txt` exactly (490 enemy
  routines + 63 ecom_b0e routines - 5 deduped aliases = 548). `docs/bank-map.html` bank 0e
  corrected from 2/860 (stale) to 548/548. Project total is 5,733/9,646 (7,572 routines total
  after bank 0e's rewrites removed most of its own generated fallback code).

- 2026-09-19: milestone 3 phase 6 batch 338, bank 0d: **CLOSED OUT.** Ported the shared `ecom_*`
  enemy-common-code cluster (62 real routines plus `label_025_b0d`) into `enemyCommonCode.c`,
  mechanically via `sed 's/_b10/_b0d/g'` on the already-shipped bank-0x10 copy of
  `common/enemies/commonCode.s` -- confirmed byte-identical addresses/instructions across banks
  0x0d/0x0e/0x0f/0x10 via `--report` on multiple sample routines.

  Fixed a real translation bug present in both the new b0d copy and the previously-shipped b10
  copy: `ecom_applyGivenVelocityGivenAdjacentWalls`'s `@updateX` entry point loaded
  `ENEMY_BASE + OBJ_Y` when the disassembly (`ld e,$8c` at ROM address 0x41a1) requires
  `OBJ_X` -- dormant in bank 0x10 since its rare late-game bosses (Twinrova/Ganon/Veran's Final
  Form/Ramrock/King Moblin's minion) never took that branch; bank 0x0d's common overworld
  enemies hit it almost immediately. Also fixed 397 call sites across 38 already-ported bank-0d
  files that called the new `ecom_*_b0d` functions without the `_hook` suffix (build-breaking,
  caught immediately by the compiler).

  After those two fixes the full 289,518-frame TAS replay still diverged at frame 29100 with an
  identical state-hash mismatch. Bisecting the *entire* game's 5294 rewritten routines (not just
  bank 0d) by activating exactly one hook at a time (`HOOK_SKIP` set to everything else) and
  binary-searching, twice independently, both times converged on
  `ecom_applyGivenVelocityGivenAdjacentWalls_b0d_hook` alone as sufficient to reproduce the
  divergence -- confirmed the bisection method itself wasn't a generic artifact by checking that
  an arbitrary unrelated hook (`clearMemory_hook`) does *not* fail the same way in isolation. A
  manual line-by-line re-check of every `CYC`/branch/register in that function against the ROM
  disassembly found nothing wrong, and the `--verify-hooks-continue` per-call diff found zero
  mismatches anywhere in 30k frames -- because the bug wasn't in the function's own instruction
  translation, it was a missing stack push. Two internal calls to the `applySpeedComponent`
  helper (a genuine ROM `call $41e1` instruction) were translated as bare C function calls with
  no matching `push_effect()`, so the helper's own `RET`/`RET_TAKEN` popped a return address it
  was never given -- 100% deterministic, not a timing artifact, triggered whenever an enemy is
  wall-adjacent on both axes at once (both the Y-branch and X-branch call sites hit in the same
  invocation). This is exactly the same failure mode the porting notes already document for
  manual RST-helper calls; the fix follows the identical pattern: `push_effect(gb, <return
  address>)` immediately before each of the two calls, applied to both the b0d and b10 copies.

  Full 289,518-frame replay now passes clean. Final bare-symbol audit: 804 `ages.sym` bare
  symbols for bank 0x0d; 474 are pure animation/OAM-data tables (confirmed via garbage-opcode
  decode) or the already-known losing `enemyCode20` alias; the remaining 327 distinct hooked
  addresses in `table.h` are all now hand-written, matching `rewritten.txt`'s count exactly
  (265 from batch 337 + 62 from this batch). `docs/bank-map.html` bank 0d corrected from
  265/675 (stale) to 327/327. Project total is 5,187/9,646.

- 2026-09-19: milestone 3 phase 6 batch 337, bank 0d (265 routines, 27 new files + 1 extension):
  started the "common enemies" sweep with three parallel agents. Audited the raw 804 bare-symbol
  count for this bank down to ~274 real routines: ~62 entries turned out to be false candidates
  (see below) and ~468 are pure animation/OAM-data-pointer tables (`enemyNNAnimations`,
  `enemyAnimationXXXXX(Loop)`, `enemyXXOamDataPointers`) -- the first filtering pass only caught
  the plain `enemyAnimationXXXXX$` pattern and missed the `Animations$`/`Loop$` suffixed variants,
  corrected before dispatching agents so none of the three wasted time trying to port garbage.

  Cluster A (17 new files: riverZora, octorok, boomerangMoblin, leever, moblinsAndShroudedStalfos,
  arrowDarknut, lynel, bladeAndFlameTrap, rope, gibdo, spark, whisp, spikedBeetle, bubbleEnemy,
  beamos, ghini, buzzblob) also extended the existing bank-0x10 `enemyCommonCode.c` with
  `label_025_b0d` -- `common/enemies/commonCode.s` assembles identically into 4 different banks
  (0d/0e/0f/10), and only the bank-0x10 copy of this one small routine existed before. Cluster B
  (10 files: buzzblob, sandCrab, spinyBeetle, gopongaFlower, armos, fish, polsVoice, likelike,
  dekuScrubEnemy, wallmaster) and cluster C (12 files: podoboo, giantBladeTrap, cheepCheep,
  podobooTower, thwimp, thwomp, veranSpider, eyesoarChild, ironMask, veranChildBee,
  anglerFishBubble, enableSidescrollDownTransition) both wrote `buzzblob.c` independently (my
  split guessed wrong about where the enemy boundary fell); cluster A's later, more complete
  rewrite superseded cluster B's partial one on disk, and both were verified byte-for-byte correct
  before/after, so nothing was lost.

  Verified all three clusters primarily with a small Python script (written this batch, reusable
  for future large-scale waves): it diffs every `CYC`/`CYCT` "to" address in a C file against the
  real next-instruction address from a `transliterate.py --report` dump, plus a flag-polarity
  cross-check against `//` mnemonic comments. Found and fixed 5 real target-vs-physical-end CYC
  bugs this way that manual reads had missed (2 in `spinyBeetle.c`, 1 in `polsVoice.c`, 2 in
  `wallmaster.c`); the agents' own self-review caught 2 more mid-flight. The script also throws
  occasional false positives at jump-table/`rst` boundaries (where the "next listed instruction"
  in the report isn't physically contiguous) -- every flagged case was individually confirmed
  against the report before deciding real-bug vs false-positive.

  Two alias groups needed a `ported.txt` correction, not just a `rewritten.txt` one: `enemyCode0c`
  (first-declared, wins over `enemyCode20`/`enemyCode22`) and `bladeTrap_subid01`/`bladeTrap_subid03`
  (win over `02`/`04`) were missing from `ported.txt` entirely while their losing siblings were
  already listed there -- confirmed via `.s` source line numbers and added the correct winners.
  Two lint failures from helpers that check `gb->pc`/`gb->sp` internally but weren't named with
  the bare `void name_hook(GB *gb)` signature the linter's hook-detector regex requires (no
  `static`, no extra parameters) -- `beamos.c` and `bubbleEnemy.c`'s single-caller angle/direction
  helpers, fixed by dropping `static` and adding the `_hook` suffix.

  **Bank 0d is not yet fully done even after this wave.** After registering all 265 routines,
  `gen_bank0d.c` shrank from ~19,300 lines to 1,453 but did not disappear: the shared `ecom_*`
  enemy-common-code cluster (`common/enemies/commonCode.s`, ~76 routines) still needs its own
  bank-0x0d pass -- only bank 0x10's copy of this cluster was ever rewritten (in the same
  `enemyCommonCode.c` file, `_b10`-suffixed), and every one of this batch's new enemy files calls
  into the still-generated `_b0d`-suffixed versions correctly but doesn't replace them. Queued as
  the immediate next wave, using the existing bank-0x10 implementation as a structural template
  since it's the identical source re-assembled at different addresses. Full gate passed: both
  builds clean, both ctest suites 8/8, 30k-frame verify-hooks-continue clean (same final state
  hash as before the batch), full 289,869-frame TAS reference replay passed. `docs/bank-map.html`
  bank 0d count updated to 265/675 (675 not yet re-audited, likely still stale given the pattern
  in every other bank this session). Project total is 5,125/9,646.

- 2026-09-19: milestone 3 phase 6 batch 336, bank 0b: **bank 0b is now fully readable C --
  `gen_bank0b.c` no longer exists.** Third and final wave of the sweep, two parallel agents, 20
  new files. Cluster C: `troy.c`, `linkedGameGhini.c`, `plen.c`, `masterDiver.c`, `dekuScrub.c`,
  `leverLavaFiller.c`, `slateSlot.c`, `syrupCucco.c`, `businessScrub.c`, `companionTutorial.c`.
  Cluster D: `faroreGiveItem.c` (plus the `interactiond9_*` cluster in the same file),
  `movingSidescrollPlatform.c` (plus `func_7fa1` and `movingPlatform_stateC`),
  `movingSidescrollConveyor.c`, `endgameCutsceneBipsomFamily.c`, `creditsTextHorizontal.c`,
  `creditsTextVertical.c`, `gashaSpot.c`, `horonDogCredits.c`, `zelda.c` (plus `zelda_state0`/
  `zelda_state1`/`zelda_loadScript`), `makuSeedAndEssences.c`.

  Process note: cluster C's dispatcher agent tried to fan out one fork per cluster item; every
  fork inherited the full batch and all 10 attempted all 10 items concurrently for the whole
  ~30-minute run, racing on the same 10 output files. Its own final self-audit claimed a clean
  result, but rather than trust a self-report from a run with that much concurrent-write risk,
  all 10 files were independently re-read and re-verified from scratch against
  `transliterate.py --report`. That paid off: found and fixed a real ordering bug in the shared
  `rst $18` helper (`SET_BC(pop_effect(gb))` was sequenced before its own cycle burn instead of
  after, present identically in `troy.c`, `linkedGameGhini.c`, and `plen.c` -- almost certainly a
  symptom of the race corrupting a shared template) and a real wrong-callee bug in
  `faroreGiveItem.c` (`bombUpgrade` called `createTreasure` directly instead of the ROM's actual
  `createTreasureAndIncSubstate`, silently skipping a required substate increment and counter
  reset -- confirmed against the raw `.s` source's `call @createTreasureAndIncSubstate`).

  A mechanical script (diff every `CYC`/`CYCT` "to" address against the report's real next-
  instruction address, plus a flag-polarity cross-check against `//` mnemonic comments) was
  written this batch and run retroactively across every file in both clusters. It caught two more
  real bugs a manual read of `gashaSpot.c` (414 instructions) had missed: one target-vs-physical-
  end address bug and, on the very same line, a `jr nc` branch coded with the wrong polarity
  (`if (F & FC)` instead of `if (!(F & FC))`), which together had silently inverted the gasha
  spot's maturity-based treasure-spawn logic. All files pass the script clean after fixes.

  Three lint failures surfaced after registering, all in the "manual CALL_C-style" helper
  functions this batch introduced for cases where a shared local is reached both by tail-goto and
  by a genuine `call` with more work after: (1) writing the literal phrase "gb->pc"/"gb->sp" in a
  flush-left explanatory comment trips the linter's raw-text register scan even though it's not
  code -- reworded to "the resulting pc/sp"; (2) `lint_game.py`'s hook-detector only recognizes a
  bare `void name_hook(GB *gb)` line (no `static`, no extra parameters) as entering hook-shim
  scope -- three helpers using their own `gb->pc`/`gb->sp` checks needed the `static` keyword
  dropped and, in one case, an extra `sp0_` parameter replaced with the helper declaring
  `uint16_t sp0_ = gb->sp;` itself instead of threading it through.

  Confirmed via the zora/greatFairy precedent (batch 334) that only bare (non-`@`) root names
  need `rewritten.txt` entries; `interactionCodec9` (syrupCucco) needed a `ported.txt` entry too
  (never hooked at all, same silent-failure class as `interactionCode67`/`interactionCodea9`).

  After registering and rebuilding, `src/game/gen_bank0b.c` disappeared entirely from `--out`'s
  output (7 bank files instead of 8). Ran the full bare-symbol-vs-registration audit
  (`ages.sym`'s 157 bare bank-0b symbols against the union of `ported.txt`+`rewritten.txt`): 5 are
  pure ROM data tables (`finalChildPersonalityTable`, `initialChildPersonalityTable`,
  `horizontalCreditsText_65b1`, `horizontalCreditsText_scriptTable`, `zora_textIndices`,
  confirmed via garbage-instruction decode in their own transliterate reports), and 22 are losing
  `zora_subid` aliases sharing an address with an already-registered first-declared winner
  (`zora_subid00`/`0A`/`0C`/`0E`/`10`/`13`), confirmed via direct address-grouping against
  `ages.sym`. That leaves exactly 130 distinct real routines in bank 0b, all hooked with zero
  gaps and zero permanently-skipped garbage (unlike bank 11's one item). `docs/bank-map.html`'s
  long-stale 770 total is corrected to the audited 130/130 -- bank 0b joins banks 00, 03, 0a, 10,
  and 11 as fully done. Full gate passed: both builds clean, both ctest suites 8/8, 30k-frame
  verify-hooks-continue clean (same final state hash as before the batch), full 289,869-frame TAS
  reference replay passed. Project total is 4,860/9,646 (down from 4,895 purely from the 770→130
  stale-count correction, not from any code being un-rewritten).

- 2026-09-19: milestone 3 phase 6 batch 335, bank 0b (22 routines, 20 new files):
  second wave of the ~99-routine sweep, two parallel agents. Cluster A: `explosionWithDebris.c`,
  `carpenter.c`, `raftwreckCutscene.c`, `kingZora.c`, `tokkey.c`, `waterPushblock.c`,
  `disappearingSidescrollPlatform.c`, `circularSidescrollPlatform.c` (plus the full shared
  `sidescrollPlatform_*`/`sidescrollingPlatformCommon` helper cluster used by four different
  interaction codes, only two of which -- `interactionCodea3`/`interactionCodea4` -- are ported
  so far), `touchingBook.c`, `makuSeed.c`. Cluster B: `a8.c`, `din.c`, `twinrovaInCutscene.c`,
  `childJabu.c`, `twinrova3.c`, `pushblockSynchronizer.c`, `ambisPalaceButton.c`,
  `symmetryNpc.c`, `pirate.c`, `tingle.c`. Notable: `waterPushblock.c`'s `swapRoomLayouts`
  faithfully reproduces a genuine ROM quirk where the 11th of 11 `call`s has a return address
  that coincidentally equals its own callee's entry point, causing the callee to run a real,
  ROM-accurate 12th time before actually returning; `circularSidescrollPlatform.c` extracted a
  shared, physically-overlapping tail (`pushLinkAwayVertical`'s own setup code runs directly into
  `pushLinkAwayHorizontal`'s body via a `jr`) into one static helper both call into. Found and
  fixed one real bug during review: `makuSeed.c`'s `createSparkle` wrote `A = 0x00` for a related-
  object marker byte the ROM report clearly showed as `ld a,$40` -- the same
  Interaction.start/INTERACTION_BASE (0x40) vs Object.start (0x00) confusion class flagged
  earlier this session, caught by cross-referencing the report's actual immediate byte rather
  than trusting the "Interaction.start" comment alone. Registration insight confirmed via the
  batch-334 zora/greatFairy/pirateShip precedent: registering only a routine's bare (non-`@`)
  root name in `rewritten.txt` collapses ALL of its `@`-scoped local labels out of
  `generated.txt` at once, regardless of how many internal call sites reference them or how the
  C implementation structures them (inline `goto`s vs separate static/public helper functions) --
  so only the 8 interaction roots plus the 12 genuinely bare `sidescrollPlatform_*`/
  `sidescrollingPlatformCommon` global helpers needed individual `rewritten.txt` entries (22
  total), not the dozens of `@`-local sub-labels initially listed. Full gate passed: both builds
  clean, both ctest suites 8/8, 30k-frame verify-hooks-continue clean, full 289,869-frame TAS
  reference replay passed. `docs/bank-map.html` bank 0b count corrected from 143 to 165 of 770.
  Project total is 4,895/9,646.

- 2026-09-19: milestone 3 phase 6 batch 334, bank 0b (4 routines, new files
  `twinrovaFlameInteraction.c`, `pirateShipInteraction.c`; extended `zora.c`, `greatFairy.c`):
  first wave of bank 0b's ~99-routine sweep (audited in batch 333's wake, after the bank-11
  near-miss taught not to trust `gen_bankXX.c` file size alone). Two parallel agents: one
  extended `zora.c` with `interactionCodeab_hook` and `greatFairy.c` with `interactionCoded5_hook`
  -- both trivial 3-instruction `rst $00` dispatchers whose targets (`zora_subid00`/`0A`/`0C`/
  `0E`/`10`/`13` and `greatFairy_subid0`/`1`) turned out to already be fully implemented from an
  earlier session; my own initial audit had over-counted these as ~18 separate missing routines
  by not accounting for ROM-address alias grouping. The other created two new files for the
  session's 5th and 6th filename-collision cases: `twinrovaFlameInteraction.c`
  (`interactionCodea9`, `object_code/common/interactions/twinrovaFlame.s`, distinct from the
  bank-0x11 `common/parts/twinrovaFlame.s` already in `twinrovaFlame.c`) and
  `pirateShipInteraction.c` (`interactionCodec2`, the bank-0x0b portion of
  `object_code/ages/interactions/pirateShip.s`, whose bank-0x01 portion is already in
  `pirateShip.c`). `interactionCodea9` was never in `ported.txt` at all (the same silent-failure
  class discovered with `interactionCode67` in batch 331) and needed adding there too;
  `interactionCodec2` was already present. `interactionCodec2`'s 16 `@`-local sub-labels
  (subid0/1/2 each with their own substates, sharing `subid1And2State0Common`/`State1` and a
  `moveOffScreen` tail) all goto-inline with zero nesting depth, confirmed via `callers: 0` on
  every sublabel. All four routines verified byte-for-byte against `transliterate.py --report`
  (addresses, cycles, CALL_C targets); confirmed via `git status`/mtime that the two pre-existing
  collision files (`twinrovaFlame.c`, `pirateShip.c`) were never touched. Full gate passed: both
  builds clean, both ctest suites 8/8, 30k-frame verify-hooks-continue clean, full 289,869-frame
  TAS reference replay passed. `docs/bank-map.html` bank 0b count corrected from 139 to 143 of
  770 (770 itself likely still a stale overcount, per the same alias-inflation pattern seen in
  banks 0a/11 -- not yet re-audited). Project total is 4,873/9,646.

- 2026-09-19: milestone 3 phase 6 batch 333, bank 11: no new code -- discovered bank 11 was
  already essentially complete too. Of 255 bare (non-`@`) symbols in `ages.sym` for bank 11, 24
  are pure ROM data tables (correctly never registered) and 227 are already hooked and readable.
  The single remaining entry, `func_11_7f64`, is not real game code at all: its own source is
  `ref/oracles-disasm/code/ages/garbage/bank11End.s` -- the disassembly project's own bucket for
  unreachable ROM padding that happens to disassemble into plausible-looking instructions. Its
  transliterate report confirms `callers: 0` and both of its own call targets are unresolved,
  matching the same "skip as garbage" category CLAUDE.md already documents for illegal-opcode
  labels. `docs/bank-map.html`'s tracking count for this bank was badly stale (227/650, likely
  counting every `@`-local label individually rather than distinct routines needing independent
  work); corrected to 649/650, with the one point of headroom being the permanently-skipped
  garbage fragment. Project total is 4,869/9,646 (unchanged, since no code was added).

- 2026-09-19: milestone 3 phase 6 batch 332, banks 00 and 03: no new code -- discovered both
  banks were already 100% complete. `setCpuToDoubleSpeed` (03:4071, `src/game/kernel.c`) and
  `wRamFunction` (00:c4b7, `src/game/ram_code.c`) were already hand-written and correctly hooked
  (both appear in `src/hooks/table.h`), just without the conventional `_hook` naming suffix,
  which made a naive `generated.txt` grep (checking for names ending in `_hook`) flag them as
  false-positive gaps. Confirmed genuinely complete via the same signal that closed out bank 0a:
  no `gen_bank00.c`/`gen_bank03.c` interpreter-fallback file exists for either bank, meaning zero
  routines in either bank still run through the generic interpreter. `docs/bank-map.html`'s
  tracking counts for these two banks were stale by one each (653/654, 636/637) from early in the
  multi-session effort; corrected to 654/654 and 637/637. Project total is 4,869/9,646.

- 2026-09-19: milestone 3 phase 6 batch 331, bank 0a (1 routine, new file `companionSpawner.c`):
  added `interactionCode67` (spawns the current animal companion via flute call or story
  cutscene), closing out bank 0a's readable-C conversion. Source is
  `object_code/common/interactions/companionSpawner.s` (shared disasm code; the specific
  `interactionCode67` symbol resolves to bank 0x0a for Ages). Notable discovery: this routine had
  never been hooked at all, unlike every other routine ported in bank 0a this session (which were
  all already milestone-2 "generated" hooks from an earlier pass, needing only a `rewritten.txt`
  addition) -- `transliterate.py --out` silently produces no new hook for a name that exists only
  in `rewritten.txt` but not in `src/hooks/ported.txt`, since `ported.txt` is what defines the
  full set of routines considered at all. Added `interactionCode67` to `ported.txt` too, which
  fixed it. Self-review (full instruction-by-instruction cross-check against the transliterate
  report, including the routine's 4 multi-caller shared locals) found zero bugs; confirmed no
  nesting depth anywhere in this file, unlike `makuTree.c`/`makuSprout.c`. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean.

  With this, a thorough cross-reference of every bare (non-`@`) bank-0a symbol in `ages.sym`
  against `ported.txt`/`rewritten.txt` combined finds only pure ROM data tables and known aliased
  duplicate addresses left unregistered -- no further bank-0a interaction/subroutine code remains
  unrewritten. Bank 0a is 172/928 per this doc's own long-running counter, but that "928" figure
  (set early in the multi-session effort) appears to have overcounted the bank's true routine
  total; the actual code-routine count is much closer to 172. The project total is 4,867/9,646.

- 2026-09-19: milestone 3 phase 6 batch 330, bank 0a (12 routines, new files `patch.c`,
  `ballInteraction.c`, `moblin.c`, `97.c`): added `interactionCode94` (INTERAC_PATCH) plus 6
  bare-global helpers, `interactionCode95` (INTERAC_BALL), `interactionCode96` (INTERAC_MOBLIN),
  and `interactionCode97` (INTERAC_97) plus 2 bare-global helpers. Found a THIRD instance of the
  file-basename-collision class: `interactions/ball.s` (bank 0x0a, this batch) and
  `parts/ball.s` (bank 0x11, already covered by the existing `src/game/ball.c`) share a basename;
  caught before any file was touched, ported to a new `ballInteraction.c` instead. `patch_subid03`
  has a 4-real-caller-plus-fallthrough shared local (`@spawnBeetle`) correctly modeled with the
  standard multi-site resume-label idiom; `interaction97_subid01`'s `@spawnBubble` has a
  genuinely nested single-caller case interacting with an outer `push bc`/`pop bc` bracket, whose
  `sp0_ - 2` resume-check depth was independently re-derived and confirmed correct. Also
  reconfirmed the aliased-bare-global-address convention (whichever ROM label is declared first
  in the source wins the hook-table slot, not necessarily the one carrying the "real" body) for
  `patch_subid04`/`05` and `patch_subid06`/`07`. Self-review found zero remaining bugs across all
  four files. Lint, both builds, 30k verification, and the full 289,869-frame reference replay
  all passed clean. Bank 0a is 171/928 and the project is 4,866/9,646.

- 2026-09-19: milestone 3 phase 6 batch 329, bank 0a (27 routines, new files `fallingRock.c` and
  `twinrovaInteraction.c`): added `interactionCode92` (INTERAC_FALLING_ROCK) plus 11 bare-global
  helpers, and `interactionCode93` (INTERAC_TWINROVA) plus 14 bare-global helpers. Discovered a
  SECOND instance of the file-basename-collision class first found with `sparkle.s`/`sparkleInteraction.c`:
  `ref/oracles-disasm/object_code/ages/interactions/twinrova.s` (INTERAC_TWINROVA, bank 0x0a) and
  `object_code/common/enemies/twinrova.s` (ENEMY_TWINROVA, bank 0x10, already covered by the
  existing `src/game/twinrova.c`) are two completely unrelated source files that share a basename
  in different disasm directories. Caught before any file was touched (the porting agent flagged
  the bank mismatch on its own instead of pasting bank-0x0a code under the existing file's
  bank-0x10 `CYC`/`CYCT` macros), so the new cluster was written to its own `twinrovaInteraction.c`
  file instead, following the established naming precedent. Both files' target-vs-physical-end
  `CYC` ranges were mechanically re-verified end to end; zero bugs found. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 159/928
  and the project is 4,854/9,646.

- 2026-09-19: milestone 3 phase 6 batch 328, bank 0a (33 routines, new file `miscPuzzles.c`):
  closes out `miscPuzzles.s` (1377 lines, the largest single source file in bank 0a so far),
  adding `interactionCode90` (INTERAC_MISC_PUZZLES, the top-level dispatcher) plus 32 bare-global
  `miscPuzzles_subidNN`/shared-helper routines spanning D6-D8 puzzles, the Hero's Cave, Jabu-jabu's
  water level, and several maku-tree gasha-seed spawns. Every bare global is independently
  registered and dispatched via a long if-chain of tail-calls from `interactionCode90_hook`,
  matching the established bare-global convention; this is the largest routine cluster ported in
  one batch this session. Produced by 4 parallel porting agents each handling a contiguous chunk
  of the file, merged into one file by hand with careful cross-verification. Found and fixed 3
  bugs during the merge review: two instances of the target-vs-physical-end `CYC` bug in one
  chunk (an unconditional `jr` whose target lands after a small ROM data table, so the `CYC` end
  used the jump target instead of the 2-byte physical instruction length — the same bug class
  documented repeatedly this session), plus one merge error introduced during assembly itself (a
  cycle-burn line for a not-taken branch was dropped and mistakenly re-inserted in the wrong
  place), self-caught before building. A genuine single-level nested `@`-local case
  (`deletePartObject`, called from within `makeTorchesUnlightable`'s own outstanding call) was
  correctly modeled with the `sp0_ - 2` depth-adjusted resume check. Every `CALL_C` target address
  and the full 33-entry dispatcher table were mechanically cross-checked against
  `src/hooks/generated.txt`/`ages.sym`. Lint, both builds, 30k verification, and the full 289,869-
  frame reference replay all passed clean. Bank 0a is 132/928 and the project is 4,827/9,646.

- 2026-09-19: milestone 3 phase 6 batch 327, bank 0a (2 routines, new files `octogonSplash.c` and
  `tokayCutsceneEmberSeed.c`): added `interactionCode8e` (INTERAC_OCTOGON_SPLASH) and
  `interactionCode8f` (INTERAC_TOKAY_CUTSCENE_EMBER_SEED), produced by parallel porting agents.
  Both are small, flat routines with no local call chains at all. Self-review found zero bugs in
  either file. Lint, both builds, 30k verification, and the full 289,869-frame reference replay
  all passed clean. Bank 0a is 99/928 and the project is 4,794/9,646.

- 2026-09-19: milestone 3 phase 6 batch 326, bank 0a (1 routine, new file `cloakedTwinrova.c`):
  added `interactionCode8d` (INTERAC_CLOAKED_TWINROVA), hand-ported directly. Notable for a
  same-address-collision-free ROM layout quirk: `@initSubid0` falls straight through into
  `@initSubid2` (no jump between them), while `@initSubid1`'s jump-table entry points to code
  physically placed AFTER `@initSubid2`, and its own top-level `@state0`/`@state1` and inner
  subid1-substate dispatches both use the implicit-fallthrough-for-index-0 convention exhaustively.
  Self-review found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame
  reference replay all passed clean. Bank 0a is 97/928 and the project is 4,792/9,646.

- 2026-09-19: milestone 3 phase 6 batch 325, bank 0a (2 routines, new files `goronElder.c` and
  `tokayMeat.c`): added `interactionCode8b` (INTERAC_GORON_ELDER) and `interactionCode8c`
  (INTERAC_TOKAY_MEAT), produced by parallel porting agents. Self-review caught a real bug in
  `goronElder.c`: the porting agent misapplied `remoteMakuCutscene.c`'s valid "trampoline, no
  pc/sp check needed" exception to a structurally different case — a `goto`-reached local with a
  genuine single real caller ending in a tail-jump, which (unlike a plain synchronous C function
  call from straight-line code) does need the standard resume-check to hand control back to its
  caller's continuation point instead of silently falling through to a bare `return`. Fixed by
  adding the missing `if (gb->pc == ... && gb->sp == sp0_) goto ...;` check. `tokayMeat.c` (pure
  jump-table dispatch, no manual call chains) had zero bugs. Lint, both builds, 30k verification,
  and the full 289,869-frame reference replay all passed clean. Bank 0a is 96/928 and the project
  is 4,791/9,646.

- 2026-09-18: milestone 3 phase 6 batch 324, bank 0a (2 routines, new files `makuSprout.c` and
  `remoteMakuCutscene.c`): added `interactionCode88` (INTERAC_MAKU_SPROUT) and `interactionCode8a`
  (INTERAC_REMOTE_MAKU_CUTSCENE), produced by parallel porting agents. `makuSprout.c` has a second
  instance of the nested-call sp-depth bug class first found in `makuTree.c`, correctly identified
  and fixed by the porting agent itself (a genuinely dead-code block, `@initGraphics`, was also
  correctly omitted matching the `rosa.c` precedent). `remoteMakuCutscene.c` has 8 nested calls to
  a shared local (`@checkEssenceObtained`) that never executes its own `ret` and instead
  tail-jumps into the already-hooked `checkFlag`; rather than the depth-adjusted sp0_ arithmetic,
  it's modeled as a small trampoline helper, since `checkFlag_hook`'s own `ret_effect` pops
  exactly what was pushed regardless of outer nesting depth, needing no pc/sp resume check at all.
  Self-review (full instruction-by-instruction cross-check against the transliterate reports)
  found zero bugs in either file. Lint, both builds, 30k verification, and the full 289,869-frame
  reference replay all passed clean. Bank 0a is 94/928 and the project is 4,789/9,646.

- 2026-09-18: milestone 3 phase 6 batch 323, bank 0a (1 routine, new file `makuTree.c`): added
  `interactionCode87` (INTERAC_MAKU_TREE), a 296-instruction, 7-subid routine covering the present
  and post-Twinrova Maku Tree cutscenes. This is the first bank-0a routine with genuinely NESTED
  manual call chains (a local pushing a resume address and, before returning, itself pushing
  ANOTHER resume address into a further local) rather than the strictly sequential single-level
  calls every prior file used. Self-review caught the naive `sp0_`-comparison idiom silently
  breaking for the two inner-most resume checks and one intermediate one, since `gb->sp` is offset
  by however many outer pushes are still outstanding at nesting depth; fixed using the
  depth-adjusted `(uint16_t)(sp0_ - 2)`/`(sp0_ - 4)` pattern already established in `roller.c` and
  `rosa.c`. An independent review re-derived the full push/pop depth trace from scratch and
  confirmed all three fixed checks and every unchanged (true depth-0) check are correct, plus
  found zero other bugs. Root-only registration confirmed empirically. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 92/928
  and the project is 4,787/9,646.

- 2026-09-18: milestone 3 phase 6 batch 322, bank 0a (1 routine, new file `makuFlower.c`): added
  `interactionCode86` (INTERAC_MAKU_FLOWER), a 2-subid maku tree flower controller (subid 0 shows
  the flower and tracks the related object's animation via a small ROM data table; subid 1 counts
  up `Interaction.zh` from 0xd4 until it wraps, then deletes). Root-only registration confirmed
  empirically (regenerated `generated.txt` shows no surviving `@`-locals). Independent review
  found zero bugs (all CYC ranges, flag polarities, and CALL_C targets cross-checked against the
  transliterate report and raw ROM bytes). Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 91/928 and the project is
  4,786/9,646.

- 2026-09-18: milestone 3 phase 6 batch 321, bank 0a (1 routine, new file
  `sparkleInteraction.c`): added `interactionCode84` (INTERAC_SPARKLE), a 16-way subid particle
  controller with two `rst $00` dispatches. Notable for a filename collision — the natural name
  `sparkle.c` (derived from `interactions/sparkle.s`) collided with a PRE-EXISTING, unrelated
  bank-0x11 file of the same name (`partCode26_hook`, ported from `parts/sparkle.s`); the new
  routine's `Write` silently overwrote the old file, which only surfaced as a linker error
  ("undefined symbol `partCode26_hook`") at build time. Recovered the original file from git
  history and moved the new routine to `sparkleInteraction.c`. Self-review (full
  instruction-by-instruction trace against the report) caught a genuine bug before the linker
  error was even found — an unconditional backward `jr` had its `CYC` end set to the jump target
  instead of the physical instruction end (the classic target-vs-physical-end zero-cycle-burn
  bug class) — fixed, and an independent review confirmed no other instances of the same class
  remained. Lint, both builds, 30k verification, and the full 289,869-frame reference replay all
  passed clean. Bank 0a is 90/928 and the project is 4,785/9,646.

- 2026-09-18: milestone 3 phase 6 batch 320, bank 0a (4 routines, new file
  `bombUpgradeFairy.c`): added `interactionCode83` (INTERAC_BOMB_UPGRADE_FAIRY, the dispatcher)
  plus `bombUpgradeFairy_subid00`/`01`/`02` (bare globals, each independently hooked). Notable
  for `subid00`'s `spawnSubid2Instance` local, reached by one genuine `call` plus one plain
  fallthrough (the established single-resume-address idiom). An independent review found a real
  bug — `Interaction.start` (a WLA-DX struct-offset symbol) was ported as the literal `0x00`
  (the struct field's own offset) instead of `0x40`/`INTERACTION_BASE` (this codebase's actual
  convention for that idiom, confirmed against the ROM's own `ld a,$40` bytes and matching
  precedent in `possessedNayru.c`/`twinrova.c`) — fixed at both call sites, and the full gate was
  re-run clean afterward. Bank 0a is 89/928 and the project is 4,784/9,646.

- 2026-09-18: milestone 3 phase 6 batch 319, bank 0a (1 routine, new file `sarcophagus.c`):
  added `interactionCode82` (INTERAC_SARCOPHAGUS), a pushable-block state machine (grab, hold,
  release, break, destroy-animation). Researched and written by a parallel subagent while this
  session gated the previous batch. Self-review (full instruction-by-instruction trace against
  the report, catching zero discrepancies, including correctly identifying a 4th jump-table
  target that collapses into the sibling `state0` dispatcher's own `break` label) found no bugs.
  Lint, both builds, 30k verification, and the full 289,869-frame reference replay all passed
  clean (gated together with batch 318). Bank 0a is 88/928 and the project is 4,783/9,646.

- 2026-09-18: milestone 3 phase 6 batch 318, bank 0a (1 routine, new file `tokayShopItem.c`):
  added `interactionCode81` (INTERAC_TOKAY_SHOP_ITEM), notable for a genuinely complex
  multi-call-site local (`checkTransformItem`, called from both `initializeItem` and `state1`,
  using two `rst $10` table lookups) that a parallel subagent got exactly right on the first
  try — verified via a full instruction-by-instruction self-review trace against the report,
  including manually deriving the routine's own un-reported address range and confirming it
  landed exactly on the next label, and specifically checking the two resume-check addresses
  (0x6252, 0x627d) against both `push_effect` call sites — no discrepancies found. Bank 0a is
  87/928 and the project is 4,782/9,646.

- 2026-09-18: milestone 3 phase 6 batch 317, bank 0a (1 routine, new file `decoration.c`):
  added `interactionCode80` (INTERAC_DECORATION), produced by a parallel subagent. Notable for a
  single-call-site local (`isSymmetryCityRoom`, ending in a tail `jp` rather than `ret`) inlined
  directly at its own call site since it has no other caller, and for using the codebase's
  existing `HANDOFF` interpreter-fallback macro as an (unreachable) safety net after exhaustively
  enumerating both jump tables' unique targets, rather than this session's usual "last target is
  an implicit fallthrough" convention — a valid stylistic choice already established elsewhere in
  the codebase (`bipin.c`), not a bug. Self-review (full instruction-by-instruction trace against
  the report, including manually deriving the one un-reported local block and confirming it lands
  exactly on the next label) and an independent review both found zero bugs. Lint, both builds,
  30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is
  86/928 and the project is 4,781/9,646.

- 2026-09-18: milestone 3 phase 6 batch 316, bank 0a (1 routine, new file
  `screenDistortion.c`): added `interactionCode7c` (INTERAC_SCREEN_DISTORTION), a tiny
  two-state routine (arm a mid-frame LCD interrupt for the wavy-screen warp effect, then drive
  the per-frame scroll buffer). Produced by a parallel subagent; self-review confirmed it matches
  the report exactly. Lint, both builds, 30k verification, and the full 289,869-frame reference
  replay all passed clean (gated together with batch 317). Bank 0a is 85/928 and the project is
  4,780/9,646.

- 2026-09-18: milestone 3 phase 6 batch 315, bank 0a (1 routine, new file `stonePanel.c`):
  added `interactionCode7b` (INTERAC_STONE_PANEL), a large state/substate machine (ancient-tomb
  panel that slides open once a lever is triggered) with a single-call-site resume-label case
  for `updateSolidityUponOpening`, reached both by fallthrough (from the "already opened on room
  entry" path) and by one genuine `call` (from substate2). Researched and written by a parallel
  subagent while this session gated the previous batch. Self-review (cross-checked
  instruction-by-instruction against the report, catching zero discrepancies) and an
  independent review both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 84/928 and the project is
  4,779/9,646.

- 2026-09-18: milestone 3 phase 6 batch 314, bank 0a (1 routine, new file
  `makuGateOpening.c`): added `interactionCode76` (INTERAC_MAKU_GATE_OPENING), the most complex
  multi-site resume-label case in bank 0a yet — FOUR different local subroutines each called via
  genuine `call` from multiple sites, two of them (`loadPoofs`, `shakeScreen`) ending in a
  tail-jump to `playSound_b00_hook` rather than a plain `ret` (verified via the established
  CALL_C-transitivity-through-a-tail-jump-chain reasoning, not a bare `ret`/`RET_TAKEN`).
  Independent review found a real bug — `loadPoofs`'s four resume-checks all compared `gb->pc`
  against the `call` instruction's own address instead of the address actually passed to
  `push_effect` (3 bytes later, the call's own end address), meaning none of the checks could
  ever match and every invocation would silently exit the whole hooked routine early instead of
  continuing — fixed, and the full gate (lint, both builds, both ctest suites, 30k verification,
  full ref-check) was re-run clean afterward. Bank 0a is 83/928 and the project is 4,778/9,646.

- 2026-09-18: milestone 3 phase 6 batch 313, bank 0a (2 routines, first batch produced with
  parallel porting agents): added `interactionCode75` (INTERAC_INTRO_SPRITE, new file
  `introSprite.c`, a 7-way-subid intro-cutscene sprite controller with three separate `rst $00`
  dispatch points reusing one jump-table helper) and `interactionCode77`
  (INTERAC_SMALL_KEY_ON_ENEMY, new file `smallKeyOnEnemy.c`, notable for faithfully reproducing
  a genuine ROM bug — an enemy-slot scan that only actually works for the first slot due to a
  `jp c` instead of `jp nc` check). Both were researched and written by parallel subagents
  (working read-only against `--report`, never touching the shared `rewritten.txt`/generated
  files) while this session ran the gate for the previous batch; the coordinator then registered,
  built, and gated both together. Self-review (of `interactionCode77`, cross-checked
  instruction-by-instruction against the report) and an independent review (of
  `interactionCode75`) both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 83/928 and the project is
  4,778/9,646.

- 2026-09-18: milestone 3 phase 6 batch 312, bank 0a (2 routines): added `interactionCode73`
  (INTERAC_GHINI_HARASSING_MOOSH, new file `ghiniHarassingMoosh.c`) and `interactionCode74`
  (INTERAC_RICKYS_GLOVE_SPAWNER, new file `rickysGloveSpawner.c`). Independent review found one
  real bug in `interactionCode73` — a missing fallthrough cycle burn for an untaken `ret nc` —
  fixed and re-verified; self-review caught a related but distinct bug in
  `rickysGloveSpawner.c` before it was ever built (three branches to a shared `@delete` label,
  whose own body is a bare `jp interactionDelete`, were bypassing that `jp`'s own cycle burn by
  tail-calling `interactionDelete_hook` directly instead of `goto`-ing to the shared block). Bank
  0a is 81/928 and the project is 4,776/9,646.

- 2026-09-18: milestone 3 phase 6 batch 311, bank 0a (1 routine, new file
  `kingMoblinDefeated.c`): added `interactionCode72` (INTERAC_KING_MOBLIN_DEFEATED), fully
  goto-inlined into one function. Introduces (for the first time in bank 0a) a technique for a
  local label called via a genuine `call` from MULTIPLE sites within the same hook, ending in a
  plain `ret`: each call site does `push_effect`+`goto` to a shared resume label, and the local's
  own `ret`/`RET_TAKEN` checks `gb->pc`/`gb->sp` against every known resume address in turn,
  falling through to a real C `return` if none match (established precedent from
  `miscellaneous1.c`, applied here to `spawnSubservientMoblin` and `spawnGoronInstance`, the
  latter also reached by a plain fallthrough as an implicit third invocation with no
  `push_effect` at all). Self-review and an independent review both found zero bugs. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a
  is 79/928 and the project is 4,774/9,646.

- 2026-09-18: milestone 3 phase 6 batch 310, bank 0a (17 routines, new file
  `companionScripts.c`): added the entire INTERAC_COMPANION_SCRIPTS routine family in one pass —
  `interactionCode71` (top dispatcher), `companionScript_subid00` through `subid0d` (14
  handlers), plus shared helpers `genericState0`, `runScript`, `subid00_state1`, `deleteSelf`,
  `delete`, `cpXToCompanion`/`cpYToCompanion`, and the four `restrictHigherX/LowerX/LowerY/
  HigherY` routines (which each call one of the cp helpers then jump into a shared tail block
  physically owned by `restrictHigherY`'s own ROM range, modeled as a plain shared static C
  helper rather than replicated `goto` targets, since it has a single entry/exit with no
  resumption expected). Uses two new per-file RST-vector helpers (`rst $00` jump table, `rst $18`
  add-double-index) plus a `rst $10` add-to-HL helper for `subid09`'s animation table lookup.
  Split into three independent reviews given the file's size; one found a real bug — the shared
  restrict-tail helper added `INTERACTION_BASE` to `OBJ_SPEED` when HL was actually still
  pointing into `w1Companion`'s own struct (left there by `cpXToCompanion`/`cpYToCompanion`), so
  the raw offset alone was needed — fixed, and the full gate (lint, both builds, both ctest
  suites, 30k verification, full ref-check) was re-run clean afterward. This closes out
  `companionScripts.s` entirely. Bank 0a is 78/928 and the project is 4,773/9,646.

- 2026-09-18: milestone 3 phase 6 batch 309, bank 0a (1 routine, new file
  `wildTokayController.c`): added `interactionCode70` (INTERAC_WILD_TOKAY_CONTROLLER), a large
  207-instruction routine fully goto-inlined into a single function (state0/state1 dispatch, 7
  substates, plus internal helpers `checkSpawnNextTokay`/`loadTokay`/`decVar3b`/
  `getRandomVar39Value`). Uses two per-file RST-vector helpers: `wildTokayController_jump_table`
  (`rst $00`) and `wildTokayController_addAToHl_from_rst` (`rst $10`, distinct from the
  bank-00 `addDoubleIndexToBc_hook` used elsewhere in the same routine for a different table).
  Split into two independent reviews given its size; the second found a real bug — a missing
  fallthrough `CYC(0x5807, 0x5808)` after a `ret nz` at the `l5804`→`l5808` join point (silently
  dropping 2 cycles when that path was taken) — fixed, and the full gate (lint, both builds, both
  ctest suites, 30k verification, full ref-check) was re-run clean afterward. Bank 0a is 61/928
  and the project is 4,756/9,646.

- 2026-09-18: milestone 3 phase 6 batch 308, bank 0a (1 top-level routine): added
  `interactionCode6e` (INTERAC_NAYRU_SAVED_CUTSCENE's own top-level dispatcher) to
  `nayruSavedCutscene.c`, **closing out the entire file** — 6 batches (303-308), 8 routines. A
  3-instruction E-preload (subid, then state) plus an `rst $00` jump table dispatching to the 5
  already-hooked subid handlers, structurally identical to `interactionCode6d`'s dispatcher
  pattern in `possessedNayru.c`. Self-review and an independent review both found zero bugs, and
  the review's whole-file sanity pass over all 8 functions found no leftover issues. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a
  is 60/928 and the project is 4,755/9,646.

- 2026-09-18: milestone 3 phase 6 batch 307, bank 0a (1 routine): added
  `interaction6e_subid04` (guards that run into the room) to `nayruSavedCutscene.c`. This routine's script
  selection uses `rst $18` (`rst_addDoubleIndex`) to index a 6-entry script pointer table by
  `Object.var03`; the RST vector is modeled with a new per-file static helper
  (`nayruSavedCutscene_add_double_index`) built to the exact same established pattern as
  `bipin_addDoubleIndex` in `bipin.c`. Self-review and an independent review both found zero
  bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference replay all
  passed clean. Bank 0a is 59/928 and the project is 4,754/9,646.

- 2026-09-18: milestone 3 phase 6 batch 306, bank 0a (3 routines, continuing
  `nayruSavedCutscene.s`): added `interaction6e_subid03`, `interaction6e_runScriptAndAnimate`, and
  `interaction6e_initRalph` (Ralph) — three bare (non-`@`-local) global labels, each independently
  hooked as its own top-level function per the established convention, since
  `runScriptAndAnimate` is also called separately by the not-yet-ported `interaction6e_subid04`.
  Transitions between the three use plain tail calls (`return;` after calling the next function),
  not `goto` or `CALL_C`, since the ROM's own control flow here is a bare fallthrough and a
  push-free `jr z`, never a real `call`. Self-review and an independent review both found zero
  bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference replay all
  passed clean. Bank 0a is 58/928 and the project is 4,753/9,646.

- 2026-09-18: milestone 3 phase 6 batch 305, bank 0a (1 routine, continuing
  `nayruSavedCutscene.s`): added `interaction6e_subid02` (Ghost Veran) — a 4-way dispatch (3
  jump-table targets plus fallthrough) with four internal-only join labels reached purely by
  fallthrough/goto, never by the top-level RST table. Uses the `push af`/`pop af` idiom and two
  `call z,playSound` conditional calls via `CALL_C_CC`. Self-review and an independent review
  both found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference
  replay all passed clean. Bank 0a is 55/928 and the project is 4,750/9,646.

- 2026-09-18: milestone 3 phase 6 batch 304, bank 0a (1 routine, continuing
  `nayruSavedCutscene.s`): added `interaction6e_subid01` (Queen Ambi) — the largest routine in
  the file so far, a 5-state machine (76 instructions) with two shared join points
  (`animate`/`setAnimation`) reached from multiple states. Self-review and an independent review
  both found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference
  replay all passed clean. Bank 0a is 54/928 and the project is 4,749/9,646.

- 2026-09-18: milestone 3 phase 6 batch 303, bank 0a (1 routine, starting
  `nayruSavedCutscene.s`): added `interaction6e_subid00` (Nayru waking up after being freed from
  possession) to new file `nayruSavedCutscene.c`. Straightforward 3-state routine; the empirical
  HOOK_LOCAL registration test confirmed all three `@state0`/`@state1`/`@state2` locals fully
  goto-inline once the root name is registered. Self-review and an independent review both found
  zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference replay all
  passed clean. Bank 0a is 53/928 and the project is 4,748/9,646.

- 2026-09-18: milestone 3 phase 6 batch 302, bank 0a (1 top-level routine): added
  `interactionCode6d` (INTERAC_POSSESSED_NAYRU's top-level dispatcher) to `possessedNayru.c`,
  **closing out the entire file** — 3 batches (300-302), 3 top-level routines. This dispatcher
  has a genuinely novel structure not seen elsewhere in bank 0a: it reads `Interaction.subid` via
  E, then OVERWRITES E with `Interaction.state` BEFORE dispatching, so whichever callee the jump
  table lands on can immediately read state without setting E itself — the exact register-preload
  dependency already found (and confirmed intentional) in both callees. Order matters critically
  here (read subid, THEN overwrite E, THEN dispatch); an independent review verified the 3-step
  sequence matches the ROM exactly and confirmed the jump table's 3-raw-entries-to-2-unique-
  addresses collapse (subid01 and subid02 both alias to `ghost`) is faithfully represented.
  Self-review and the independent review both found zero bugs in this final batch. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a
  is 52/928 and the project is 4,747/9,646.

- 2026-09-18: milestone 3 phase 6 batch 301, bank 0a (1 top-level routine, continuing
  `possessedNayru.s`): added `possessedNayru_ghost` — like `subid00` before it, relies on E
  already set by the not-yet-written `interactionCode6d` dispatcher. Self-review and an
  independent review both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 51/928 and the project is
  4,746/9,646. Only the `interactionCode6d` dispatcher remains to close out this file.

- 2026-09-18: milestone 3 phase 6 batch 300, bank 0a (1 top-level routine, first slice of a new
  file): started `object_code/ages/interactions/possessedNayru.s`
  (`interactionCode6d`/INTERAC_POSSESSED_NAYRU, `possessedNayru.c`) — 3 top-level routines total
  (the dispatcher, `subid00`, and `ghost`). This batch: `possessedNayru_subid00`. Structurally
  novel: this routine relies on register E already holding `INTERACTION_BASE + OBJ_STATE`, set by
  the not-yet-written `interactionCode6d` dispatcher immediately before its jump-table dispatch —
  the routine's own code never sets E itself, matching the ROM's own omission of `ld e,...` at
  its entry point. Verified this is safe in this project's emulation model (Z80 registers are
  fields on the shared `GB` struct, not per-function C locals, so a value left by one hooked
  routine is visible to whichever hooked routine runs next). Self-review and an independent
  review both found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame
  reference replay all passed clean. Bank 0a is 50/928 and the project is 4,745/9,646.

- 2026-09-18: milestone 3 phase 6 batch 299, bank 0a (1 top-level routine): added
  `interactionCode6c` (INTERAC_FAIRY_HIDING_MINIGAME's top-level dispatcher) to
  `fairyHidingMinigame.c`, **closing out the entire file** — 4 batches (296-299), 7 top-level
  routines (the dispatcher, 3 subids, 3 shared helpers). A much simpler capstone than
  `miscellaneous1.s`'s 23-entry dispatcher: just 3 clean jump-table entries with no aliasing. An
  independent review independently re-derived the 3-entry address table from `ages.sym` and
  confirmed it correct. Self-review and the independent review both found zero bugs in this final
  batch. Lint, both builds, 30k verification, and the full 289,869-frame reference replay all
  passed clean. Bank 0a is 49/928 and the project is 4,744/9,646.

- 2026-09-18: milestone 3 phase 6 batch 298, bank 0a (1 top-level routine, continuing
  `fairyHidingMinigame.s`): added `fairyHidingMinigame_subid02` (checks for Link leaving the
  hide-and-seek area) — the last subid in this file; only the top-level `interactionCode6c`
  dispatcher remains. Self-review and an independent review both found zero bugs (the review
  specifically double-checked the routine's script-address literal against the other two subids'
  script addresses in the same file, to rule out a copy-paste slip). Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 48/928
  and the project is 4,743/9,646.

- 2026-09-18: milestone 3 phase 6 batch 297, bank 0a (2 top-level routines, continuing
  `fairyHidingMinigame.s`): added `fairyHidingMinigame_subid01` (the hiding spot for a fairy, a
  4-way jump table using three different bank-0 lookup helpers — `lookupKey`, `checkFlag`,
  `setFlag`, each with its own A/HL register-passing convention) and the shared helper
  `fairyHidingMinigame_checkMinigameActive`. A register-juggling sequence in `state2`
  (`L = Interaction.var03; E = L; A = mem_rd(gb, DE); mem_wr(gb, HL, A)`) copies var03 from the
  original hiding-spot interaction into the newly-spawned fairy interaction — verified against
  `getFreeInteractionSlot`'s actual page-allocation behavior to confirm HL and DE genuinely point
  at two different interaction slots sharing the same field offset, not the same address as
  initially assumed. Self-review and an independent review both found zero bugs. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a
  is 47/928 and the project is 4,742/9,646.

- 2026-09-18: milestone 3 phase 6 batch 296, bank 0a (3 top-level routines, first slice of a
  new file): started `object_code/ages/interactions/fairyHidingMinigame.s`
  (`interactionCode6c`/INTERAC_FAIRY_HIDING_MINIGAME, `fairyHidingMinigame.c`) — this file has 7
  top-level routines total (the dispatcher, subid00-02, and 3 shared helpers), ported across
  multiple batches like `miscellaneous1.s` before it. This batch: `fairyHidingMinigame_subid00`
  (begins the minigame), `fairyHidingMinigame_spawn3FairiesAndDelete` (a 3-iteration loop calling
  `getFreeInteractionSlot_hook` independently each pass), and `fairyHidingMinigame_check-
  BeginCutscene`. Self-caught and fixed a botched loop rewrite (an accidental duplicate/reordered
  copy of the loop body) before ever building. Self-review and an independent review both found
  zero bugs in the corrected version. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 45/928 and the project is
  4,740/9,646.

- 2026-09-18: milestone 3 phase 6 batch 295, bank 0a (1 top-level routine): added
  `interactionCode6b` (INTERAC_MISCELLANEOUS_1's top-level dispatcher) to `miscellaneous1.c`,
  **closing out the entire file** — the largest and most complex file in bank 0a, spanning
  batches 276 through 295 (20 batches, 24 top-level routines: interactionCode6b itself, 20
  distinct subid handlers behind 23 dispatch entries after aliasing, and 3 shared helpers). The
  dispatcher's own correctness rests entirely on a 23-entry jump table collapsing to 19 unique
  addresses across three alias groups (subid03/12, subid0a/0b/0c, subid13/14); an independent
  review re-derived the full 23-entry address table directly from `ages.sym` from first
  principles and confirmed every mapping, alias collapse, and the final fallback case are
  correct. Along the way this file caught: one missing-fallthrough-`CYC` bug (subid01), three
  self-caught flag-polarity mistakes (subid07, subid0a, and a rejected draft), a genuinely novel
  conditional-call-to-goto-inlined-local construct (subid0d), a two-layer `CALL_C` transitivity
  case (subid0a's call into a fallthrough into a tail-jump), a cross-routine jump into another
  already-ported routine's inline body (subid05, subid15 — the latter catching a real target-vs-
  physical-end bug on an *unconditional* `jp` that silently zeroed its cycle burn via unsigned
  loop wraparound), and one purely stylistic fix. Self-review and an independent review both
  found zero bugs in this final batch. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 42/928 and the project is
  4,737/9,646.

- 2026-09-18: milestone 3 phase 6 batch 294, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid16` (a flame that appears for `[counter1]`
  frames) to `miscellaneous1.c` — the LAST of the file's ~23 subid routines. Both unconditional
  `jp`s were double-checked against the exact bug class just found and fixed in the previous
  batch (`subid15`) and both correctly use physical-end addresses. Self-review and an
  independent review both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 41/928 and the project is
  4,736/9,646. Every subid handler `interactionCode6b`'s dispatcher needs is now written; only
  the dispatcher itself remains to close out this file.

- 2026-09-18: milestone 3 phase 6 batch 293, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid15` (the stone statue of Link as seen in-game)
  to `miscellaneous1.c`. Both of its exits are tail-jumps into `interaction6b_subid0e`'s own
  inline body, so that entire body was replicated byte-for-byte inside this function too, the
  same technique already used for `subid05`'s one-instruction case. Independent review confirmed
  the duplicated section was identical to the original, but found a real bug in subid15's own
  unique code: the final unconditional `jp` used its jump target as the `CYC` end address instead
  of the instruction's own physical end, which (due to the burn_rom loop's forward-only unsigned
  comparison) silently burned zero cycles for that instruction whenever the branch was reached.
  Fixed (`CYC(0x5230, 0x50be)` → `CYC(0x5230, 0x5233)`); lint, both builds, 30k verification, and
  the full 289,869-frame reference replay all re-ran clean. Bank 0a is 40/928 and the project is
  4,735/9,646.

- 2026-09-18: milestone 3 phase 6 batch 292, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid13` (aliased with `interaction6b_subid14`, same
  bytes at the same address — the Goron bomb statue, left/right) to `miscellaneous1.c`. Self-review
  and an independent review both found zero bugs. Lint, both builds, 30k verification, and the
  full 289,869-frame reference replay all passed clean. Bank 0a is 39/928 and the project is
  4,734/9,646.

- 2026-09-18: milestone 3 phase 6 batch 291, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid11` (triggers the cutscene after beating
  Jabu-Jabu) to `miscellaneous1.c` — unusual for this file in having no `checkInteractionState`
  dispatch at all; it just runs unconditionally every frame until it deletes itself, with both
  its conditional branches converging on the same shared tail-jump into `interactionDelete_hook`.
  Self-review and an independent review both found zero bugs. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 38/928
  and the project is 4,733/9,646.

- 2026-09-18: milestone 3 phase 6 batch 290, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid10` (the unfinished stone statue of Link in the
  credits cutscene) to `miscellaneous1.c` — the largest and most complex function in this file so
  far, 63 instructions with an 8-way substate jump-table dispatch. Two substates rely on `HL`
  still pointing at `Interaction.counter1` left over from `interactionDecCounter1_hook`'s own
  internal decrement, with no redundant re-derivation — verified against that hook's actual C
  implementation to confirm the inherited-register assumption is correct. Self-review and an
  independent review (which exhaustively checked all ten conditional branches for the missing-
  fallthrough-CYC bug class) both found zero bugs. Lint, both builds, 30k verification, and the
  full 289,869-frame reference replay all passed clean. Bank 0a is 37/928 and the project is
  4,732/9,646.

- 2026-09-18: milestone 3 phase 6 batch 289, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid0f` (the switch that opens the path to Nuun
  Highlands) to `miscellaneous1.c`. A 3-way jump-table dispatch reuses the file's existing
  generic jump-table helper; `state2`'s `l5136` label re-tests flags from an earlier `or a` after
  a goto, matching the pattern already established in this file's `subid00`. Self-review and an
  independent review both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 36/928 and the project is
  4,731/9,646.

- 2026-09-18: milestone 3 phase 6 batch 288, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid0e` (the stone statue of Link that appears
  unconditionally) to `miscellaneous1.c`. A straightforward linear/branching call sequence with
  no tail-jumps at all. Self-review and an independent review (which specifically confirmed the
  `res 6,(hl)` bit-clear wasn't accidentally inverted to a bit-set) both found zero bugs. Lint,
  both builds, 30k verification, and the full 289,869-frame reference replay all passed clean.
  Bank 0a is 35/928 and the project is 4,730/9,646.

- 2026-09-18: milestone 3 phase 6 batch 287, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid0d` (blocks that move over when pulling the
  lever to get flippers) to `miscellaneous1.c`. Introduces a conditional `call nc` to a goto-
  inlined local (`@checkLinkSquished`), combining `CALL_C_CC`-style branch-conditional cycle
  accounting with the manual `push_effect`+`goto`+resume-check local-call idiom — the not-taken
  side does no push at all (matching real `call cc` semantics), and the single resume-check at
  the local's own `ret` is sufficient since it has exactly one caller. Independent review
  confirmed the construct correct and noted precedent for it already exists elsewhere in the
  project (fallingBoulderSpawner.c, lighting.c). Self-review and the independent review both
  found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame reference
  replay all passed clean. Bank 0a is 34/928 and the project is 4,729/9,646.

- 2026-09-18: milestone 3 phase 6 batch 286, bank 0a (2 top-level routines, continuing
  `miscellaneous1.s`): added `interaction6b_subid0a` (aliased with `subid0b`/`subid0c` — flippers,
  cheval rope, and bomb treasures) and the bare-global helper `interaction6b_initGraphicsAndLoad-
  Script` (falls through, no jump, into the already-hooked `interaction6b_loadScript`) to
  `miscellaneous1.c`. Self-caught a `jr nc` flag-polarity mistake before building. Traced the
  two-layer `CALL_C` transitivity through `initGraphicsAndLoadScript`'s fallthrough into
  `loadScript`'s own tail-jump into `interactionIncState_hook`, confirming subid0a's own pushed
  return address is what correctly resumes execution, matching the shallower precedent already
  verified for subid04. Self-review and an independent review (which redid the flag-polarity
  check and the full transitivity trace) both found zero bugs. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 33/928
  and the project is 4,728/9,646.

- 2026-09-18: milestone 3 phase 6 batch 285, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid09` (the shovel Rosa gives you in a linked game)
  to `miscellaneous1.c`. Introduces this session's first CONDITIONAL `call` (`call
  nz,objectSetVisible83`), modeled with the established `CALL_C_CC` macro inside an `if/else`
  where the `else` branch's bare `CYC` covers the not-taken cost. Independent review confirmed
  the polarity, addresses, and cycle accounting are all correct, and found one purely stylistic
  fix (an existing `OBJ_VAR3E` macro wasn't used in favor of the equivalent raw `0x3e` literal) —
  applied, with no behavior change. Lint, both builds, 30k verification, and the full 289,869-frame
  reference replay all passed clean. Bank 0a is 31/928 and the project is 4,726/9,646.

- 2026-09-18: milestone 3 phase 6 batch 284, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid08` (part of the cutscene where tokays steal
  your stuff) to `miscellaneous1.c`. Self-review and an independent review (which scrutinized
  all four conditional branches for the missing-fallthrough-CYC bug class already found once in
  this file) both found zero bugs. Lint, both builds, 30k verification, and the full 289,869-frame
  reference replay all passed clean. Bank 0a is 30/928 and the project is 4,725/9,646.

- 2026-09-18: milestone 3 phase 6 batch 283, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid07` (a possibly-unused seed satchel that slowly
  falls toward Link) to `miscellaneous1.c`. Self-caught a `cp`/`jp nc` flag-polarity mistake
  before ever building (Z80 `cp` sets carry on a borrow, so `jp nc`'s branch-taken condition is
  `!(F & FC)`, not `F & FC`) — an inversion here would have silently reversed whether the falling
  satchel deletes itself past a certain height, undetectable by lint or the build. Independent
  review re-verified the fix explicitly and found nothing else wrong. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 29/928
  and the project is 4,724/9,646.

- 2026-09-18: milestone 3 phase 6 batch 282, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid06` (manages the cutscene after beating Dungeon
  3 — lightning flash, screen fade, room transition to Ambi's palace) to `miscellaneous1.c`. A
  3-way substate dispatch reuses the file's existing generic jump-table helper. Self-review and
  an independent review (which scrutinized all 6 conditional branches for the missing-fallthrough-
  CYC bug class already found once in this file) both found zero bugs. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 28/928
  and the project is 4,723/9,646.

- 2026-09-18: milestone 3 phase 6 batch 281, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid05` (the intro cutscene where lightning strikes
  a guy, spawning a `PART_LIGHTNING` part from a raw byte-pair position table via RST $18) to
  `miscellaneous1.c`. A genuinely novel pattern: its top-level jump table's state-0 entry points
  at 0x4eb7, a MIDDLE address inside the already-written `interaction6b_subid02_hook`'s own
  inlined `@loadScript` local, not an independently-hookable entry point. Modeled by replicating
  that exact instruction's semantics and CYC range inline in subid05_hook too (byte-identical to
  subid02_hook's own line for the same address), since the same ROM bytes produce the same
  behavior regardless of which routine's dispatch lands on them, and `burn_rom` is fully stateless
  so there's no double-counting risk. Self-review and an independent review (which confirmed the
  `burn_rom` statelessness claim by reading its implementation) both found zero bugs. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank 0a
  is 27/928 and the project is 4,722/9,646.

- 2026-09-18: milestone 3 phase 6 batch 280, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid04` (moblins-attack-maku-sapling cutscene
  script) to `miscellaneous1.c`. Exercises a genuinely novel case for the shared
  `interaction6b_loadScript` helper: subid04 reaches it via a real `call` (unlike subid02's
  `jp` tail-jump), so loadScript's own bare, unchecked tail-jump into `interactionIncState_hook`
  ends up popping subid04's own pushed return address when `interactionIncState_hook`'s `ret`
  fires — verified via an explicit trace through `CALL_C`'s macro expansion that this resolves
  correctly with no special-casing needed: the calling side's own `CALL_C` check transparently
  detects the correct pc/sp and falls through normally, exactly matching the ROM's real behavior.
  Self-review and an independent review (which redid that exact trace) both found zero bugs.
  Lint, both builds, 30k verification, and the full 289,869-frame reference replay all passed
  clean. Bank 0a is 26/928 and the project is 4,721/9,646.

- 2026-09-18: milestone 3 phase 6 batch 279, bank 0a (2 top-level routines, continuing
  `miscellaneous1.s`): added `interaction6b_subid03` (aliased with `interaction6b_subid12`, the
  same bytes at the same address — the Seasons troupe member with a guitar/tambourine) and the
  shared bare-global helper `interaction6b_initGraphicsAndIncState` (8 callers total; only
  subid03/12's is written so far) to `miscellaneous1.c`. Confirmed the two alias names resolve to
  the same ROM address via `ages.sym`, so only one needed registration. Self-review and an
  independent review both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 25/928 and the project is
  4,720/9,646.

- 2026-09-18: milestone 3 phase 6 batch 278, bank 0a (2 top-level routines, continuing
  `miscellaneous1.s`): added `interaction6b_subid02` (Ralph/Ambi's-palace cutscene script) and the
  shared bare-global helper `interaction6b_loadScript` (the RST $18 script-table lookup used by
  subid02 now and by subid04/subid05 later) to `miscellaneous1.c`. All three of loadScript's
  callers tail-jump (`jp`, not `call`) at bare `sp0_`, so loadScript's own trailing tail-jump into
  `interactionIncState_hook` needs no resume-check. Self-review and an independent review (which
  re-scrutinized every conditional branch for the missing-fallthrough-CYC bug class just found in
  the previous batch) both found zero bugs. Lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 23/928 and the project is
  4,718/9,646.

- 2026-09-18: milestone 3 phase 6 batch 277, bank 0a (1 top-level routine, continuing
  `miscellaneous1.s`): added `interaction6b_subid01` (spawns Nayru, Ralph, and the animals in the
  intro cutscene before Nayru is possessed) to `miscellaneous1.c`. Independent review caught a
  real bug: the not-taken side of a conditional `jp c,interactionDelete` was missing its `CYC`
  cycle burn entirely (3 cycles silently dropped whenever the carry flag was clear) — fixed by
  adding the missing `CYC(0x4ea6, 0x4ea9);` before the final `ret`, matching the established
  sibling convention used everywhere else in this file and project. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all re-ran clean after the fix. Bank
  0a is 21/928 and the project is 4,716/9,646.

- 2026-09-18: milestone 3 phase 6 batch 276, bank 0a (2 top-level routines, first slice of a
  much larger file): started `object_code/ages/interactions/miscellaneous1.s`
  (`interactionCode6b`/INTERAC_MISCELLANEOUS_1, `miscellaneous1.c`) — this source file has ~20
  independently-hookable top-level routines (`interactionCode6b`'s dispatcher plus
  `interaction6b_subid00` through `subid16`, several shared helpers), by far the largest bank-0a
  file, so it's being ported across several batches. This batch: `interaction6b_subid00` (shows
  Impa's "Help" text when Link's about to screen-transition) and its bare-global helper
  `interaction6b_checkLinkPressedUpAtScreenEdge`. `decCounter1IfTextNotActive`'s two exits (an
  early `ret nz` and a tail-jump into `interactionDecCounter1_hook`) both resume at the same
  bare-`sp0_` point in `@substate1`, which then does its OWN separate `ret nz` re-checking
  whichever flags the resumed exit left behind — verified that `push_effect`/`pop_effect`/
  `ret_effect` never touch F, so the flag pass-through is sound. Self-review and an independent
  review (focused specifically on that flag pass-through) both found zero bugs. Lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all passed clean. Bank
  0a is 20/928 (interactionCode6b itself still needs the other ~19 subids before its dispatcher
  can be written) and the project is 4,715/9,646.

- 2026-09-18: milestone 3 phase 6 batch 275, bank 0a (1 root routine): ported
  `object_code/ages/interactions/cheval.s` (`interactionCode6a`, `cheval.c`) — Cheval. Has three
  `rst $00` jump-table dispatch sites, but only the top-level state0/state1 one is genuinely
  2-way; the two nested subid dispatches (inside state0 and state1) each have exactly one `.dw`
  entry in the ROM, so they're modeled by still calling the shared jump-table helper for its real
  cycle cost and side effects, then following an unconditional `goto` without branching on its
  returned target (there is nowhere else for it to go). No internal `call`s anywhere in the file —
  every transfer is a jr/jp/jump-table goto — so no push_effect/resume-check machinery was needed.
  Self-review and an independent review both found zero bugs. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean. Bank 0a is 18/928
  and the project is 4,713/9,646.

- 2026-09-18: milestone 3 phase 6 batch 274, bank 0a (1 root routine): ported
  `object_code/ages/interactions/rafton.s` (`interactionCode69`, `rafton.c`) — Rafton, the
  raft-shop NPC. Unusually simple for this bank: every internal transfer (three separate `rst $00`
  jump-table dispatches, several `jr`/`jp` exits) is a pure goto, never a genuine `call`, so no
  `push_effect`/resume-check machinery was needed anywhere — `sp0_` is declared only because the
  `CALL_C` macro itself references it. All three jump-table dispatch sites correctly reuse the
  same `interactionCode69_jump_table` helper (each with its own `push_effect` of a distinct return
  address beforehand), matching the established precedent from `essence.c`. Self-review and an
  independent review (which also read the raw RST $00 handler in the disassembly to confirm it
  ends in `jp hl` rather than `ret`, justifying the no-resume-check design) both found zero bugs.
  Lint, both builds, 30k verification, and the full 289,869-frame reference replay all passed
  clean. Bank 0a is 17/928 and the project is 4,712/9,646.

- 2026-09-18: milestone 3 phase 6 batch 273, bank 0a (1 root routine): ported
  `object_code/ages/interactions/rosa.s` (`interactionCode68`, `rosa.c`) — Rosa the Subrosian
  dancer, with 5 call-reached HOOK_LOCALs all goto-inlined into the one function
  (`initGraphicsAndLoadScript`, `loadScriptFromTableAndInitGraphics`, `loadScriptAndIncState`,
  `loadScriptFromTableAndIncState`, `getScript`). The trickiest nesting pattern encountered since
  the roller.c/vasu.c stack-pointer bugs: `initGraphicsAndLoadScript` and
  `loadScriptFromTableAndInitGraphics` are each called once from a state handler (bare `sp0_`),
  then `jr` at that same depth into `loadScriptAndIncState`/`loadScriptFromTableAndIncState`,
  which each make a further nested call into the shared `getScript` (requiring its two resume
  checks to compare against `sp0_-2`) before ending with an unconditional `jp interactionIncState`
  — a tail jump into an already-hooked external routine whose own `ret_effect` is what actually
  pops the still-outstanding *outer* call and resumes this function, needing the established
  tail-jump-then-check pattern rather than CALL_C. A genuinely dead "Unused" 3-instruction block
  in the ROM (`initGraphicsAndIncState`, 0 callers, no fallthrough) was confirmed unreachable and
  omitted rather than modeled. Self-review and an independent review explicitly primed on this
  exact bug class both found zero bugs; lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean. Bank 0a is 16/928 and the project is
  4,711/9,646.

- 2026-09-18: milestone 3 phase 6 batch 272, bank 0a (1 root routine): ported
  `object_code/common/interactions/bubble.s` (`interactionCode91`, `bubble.c`) — the underwater
  bubble hazard, with two subids (a moving/killable bubble and a stationary bubble spawner) and
  one HOOK_LOCAL, `@checkDelete`, reached from both subid-00 states at the same top-level stack
  depth (no nested `push_effect` complexity here, unlike the prior two files). Self-review caught
  one target-vs-physical-end `CYCT` mistake before build. Independent review then found a real,
  execution-invisible off-by-one: `subid01_state0`'s closing `ret` at 0x4a38 was marked as
  `RET(0x4a39)` (one byte past the actual `ret` opcode, into `checkDelete`'s first instruction) —
  wrong `burn_rom` byte range and wrong PC anchor, though the M-cycle total for the block was
  unaffected, which is presumably why no timing check could have caught it. Fixed; lint, both
  builds, 30k verification, and the full 289,869-frame reference replay all re-ran clean. Bank 0a
  is 15/928 and the project is 4,710/9,646.

- 2026-09-18: milestone 3 phase 6 batch 271, bank 0a (1 root routine, plus a same-batch fix to
  the batch-267 `roller.c`): ported `object_code/common/interactions/vasu.s`
  (`interactionCode89`, `vasu.c`) — Vasu's ring-appraisal shop, its two snake NPCs, and the
  blue-snake GBC-link ring-transfer minigame (SVBK-bank-switched reads of `w4RingFortuneStuff`
  via the standard `push af`/switch-to-bank-4/`pop af` idiom). The densest goto-inlined single
  function so far: `@updateState` (itself one `push_effect` deep from the top level) contains a
  6-state machine, one of whose states (`@state5`) has its own 5-way substate dispatch, plus two
  further HOOK_LOCAL callees (`@checkRingBoxAndRingsObtained`, two callers;
  `@setScriptAndGotoState4`, reached both by fallthrough and by a genuine nested call). Discovered
  while investigating this file's nesting: a real, subtle bug in the just-committed-to-staging
  `roller.c` (batch 267) — a resume-check for a doubly-nested `HOOK_LOCAL` compared the stack
  pointer against the wrong reference (`sp0_` instead of `sp0_-2`), invisible to every prior test
  and review since it only affects which path resumes in native code vs. the interpreter, never
  the resulting game state. Fixed that first (independent review confirmed the fix and found no
  other instance in that file), then wrote `vasu.c` from scratch applying the corrected
  understanding — and STILL got the analogous case backwards in the new file (captured `sp1_`
  after its push instead of before, backwards from `roller.c`'s mistake), catching that only via
  a third, explicitly-arithmetic independent review pass after self-review found 11 *other* real
  bugs first (a wrong immediate value, `alu_rl` used where `rla`'s different flag behavior was
  needed, a merged `ldd`+`dec` treated as one instruction, and 8 instances of the recurring
  target-vs-physical-end `CYCT` mistake). New porting-notes lesson on the nested-stack-pointer
  pattern, written up in detail given how expensive both mistakes were to find. Lint, both builds,
  30k verification, and the full 289,869-frame reference replay all passed clean after every fix
  (this TAS route never exercises Vasu, so only the byte-level checks and reviews could have
  caught any of this). Bank 0a is 14/928 and the project is 4,709/9,646.

- 2026-09-18: milestone 3 phase 6 batch 270, bank 0a (4 top-level routines): ported
  `object_code/common/interactions/essence.s` (`interactionCode7f` + `interaction7f_subid00` +
  `interaction7f_subid01` + `interaction7f_subid02`, `essence.c`) — the dungeon essence pickup,
  its pedestal, and its glow effect; the largest and most complex bank-0a file to date.
  `interaction7f_subid00` alone is an 8-state, 181-instruction machine covering the essence's
  entire idle/approach/fall/pickup/cutscene/warp-out lifecycle, with three RST $10 lookups
  (sprite data, float-offset animation, per-essence get-text index) and one RST $18 lookup
  (per-essence warp destination table), correctly modeling only the ROM_AGES `.ifdef` branches
  throughout (dungeon-6 override, essence sprite/warp data tables). `interaction7f_subid02` has
  a `@copyEssencePosition` HOOK_LOCAL reached by one genuine call, ending in a tail-jump into the
  already-hooked `objectTakePosition`. Extra care taken with `ldi`/`inc hl`/`inc l` distinctions
  given several of each appear back to back in `@state0` and `@state4`. Zero bugs found by
  self-review or independent review; lint, both builds, 30k verification, and the full
  289,869-frame reference replay all passed clean (this TAS route apparently never reaches an
  essence pickup, so only the static byte-level checks could have caught a bug here). Bank 0a is
  13/928 (4 root routines added) and the project is 4,708/9,646.

- 2026-09-18: milestone 3 phase 6 batch 269, bank 0a (1 root routine): ported
  `object_code/common/interactions/minibossPortal.s` (`interactionCode7e`, `minibossPortal.c`)
  — the dungeon miniboss-defeat portal and its Hero's Cave warp-point variant, ROM_AGES branch
  only (Seasons-only `@subid02`/`wc64a` paths correctly omitted). A 14-label internal family, an
  RST $10 single-index lookup feeding an RST $18 double-index lookup in `@minibossState0`, and
  `@spinLink` — a HOOK_LOCAL with two genuine callers and both a literal-ret exit and a
  tail-jump-into-`interactionDecCounter1` exit, needing a two-way resume check at every exit.
  `@commonState0` is reached two ways (a pure fallthrough from `@minibossState0`, and a genuine
  `jp @commonState0` from `@herosCaveState0`) with neither needing any resume-check machinery
  since neither is a `call`. Given the target-vs-physical-end bug just caught in the immediately
  preceding batch's independent review, this batch's review prompt explicitly flagged that pattern
  for extra scrutiny at the two similar-looking branches in this file — both were correct.
  Independent review found zero bugs. Bank 0a is 9/928 and the project is 4,704/9,646.

- 2026-09-18: milestone 3 phase 6 batch 268, bank 0a (4 top-level routines): ported
  `object_code/common/interactions/spinner.s` (`interactionCode7d` + `spinner_subid02` +
  `spinner_updateLinkPosition` + `spinner_setLinkRelativePosition`, `spinner.c`) — the
  blue/red spinner puzzle and its rotating arrow child object. Four independently-hooked
  top-level routines in one file: `interactionCode7d` dispatches through a 3-way then a 5-way
  RST $00 table (one entry tail-calling the already-hooked `spinner_subid02`, another
  tail-calling the already-hooked `interactionRunScript`); `spinner_updateLinkPosition` ends
  with a genuine ROM-level fallthrough (no `ret`, no `jp`) straight into
  `spinner_setLinkRelativePosition`, modeled as a bare tail C call; `spinner_setLinkRelativePosition`
  has an internal `@func` HOOK_LOCAL called once via a real `call` and fallen into a second time
  with no call at all, needing a dual-path `ret` exit (goes to an internal label if the resume
  address matches, otherwise a plain `return` for the fallthrough invocation, whose real return
  address belongs to the function's own external caller). Independent review caught 3 real
  bugs — the recurring target-vs-physical-end mistake on three `CYCT` ranges (using the branch's
  jump target as the burn end instead of the physically-next instruction, corrupting cycle counts
  on the taken side; already well-documented in this file's own lessons, so no new entry added)
  plus a style nit (raw hex instead of the existing `OBJ_VAR39`/`OBJ_VAR3A` macros) — fixed and the
  full gate re-run clean. None of these four routines were exercised by the TAS route in either
  verify pass, so only the byte-level report cross-check and independent review could have caught
  them. Bank 0a is 8/928 (4 root routines added) and the project is 4,703/9,646.

- 2026-09-18: milestone 3 phase 6 batch 267, bank 0a (1 root routine): ported
  `object_code/common/interactions/roller.s` (`interactionCode7a`, `roller.c`) — the pushable
  roller puzzle object, the densest bank-0a file yet: a 16-label internal family, a 3-way RST $00
  dispatch, and three call-reached `HOOK_LOCAL` locals (`@preventLinkFromPassing` with two callers
  and three exit types, `@checkRollerCanBePushed`/`@nextTile`, and
  `@updateLinkPositionWhileRollerMoving`/`@squashLink`). Following the `veranAcidPool.c` precedent,
  every local stayed `goto`-inlined inside the one top-level `_hook` function rather than factored
  into separate C functions, after discovering mid-batch that `lint_game.py`'s "emulated register
  outside a _hook shim" rule only recognizes a literal `void NAME_hook(GB *gb)` signature line —
  a `static` helper touching `gb->pc`/`gb->sp` always trips it. Self-review caught and fixed 4 bugs
  before the first build (two `alu_add` void-misuses, one `ldi (hl),a` incorrectly using
  flag-affecting `alu_inc8` instead of `SET_HL`, one broken placeholder line). Independent review
  then caught 2 further real bugs: `@checkRollerCanBePushed`'s early `ret nz` and all five exits of
  `@updateLinkPositionWhileRollerMoving`/`@squashLink` were missing their resume check back to the
  caller, silently falling back to the interpreter on those paths (behaviorally correct, so no
  test caught it, but defeating the rewrite on those edges) — fixed and the full gate (build×2,
  ctest×2, 30k verify, full 289,869-frame reference replay) re-run clean. Two new porting-notes
  lessons recorded (the `HOOK_LOCAL`-must-stay-`goto`-inlined pattern, and a `lint_game.py` false
  positive from a flush-left comment containing a parenthesis). Bank 0a is 4/928 and the project
  is 4,699/9,646.

- 2026-09-18: milestone 3 phase 6 batch 266, bank 0a (1 root routine): ported
  `object_code/common/interactions/movingPlatform.s` (`interactionCode79`, `movingPlatform.c`) —
  a script-driven moving platform with a 9-label internal family (2 RST $00 dispatches, an
  RST $18 double-index collision-size lookup, and `@checkLinkTouching`, a private local reached
  by two genuine `call`s and ending in a tail-jump into the already-hooked
  `interactionCheckContainsPoint`). Confirmed via the same test-then-register procedure as batch
  265 that none of the 9 sub-labels needed `rewritten.txt` entries; `checkLinkTouching`'s two call
  sites hand-inline `push_effect`/direct-call/`pc==ra && sp==sp0_` instead of `CALL_C`, since it
  has no independent hook to dispatch through. Self-caught and fixed a byte-range bug in the
  second RST $00 dispatch (burned the whole opcode+table span instead of the 1-byte opcode) before
  the first build. Bank 0a is 3/928 and the project is 4,698/9,646. Lint, both builds, 30k
  verification, and the full 289,869-frame reference replay all passed clean (this routine went
  unexercised by the TAS route in all three); independent review of the whole file found zero bugs.

- 2026-09-18: milestone 3 phase 6 batch 265, bank 0a (1 root routine): ported
  `object_code/common/interactions/switchTileToggler.s` (`interactionCode78`, `switchTileToggler.c`)
  — a crystal-switch-driven floor tile toggle, dispatched through a 2-way RST $00 jump table and
  an RST $18 double-index lookup into a 24-entry on/off tile-pair table. `@state0`/`@state1` are
  purely internal (no external callers), so per lint they stay goto-inlined without their own
  `rewritten.txt` entries — only the top-level `interactionCode78` needed registering; adding the
  sub-labels caused them to vanish from `generated.txt` entirely and made lint fail with missing
  `_hook` entries, since the tool only keeps `_hook`-suffixed generated entries for names that are
  independently reachable. New porting-notes lesson. Bank 0a is 2/928 and the project is
  4,697/9,646. Lint, both builds, 30k verification, and the full 289,869-frame reference replay
  all passed clean; independent review found zero bugs.

- 2026-09-18: milestone 3 phase 6 batch 264, bank 0a (1 root routine): ported
  `object_code/common/interactions/bombFlower.s` (`interactionCode6f`, `bombFlower.c`) — first
  bank 0a routine. In the ROM_AGES build the entire Seasons-only bomb-flower state machine
  (`bomb_flower_subid0`/`subid1`) is compiled out by `.ifdef`, leaving a single unconditional
  `jp interactionDelete`. Confirmed no Ages-side subid labels exist in `ages.sym` before writing
  the trivial tail-jump. Independent review caught a real convention slip (`CYCT` used for an
  unconditional `jp`, should be plain `CYC` — no timing effect since `insn_cycles` ignores the
  taken flag for opcode 0xc3, but fixed for consistency with every other unconditional-jp file).
  Bank 0a is 1/928 and the project is 4,696/9,646. Lint, both builds, 30k verification, and the
  full 289,869-frame reference replay all passed clean after the fix.

- 2026-09-16: milestone 3 phase 6 batch 263, bank 11 (1 root routine): ported
  `object_code/ages/parts/triforceStone.s` (`partCode5a`, `partCode5a.c`) — the stone
  blocking the path to Nayru at the start of the game: a one-shot state that deletes itself
  if the room was already solved, otherwise picks an X position based on which room-flag
  bit is set, makes its tile solid, and reloads the room's palette. Named `partCode5a.c`
  rather than `triforceStone.c` because a DIFFERENT, unrelated bank-08 file already used
  that name for `interactionCode34_hook` (the pushable-stone puzzle, ported from a
  same-named but different-directory `.s` source) — caught mid-batch when a build failed on
  a missing symbol after the new file had silently overwritten the old one; the original was
  recovered from git history and the new port renamed to avoid the collision. Zero further
  bugs found by self-review or independent review, which also explicitly re-verified the
  bank-08 file was restored byte-for-byte and both files build without symbol collisions.
  **This is the last portable routine in bank 11.** The one remaining unregistered address
  (`func_11_7f64`, in the disassembly's own `garbage/bank11End.s`) is dead code with zero
  callers anywhere in the ROM whose final instruction jumps into the middle of the
  already-ported bank-0 `getRelativeAngle_hook` — exactly the "jumps into rewritten X at
  offset (interpreted)" situation that has shown up as a harmless, permanently-tolerated
  regen warning all session; porting it would require exposing a new mid-function label in
  an unrelated, already-verified bank-0 file for code nothing ever calls, so it is left
  interpreted. Bank 11 is 227/651 top-level routines registered (functionally complete) and
  the project is 4,440/9,927. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 262, bank 11 (1 root routine): ported
  `object_code/ages/parts/blackTowerMovingFlames.s` (`partCode59`, `blackTowerMovingFlames.c`)
  — the moving flame hazard in the Black Tower: a 6-state RST $00 dispatch picking a speed
  from a 4-entry table (reused twice, indexed by subid and its complement) for two facing
  directions, a state that makes a genuine cross-bank call into
  `blackTower_getMovingFlamesNextTileCoords` (bank 0x10) via `interBankCall` — the first
  `callab` this session, modeled as a plain `CALL_C` to the already-hooked
  `interBankCall_hook` with no special resume logic needed — and a movement state that
  homes toward the next tile coordinate once it stops matching the current position. Zero
  bugs found by self-review or independent review. Bank 11 is 226/651 and the project
  4,439/9,926. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 289,869 frames with state
  `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 261, bank 11 (1 root routine): ported
  `object_code/ages/parts/veranBeeProjectile.s` (`partCode58`, `veranBeeProjectile.c`) — the
  bee-shaped projectile fired by Veran's boss form: an entry check deleting itself invisibly
  on a wall collision, a 4-state RST $00 dispatch (spawn with a beam sound, track its
  carrier until in range then home in and delete once close enough, and a final state that
  raises a scent-seed-like trigger flag and immobilizes Link if a game key was just pressed
  while adjacent). One of the densest branch counts this session (eight conditionals). Zero
  bugs found by self-review or independent review. Bank 11 is 225/651 and the project
  4,438/9,925. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 289,869 frames with state
  `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 260, bank 11 (1 root routine): ported
  `object_code/ages/parts/veranAcidPool.s` (`partCode57`, `veranAcidPool.c`) — the most
  structurally complex file this batch: a 7-state RST $00 dispatch for Veran's acid pool
  attack, with a shared "set tiles from a table" helper (`func_7de1`, a 4-iteration inner
  loop with register-preserving `push`/`pop` around a nested tile-set call) and a shared
  "check/clear a tile" helper (`func_7db7`) reached both by fallthrough and by a genuine
  internal `call` from the final state — the first time this session a `HOOK_LOCAL` block
  needed the same `gb->pc`/`gb->sp` resume check at BOTH a literal `ret` exit and a
  tail-jump-into-an-external-hook exit, documented as a new porting-notes lesson (a
  straightforward union of two previously-separate precedents). Zero bugs found by
  self-review or a dedicated independent review focused entirely on tracing that mechanism.
  Bank 11 is 224/651 and the project 4,437/9,924. Gates: lint 0, 30k verify 0 failures with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869
  frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 259, bank 11 (1 root routine): ported
  `object_code/ages/parts/octogonBubble.s` (`partCode55`, `octogonBubble.c`) — the bubble
  fired by an Octogon enemy: an entry check for a Link collision that, when Link is
  vulnerable, immobilizes him and switches the bubble to a collided state; a 4-state RST
  $00 dispatch (spawn, float forward with a 4-entry sine-like z-position table, wait for the
  destroy animation to finish, then take Link's position while collided and animate once he
  starts standing back up). Zero bugs found by self-review or independent review. Bank 11 is
  223/651 and the project 4,436/9,923. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 258, bank 11 (4 root routines): ported
  `object_code/ages/parts/roomOfRitesFallingBoulder.s` (`partCode54` + `func_7ad3` +
  `func_7adb` + `func_7b0a`, `roomOfRitesFallingBoulder.c`) — four independently-registered
  top-level routines wired by bare tail-calls, forward-declared where called before their
  definition. `partCode54` dispatches on subid/state to either the "already spawned, waiting
  to respawn" path (`func_7ad3`) or the "not yet spawned" path (`func_7adb`), which computes
  a random spawn position from the camera offset and falls into the falling/landing update
  loop (`func_7b0a`), which spawns rock debris and deletes on hazard contact. Caught and
  fixed three missing `sp0_` declarations during self-review before building. Zero further
  bugs found by self-review or a detailed independent review of all four routines. Bank 11
  is 222/651 and the project 4,435/9,922. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 257, bank 11 (1 root routine): ported
  `object_code/ages/parts/ramrockSeedFormOrb.s` (`partCode4f`, `ramrockSeedFormOrb.c`) —
  the orb fired by Ramrock's seed-form attack: a spawn state, a tracking state waiting for
  the related object to reach the right animation frame before aiming and speeding toward
  Link, and a homing state nudging its angle toward Link every 10 frames while applying
  speed and deleting once it leaves the screen. Zero bugs found by self-review or
  independent review. Bank 11 is 218/651 and the project 4,431/9,918. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 256, bank 11 (1 root routine): ported
  `object_code/ages/parts/smogProjectile.s` (`partCode4a`, `smogProjectile.c`) — the
  projectile fired by a Smog enemy: a spawn state aiming toward Link and, for the large
  smog's variant, boosting its speed/collision mode; a tracking state deleting on boss
  defeat, screen exit, or a single-remaining-enemy check, then either passing through
  everything (large-smog subid) or checking for an item/wall collision to trigger a destroy
  animation; and a final animate-then-delete state. The densest branch count of any file
  this session (eight conditionals), each verified independently by both self-review and
  independent review. Zero bugs found. Bank 11 is 217/651 and the project 4,430/9,917. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk
  suites 8/8.
- 2026-09-16: milestone 3 phase 6 batch 255, bank 11 (3 root routines): ported
  `object_code/ages/parts/octogonDepthCharge.s` (`partCode48` + `octogonDepthCharge_subid1`
  + `octogonDepthCharge_subid0`, `octogonDepthCharge.c`) — the biggest file this session:
  three independently-registered top-level routines wired by bare tail-calls. `partCode48`
  dispatches on subid via `normalStatus`, conditionally tail-jumping into the externally-hooked
  `octogonDepthCharge_subid0` or bare-falling-through into `octogonDepthCharge_subid1` (a
  forward-declared sibling function defined later in the file). `subid1` is the small split
  projectile (spawn then fly-and-delete-on-collision). `subid0` is the large projectile before
  splitting: a 4-state RST $00 dispatch that spawns either already underwater (random position
  from a 4-entry table) or shot up first, floats, delays, then splits into four `subid1`
  children on landing. Caught and fixed two self-review issues before building (a missing
  `sp0_` local in `octogonDepthCharge_subid1_hook`, and a stray unused `belowWater:` label with
  no goto reaching it). Zero further bugs found by self-review or a detailed independent review
  of all three routines. Bank 11 is 216/651 and the project 4,429/9,916. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 254, bank 11 (1 root routine): ported
  `object_code/ages/parts/bomb.s` (`partCode47`, `bomb.c`) — the standard thrown bomb: a
  4-state RST $00 dispatch (spawn with gravity, wait to be thrown while tracking its
  carrier, apply speed until it lands and switch to an exploding animation/sound, then
  shrink and delete once the explosion animation ends). The first `call playSound` this
  session that is a genuine call-and-return rather than a tail-jump (the source does `call
  playSound` followed by a separate `jp objectSetVisible83`), modeled as `CALL_C` rather
  than the usual `playSound_b00_hook(gb); return;` terminal pattern. Zero bugs found by
  self-review or independent review, which specifically verified that distinction. Bank 11
  is 213/651 and the project 4,426/9,913. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 253, bank 11 (1 root routine): ported
  `object_code/ages/parts/seedShooterEyeStatue.s` (`partCode46`, `seedShooterEyeStatue.c`) —
  the seed-shooter eye statue enemy's tracking part: an inverted-sense entry guard (flags
  inherited from the caller, like `partCode3c.c`'s) that on first activation sets an active
  trigger flag and makes the statue's eye visible, then a normal-status path that counts
  down and clears the trigger/hides the eye once expired. Zero bugs found by self-review or
  independent review. Bank 11 is 212/651 and the project 4,425/9,912. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 252, bank 11 (1 root routine): ported
  `object_code/ages/parts/fallingBoulderSpawner.s` (`partCode45`, `fallingBoulderSpawner.c`)
  — the falling boulder hazard: a spawn state picking an initial appear delay from a
  per-subid table, a waiting state that transitions into a shared "bounce" block once its
  delay expires, and a falling state that conditionally re-enters that same bounce block via
  a genuine `call z` whenever the boulder's vertical speed update reports it hit the ground.
  The bounce block never executes its own `ret`; it always terminates by tail-jumping into
  `playSound`, so the two ways of reaching it are told apart after the call returns by
  checking `gb->pc`/`gb->sp` against the pushed resume address — generalizing the
  HOOK_LOCAL-tail-jumps-into-an-external-hook pattern from `rotatableSeedThing.c` to a
  *conditional* internal call for the first time this session. Zero bugs found by self-review
  or a dedicated independent review focused entirely on tracing that mechanism. Bank 11 is
  211/651 and the project 4,424/9,911. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 251, bank 11 (1 root routine): ported
  `object_code/ages/parts/tingleBalloon.s` (`partCode44`, `tingleBalloon.c`) — Tingle's
  balloon: a spawn state setting a fixed lifetime and floating speed, an update state that
  reverses its vertical float direction (via a `cpl`/`inc a` two's-complement negation) once
  its lifetime counter expires and continues tracking Tingle's own z-position, and a
  been-hit path that bumps Tingle's own state, spawns an explosion interaction, and deletes.
  Zero bugs found by self-review or independent review. Bank 11 is 210/651 and the project
  4,423/9,910. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 289,869 frames with state
  `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 250, bank 11 (1 root routine): ported
  `object_code/ages/parts/plasmarineProjectile.s` (`partCode43`, `plasmarineProjectile.c`) —
  the projectile fired by the Plasmarine boss: the first file this session with zero RST
  jump tables, using pure straight-line `jr`/`jp` control flow instead — a lifetime/collision
  state checking the projectile's color against the boss's, homing in on Link once its
  lifetime counter runs low, damaging the boss (and breaking its shell at zero health) on a
  color-matched collision, and a spawn state whose fire speed depends on the boss's remaining
  health. Independent review caught a real bug: the "flicker visibility in the last 30 frames"
  check had inverted `jr nc` polarity, which would have flickered visibility for the entire
  lifetime EXCEPT the last 30 frames instead of only during them; fixed and the full gate was
  re-run clean. Bank 11 is 209/651 and the project 4,422/9,909. Gates: lint 0, 30k verify 0
  failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 249, bank 11 (1 root routine): ported
  `object_code/ages/parts/pumpkinHeadProjectile.s` (`partCode42`, `pumpkinHeadProjectile.c`) —
  the projectile fired by the Pumpkin Head enemy: verifies the caller wants a live instance
  (else deletes), a spawn state computing a spread angle from a per-subid offset table and a
  loop spawning two additional projectiles at fixed angle deltas from a second table (using
  `addAToBc` rather than the more common `rst_addAToHl` for the first table lookup), a chase
  state waiting out a counter before making itself visible, and a movement state applying
  speed/animation and deleting on tile collision or out-of-bounds. Zero bugs found by
  self-review or independent review. Bank 11 is 208/651 and the project 4,421/9,908. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk
  suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 248, bank 11 (1 root routine): ported
  `object_code/ages/parts/shadowHagShadow.s` (`partCode41`, `shadowHagShadow.c`) — the shadow
  clones chased by/chasing the Shadow Hag: a 4-way RST $00 dispatch whose 4th entry points
  directly at the external `partDelete` hook (rather than a local label), spawn state picking
  a random one of four fixed angles, a chase state nudging its angle toward Link once the hag's
  own counter1 signals a converge command, and a converge-back state doing a two-axis proximity
  check against the hag's position before flipping to a self-deleting state. Independent review
  caught a real bug: both `jr nc` proximity checks had inverted polarity (`F & FC` instead of
  `!(F & FC)`), which would have made the shadow jump straight to the angle-update path when
  CLOSE to the hag instead of when far; fixed and the full gate was re-run clean. Bank 11 is
  207/651 and the project 4,420/9,907. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 247, bank 11 (1 root routine): ported
  `object_code/ages/parts/headThwompBombDropper.s` (`partCode40`, `headThwompBombDropper.c`) —
  the bomb dropped by the Head Thwomp miniboss: verifies the related object is still the
  thwomp itself (else deletes), applies gravity until it either lands (then falls into
  `objectApplySpeed`/copies the dropper's position and continues) or times out, and a spawn
  state that picks a random horizontal speed and vertical velocity pair from two 4-entry
  tables via `rst_addAToHl`, plus a random horizontal angle offset. Caught and fixed one
  self-review mistake before building (`alu_swap` misused as a void mutator instead of using
  its return value). Zero further bugs found by self-review or independent review. Bank 11 is
  206/651 and the project 4,419/9,906. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 246, bank 11 (1 root routine): ported
  `object_code/ages/parts/3e.s` (`partCode3e`, `partCode3e.c`) — tracks the enemy index of
  every loaded Ambi Guard in `Part.var30-3f`: a 4-state RST $00 dispatch (state0 scans the
  full enemy table recording each Ambi Guard's slot, state1 waits for a related object's
  collision type to settle before recording it, state2 stamps every tracked Ambi Guard with a
  shared relatedObj2 pointer, state3 is a simple countdown). The first file this session with
  real backward-branching loops (`jr c,-` style), modeled as C `goto` back to `L_<addr>:`
  labels rather than the straight-line/goto-forward style of every prior batch. Zero bugs found
  by self-review or independent review. Bank 11 is 205/651 and the project 4,418/9,905. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk
  suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 245, bank 11 (1 root routine): ported
  `object_code/ages/parts/headThwompCircularProjectile.s` (`partCode3c`,
  `headThwompCircularProjectile.c`) — the circling projectile fired by the Head Thwomp
  miniboss: an inverted-sense guard at entry (`jp nz,partDelete` with no preceding compare,
  relying on flags inherited from the caller), a spawn state setting its initial speed and
  visibility, and an update state that advances its orbit angle from a toggling direction bit
  in `var30` before applying speed and animating. Zero bugs found by self-review (after fixing
  one self-caught mistake before building: a missing `sp0_` local broke `CALL_C` compilation)
  or independent review. Bank 11 is 204/651 and the project 4,417/9,904. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 244, bank 11 (1 root routine): ported
  `object_code/ages/parts/3b.s` (`partCode3b`, `partCode3b.c`) — the boulder thrown by the head
  thwomp (purple face) miniboss: a top-level dispatcher that branches on subid to either the
  "core" boulder (its own 3-state RST $00 dispatch: spawn with random position offset from the
  camera, apply gravity until it lands or times out then switches to a break animation/sound,
  and a third state that swaps its collision radius via a second `rst_addAToHl` table) or a
  thrown instance (a near-duplicate 3-state dispatch whose third jump-table entry deliberately
  reuses the core boulder's `state2` code). Every sub-label is `HOOK_LOCAL`, so all of it is
  goto-inlined into one C function; only `partCode3b` itself is a top-level hook. Zero bugs
  found by self-review or independent review. Bank 11 is 203/651 and the project 4,416/9,903.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk
  suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 243, bank 11 (1 root routine): ported
  `object_code/ages/parts/headThwompFireball.s` (`partCode39`, `headThwompFireball.c`) — the
  fireball spat out by the Head Thwomp miniboss: a 3-state RST $00 dispatch that spawns with a
  random angle/speed pair looked up via `rst_addAToHl`, applies its speed and gravity each frame
  until it either times out or hits a tile (switching to a "break" animation and sound), and a
  third state that turns the animation-parameter high bit into a check that swaps its collision
  radius via a second `rst_addAToHl` table before falling into shared animation. Independent
  review caught the recurring "unconditional jump end = branch target, not `from+len`" bug on
  the first `jp playSound_b00` (its `CYC` end had swallowed 4 bytes of the intervening
  `table_6c66` speed-lookup table); fixed and the full gate was re-run clean. Bank 11 is
  202/651 and the project 4,415/9,902. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 242, bank 11 (3 root routines): ported
  `object_code/ages/parts/veranProjectile.s` (`partCode37` + `veranProjectile_subid0` +
  `veranProjectile_subid1`, `veranProjectile.c`) — Veran's projectile attack: a thin dispatcher
  (`partCode37`) that tail-jumps to `veranProjectile_subid1` (an individual fired projectile,
  its own 3-state RST $00 dispatch) or falls straight through into `veranProjectile_subid0`
  (the "core" spawner, also its own 3-state RST $00 dispatch, firing new projectiles every 8
  frames at an angle derived from its own counter1). All three routines are independently
  registered (non-`HOOK_LOCAL`), so this is three separate top-level `_hook` functions in one
  file, wired together with bare tail-calls (`jp`/fallthrough, no `push_effect` needed) rather
  than the goto-inlining used for `HOOK_LOCAL` blocks. Zero bugs found by self-review or
  independent review. Bank 11 is 201/651 and the project 4,414/9,901. Gates: lint 0, 30k verify
  0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 241, bank 11 (1 root routine): ported
  `object_code/ages/parts/candleFlame.s` (`partCode36`, `candleFlame.c`) — the flame effect on a
  lit candle enemy: verifies the related object is still a candle (else deletes), a 3-state RST
  $00 dispatch tracking the parent candle's speed to switch animation once it stops moving, then
  deletes if the parent's collision mode became Podoboo, otherwise syncs position and continues
  animating. First file this session with a genuine `push bc`/`pop bc` register save-restore
  across a `call` (not a call/return-address push) — modeled as `push_effect(gb, BC)` /
  `SET_BC(pop_effect(gb))`, confirmed by independent review to match established precedent in
  `commonBombAndBraceletCode.c`. Zero bugs found by self-review or independent review. Bank 11
  is 198/651 and the project 4,411/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 240, bank 11 (1 root routine): ported
  `object_code/ages/parts/ramrockSeedFormLaser.s` (`partCode34`, `ramrockSeedFormLaser.c`) —
  Ramrock's seed-form laser attack: tracks Link's x position while charging, pulsates between
  red and blue every 4 frames, then a 5-way RST $00 dispatch (4 local states plus a direct
  `partDelete` table entry) handling windup, a chain of self-respawning laser segments (each
  spawned copy inherits `var03+1`, `relatedObj1` pointing back at its spawner), a related-enemy
  subid gate, and the final moving/deleting state. Zero bugs found by self-review or independent
  review. Bank 11 is 197/651 and the project 4,410/9,901. Gates: lint 0, 30k verify 0 failures
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869
  frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 239, bank 11 (2 root routines): ported
  `object_code/ages/parts/rotatableSeedThing.s` (`partCode33` + `func_65d5`,
  `rotatableSeedThing.c`) — the rotatable seed-thing puzzle object: a 4-state RST $00 dispatch
  handling rotation-direction setup, spawning a companion collision-radius part (RST $18
  double-index lookup into a per-direction offset table), toggling switch-block/trigger state
  based on a rotation bitmask, and detecting collisions between the rotation arms. The most
  structurally complex file this session: ~13 `HOOK_LOCAL` sub-labels inlined into one C
  function via `goto`, four of which are genuine internal `call`s (not just `jr`/`jp`) between
  those sub-labels. One shared HOOK_LOCAL block (`func_6515`/`subid0_state0`, joined by
  fallthrough) is reached via THREE different edges — a bare top-level `jr z` and two separate
  internal `call` sites — requiring a new generalization of the established "resume after a
  local `ret`" pattern: check `gb->pc`/`gb->sp` against every known resume address after the
  `RET`/`RET_TAKEN` macro's internal pop, falling back to a true top-level `return;` only when
  none match. Documented as a new lesson in `docs/porting-notes.md`. Caught and fixed two real
  bugs before the gate: `alu_rlc(gb, v)` returns the rotated value and must be assigned back
  (`C = alu_rlc(gb, C)`, not a bare statement with a nonexistent register-selector constant), and
  a tail `jp partSetAnimation`'s own 3-byte cycle burn was initially forgotten entirely. Zero
  further bugs found by a dedicated independent-review pass. Bank 11 is 196/651 and the project
  4,409/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and
  quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 238, bank 11 (1 root routine): ported
  `object_code/ages/parts/subterrorDirt.s` (`partCode32`, `subterrorDirt.c`) — the dirt-clod
  effect from Subterror's dig attack: a 2-state RST $00 dispatch (state0 plays SND_DIG and falls
  straight into state1's animate-then-sync-visibility-to-animParameter-then-conditionally-delete
  logic). Short 15-instruction file. Zero bugs found by self-review or independent review. Bank
  11 is 194/651 and the project 4,407/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 237, bank 11 (1 root routine): ported
  `object_code/ages/parts/babyBall.s` (`partCode2f`, `babyBall.c`) — the object that turns Link
  into a baby: charges up (with a related-enemy-health check gating existence), fires a beam
  toward the enemy target, then a 3-state RST $00 dispatch handling charge/fire/move-and-collide,
  with a special case that nudges the angle toward Link when the related enemy (Veran's fairy
  form) is attacking and mid-beat. Independent review caught a real bug before commit: the
  unconditional `jr @animate` at ROM 0x6491 had its `CYC` end address copied from the
  displayed branch target (0x64b6) instead of the instruction's own physical end (0x6493) — the
  same "target vs physical end" trap documented earlier this session, now recurring even with
  the explicit warning in the review brief. Also fixed a mislabeled field comment (0x29 is
  `Object.health`, not `Object.relatedObj1`, matching the source's own `ld a,Object.health`).
  Full gate re-run clean on the fixed build. Bank 11 is 193/651 and the project 4,406/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk
  suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 236, bank 11 (1 root routine): ported
  `object_code/ages/parts/seaEffects.s` (`partCode2e`, `seaEffects.c`) — the whirlpool and
  pollution-tile effect object: waits for Link to be in normal state and not diving underwater,
  then walks a chain of positions ahead of Link (RST $18 double-index lookup into a table of
  (dy,dx) offsets) checking a harmful-water-tiles collision table, dispatching via a 3-way
  RST $00 jump table on hit (electric-water shock damage, raft-only collision, or a
  push-away-from-hazard response that also handles the currents/pollution tile pull via a
  separate RST $10 single-index speed-value lookup). The most complex file this session by
  instruction count (155 instructions, 3 literal `ret`/`ret cc` exits in a genuine top-level
  hook, 2 different RST helpers). Caught and fixed one real build error before the gate: `A =
  alu_add(gb, A)` doesn't compile because `alu_add` is `void` and sets `gb->a` internally — every
  other `alu_add`/`alu_sub`/etc. call in the file was already a bare statement, this was the one
  place a stray assignment slipped in. Zero bugs found by self-review or independent review.
  Bank 11 is 192/651 and the project 4,405/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 235, bank 11 (1 root routine): ported
  `object_code/ages/parts/veranFairyProjectile.s` (`partCode2d`, `veranFairyProjectile.c`) —
  Veran's fairy-form projectile: waits for its related object (the boss) to exist and be alive,
  fires toward the enemy target with a short lifespan, checks bounds, then animates; deletes
  with a puff if the related object is gone. Short, single-block routine with three internal
  labels reached by plain `goto`. No bugs found by self-review or independent review. Bank 11
  is 191/651 and the project 4,404/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 289,869 frames with
  state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 234, bank 11 (1 root routine): ported
  `object_code/common/parts/twinrovaProjectile.s` (`partCode4b`, `twinrovaProjectile.c`) — the
  red/blue Twinrova projectile shared by both bosses: reflects off Link's L3 shield, checks
  whether the firing Twinrova is already dead, then runs a 4-state RST $00 dispatch (spawn,
  charge-up with parent-position tracking, moving, and same-color-versus-opposite-color
  collision handling that decrements both twinrovas' health and sets a "twinrova defeated"
  signal bit). All nine internal state labels are `HOOK_LOCAL` (no external caller reaches them
  directly), so they're inlined as plain `goto` targets with no `push_effect` needed — the RST
  $00 jump-table helper is the byte-identical pattern established by `donkeyKongFlame.c`. Caught
  and fixed one self-review bug before building: three unconditional `jp` tail-calls
  (`partAnimate`/`partDelete`) were accidentally written with `CYCT` instead of plain `CYC` —
  `CYCT` is reserved for the taken branch of a true conditional, never an unconditional jump;
  independent review re-verified no other instance slipped through. Also discovered
  `partCode4d` is a dead duplicate label at the same address as `partCode4b` in the source (never
  referenced by the object-code table), so only `partCode4b` was registered — registering both
  names made lint fail with "partCode4d has no partCode4d_hook entry" since only one hook
  function exists per address. Bank 11 is 190/651 and the project 4,403/9,901. Gates: lint 0,
  30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 289,869 frames with state `dfb98b52a3b12c03`, normal and quirk suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 233, bank 11 (5 root routines): ported
  `object_code/ages/parts/donkeyKongFlame.s` (`partCode2c` + `func_6248` + `func_6256` +
  `func_6261` + `func_6270`, `donkeyKongFlame.c`) — the Donkey Kong minigame's flame hazard: a
  5-state RST $00 dispatch (with a further nested 2-way dispatch inside state3's substates)
  covering spawn, sidescroll gravity, ground-bounce, and player-collision detection/response.
  This session's most delicate stack mechanism: `func_6248` (independently registered, reached
  via two genuine `call`s and one conditional `jp`) contains a literal `pop hl` that, on one
  branch, discards whatever's on top of the emulated stack and permanently exits via a tail-call
  chain to `objectCreatePuff`/`partDelete` without ever returning to its logical caller. Verified
  by direct trace (both independently and via a dedicated independent-review pass) that because
  every RST $00 dispatch in this file is self-canceling, `gb->sp` equals `partCode2c_hook`'s own
  entry SP at all three of `func_6248`'s entry points, so the bare `pop_effect()` call correctly
  balances the stack regardless of which edge was taken, and `CALL_C`'s `hook_continue` fallback
  (triggered when this path is taken from a genuine call site, since PC never returns to the
  logical call site) resolves to a zero-iteration no-op rather than papering over a real bug.
  Full 289,943-frame reference replay matched the baseline hash exactly. Bank 11 is 189/651 and
  the project 4,402/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 232, bank 11 (1 root routine): ported
  `object_code/ages/parts/timewarpAnimation.s` (`partCode2b`, `timewarpAnimation.c`) — the
  time-warp visual effect: applies its speed and deletes once it drifts far enough down-screen,
  otherwise deletes if the related object (the thing being warped) has advanced past state 3 with
  a certain subid bit set, and finally flickers visibility based on a byte read from the RELATED
  object (not the current Part) using whatever H the last `call` happened to leave behind — a
  real ROM quirk preserved faithfully rather than "fixed" with an inserted `H = D`. Single root,
  no locals, no literal `ret`. Zero bugs found by independent review, which specifically traced
  every write to H through the whole routine to confirm the quirk. Bank 11 is 184/651 and the
  project 4,397/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 231, bank 11 (1 root routine): ported
  `object_code/ages/parts/sparkle.s` (`partCode26`, `sparkle.c`) — the sparkle particle: on
  spawn, picks a random speed/lifetime quadruple via an RST $10 table lookup; every frame
  afterward, integrates a 16-bit fixed-point position from its speed (two separate `add hl,bc`
  accumulations), decays that speed toward zero via a two's-complement step once its counter
  expires, and flickers/deletes based on the accumulated Z depth. Single root, no locals, no
  literal `ret` anywhere (every exit is a `jp` tail-call). Correctly distinguishes two different
  negation idioms in the speed-decay step: a fresh `cpl`/`add $01` for the low byte and a
  carry-propagating `cpl`/`adc $00` for the high byte. Zero bugs found by independent review.
  Bank 11 is 183/651 and the project 4,396/9,901. Gates: lint 0, 30k verify 0 failures with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames
  with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 230, bank 11 (1 root routine): ported
  `object_code/ages/parts/wallArrowShooter.s` (`partCode25`, `wallArrowShooter.c`) — the
  wall-mounted arrow shooter: on first update, derives its firing angle from its own subid;
  every frame, checks whether the enemy target is roughly aligned on the shooter's fixed axis
  (X or Y, chosen by a subid bit) and within a threshold distance, and if so, spawns an arrow
  offset by a direction-indexed table looked up via RST $18 (`addDoubleIndex`). Single root, no
  locals. Zero bugs found by independent review, including a careful re-derivation of the
  `bit 0,a` polarity trap governing which axis (X vs Y) the alignment check uses. Bank 11 is
  182/651 and the project 4,395/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 229, bank 11 (1 root routine): ported
  `object_code/ages/parts/grottoCrystal.s` (`partCode24`, `grottoCrystal.c`) — the grotto crystal
  switch: when triggered, toggles the global switch state via XOR with its own subid, resets its
  collision visibility, and spawns a sarcophagus interaction; otherwise waits one frame then
  checks the room's flag byte to decide whether to reset visibility again before making its tile
  solid and becoming visible. Single root, no locals. Contains two source quirks faithfully
  preserved rather than "corrected": a garbled `ldbc, INTERAC_SARCOPHAGUS $80` macro invocation
  that the transliterator resolves to the literal `ld bc,$8280`, and a bare `ld h,$cf` that sets
  only the high byte of HL, leaving L as whatever the immediately preceding call left there.
  Zero bugs found by independent review. Bank 11 is 181/651 and the project 4,394/9,901. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 228, bank 11 (1 root routine): ported
  `object_code/ages/parts/jabuJabusBubbles.s` (`partCode16`, `jabuJabusBubbles.c`) — Jabu Jabu's
  bubble: a 4-state RST $00 dispatch (rarer 4-entry table for this bank) spawning at a random
  position/speed via an RST $10 table lookup, flickering visibility while idle, tracking its
  spawn point plus a 16-bit offset via a genuine `add hl,bc`, then applying speed and shrinking
  until it either leaves a Y bound or its speed decays to nothing. Single root, no locals.
  Contains a real data-flow trap re-verified independently: `dec (hl)` (decrementing
  Part.counter2 in memory) is immediately followed by `and $0f`, which operates on the STALE
  value already in `A` from several instructions earlier (the Part.visible flip), not a fresh
  read of the just-decremented byte — traced every assignment to `A` through the whole state to
  confirm no spurious re-read was introduced. Zero bugs found by independent review. Bank 11 is
  180/651 and the project 4,393/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 227, bank 11 (2 root routines): ported
  `partCodeNil` and `partCode00` (`partCodeStubs.c`) — two trivial single-instruction part-code
  table filler entries with no corresponding `.s` source file (only referenced from
  `data/partCodeTable.s`'s `.dw` list); verified directly against the transliteration report.
  `partCodeNil` is a bare `ret` (top-level exit); `partCode00` is an unconditional `jp partDelete`
  tail-call. Bank 11 is 179/651 and the project 4,392/9,901. Gates: lint 0, 30k verify 0 failures
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174
  frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 226, bank 11 (4 root routines): ported
  `object_code/common/parts/blueEnergyBead.s` (`partCode53` + `createEnergySwirlGoingOut_body` +
  `createEnergySwirlGoingIn_body` + `func_5e1a`, `blueEnergyBead.c`) — the blue energy bead
  projectile plus the two utility routines used elsewhere to spawn a full ring of them. The two
  swirl routines share almost their entire body (a genuine 8-iteration `push de`-protected loop
  spawning up to 8 beads around a circle) starting right after their own distinct 1-2 byte
  preambles; modeled as a shared, non-static `_hook`-suffixed helper function
  (`blueEnergyBead_swirlBody_hook`, needed because `tools/lint_game.py` requires any function
  touching emulated registers to match `void ..._hook(GB *gb)` exactly, no `static`, per the
  established `enemySword_func_5273_hook` precedent) reached via a bare call from both entry
  points — hit this exact lint failure and a related missing-`sp0_` build failure while authoring,
  both fixed before the gate ran. Zero further issues found by independent review, which
  specifically verified the shared body's byte-exact hand-off between the two callers and the
  genuine value-preserving `push de`/`pop de` pair. Bank 11 is 177/651 and the project 4,390/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 225, bank 11 (1 root routine): ported
  `object_code/common/parts/52.s` (`partCode52`, `52.c`) — Ganon's part 52, the most complex file
  ported this session: a three-level RST $00 dispatch (an outer subid0/subid1/subid2 selector,
  with each subid running its own inner state0/state1/state2 dispatch), where subid1's and
  subid2's inner dispatch tables each carry a 4th entry that jumps directly into subid0's own
  state2 code rather than their own — modeled as a single shared `subid0_state2` label reached
  via `goto` from all three paths, per the shared-target-label lesson from two batches ago. Also
  contains a private local (`func_5d31`) invoked once via a genuine `call` and a second time via
  pure fallthrough immediately afterward — the exact same ROM bytes executed twice with different
  stack semantics each time (one has a pending call return address to resume, the other doesn't),
  modeled as two deliberately duplicated inline copies with different exit treatment (`ret_effect`
  + `goto` back to the caller for the call-reached copy; top-level `RET_TAKEN` for the
  fallthrough-reached copy). Zero bugs found by independent review, which specifically traced the
  cross-subid jump-table entries and both `func_5d31` copies in detail. Bank 11 is 173/651 and
  the project 4,386/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 224, bank 11 (1 root routine): ported
  `object_code/common/parts/51.s` (`partCode51`, `51.c`) — Ganon's part 51: two nested RST $00
  dispatches (an outer subid0/subid1/subid2 dispatch, and a further inner state0/state1/state2
  dispatch inside subid1) plus an RST $18 "addDoubleIndex" table lookup for a two-frame
  animation. The outer jump table's `.dw` declaration order does NOT match physical ROM layout —
  subid2's code appears before subid1's in the file, but subid1 is index 1 and subid2 is index 2
  — verified correct against the actual `.dw` order rather than physical address order.
  Independent review found a real cycle-accounting bug before commit: a `jr nz` shortcut directly
  to `partAnimate_hook` skipped the shared `animate:` label's own 4-cycle `jp` burn whenever that
  branch was taken, since nothing else `goto`s that label to trigger it; fixed by changing the
  inlined call to `goto animate;` so the label's own burn always fires. Also fixed a
  field-offset comment nit (0xe8 is `Part.damage`, confirmed via real precedent, not left bare).
  Full gate re-verified clean after both fixes. Bank 11 is 172/651 and the project 4,385/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 223, bank 11 (2 root routines): ported
  `object_code/common/parts/ganonTrident.s` (`partCode50` + `func_5b2b`, `ganonTrident.c`) —
  Ganon's trident: deletes if the related Ganon object has already reached a terminal state,
  otherwise runs an RST $00 dispatch (spawn-in-place / airborne throw-cycle / land-and-check).
  An unusual case: a genuine `push hl` (a VALUE, not a return address) at the routine's entry is
  popped by exactly ONE of three different exit paths depending on control flow — `state0`,
  `state2` (reached either directly or via `state1`'s fallthrough), or `func_5b2b_hook` (an
  independently-registered local reached via a bare `jr nz` tail-jump from `state1`, popping the
  SAME value across the C function boundary via the shared emulated stack). Verified the two
  pushes (the HL value, then the RST dispatch's own self-canceling return-address push) nest
  correctly in LIFO order with no leak or double-pop on any path. Zero bugs found by independent
  review, which specifically traced every consumer of the pushed value. Bank 11 is 171/651 and
  the project 4,384/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 222, bank 11 (1 root routine): ported
  `object_code/common/parts/twinrovaSnowball.s` (`partCode4e`, `twinrovaSnowball.c`) — Twinrova's
  snowball projectile: destroys itself and spawns snow debris when hit by a sufficiently strong
  weapon or already at maximum item-collision level, otherwise runs an RST $00 dispatch that
  teleports in, waits/animates while flashing, then aims at and moves toward the enemy target
  until it either leaves the screen or collides with a tile. Single root, no locals. Zero bugs
  found by independent review, including a fresh re-derivation of the `bit 0,(hl)` polarity trap
  and all six conditional branches' physical byte-ends. Bank 11 is 169/651 and the project
  4,382/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 221, bank 11 (1 root routine): ported
  `object_code/common/parts/twinrovaFlame.s` (`partCode4c`, `twinrovaFlame.c`) — Twinrova's flame
  projectile: an RST $00 dispatch across an ignition state (which sets up appearance and plays a
  sound) and two further sub-states that animate/apply speed and check tile collision, plus a
  non-RST subid path used for the flame's simpler travel-and-delete behavior. On the terminal
  tile-collision path it runs a genuine 3-iteration loop (`dec b`/`jr nz`) spawning up to three
  secondary flame parts at incrementing angles around a circle via `getFreePartSlot`, modeled
  with two goto labels for the loop top and continuation point. Single root, no independently-
  registered locals. Zero bugs found by independent review, including a careful re-derivation of
  every branch's physical byte-end and confirmation that the loop's `getFreePartSlot` success
  polarity (Z set = success) matches every other caller in the codebase. Bank 11 is 168/651 and
  the project 4,381/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 220, bank 11 (1 root routine): ported
  `object_code/common/parts/greatFairyHeart.s` (`partCode30`, `greatFairyHeart.c`) — the great
  fairy's healing heart: on first update sets its counter and becomes visible, then flies in a
  circular arc toward the enemy target position; once its counter expires it starts a 32-step
  angle countdown, and once THAT expires, decrements `wLinkMaxHealth` at the related object and
  deletes itself if the player's displayed hearts already equal max health. Single flat routine,
  no locals, three top-level `ret nz` exits. Zero bugs found by independent review, including a
  focused re-check of the file's one `jr nz` for the target-vs-physical-end confusion class.
  Bank 11 is 167/651 and the project 4,380/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 219, bank 11 (2 root routines): ported
  `object_code/common/parts/beam.s` (`partCode29` + `func_5758`, `beam.c`) — the beam projectile:
  RST $00 dispatch across firing/traveling states, deriving its animation frame from the low
  nibble of its angle via an RST $10 table lookup, and periodically toggling visibility based on
  the frame counter while checking for tile collision. `func_5758` is reached via TWO different
  edges from the same file — a `jr nz` tail-jump (state1, bare tail-call) and a genuine `call`
  (state2, `CALL_C`) — correctly modeled per-edge rather than as a fixed property of the callee;
  since `func_5758` has no literal `ret` at all (both its exits are tail-calls to already-hooked
  routines), neither edge has any `RET`/`ret_effect` concern. Zero bugs found by independent
  review, including a full individual re-check of every `jr`/`jp` end address following the
  target-vs-physical-end confusion caught in the previous batch. Bank 11 is 166/651 and the
  project 4,379/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 218, bank 11 (3 root routines): ported
  `object_code/common/parts/smallFairy.s` (`partCode28` + `func_56b6` + `func_56cd`,
  `smallFairy.c`) — the small healing fairy: RST $00 dispatch across spawn/wander/collection
  states, picking a random speed/counter/angle when starting to wander, checking collision with
  Link, and giving a random ring or heart refill treasure when collected. `func_56b6` (reached
  only via an unconditional `jp` tail-jump, never a call) correctly uses `RET_TAKEN` for its own
  exits, top-level style; `func_56cd` (reached via a genuine `call`) correctly uses `CALL_C` at
  its call site and `RET`/`RET_TAKEN` at its own exits, applying the stack-leak lesson from three
  batches ago. Self-review caught a real bug before the gate ran: a `jr nc` had its `CYC`/`CYCT`
  end address confused with its branch target (0x56c4) instead of its own physical byte-end
  (0x56c3) — a recurrence of the byte-range-vs-target confusion class, fixed before any build.
  Zero further issues found by independent review. Bank 11 is 164/651 and the project 4,377/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 217, bank 11 (1 root routine): ported
  `object_code/common/parts/lighting.s` (`partCode27`, `lighting.c`) — the lightning bolt part:
  spawns aimed at the enemy target when one exists, then either strikes (playing a sound and
  becoming visible) or animates through its lifetime, running a private helper twice per frame
  that derives collision/position offsets from its animation-parameter nibbles and can spawn a
  companion interaction via `getFreeInteractionSlot`. Two private (`HOOK_LOCAL`) sub-labels,
  `func_55a6` and `func_55e7`, are each reached via a genuine `call`/`call nz` — confirmed (by
  direct `generated.txt` diffing) that their standalone hook entries are removed once `partCode27`
  is registered, so they were inlined via `goto` with explicit `push_effect` for each call site.
  The first draft treated their tail-jumps into already-hooked external routines
  (`setScreenShakeCounter`, `objectCopyPositionWithOffset`) as terminal exits, which silently
  dropped `partCode27_hook`'s own remaining logic whenever that path was taken — a real bug caught
  not by self-review, not by independent review, not by the 30k-frame verify pass, but by the
  short `test_tas` ctest failing with a state mismatch at frame 10560. Fixed by having the
  tail-jump call the external hook and then `goto` the correct resume label, since on real
  hardware the external routine's own `ret` pops the same return address pushed for the original
  call. Two new porting-notes.md lessons recorded. Bank 11 is 161/651 and the project 4,374/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8 (including the short TAS ctest that first caught the bug).

- 2026-09-15: milestone 3 phase 6 batch 216, bank 11 (3 root routines): ported
  `object_code/common/parts/fallingFire.s` (`partCode23` + `func_5535` + `func_553f`,
  `fallingFire.c`) — falling fire from a torch/ceiling: RST $00 dispatch across three subids
  (spawning, waiting, falling), spawning a new falling-fire part via `getFreePartSlot` when
  triggered. `func_553f` (an animation-frame-cycling helper with its own RST $10 table lookup) is
  reached via a genuine `call` and is the first CALL_C-invoked local written since the stack-leak
  fix — correctly closes its own literal `ret` with `RET`, cross-checked directly against the
  auto-generated ground truth in `gen_bank11.c` before it was deleted by the regen step.
  `func_5535` (reached only via a `jr z` tail-jump, no literal `ret` at all) correctly stays a bare
  tail-call. `partCode23_hook`'s own four top-level `ret`/`ret nz` exits correctly use
  `RET`/`RET_TAKEN`. Zero bugs found by independent review. Bank 11 is 160/651 and the project
  4,373/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 215, bank 11 (1 root routine): ported
  `object_code/common/parts/cuccoAttacker.s` (`partCode22`, `cuccoAttacker.c`) — the cucco's
  attacker projectile: 3-state RST $00 dispatch spawning at a random screen edge with a random
  speed/angle toward the target (four RST $10 `addAToHl` table lookups into `@speedVals`,
  `@xOrYVals` used twice, and `@screenEdgePositions`), then counting down and animating until
  screen-boundary exit deletes it. Single root, no independently-registered locals, no literal
  `ret` anywhere in the whole routine (every exit is a `jp` tail-call or consumed by the RST $00
  dispatch's self-canceling push), so the newly-found CALL_C/`ret_effect` bug class does not apply
  here. Proactively re-derived the `bit 0,b`/`jr nz` polarity from `BIT`'s actual flag semantics
  rather than pattern-matching. Zero bugs found by independent review. Bank 11 is 157/651 and the
  project 4,370/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 214, bank 11 (4 root routines): ported
  `object_code/common/parts/moblinBoomerang.s` (`partCode21` + `func_53f5` + `func_541a` +
  `func_542a`, `moblinBoomerang.c`) — the moblin's boomerang projectile. All three locals have
  their own independent hook-table entries and are each reached via a genuine `call` from
  `partCode21`, so each uses `CALL_C` at its call site. Self-review caught a `jr nc` polarity
  inversion (state2's collision-check branch) before the gate ran, plus a wrong field-offset
  comment (0xea is `Part.var2a`, not `invincibilityCounter`, which is 0xeb). The full-game
  `--ref-check` then caught a second, more serious bug that survived self-review, independent
  review, AND a clean 30k-frame `--verify-hooks-continue` pass: the three `CALL_C`-invoked locals
  used bare `CYC`/`CYCT` + `return;` for their literal `ret` exits instead of `RET`/`RET_TAKEN`,
  leaking 2 bytes onto the emulated stack per call (`CALL_C_` always `push_effect()`s the return
  address and expects the callee to pop it via `ret_effect()`) — this caused a real frame-hash
  mismatch at frame 42864 once enough calls accumulated. Root-caused by comparing against the
  auto-generated fallback interpreter in `gen_bank11.c` (ground truth, already using
  `PUSH`/`POP`/`RET`/`RET_TAKEN` for this exact code) and fixed; see the corrected porting-notes.md
  lesson. The same bug shape, found latent in `dekuScrubProjectile.c` from batch 211, was fixed in
  a preceding commit. Bank 11 is 156/651 and the project 4,369/9,901. Gates: lint 0, 30k verify 0
  failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 213, bank 11 (1 root routine): ported
  `object_code/common/parts/fire.s` (`partCode20`, `partCode20.c`) — the fire keese's fire
  projectile: while its state is nonzero, decrements counter1 and deletes when it reaches zero,
  otherwise animates; on first update (state 0) it increments its own state, sets counter1 to
  0xb4, and makes itself visible. Single root, no locals, no RST dispatch. Self-review caught one
  byte-length bug before the gate ran: the final `jp objectSetVisible82` end address was written
  as `0x5396` instead of the correct `0x5395` (jp is always 3 bytes), fixed before any build.
  Independent review confirmed zero further issues. Bank 11 is 152/651 and the project 4,365/9,901.
  Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-15: milestone 3 phase 6 batch 202-B, bank 0b (9 routines): added the Vire interaction
  dispatcher, all three minigame/entrance state machines, script and counter helpers, and the
  disable/delete control helpers. Regeneration leaves 770 executable Bank 0b hooks, 139 rewritten;
  the project is 4,619/9,491. All lint, replay, and normal/quirk test gates passed.

- 2026-09-15: milestone 3 phase 6 batch 201-B, bank 0b (12 routines): added the Tuni Nut,
  Volcano Handler, and Book of Seals podium interaction roots and state handlers. The rewritten
  registry now removes their data-only labels, leaving 789 executable Bank 0b hooks; the project is
  4,613/9,510. All lint, replay, and normal/quirk test gates passed.

- 2026-09-15: milestone 3 phase 6 batch 200-B, bank 0b (13 routines): added the Harp of Ages
  spawner and Final Dungeon Energy interaction state machines, including their nested RST
  dispatches and cutscene transitions. Bank 0b is 118/805 and the project is 4,601/9,526. All
  lint, replay, and normal/quirk test gates passed.

- 2026-09-15: milestone 3 phase 6 batch 199-B, bank 0b (3 routines): added the Zelda approach
  trigger dispatcher and both state handlers from `zeldaApproachTrigger.s`; exact branch-byte
  burns were corrected before gating. Bank 0b is 105/805 and the project is 4,588/9,526. All
  lint, replay, and normal/quirk test gates passed.

- 2026-09-15: milestone 3 phase 6 batch 197, bank 0b (11 routines): added the C1 harp codec
  interaction dispatcher, its three substates, graphics continuation, counter/movement helper,
  sparkle creator, and unused visibility helper; all gates passed. Bank 0b is 77/819 and the
  project is 4,560/9,528.

- 2026-09-15: milestone 3 phase 6 batch 198, bank 0b (25 routines): added Human Veran, title
  clouds, intro bird, Link ship, and their shared intro movement helpers; all gates passed. Bank
  0b is 102/805 and the project is 4,585/9,526.

- 2026-09-15: milestone 3 phase 6 batch 196, bank 0b (1 routine): added the harp-song interaction state machine; all gates passed.

- 2026-09-15: milestone 3 phase 6 batch 195, bank 0b (1 routine): added the Black Tower door-handler state machine; all gates passed.

- 2026-09-15: milestone 3 phase 6 batch 194, bank 0b (11 routines): added banana, pirate-captain,
  and create-object-at-each-tileindex interaction paths. The quirk build caught `alu_add` being
  treated as a value-returning helper; it mutates A in place. All replay and test gates passed.

- 2026-09-15: milestone 3 phase 6 batch 193, bank 0b (5 routines): added the wooden-tunnel
  dispatcher and its state/solidity paths. Bank 0b is 53/819 and the project is 4,534/9,544.
  Lint, 30k verification, full reference replay, and both normal and quirk suites passed.

- 2026-09-15: milestone 3 phase 6 batch 192, bank 0b (8 routines): added the exclamation-mark
  and floating-image dispatch/state paths, including durable continuations after their
  thread-switching graphics calls. Bank 0b is 48/824 and the project is 4,529/9,545. Lint, 30k
  verification, full reference replay, and both normal and quirk suites passed.

- 2026-09-15: milestone 3 phase 6 batch 191, bank 0b (16 routines): added the Bipin/Blossom
  family-spawner dispatcher, its child-stage/personality table entries, and the shared
  exclamation-mark/floating-image constructors. The table-local aliases are durable readable
  hooks so table dispatch cannot resume in generated code. Bank 0b is 40/825 and the project is
  4,521/9,546. Lint, 30k verification, full reference replay, and both normal and quirk suites
  passed.

- 2026-09-15: milestone 3 phase 6 batch 190, bank 0b (5 routines): added the Game Complete
  dialogue, Kiss Heart, and interaction $CF state/script paths, including durable post-graphics
  continuations for the call-return boundaries. Bank 0b is 25/836 and the project is
  4,505/9,557. Lint, 30k verification, full reference replay, and both normal and quirk suites
  passed.

- 2026-09-15: milestone 3 phase 6 batch 189, bank 0b (20 routines): added the Zora and Great
  Fairy interaction initialization/state paths, their thread-switching call continuations, and
  canonical shared-entry wrappers. Bank 0b is 20/841 and the project is 4,500/9,562 after
  merging Bank 11. Lint, 30k verification, full reference replay, and both normal and quirk
  suites passed before the merge.

- 2026-09-15: milestone 3 phase 6 batch 212, bank 11 (2 root routines): ported
  `object_code/common/parts/wizzrobeProjectile.s` (`partCode1f` + `func_5369`,
  `wizzrobeProjectile.c`) — the wizzrobe's teleporting projectile: deletes when out of bounds or
  colliding with a tile, otherwise applies speed; `func_5369` (independently registered, reached
  only via a `jr z` tail-jump, never a genuine call) sets up speed/animation and makes it visible.
  Neither routine contains a literal `ret` at all — every exit is a `jp` tail-call — so there are
  no RET/RET_TAKEN concerns anywhere in the file. Zero bugs found on both self-review and
  independent review. Bank 11 is 151/651 and the project 4,364/9,901. Gates: lint 0, 30k verify
  0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 211, bank 11 (5 root routines): ported
  `object_code/common/parts/dekuScrubProjectile.s` (`partCode1e` + `func_52f4` + `func_52fd` +
  `func_5313` + `func_5336`, `dekuScrubProjectile.c`) — the deku scrub's spat seed projectile.
  All four locals turned out to have their own independent hook-table entries (unlike the usual
  private-local case), so each became its own `_hook` function: the two genuinely `call`-reached
  ones (`func_52fd`, `func_5336`) use `CALL_C` at their call sites per the `gashaTree.c` `func_5010`
  precedent, while the two `jr`-reached ones (`func_52f4`, `func_5313`) are bare tail-calls with no
  push, per the `octorokProjectile.c` cross-file-tail-jump precedent. All four locals are
  "round-trip" — reached by exactly one call/CALL_C site each — so every one of their `ret` exits
  (including two conditional ones in `func_5336`) uses bare `CYC`/`CYCT` + `return` rather than
  `RET`/`RET_TAKEN`, and both conditional bare-return exits were double-checked to use `CYCT` (not
  `CYC`) on their taken branch given the `enemySword.c` bug class from the previous batch. Two
  `bit N,(hl)`/`jr`-or-`ret` polarity pairs were re-derived from `BIT`'s actual Z-means-clear
  semantics rather than pattern-matched, per the lesson from the same previous batch. Also caught
  before the gate: `playSound` needed the bank-disambiguated canonical name
  `playSound_b00_hook`, not a plain `playSound_hook`. Zero bugs found by independent review. Bank
  11 is 149/651 and the project 4,362/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 210, bank 11 (1 root routine): ported
  `object_code/common/parts/enemySword.s` (`partCode1d`, `enemySword.c`) — the enemy sword-swing
  effect: gates on the related enemy's status, updates counters, sets a hit-lock bit and calls a
  private local (`func_5273`) that checks the related enemy's health/stun/invincibility state
  (a genuine round-trip call whose push and pop cancel out, so all three of its exits use bare
  `CYC`/`CYCT` + `return` rather than `RET`/`RET_TAKEN`) before falling into a shared
  angle/position-table lookup section (reached both by fallthrough and by a `jr` tail-jump from a
  second entry point, using two RST $10 `rst_addAToHl` table lookups). Self-review caught and
  fixed two bugs before the gate: both RST $10 sites were missing their own 1-byte cycle burn
  (the `bridgeSpawner.c` bug class recurring), and a conditional bare-return exit in `func_5273`
  used `CYC` instead of `CYCT` for its taken branch (the `partCommonCode.c` bug class recurring).
  Independent review then caught a third, more serious bug: an inverted `jr nz`/`bit 0,(hl)`
  branch polarity that changed actual game behavior, not just cycle accounting; fixed and the
  full gate re-verified. Bank 11 is 144/651 and the project 4,357/9,901. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches
  over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 209, bank 11 (1 root routine): ported
  `object_code/common/parts/stalfosBone.s` (`partCode1c`, `stalfosBone.c`) — the stalfos's thrown
  bone: deletes on a specific status; a 3-state RST $00 dispatch sets up and animates toward the
  enemy target, applies speed after tile/screen collision checks (branching three ways off a
  single `partCommon_checkTileCollisionOrOutOfBounds_hook` call's carry+zero result, shared via a
  `jr c` into a separate `jr z` label reusing the same flags), or updates a Z-speed component and
  bounces or continues animating based on a frame-counter bit. The RST dispatch's own push is
  self-canceling (the shared jump-table helper pops the exact address it just pushed before
  jumping to the resolved target), so the file's one literal `ret c` — reached from inside the
  dispatched state2 code — is still a top-level hook exit and correctly uses `RET_TAKEN`, per the
  `switch.c`/`lynelBeam.c` precedent. Independent review additionally confirmed this empirically
  via the TAS ctest's stack-consistency checks. Zero bugs found on both self-review and
  independent review. Bank 11 is 143/651 and the project 4,356/9,901. Gates: lint 0, 30k verify
  0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 208, bank 11 (1 root routine): ported
  `object_code/common/parts/lynelBeam.s` (`partCode1b`, `lynelBeam.c`) — the lynel's beam
  projectile: deletes on a specific status, otherwise a straight-line setup on `state != 0`
  (position/speed/angle/animation) or, once active, checks screen boundary and applies speed
  before flipping an OAM flag every 4th frame and returning. A simple single-root file with no
  RST and no internal locals — both literal `ret`s are top-level hook exits using RET/RET_TAKEN
  per the established `switch.c` precedent, since the push being consumed belongs to the
  emulator's hook-dispatch call, not to anything inside this file. Zero bugs found on both
  self-review and independent review. Bank 11 is 142/651 and the project 4,355/9,901. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 207, bank 11 (1 root routine): ported
  `object_code/common/parts/enemyArrow.s` (`partCode1a`, `enemyArrow.c`) — an enemy-fired arrow
  with two subids (goron pot smash vs. plain arrow) each with their own two-state RST $00
  dispatch, sharing a common `func_11_513a` fall-through that computes a bounce state and hands
  off to `partCommon_bounceWhenCollisionsEnabled_hook` when it goes out of bounds or fails a
  tile-collision check. Notable: no literal `ret` anywhere in the routine — every exit is a
  tail `jp`/`jr`, including a real cross-jump where `subid1@state1`'s not-nonzero branch jumps
  directly into the middle of `subid0@state1`'s code with a plain unconditional `jr` (no push,
  modeled as both entry points sharing one `goto subid0_state1` label). Zero bugs found on both
  self-review and independent review. Bank 11 is 141/651 and the project 4,354/9,901. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 206, bank 11 (1 root routine): ported
  `object_code/common/parts/fireProjectiles.s` (aliased labels `partCode19`/`partCode31` —
  PART_ZORA_FIRE / PART_GOPONGA_PROJECTILE at the same ROM address; canonical hook name
  `partCode19_hook` per `ages.sym` label order, `partCode19` registered in `rewritten.txt`,
  `fireProjectiles.c`) — a homing projectile: `state0` sets up its counter/speed and makes it
  visible; `state1` decrements a shared counter and, once it expires, computes its travel angle
  either toward a stored target position (goponga seed pod, read from `hFFB2`/`hFFB3`) or toward
  the nearest enemy (zora fire, via `objectGetAngleTowardEnemyTarget_hook`), gated by a bit in
  `Part.subid`; `state2` flips an OAM flip-flag every 4th frame, applies speed, and deletes itself
  or animates depending on whether it left the screen boundary. Independent review found one
  cosmetic nit (raw `0xffb2`/`0xffb3` instead of the named `hFFB2`/`hFFB3` constants already used
  elsewhere in the project) and no functional bugs; fixed and re-verified. Bank 11 is 140/651 and
  the project 4,353/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 205, bank 11 (1 root routine): ported
  `object_code/common/parts/octorokProjectile.s` (`partCode18`, `octorokProjectile.c`) — the
  octorok rock projectile: deletes itself on a specific status, or resets to state 2 if idle too
  long; a 4-entry RST $00 dispatch sets up its speed and appearance, checks screen/tile collision
  before applying speed (bouncing back or deleting on a wall hit), bounces off collidable
  surfaces, or — for the 4th jump-table entry — tail-calls straight into
  `partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook`, an already-independently-registered hook
  from a different file (`partCommonCode.c`, several batches back), since a jump table entry
  pointing at another file's routine needs no local `goto` label at all, just the dispatch's own
  default/else branch. Zero bugs found on both self-review and independent review. Bank 11 is
  139/651 and the project 4,352/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 204, bank 11 (2 root routines): ported
  `object_code/common/parts/gashaTree.s` (`partCode17` + `func_5010`, `gashaTree.c`) — the gasha
  seed tree: checks a flag and Link's vulnerability before activating, then a 3-state dispatch sets
  up collision/position, waits for the related tree-planting object to change ID (deleting itself
  if not), or (a further 3-substate dispatch) knocks a seed off, bounces it toward the ground, and
  checks tile collision before settling. Unusually, `func_5010` is a SEPARATE independently
  registered root defined at the bottom of the same source file and reached only from within
  `partCode17`'s own code — since it genuinely has its own hook-table row, its one genuine-call
  reach site correctly uses `CALL_C` (not the bare push_effect+call pattern used for true
  unregistered locals elsewhere this session). Two bugs caught by self-review before the gate: the
  extracted local `func_4fb2` was declared `static` (breaking `lint_game.py`'s exact-match regex,
  the same mistake made once before in `lightableTorch.c`), and — a genuinely new bug class — both
  of `func_4fb2`'s bare-call sites were missing the `call` instruction's own cycle burn entirely,
  since "no push_effect needed" was correctly reasoned but mistakenly taken as license to skip the
  burn line too; caught by the address-coverage diff surfacing two gaps that weren't explainable by
  the usual `CALL_C`/RST omissions. Both fixed; independent review came back completely clean. Bank
  11 is 138/651 and the project 4,351/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 203, bank 11 (1 root routine): ported
  `object_code/common/parts/owlStatue.s` (`partCode13`, `owlStatue.c`) — the owl statue hint part:
  on a specific "just talked" status check, advances to a listening state; a 4-state dispatch then
  makes the statue solid, waits idle, periodically spawns sparkle-interaction particles at
  angle-derived offsets (RST $10 table lookup) once talked to, or shows a specific hint text once
  its animation reaches a particular frame. Self-review caught a real bug: `jp
  objectCopyPositionWithOffset` was burned to the address of the NEXT VISIBLE LABEL in the
  disassembly (`@state3`) rather than the `jp`'s own 3-byte end — with a 12-byte, 6-entry data
  table sitting physically between the two addresses, making the mistake look plausible at a
  glance. Both RST call sites (jump-table dispatch and single-index lookup) correctly include
  their own cycle burn on the first attempt. Independent review came back completely clean. Bank 11
  is 136/651 and the project 4,349/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 202, bank 11 (1 root routine): ported
  `object_code/common/parts/flame.s` (`partCode12`, `flame.c`) — the burning-enemy fire part: tracks
  the enemy it's attached to by ID, keeps applying a bounce/damage-over-time effect and mirroring
  its Z position while burning, and once its counter expires restores the enemy's saved health
  (disabling collision if it died) and stuns it before deleting itself. `flame_state0_hook` is a
  pattern-a local (conditional `call z`, ends via tail-jump into `objectSetVisible80`, needs
  `push_effect` at the call site). Notable only for a plain register-to-register scratch trick
  (`ld c,h`/`ld h,c`, saving which enemy/object page is being pointed at across an intervening
  `partCommon_decCounter1IfNonzero` call) modeled as direct `C = H;`/`H = C;` assignment, with no
  stack involvement at all. Zero bugs found on both self-review and independent review. Bank 11 is
  135/651 and the project 4,348/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 201, bank 11 (1 root routine): ported
  `object_code/common/parts/seedOnTree.s` (`partCode10`, `seedOnTree.c`) — the seed-bearing tree
  part: on being hit, sets its state to give a seed; five states set up appearance, sit idle, move
  toward/give itself to Link on collision, and (via a substate dispatcher) either teach the seed
  type the first time it's obtained or just give it directly, before deleting itself and possibly
  poking a related object; a "dead" (satchel-check) path either shows a get-the-satchel message or
  knocks itself off the tree to fall and be picked up. `@giveSeed` (straight-line, no `ret` at all)
  is reached two genuinely different ways — a plain `jr` tail-hand-off from one state (never
  returns to that state's own context) and a genuine `call` from another (does expect to resume
  afterward, traced by hand through `giveTreasure`'s own eventual real `ret`) — caught and fixed a
  real bug before self-review even began: the first draft treated both reach points identically,
  which would have made the tail-hand-off path incorrectly execute code real hardware never reaches
  from there. All four RST call sites (two `rst $00`, one `rst $18`, one `rst $10`) correctly
  include their own cycle burn on the first attempt, applying the lesson from the previous two
  batches. Independent review came back completely clean. Bank 11 is 134/651 and the project
  4,347/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 200, bank 11 (1 root routine): ported
  `object_code/common/parts/respawnableBush.s` (`partCode0f`, `respawnableBush.c`) — the
  respawning cuttable bush: on being cut, sets the "cut" tile, has a 50/50 chance of spawning an
  item drop, and creates grass debris; a 5-state timer then regrows it through
  regenerating/ready/normal tiles. `setTileHere` is reached three distinct ways with no push at any
  of them (a genuine call continuing inline afterward, a `jr` tail-call, and a zero-byte pure
  fallthrough) — safe because its body never contains a real `ret`/`ret_effect` at all, only a
  self-balanced local `push af`/`pop af` scratch pair and a final tail-jump into `setTile`, so it
  behaves correctly regardless of which of the three shapes the caller expects. Zero bugs found on
  both self-review and independent review — the 200th batch of milestone 3 phase 6. Bank 11 is
  133/651 and the project 4,346/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 199, bank 11 (1 root routine): ported
  `object_code/common/parts/detectionHelper.s` (`partCode0e`, `detectionHelper.c`) — the guard's
  detection-projectile helper: a 4-subid dispatcher where subid0 (the "controller") tracks its
  parent guard's angle/position and periodically spawns forward- and side-detection projectiles
  (subids 1/2) via a shared spawn helper; subid1 moves toward Link, triggering the guard or
  deleting itself on collision; subid2 (aliasing subid3, the same ROM address) is a short-lived
  variant used for close-range detection at angles derived from `var03`, sharing subid1's own
  movement/collision code via a cross-subid `jr`. `spawnCollisionHelper` is invoked from three
  places with no push at any of them (two genuine calls each continuing inline afterward, one pure
  fallthrough with zero bytes between caller and callee) — both of its own exits correctly use bare
  `CYC`/`CYCT`+`return`, never `RET`/`RET_TAKEN`, since nothing was ever pushed to consume. Two bugs
  caught by self-review before the gate: an unconditional `jr` miscounted as 3 bytes instead of 2,
  and an `rst $10` instruction missing its own 1-byte cycle burn before invoking the helper — the
  same class just found by independent review in the previous batch's `bridgeSpawner.c`, this time
  caught on the first self-review pass by specifically checking for it. Independent review then
  came back completely clean. Bank 11 is 132/651 and the project 4,345/9,901. Gates: lint 0, 30k
  verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches
  over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 198, bank 11 (1 root routine): ported
  `object_code/common/parts/bridgeSpawner.s` (`partCode0c`, `bridgeSpawner.c`) — the growing-bridge
  spawner: every other update, places the next bridge tile (direction-dependent tile pair, RST $18
  double-indexed off the current angle) into the room layout, plays a door-closing sound, and moves
  to the next tile position along its angle direction once the bridge segment count hits zero.
  Independent review caught a real bug: the `rst $18` (`addDoubleIndexToHl`) call site was missing
  its own 1-byte `CYC` burn for the RST instruction before invoking the helper — separate from the
  helper's own internal push/pop bookkeeping for the RST's return behavior — silently undercounting
  the block by 4 cycles even though control flow was otherwise correct. A quick audit confirmed
  every other RST call site written this session (`lightableTorch.c` x4, `volcanoRock.c` x3,
  `ball.c`, `movingOrb.c`) already had this burn, so it was a one-off slip. Bank 11 is 131/651 and
  the project 4,344/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 197, bank 11 (1 root routine): ported
  `object_code/common/parts/movingOrb.s` (`partCode0b`, `movingOrb.c`) — the moving orb puzzle
  piece: on being just hit, toggles a bit in `wToggleBlocksState` and flickers OAM flags; otherwise
  runs a scripted movement (states 0-7, loading the Ages-specific `bank0e.orbMovementScript`) or
  one of four directional-approach states (8-B, up/right/down/left) checking whether it's reached
  its destination coordinate before applying speed or continuing the movement script, or a waiting
  state (C) counting down before resuming the script. Confirmed the assembled ROM bytes reflect the
  `.ifdef ROM_AGES` branch (not Seasons) by checking the actual `ld hl,$6b22` immediate against the
  report. Four near-identical directional state blocks each load a different, sometimes
  role-swapped, pair of field offsets (E/L) before comparing — verified none were copy-paste
  inverted between siblings. Zero bugs found on both self-review and independent review. Bank 11 is
  130/651 and the project 4,343/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 196, bank 11 (1 root routine): ported
  `object_code/common/parts/button.s` (`partCode09`, `button.c`) — the pressable floor button,
  this session's most structurally complex single-root file. A `ret nz` early-exit
  (`@checkButtonPushed`) is physically reached two different ways — pure top-level fallthrough
  within `partCode09_hook` itself (a genuine root-hook exit, needs `RET_TAKEN`) and via
  `@updateTileBeforeDeletion` (itself reached by a genuine `call` from `@delete` whose call site
  does `push_effect`) — and by hand-tracing real hardware's push/pop, BOTH of
  `@updateTileBeforeDeletion`'s exit paths (the early `ret`, and a fallthrough tail-jumping into
  `playSound`) converge on consuming that same pushed address and reaching `@delete`'s own
  `jp partDelete`. Since the shared code behaves differently by context, it's duplicated once
  inline in `partCode09_hook` and once inside `button_updateTileBeforeDeletion_hook`. Self-review
  caught a `jr` byte-length arithmetic slip (miscounted as 3 bytes instead of 2) before the gate;
  independent review then caught a genuine stack-corruption bug in the duplicate copy — its `ret nz`
  used a bare `CYC`+`return` on the reasoning that "no push happened for this specific transition,"
  which gets the next C statement right but never calls `pop_effect`, silently drifting `gb->sp` by
  2 bytes whenever that path fires, since a push from three frames up (`@delete`'s own) genuinely
  needed consuming here. Fixed to `RET_TAKEN`; full gate re-verified green. Bank 11 is 129/651 and
  the project 4,342/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 195, bank 11 (1 root routine): ported
  `object_code/common/parts/darkRoomHandler.s` (`partCode08`, `darkRoomHandler.c`) — the dark-room
  torch-lighting palette handler: brightens or darkens the room's palette (fully or incrementally)
  based on how many lightable torches are currently lit versus the room's total. Two locals with no
  independent hook row (`state0`, `spawnLightableTorch`) are each reached via a genuine conditional
  `call z` from a caller that does NOT push, and each ends via its own literal `ret` with no
  tail-jump out — modeled with a bare `CYC`+`return` at their own final `ret`, never `RET`/
  `RET_TAKEN` (whose `ret_effect()` would otherwise pop a stack no one pushed, the exact bug class
  found and fixed two batches ago in `partCommonCode.c`). `spawnLightableTorch` additionally uses a
  real `push hl`/`push bc` pair as pure local scratch storage, unrelated to and safely coexisting
  with that call/return-address reasoning. Zero bugs found on both self-review and independent
  review. Bank 11 is 128/651 and the project 4,341/9,901. Gates: lint 0, 30k verify 0 failures with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 194, bank 11 (1 root routine): ported
  `object_code/common/parts/shadow.s` (`partCode07`, `shadow.c`) — the parent-following shadow
  part: deletes itself if its parent object's ID changed, otherwise tracks the parent's position
  with an offset, hides itself if the parent is on the ground, and otherwise flickers visibility
  and picks a shrink-with-height animation frame from a lookup table based on how far above the
  ground the parent is. `shadow_initialize_hook` is a pattern-a local (no independent hook row,
  reached via a genuine conditional `call z`, never returns via its own `ret`) modeled with
  `push_effect` and no `return;` at the call site, continuing inline — with the taken/not-taken
  cycle cost for the conditional call itself burned correctly via `CYCT`/`CYC` around the
  `push_effect` (a subtly different bug shape from the earlier `CALL_C`/`CALL_C_CC` lesson, since
  no `CALL_C`-family macro is involved at all here). Zero bugs found on both self-review and
  independent review. Bank 11 is 127/651 and the project 4,340/9,901. Gates: lint 0, 30k verify 0
  failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 193, bank 11 (1 root routine): ported
  `object_code/common/parts/lightableTorch.s` (`partCode06`, `lightableTorch.c`) — the lightable
  torch part, the largest single-root file this session (111 instructions, 4 nested RST $00
  dispatches: subid selector, then a separate state selector per subid). Subid 0 stays lit forever
  once lit; subid 1 stays lit for `[counter2]` frames then extinguishes; subid 2 tracks another
  object's related tile via `getTileAtRelatedObjPosition` to decide when to light/extinguish.
  `getTileAtRelatedObjPosition` is a pattern-a local (no independent hook row, never returns via
  its own `ret`, always tail-jumps into `getTileAtPosition`) called via genuine `call` from three
  separate points, each correctly guarded with its own `push_effect`. One tail-jump
  (`gotoState1IfTileAtRelatedObjPositionIsNotLit`, reached only via `jp nz` from within a sibling
  state) is modeled as a plain `goto` within the same function, needing no push at that transition.
  Lint caught one structural mistake before the gate: the extracted local was declared `static`,
  which fails `lint_game.py`'s exact-match regex for recognizing a `_hook`-suffixed register-access
  context (the regex requires the line to literally start with `void`, not `static void`) — fixed
  by dropping `static`. Zero address/logic bugs found on either self-review or independent review,
  despite the file's size and four levels of dispatch. Bank 11 is 126/651 and the project
  4,339/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 192, bank 11 (1 root routine): ported
  `object_code/common/parts/switch.s` (`partCode05`, `switch.c`) — the floor switch part: on being
  hit, toggles a bit in `wSwitchState` and updates its tile (overworld switches flip and delete
  themselves, setting a room flag; dungeon switches just recolor based on whether all switches in
  the group are toggled), then plays a sound; on first normal update, makes itself solid and
  records its tile position. `switch_updateTile_hook` is a pattern-a local with no independent hook
  row that never returns via its own `ret` — every path tail-jumps into another hook — so its one
  call site uses `push_effect` before invoking it, continuing inline afterward (matching the
  `itemDrop.c`/`enemyDestroyed.c` precedent). Zero bugs found on both self-review and independent
  review. Bank 11 is 125/651 and the project 4,338/9,901. Gates: lint 0, 30k verify 0 failures with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 191, bank 11 (1 root routine): ported
  `object_code/common/parts/bossDeathExplosion.s` (`partCode04`, `bossDeathExplosion.c`) — the boss
  death explosion part: on first update, makes itself visible and plays a big-explosion sound
  (unless its subid is nonzero); on subsequent updates, animates until done, then decrements the
  enemy count and either deletes itself, drops an item, or replaces itself with the drop depending
  on subid and `decideItemDrop`. Notable only for containing one conditional `call nz,playSound` —
  the first file since the `CALL_C`/`CALL_C_CC` cycle-timing lesson was documented, and the first
  time it was applied correctly on the initial draft rather than caught afterward. Both self-review
  and independent review came back completely clean. Bank 11 is 124/651 and the project
  4,337/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 190, bank 11 (1 root routine): ported
  `object_code/common/parts/orb.s` (`partCode03`, `orb.c`) — the toggle-block orb switch: on being
  just hit, toggles a bit in `wToggleBlocksState` and plays a sound; otherwise, on first update,
  makes its tile solid, derives a bitset from its subid via a small lookup table, and sets its OAM
  flags/backup based on whether that bit is currently toggled. A raw ROM quirk (`ld h,Part.zh`
  setting `H` directly to the struct-offset immediate, not the usual `ld l,`) modeled literally.
  Self-review caught one real bug: a `jr z` burned to its jump target instead of its own 2-byte end
  — the target label happened to sit exactly one byte past the fallthrough path's own next
  instruction, making the two addresses easy to confuse. Independent review came back clean. Bank
  11 is 123/651 and the project 4,336/9,901. Gates: lint 0, 30k verify 0 failures with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 189, bank 11 (1 root routine): ported
  `object_code/common/parts/enemyDestroyed.s` (`partCode02`, `enemyDestroyed.c`) — the generic
  enemy-destroyed part (animate, alternate a decoration OAM flag every other frame, wait for a
  death-animation-parameter signal, then decide and spawn an item drop or delete). Two locals with
  no independent hook row, each reached via a genuine `call`/`call z` from the root with no further
  internal dispatch of their own — modeled with the established `push_effect(gb, ra); helper(gb);`
  pattern (matching `itemDrop.c`'s `getRelatedObj1ID` precedent), continuing inline afterward with
  no `return;` at the call site. Self-review caught two real bugs before compiling: a 3-byte
  `ld a,($cc00)` instruction mis-modeled as spanning 4 bytes (absorbing the following `rrca`), which
  shifted every subsequent address in that block by one and dropped a cycle burn entirely; and a
  conditional `call c,partSetAnimation` inside the local's own taken-branch that used plain
  `CALL_C` (always burns not-taken timing) instead of `CALL_C_CC` (burns taken timing via `CYCT`) —
  caught by independent review, not self-review, since it's a pure cycle-count bug the
  address-coverage diff can't see (identical byte range either way). Bank 11 is 122/651 and the
  project 4,335/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6, bank 11 bugfix: found and fixed a real stack-corruption bug in
  the previous batch's `partCommonCode.c` (`commonCode_checkOutOfBounds_roundAngleToDiagonal`) — a
  local helper reached via a genuine `call` with no `push_effect` at the call site was incorrectly
  using `RET`/`RET_TAKEN` (which unconditionally pop the emulated stack) instead of plain
  `CYC`+bare-`return`. This popped an unrelated outer caller's legitimately-pushed return address
  every time the code path executed, permanently shifting `gb->sp` by 2 bytes — a bug that survived
  the original batch's full gate (build, lint, ctest, 30k verify, 290,174-frame full replay,
  independent review) undetected, since `partCommon_checkOutOfBounds` is apparently never exercised
  by the recorded TAS movie. Found only while reasoning through why a sibling local in the same file
  safely uses the same macro. An audit of all 10 files written this session (independent agent)
  found no other instances. Fixed to match the already-established `ball_func_6b00` precedent (no
  push at the call site, no `RET`/`RET_TAKEN` inside, just `CYC`/`CYCT` plus bare `return`). Gates
  re-verified green after the fix: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal
  and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 187, bank 11 (12 root routines): ported
  `object_code/common/parts/commonCode.s` (`partCommonCode.c`) — the shared "part common" utility
  library used by nearly every other file in this bank: tile-collision-in-front/at-angle checks
  (with and without hole-tolerance), the enemy-standard-update analogue (uninitialized/invincible/
  collision/dead dispatch, including an inter-bank `callab` to bank 0x3f), out-of-bounds and
  collision-or-out-of-bounds checks, counter1 decrement, collision bounce, speed-update-and-delete,
  position-offset-and-radius-from-angle, and substate increment. Named `partCommonCode.c` rather
  than the source's own basename because `src/game/commonCode.c` already exists as an unrelated,
  previously-committed file for a different bank's identically-named `commonCode.s` (this project
  has four separate `commonCode.s` files across `common/{specialObjects,itemParents,parts,enemies}`
  and each needs a disambiguated C filename, matching the established `itemParentCommonCode.c`/
  `enemyCommonCode.c` precedent) — caught only after a `Write` to the naive filename silently
  overwrote the existing bank-5 file (never committed, no data lost; recovered via git checkout and
  moved the new content to the correct name). Two of the twelve routines share a common tail
  physically embedded inside one of them (`partCommon_getTileCollisionInFront_allowHoles`'s own
  address range is jumped into by `partCommon_getTileCollisionAtAngle_allowHoles`), extracted into
  a private helper per the established shared-tail-extraction pattern. Rewriting these
  previously-bare shared routines required renaming their bare-name `CALL_C` references to the new
  `_hook` suffix across nine already-committed files (`ball.c`, `blueStalfosProjectile.c`,
  `itemDrop.c`, `kingMoblinBomb.c`, `ramrockGloveFormArm.c`, `updateParts.c`, `veranSpiderweb.c`,
  `vireProjectile.c`, `volcanoRock.c`) that already called them — the build fails to link otherwise,
  since a rewritten routine's canonical hook name always gains the `_hook` suffix everywhere.
  Self-review caught one real bug: an unconditional `jr` in the enemy-standard-update analogue was
  burned to its jump target instead of its own 2-byte end. Independent review came back completely
  clean. Bank 11 is 121/651 and the project 4,334/9,901. Gates: lint 0, 30k verify 0 failures with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with
  state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 186, bank 11 (11 root routines): ported
  `object_code/common/parts/volcanoRock.s` (`partCode11`, `volcanoRock.c`) — the erupting volcano
  rock projectile's subid0 (launch/reset-and-relaunch), subid1 (drop-and-bounce with a shared
  4-substate hazard/animation/collision tail), and subid2 (random-respawn variant that falls
  straight through into `volcanoRock_setRandomPosition`, matching the ROM's own fallthrough with
  zero invented cycle burn), plus two small locals: `volcanoRock_subid0_setSpeedFromAngle`
  (angle-tier speed lookup, reached only by tail-jump/fallthrough, no push/ret needed) and
  `volcanoRock_setCollisionSize` (RST $10 table lookup, reached by genuine `call` via `CALL_C`).
  Written with the full manual address-by-address derivation done up front (every `CYC`/`CYCT`
  pair copied directly from the transliterate report's consecutive addresses, in direct response
  to `ball.c`'s high bug count last batch) — the address-coverage diff still caught one real bug
  before the gate: both `jr c` branches in `volcanoRock_subid0_setSpeedFromAngle` were burned to
  their jump target (`0x4c17`) instead of their own 2-byte end (`0x4c0f`/`0x4c15`), fixed and
  reconfirmed via a full unconditional-jp/jr sweep and polarity re-read of all 12 conditionals in
  the file. Independent review came back completely clean. Bank 11 is 109/651 and the project
  4,322/9,901. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and
  quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 185, bank 11 (14 root routines): ported
  `object_code/ages/parts/ball.s` (`partCode38`, `ball.c`) — the shooting gallery's rolling ball, its
  4-state dispatcher (init/rolling/collided/thrown-out), and a scattered handful of tile-collision,
  target-hit, and falling-rock-interaction helpers. `func_6b00` is a true local reached via genuine
  `call` from three sites, implementing an unusual `scf`/`push af`/.../`pop af`/`ccf`-or-not idiom
  to compute a carry-flag result while leaving `A` untouched — modeled with `push_effect`/
  `pop_effect` on a packed 16-bit A:F value and no call-boundary push/ret_effect, matching the
  established plain-local-helper precedent. Self-review this batch was this session's worst by a
  wide margin: 12 real bugs found and fixed before the file even compiled — four places where a
  plain `jp` tail-jump was wrongly written as `CALL_C`, six systematically inverted `jr nc`
  branches (a copy-paste error that propagated across every collision-check block in the file), one
  `jr c` burned to its jump target instead of its own end, and one off-by-one instruction-boundary
  shift inside `func_6b00` that mislabeled three consecutive byte ranges. All caught by re-deriving
  every instruction address-by-address against the ROM before compiling, rather than trusting the
  first draft — both the full address-coverage script and a subsequent maximally-thorough
  independent review (explicitly told to trust nothing already "fixed") came back completely clean
  afterward. Bank 11 is 98/651 and the project 4,311/9,901. Gates: lint 0, 30k verify 0 failures
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames
  with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-15: milestone 3 phase 6 batch 184, bank 11 (5 root routines): ported
  `object_code/common/parts/vireProjectile.s` (`partCode3a`, `vireProjectile.c`) — the Vire ghost's
  spinning projectile, its 4-subid dispatch (rising/beam/split-into-five/split-off-baby-ball), and
  four small already-independently-hooked helpers it calls via `CALL_C`. `func_6d22` is a genuinely
  local subroutine (`; @param b angle / @param e subid`, no independent hook row) reached two
  different ways: pure ROM fallthrough from `subid1` (needing no `push_effect`, since whatever
  return address is already legitimately on the stack from the enclosing hook's own dispatch is
  exactly correct once the chain eventually reaches `objectCopyPosition_hook`'s real `ret`), and a
  genuine `call` from `subid2_state2`'s 5-part spawn loop expecting a return-and-continue (needing
  `push_effect(gb, continuation)` at that call site specifically, per the established pattern-b
  convention) — caught and fixed during self-review before the gate ran, since the first draft
  wrongly tried `CALL_C` on this non-hook local. Independent review then found two more real bugs
  self-review missed: a CRITICAL one (an unconditional `jp` at the end of the spawn loop was burned
  3 bytes into the immediately-following `.db` jump-table data instead of stopping at its own
  3-byte end, which would have hit `burn_rom`'s hard-abort — `exit(4)` — the first time this
  5-part spawn loop ever completed), and eight inverted `jr nc` branches (every "select value by
  distance/health tier" block in the file had its `cp`/`jr nc` polarity flipped). Both fixed and
  reverified, plus an extra sweep of every remaining unconditional `jp` and `FC`-flag conditional
  in the file turned up nothing further. Bank 11 is 84/660 and the project 4,297/9,910. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 183, bank 11 (1 root routine, 247 instructions): ported
  `object_code/ages/parts/veranSpiderweb.s` (`partCode56`, `veranSpiderweb.c`) — Veran's spiderweb
  boss-arena part: a 4-subid dispatcher (the web core, its beam-firing/reflecting sub-object with a
  nested 5-state machine, a damage-tracking piece, and a companion-interaction piece), all as one
  large function with internal goto labels since only `partCode56` itself has an independent hook
  row. Both self-review (28 conditionals re-verified against ROM mnemonics one at a time) and
  independent review (which additionally recomputed every labeled block's own reported cycle total
  by hand and diffed it against the C file's burn ranges) came back completely clean — zero bugs
  found on either pass. Bank 11 is 79/674 and the project 4,292/9,924. Gates: lint 0, 30k verify 0
  failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 182, bank 11 (13 root routines): ported
  `object_code/ages/parts/kingMoblinBomb.s` (`partCode3f`, `kingMoblinBomb.c`) — the bomb King
  Moblin throws (9-state dispatcher: init, waiting/shared logic, held-by-Link, thrown, waiting to
  be picked up, exploding, and an apparently-unused upward-lob/land path) plus its Link- and
  King-Moblin-collision checks. `common_kingMoblinBomb_state1` is genuinely called from three
  sibling states expecting a normal return, and separately two pairs of adjacent roots
  (`state3`→`state4`, `state7`→`state8`) reach each other by pure fallthrough with zero cost. A
  `call z, playSound` whose taken and not-taken paths both converge on the same following
  `objectApplySpeed` tail-call uses `CALL_C_CC` for the taken side and a plain `CYC` for the
  not-taken side, with the shared continuation written once. Self-review caught one `jr` burned to
  its jump target instead of its own end before the gate ran; independent review, given the recent
  run of concentrated bugs in adjacent files, did a full independent block-cycle-total
  recomputation across every labeled block and found nothing further. Bank 11 is 78/691 and the
  project 4,291/9,941. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 181, bank 11 (10 root routines): ported
  `object_code/ages/parts/blueStalfosProjectile.s` (`partCode3d`, `blueStalfosProjectile.c`) — the
  charge-and-throw fireball part for the Blue Stalfos enemy, its baby-ball explosion fragments, and
  the reflect-off-sword/hit-Link paths. `blueStalfosProjectile_checkShouldExplode` has a genuine
  stack-discarding divergence: called via ordinary `call` from two states each expecting a normal
  return-and-continue, but on its "should explode now" exit it does `pop bc` to discard its own
  return address before its final `ret`, so real hardware returns two levels up the stack, past
  its immediate caller entirely. Modeled with nothing more than a literal `SET_BC(pop_effect(gb))`
  and a plain `CALL_C` at both call sites — `CALL_C`'s own built-in `gb->pc==ra && gb->sp==sp_+2`
  check already detects this exact divergence (worked through by hand: the extra pop leaves
  `sp==sp_+2` with `pc` unchanged, then the function's own final `ret` pops again, landing on
  neither the expected `pc` nor the expected `sp`) and correctly hands off via `hook_continue`
  without any special-casing needed at the call sites. After the prior batch's unusually high bug
  count, this one was written with a full manual address-by-address derivation completed and
  cross-checked *before* first compile rather than after; both self-review and independent review
  came back clean, with zero bugs found. Bank 11 is 65/700 and the project 4,278/9,950. Gates:
  lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 180, bank 11 (1 root routine, 179 instructions):
  ported `object_code/common/parts/itemFromMaple.s` (`itemFromMaple.c`) — `partCode14`/`partCode15`
  are two labels aliasing the exact same address (`PART_ITEM_FROM_MAPLE`/`_2` share identical code),
  so the file's sole registered root is canonically `partCode14_hook` even though the disassembly's
  own comments call it "partCode15"; it's a 5-state item-from-Maple-the-monkey dispatcher (spawn,
  falling, waiting-to-be-picked-up, just-hit, being-carried-to-Maple) plus the Link-collected-item
  exit path with its treasure/ring lookup. Three genuinely local helpers (`setOamData`,
  `setDroppedItemPosition`, `moveToMaple`) are reached by real `call`s with no independent hook
  registration; `setOamData` tail-jumps into the already-registered `partSetAnimation_hook` rather
  than its own `ret`, needing the `push_effect(gb, continuation); helper(gb);` treatment established
  in `itemDrop.c`, while `setDroppedItemPosition` and `moveToMaple` end in their own real `ret`s and
  need no such push. This batch had the highest bug density of any batch so far: self-review found
  and fixed 10 `jr`-target-vs-own-end byte-range bugs, ALL but one concentrated in
  `setDroppedItemPosition`; a first independent review then found 6 inverted `jr c`/`jr nc`/`ret nc`
  polarity bugs, five more in that SAME function plus one in `substate2`; a third, from-scratch
  review pass (triggered by that density rather than trusting the fixes) then found one more real
  bug the first two passes missed — a plain 4-cycle undercount (a `jp` instruction executed right
  after a local `call` returned had no `CYC` burn at all) — using a new technique: recomputing each
  ROM label's own reported total cycle count by hand and diffing it against the sum of the C file's
  burn ranges for that same address span, which catches a missing/extra burn a pure address-set
  membership check can miss when nothing else in the file happens to reuse that same range. 17 real
  bugs in one ~230-line file, this session's worst so far, all in ordinary control flow with nothing
  structurally unusual about the ROM code — the lesson isn't "this pattern is risky," it's that
  local helper functions get systematically less scrutiny while being written than the root's own
  main dispatch, and deserve the same address-by-address rigor. Bank 11 is 55/711 and the project
  4,268/9,961. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 179, bank 11 (16 root routines): ported
  `object_code/common/parts/spikedBall.s` (`PART_SPIKED_BALL`, `spikedBall.c`) — the ball-and-chain
  soldier's spiked ball head (6 states: init, slow/fast rotation, throw-alignment wait, released,
  and retracting-collision) and its decorative chain link, plus five leaf helpers. Three separate
  registered roots reach each other by pure ROM fallthrough with no jump instruction at all
  (`spikedBall_head_state0`→`state1`, `state2`→`setDefaultDistanceAway`,
  `setDefaultDistanceAway`→`updatePosition`) — each ends with a plain tail-call to the next root's
  `_hook`, with any real `call` before the fallthrough point going through `CALL_C` first and no
  cycle burn invented for the zero-byte transition itself. `spikedBall_chain`'s entry deliberately
  reuses the `E` register its caller (`partCode2a`) already loaded, matching the ROM's own
  `ld a,(de)` with no preceding `ld e,`. Self-review caught a `jr` burned to its jump target instead
  of its own 2-byte end (in `spikedBall_updateStateFromParent`, both instances of the same
  duplicated `jr z,+`) before the gate ran; independent review then caught two inverted `jr nc`
  branches (in `partCode2a`'s sword/shield collision check and `spikedBall_head_state3`'s
  throw-alignment check) that both self-review and the automated address-coverage check missed,
  since flag polarity isn't something an address-based check can see at all. Bank 11 is 54/727 and
  the project 4,267/9,977. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 178, bank 11 (16 root routines): ported
  `object_code/ages/parts/bigBangBombSpawner.s` (`PART_BIGBANG_BOMB_SPAWNER`,
  `bigBangBombSpawner.c`) — a 6-state bomb-spawner dispatcher (`partCode49`) plus fifteen small
  leaf helper roots. Two distinct "shared tail across separately-registered roots" shapes: three
  hazard-check roots (`func_78bd`, `func_78ce`, `func_78d2`) each `jr` into a fourth root's
  (`func_78dd`) own address range at the byte *after* its `ld a,$03` prologue — rather than let
  three of the four callers re-execute a sibling root's prologue (which would silently overwrite
  the A value each had just set), the shared 3-instruction tail was pulled into its own private
  static helper that all four call. Four direction-lookup roots (`func_79c4/79cb/79d2/79d9`) each
  reach a single shared `jp add16BitRefs` instruction (one `jr` each, one direct fallthrough) —
  simple enough that each of the four just duplicates the one-instruction `CYC` burn plus tail-call
  rather than needing a helper. Two RST `$00` jump tables (6-entry and a 4-entry one with two
  identical stub-target entries), one RST `$18` (add-double-index) each in two roots, and RST `$10`
  (add-A-to-HL) in three roots all used the by-now-standard private per-file helper trio. A full
  address-coverage self-check and an independent review both came back clean — no bugs found, only
  two purely cosmetic/no-behavioral-impact style notes not applied. Bank 11 is 38/734 and the
  project 4,251/9,984. Gates: lint 0, 30k verify 0 failures with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 177, bank 11 (6 root routines): ported
  `object_code/ages/parts/ramrockGloveFormArm.s` (`PART_RAMROCK_GLOVE_FORM_ARM`,
  `ramrockGloveFormArm.c`) — the Ramrock boss's detachable glove/arm part, a 7-state jump-table
  dispatcher (`partCode35`) with a parallel 6-entry "subid bit 7 set" variant table sharing most of
  its state bodies, plus five small already-independently-hooked helper roots it calls
  (`func_693b`, `state0func_6956`, `state3func_6970`, `state0func_6992`, `func_69a5`) reached via
  genuine `call`s needing `CALL_C`. One true local (`state0func_6731`, reached by `call` from two
  sites, always falls through to a plain `ret`) was implemented as a bare CYC-only helper with no
  push/ret_effect, since it is never independently hook-dispatched. Another local
  (`state4func_68d7`) is reachable both by a `call` (needing a continuation) and by a `jr`
  fallthrough from the "subid bit 7" variant (whose `ret` is the outer routine's real end) — rather
  than give one block two incompatible return semantics, its five instructions are duplicated: once
  inlined in `state4` with no `RET`, once as a standalone label ending `RET`. Two RST `$00`
  jump-table dispatches and one substate table needed the same private per-file helper convention
  as `updateParts.c`/`itemDrop.c`. Independent review found two real cycle-accounting bugs after a
  full byte-address self-check (which confirmed every ROM instruction in range was covered, but
  couldn't tell a *duplicated* code path was missing a cycle burn on only one of its two copies):
  a `jr nc` inside `func_693b` was burned to its branch target instead of its own 2-byte end, and
  the inlined copy of `state4func_68d7` was missing the 4-cycle cost of its own `ret` (silently
  absorbed into the following `call`'s burn range instead of charged separately) — both fixed and
  reverified. Bank 11 is 22/747 and the project 4,235/9,997. Gates: lint 0, 30k verify 0 failures
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames
  with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 176, bank 11 (13 root routines, new branch
  `claude/bank11-phase6` off latest `main`): opened bank 11 — the `object_code/*/parts` tree (item
  projectiles, drops, and effects spawned by enemies/interactions) — with the whole
  `object_code/common/parts/itemDrop.s` (`PART_ITEM_DROP`, `itemDrop.c`), covering the drop's
  4-state lifecycle (falling, bouncing, waiting to be picked up, and the just-hit/collected path),
  its gfx/speed/fairy-movement initializers, and the hazard/sidescroll/ground collision helpers.
  Two locals reached by genuine `call`s needed the plain-function-plus-`push_effect` treatment:
  `getRelatedObj1ID` (called once, tail-jumps into a registered hook whose own `ret` naturally
  continues the caller) and `checkCollidedWithLink` (called once, but its ground truth ends in a
  `pop hl` that discards the pushed return address and diverts permanently into a sibling
  `linkCollectedItem` function on one of its three exits) — the latter needed the caller's own
  continuation split into a fourth function, `afterCollisionCheck`, so the diverting exit could
  skip it validly. Three private RST vector helpers (add-A-to-HL, add-double-index-to-HL, and the
  jump table) and three mis-decoded data tables (`@spriteData`, `@itemDropTreasureTable`,
  `@speedTable`) needed no hook. Review found and fixed a critical bug that would have crashed the
  emulator on contact (`burn_rom`'s own safety check aborts on an unconditional `jr` whose range
  doesn't end at its own 2-byte boundary): an unconditional `jr @onWaterSidescrolling` had been
  burned three bytes wide instead of two, swallowing the first byte of the next label. Also fixed
  an inverted `ret nc`/`ret c` polarity that would have shown the "about to disappear" flicker
  during the wrong 60-frame window instead of the last one, and roughly a dozen `jr`/`jp` byte-range
  bugs missed by the usual width-based scan because their targets happened to fall within 1-4 bytes
  of the instruction's own end (the scan's normal tolerance) — this batch's real lesson: for a
  2-byte `jr`, only a target exactly 2 bytes past the start is "normal"; anything else, however
  small the apparent diff, is a bug, and must be checked by mnemonic, not by diff magnitude. A 30k-
  frame verify and the full 290,174-frame reference replay both passed clean with the expected
  state hashes; the movie only ever exercises `partCode01`'s own top-level dispatch (794 calls) and
  never any of its own callees, so the two independent instruction-level reviews are this batch's
  only correctness evidence for nearly the whole file. Bank 11 is 16/775 and the project
  4,229/10,025. Gates: lint 0, 30k verify 16 pre-existing/unrelated `lcdVector_hook` failures
  (unchanged state hash `99e1f928f2cab55a`), full reference replay 0 state-hash mismatches over
  290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 175, bank 9 (3 routines): completed Old Zora, Toilet Hand,
  and Mask Salesman. Independent checkpoint replays found Poe (`interactionCode59`) divergent at
  frame 33,300, so it remains generated alongside the earlier deferred Hardhat Worker. Bank 9 is
  207/423 and the project 4,249/10,053. Lint, 30k verification, full reference replay, and both
  normal and quirk suites passed.

- 2026-09-14: merged `claude/bank10-phase6` (batches 162–179): completed every remaining bank-10
  routine, including Ramrock, Twinrova, merged Twinrova, Veran Final, and Ganon. Bank 10 is now
  fully readable C at 454/454; the regenerated project registry is 4,246/10,079. Claude's branch
  passed lint, 30k hook verification, the full reference replay, and normal and quirk suites per
  batch before this merge.

- 2026-09-14: milestone 3 phase 6 batch 174, bank 9 (1 routine): completed interaction 61's lever
  dispatcher, state/substate machine, and private connection/pull helpers. Bank 9 is 204/449 and
  the project 3,904/10,387. Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with
  state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches across 11,451,027 calls
  over 290,174 frames with state a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 173, bank 9 (1 routine): completed interaction 55's
  dispatcher and private script/graphics path. Interaction 58 and roots 59–5C were conservatively
  deferred after independent full-replay mismatches. Bank 9 is 203/472 and the project
  3,903/10,410. Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with state
  3e450c2620a3f6a3, full reference replay 0 state-hash mismatches across 11,451,027 calls over
  290,174 frames with state a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 172, bank 9 (10 routines): completed Mamamu Dog's two
  subid state machines, graphics/script initialization, and shared movement/target helpers.
  Bank 9 is 202/477 and the project 3,902/10,415. Gates: lint 0, 30k verify 0 failures across
  4,484,031 calls with state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches
  across 11,451,027 calls over 290,174 frames with state a62ae98192befee8, normal and quirk
  suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 171, bank 9 (1 routine): completed interaction 60's
  treasure dispatcher, spawn/grab state machine, and internal visibility/treasure/touch helpers.
  Bank 9 is 192/502 and the project 3,892/10,440. Gates: lint 0, 30k verify 0 failures across
  4,484,031 calls with state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches
  across 11,451,027 calls over 290,174 frames with state a62ae98192befee8, normal and quirk
  suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 170, bank 9 (4 routines): completed interactions 51–54,
  including their reachable state/script/graphics paths; Mamamu Dog retains its standalone
  dynamic subid routines in generated C. Bank 9 is 191/541 and the project 3,891/10,479. Gates:
  lint 0, 30k verify 0 failures across 4,484,031 calls with state 3e450c2620a3f6a3, full
  reference replay 0 state-hash mismatches across 11,449,589 calls over 290,174 frames with state
  a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 169, bank 9 (1 routine): completed interaction 3F's
  dispatcher, all subid/state paths, graphics/script initialization, and private local tails.
  Bank 9 is 187/566 and the project 3,887/10,504. Gates: lint 0, 30k verify 0 failures across
  4,484,031 calls with state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches
  across 11,449,589 calls over 290,174 frames with state a62ae98192befee8, normal and quirk
  suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 168, bank 9 (32 routines): completed Tokay's dispatcher,
  thief and participant state paths, held-item/accessory initializers, script loader, and
  stink-bag cutscene. Bank 9 is 186/586 and the project 3,886/10,524. Gates: lint 0, 30k verify
  0 failures across 4,484,031 calls with state 3e450c2620a3f6a3, full reference replay 0
  state-hash mismatches across 11,449,589 calls over 290,174 frames with state a62ae98192befee8,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 167, bank 9 (23 routines): completed the Subrosian and
  Impa NPC dispatchers, each state/subid/script path, and their private local return/lookup tails.
  Bank 9 is 154/628 and the project 3,854/10,566. Gates: lint 0, 30k verify 0 failures across
  4,484,031 calls with state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches
  across 11,449,589 calls over 290,174 frames with state a62ae98192befee8, normal and quirk
  suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 166, bank 9 (13 routines): completed Ambi's dispatcher,
  main state, update/script path, subids 1-6/8/A, local return, and script loader. The completed
  state/substate tables and local paths absorbed twenty-six generated rows. Bank 9 is 131/659 and
  the project 3,831/10,597. Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with state
  3e450c2620a3f6a3, full reference replay 0 state-hash mismatches across 11,449,592 calls over
  290,174 frames with state a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 165, bank 9 (1 new routine): ported the Past Old Lady's
  subid dispatcher and its private graphics/script initializer paths, then simplified the already
  registered Ghost Veran, bear, and bird dispatch hooks into readable direct state calls. Eight
  private generated rows disappeared. Bank 9 is 118/685 and the project 3,818/10,623. Gates:
  lint 0, 30k verify 0 failures across 4,484,031 calls with state 3e450c2620a3f6a3, full reference
  replay 0 state-hash mismatches across 11,449,588 calls over 290,174 frames with state
  a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 164, bank 9 (18 routines): completed the remaining
  Forest Fairy entries: the subid 1 discovery path, the subid 2-4 state dispatch and movement
  states, generic NPC setup/update, and the post-maze, post-Jabu, secret, postgame, and
  companion-search NPC paths. Eight private table/local rows left the generated registry. Bank 9
  is 117/693 and the project 3,817/10,631. Gates: lint 0, 30k verify 0 failures across 4,484,031
  calls with state 3e450c2620a3f6a3, full reference replay 0 state-hash mismatches across
  11,449,588 calls over 290,174 frames with state a62ae98192befee8, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 163, bank 9 (16 routines): ported forest-fairy subid 0's
  dispatcher, states 0-3, movement-preset loader, movement, and animation; the misc-man and
  mustache-man dispatchers; and the remaining five readable soldier tails. The private table
  locals vanished with their sole callers. The build caught private helpers using `CALL_C` without
  the required caller-owned `sp0_` thread context; passing it before the instruction reviews made
  the stack-aware call path explicit. Bank 9 is 99/701 and the project 3,799/10,639. Gates: lint
  0, 30k verify 0 failures across 4,484,031 calls with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches across 11,449,588 calls over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 162 (17 routines): added the final shop-item purchase
  state plus palace-soldier substates, escort/NPC subids, initialization, and dungeon-six check
  helpers. The second instruction review caught six static-JP paths falling through to their next
  C label; each now returns after its direct target. Twenty-five generated local rows disappeared;
  bank 9 is 83/721 and the project 3,701/10,693. Gates: lint 0, 30k verify 0 failures across
  4,484,031 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches
  across 11,449,563 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-14: milestone 3 phase 6 batch 168, bank 10 (11 routines, branch
  `claude/bank10-phase6`): ported the whole `kingMoblinMinionMain.s` — states 0, 2-9, A, and the
  shared `animate` tail (`kingMoblinMinionMain.c`). State 0 indexes a private 4-byte-per-subid
  data table via a local RST $18 add-double-index vector; state 7 indexes a private per-subid
  angle-pair table via RST $10 add-A-to-HL. Both tables had been mis-decoded as fake executable
  routines by the transliterator (same class as the batch-165/166 lookup tables) and needed no
  hook entries. Independent review caught a missing one-cycle burn on the RST $10 helper's carry
  path (the vector's final unconditional `ret` at `$0014` was left unburned) before the gate — the
  same helper existed correctly in `enemyCommonCode.c` and was miscopied. Six generated local rows
  (both data tables plus four absorbed jump/tail locals) disappeared; bank 10 is 112/762 and the
  project 3,766/10,684. Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 167, bank 10 (2 routines, branch
  `claude/bank10-phase6`): ported the first two small `object_code/common/interactions/` files —
  `interactionCodee0` (`eraOrSeasonInfo.c`, a 4-state RST $00 dispatcher) and `interactionCodee5`
  (`ringHelpBook.c`, whose `runState` local is a second RST $00 dispatcher reached through a real
  local `call`). Both routines' local dispatch targets disappeared from the transliterator's
  reachability scan once their sole callers became readable, the same pattern as batch 164's bounce
  locals; `runState` itself lost its own hook row for the same reason and became a plain internal
  helper after the first lint pass caught it. Two independent instruction reviews found no further
  defects. Ten generated local rows disappeared; bank 10 is 101/768 and the project 3,755/10,690.
  Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 166, bank 10 (6 routines, branch
  `claude/bank10-phase6`): opened `object_code/common/enemies/commonBossCode.s` (new file
  `enemyCommonBossCode.c`) with `enemyBoss_dead` (absorbing its `alreadyPlayedDeathSound` local),
  `enemyBoss_spawnShadow`, `enemyBoss_initializeRoom` (with a conditional `call nz` needing
  `CALL_C_CC`) tail-calling `enemyBoss_initializeRoomWithoutExtraGfx`, and the
  `enemyBoss_beginMiniboss`/`beginBoss` pair sharing a common tail. Independent review found and
  fixed a field-offset error: two reads of `$81` (the enemy's own `OBJ_ID`, read via `D:E` before a
  new part is set up) had been written as `PART_BASE + OBJ_XH` instead of `ENEMY_BASE + OBJ_ID`.
  One generated local row disappeared; bank 10 is 99/778 and the project 3,753/10,700. Gates:
  lint 0, 30k verify 0 failures across 4,484,031 calls with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`,
  normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 165, bank 10 (21 routines, branch
  `claude/bank10-phase6`): completed `enemyCommonCode.c` with the remaining angle helpers
  (`ecom_updateCardinalAngleAwayFromTarget`/`TowardTarget`, `ecom_updateAngleTowardTarget`,
  `ecom_setRandomCardinalAngle`, `ecom_setRandomAngle`, `ecom_updateAnimationFromAngle`,
  `ecom_flickerVisibility`), the position/subid helpers (`ecom_getSubidAndCpStateTo08`,
  `ecom_moveTowardPosition`, `ecom_readPositionVars`, `ecom_setZAboveScreen`), the kill-object
  chain (`ecom_killObjectH`/`killRelatedObj`/`killRelatedObj1`/`killRelatedObj2`), the gale-seed
  pair (`ecom_galeSeedEffect`, `ecom_blownByGaleSeedState`), the scent-seed pair
  (`ecom_checkScentSeedActive`, `ecom_updateAngleToScentSeed`), and the fall-to-ground pair
  (`ecom_fallToGroundAndSetState8`/`State`) — completing the whole `commonCode.s` source for bank
  10. `ecom_updateAnimationFromAngle`'s `@angleToAnimIndex` table and `ecom_galeSeedEffect`'s
  oscillation table are private lookup data the transliterator had mis-decoded as code (see
  porting-notes); neither needed a hook entry. Independent review caught one inverted `jr nz`
  branch in `ecom_setZAboveScreen`'s Z-clamp before the gate. Bank 10 is 93/779 and the project
  3,747/10,701. Gates: lint 0, 30k verify 0 failures across 4,484,031 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches over 290,174 frames with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 164, bank 10 (12 routines, branch
  `claude/bank10-phase6`): finished `enemyCommonCode.c`'s bounce/spawn/counter cluster —
  `ecom_bounceOffWallsAndHoles`, `ecom_bounceOffWalls`, `ecom_bounceOffScreenBoundary` (a shared
  common body with a second bank-10 copy of the RST $10 vector, its `getDirectionsHit` local, and
  its `reverseDirection` local), `ecom_randomBitwiseAndBCE`, `ecom_setSpeedAndState8(AndVisible)`,
  the `ecom_spawnUncountedEnemyWithSubid01`/`ecom_spawnEnemyWithSubid01` pair with their shared
  tail, `ecom_spawnProjectile`, and `ecom_decCounter1`/`ecom_dec16BitCounter`/`ecom_decCounter2`.
  `reverseDirection`'s apparent independent external caller turned out to be a report-formatting
  artifact (that caller actually calls `ecom_bounceOffWallsAndHoles` itself); once the three bounce
  roots were rewritten the transliterator no longer discovered either bounce-local address at all,
  so both became plain internal helpers with no hook-table row rather than promoted locals. Applying
  the batch-163 lesson, every unconditional `jr` was recomputed at exactly `addr+2` and the local
  call to `getDirectionsHit` got its `push_effect` from the first draft; two independent instruction
  reviews against the pre-rewrite transliteration found no further defects. Two more generated rows
  disappeared; bank 10 is 72/783 and the project 3,726/10,705. Gates: lint 0, 30k verify 0 failures
  across 4,484,031 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 163, bank 10 (15 routines, branch
  `claude/bank10-phase6`): continued `enemyCommonCode.c` with the velocity/adjacent-walls chain —
  `ecom_updateMovingPlatform`, `ecom_applyGivenVelocity`, the four `ecom_applyVelocityFor*`
  entry points, `ecom_applyVelocityGivenAdjacentWalls`, `ecom_applyGivenVelocityGivenAdjacentWalls`
  (with its `applySpeedComponent` local), the four `ecom_get*AdjacentWallsBitset*` entry points
  and `label_025`, `ecom_getAdjacentWallsBitset` (with its `checkCollisionAt` local, and a local
  bank-10 copy of the shared RST $10 add-A-to-HL vector), and `ecom_getAdjacentWallTableOffset`.
  The first draft mis-set several unconditional `jr` burns to the call-instruction's 3-byte width
  instead of `jr`'s own 2 bytes, inverted one `jr nz` branch outright (running the velocity-apply
  call on the wrong side and skipping it on the other), dropped the second half of a two-check
  tile-collision dispatch (both taken and fallthrough arms called the same collision variant), and
  omitted the `push_effect` before a real `call` into the local `checkCollisionAt` helper. All four
  were caught by a full instruction-by-instruction re-derivation against the pre-rewrite
  `gen_bank10.c` transliteration (the actual second independent review) before the gate ran. Two
  more generated local rows (`applySpeedComponent`, `checkCollisionAt`) disappeared with their
  parents; bank 10 is 60/785 and the project 3,714/10,707. Gates: lint 0, 30k verify 0 failures
  across 4,484,031 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash
  mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 162, bank 10 (15 routines, branch
  `claude/bank10-phase6`): opened the bank-10 sweep with `object_code/common/enemies/commonCode.s`'s
  shared enemy helpers (`enemyCommonCode.c`) — incState/incSubstate, the knockback update chain
  (with and without solidity), the hazard-check chain including the shared `checkHazardsCommon`
  body with its caller-escaping hazard-effect tail, and the splash/lava-splash/delete-enemy and
  fall-in-hole/fall-down-hole chains, including the `checkInCenterOfHole` local as a plain static
  helper. Two independent instruction-level review passes against the ROM report and the
  pre-rewrite `gen_bank10.c` transliteration found and fixed three defects before the gate: an
  unconditional `jr`'s byte-end burned through its jump target twice (`ecom_checkHazardsCommon`'s
  tail jump to `ecom_makeLavaSplashAndDelete`, and `ecom_fallDownHoleAndDelete`'s jump to
  `ecom_decNumEnemiesAndDelete`), and a `jr nc` in `ecom_fallingInHole`'s animation-counter clamp
  had its taken/not-taken `CYC`/`CYCT` reversed. Four generated local rows (three
  `checkHazardsCommon` sub-block entries and the `fallingInHole` center-of-hole helper) disappeared
  with their now-readable parents; bank 10 is 45/792 and the project 3,699/10,714. Gates: lint 0,
  30k verify 0 failures across 4,484,031 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches over 290,174 frames with state `a62ae98192befee8`, normal and quirk
  suites 8/8. Ambiguous/deferred: none this batch.

- 2026-09-14: milestone 3 phase 6 batch 161 (17 routines): added shop-item state/display/grab
  paths, six soldier dispatcher/subid roots, and the seasons-fairy interaction state machine.
  Independent review repaired two omitted shop-item table targets and the fairy's JR/call timing
  path. Thirty-seven generated rows disappeared; bank 9 is 66/746 and the project 3,528/11,187.
  Gates: lint 0, 30k verify 0 failures across 4,525,583 calls with state `3e450c2620a3f6a3`,
  full reference replay 0 state-hash mismatches across 11,494,902 calls with state
  `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-14: milestone 3 phase 6 batch 160 (30 routines): continued bank 9 with the complete
  shopkeeper state/price/script path, intro-sprite and Triforce subid state machine, sword/rabbit/
  explosion interactions, and the Z-oscillation helper. Two independent reviews per source caught
  and corrected bank-0 RST vector burns/returns, a double-timed RST push, and a dynamic dispatcher
  continuation that needed to retain its local return frame. Forty generated local rows disappeared;
  bank 9 is now 49/783 and the project 3,511/11,224 across seventeen banks. Gates: lint 0, 30k
  verify 0 failures across 4,552,724 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 11,496,353 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.

- 2026-09-13: milestone 3 phase 6 batch 159 (19 routines): opened bank 9 with the bear state
  dispatcher, bird movement/hop handlers, ghost-Veran states, NPC game-progress checks, and
  pickaxe-worker interaction helpers. Two independent instruction reviews checked all 19 reports,
  conditional timing, smart calls, real RST stack frames and table dispatch, and the inline-data
  boundaries. Regeneration absorbed 112 local/data rows. The parallel bank-8 merge is reconciled
  at 53/678 hooks; bank 9 begins at 19/823, bringing the project to 3,481 readable hooks out of
  11,264 across seventeen banks. Gates: lint 0, 30k verify 0 failures across 4,567,475 calls with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,520,471 calls
  with state `a62ae98192befee8`, normal and quirk suites 8/8.

- 2026-09-13: milestone 3 phase 6 batch 157 (23 routines): opened bank 8 with the Impa cutscene
  state's substates, animation/movement, stone approach, Link screen-exit, and close-position
  helpers. Two independent instruction reviews verified 326/326 bank-08 report instruction starts,
  every RST $00 table and real return frame, local continuation ownership, data boundaries, and
  smart call classifications. Integration retargeted bank-0's close-position wrapper to the new
  bank-8 C helper while preserving its entry stack for nonlocal returns. Twenty generated rows
  disappeared; the project now has 3,432 readable hooks out of 11,576. Gates: lint 0, 30k verify
  0 failures across 4,573,740 calls with state 3e450c2620a3f6a3, full reference replay 0
  state-hash mismatches across 11,526,839 calls with state a62ae98192befee8, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 156 (13 routines): completed the final bank-5 Link
  sidescrolling swimming/in-air/state roots, cliff and hole movement, position/angle helpers, and
  immobilization helpers; the callable `$6164` cliff helper was absorbed with real local call
  frames. Thirty-two generated local/data rows were removed, so bank 5 is fully readable and
  `gen_bank05.c` is deleted. Two independent instruction reviews verified 651 exact instruction
  starts (603 newly-owned after the shared `$5dac` tail), all three RST `$00` tables plus RST
  `$10`/`$18` stack effects, seven conditional smart calls, table/data exclusions, real local
  return guards, and every caller retarget. Review added the known `$5759` dispatcher case and the
  local-call nonlocal-return guards. The project now has 3,409 readable hooks out of 11,596. Gates:
  lint 0, 30k verify 0 failures across 4,584,109 calls with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches across 11,537,208 calls with state `a62ae98192befee8`, normal and
  quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 155 (31 routines): rewrote Link's overworld swimming,
  diving, velocity, jumping, animation, speed, state, and position helpers, plus companion mounting
  completion, and promoted the callable mermaid-suit velocity continuation at bank 5 `$58f7` to a
  durable hook. Thirty-six generated local rows were absorbed. Two independent instruction reviews
  per routine verified 727 unique physical instructions, three RST `$00` dispatchers, the shared
  underwater/drowning and velocity tails, real DE/AF/HL/RST stack effects, every inline-data
  exclusion, and all smart-call classifications; review corrected conditional-call timing at
  `$5675` and `$5b37`. The project now has 3,396 readable hooks out of 11,628. Gates: lint 0, 30k
  verify 0 failures across 4,584,109 calls with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches across 11,537,208 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 154 (25 routines): completed all twenty-four remaining
  Dimitri states, substates, movement, collision, waterfall, and grabbable-object helpers, plus the
  link-riding-animal state machine. Eighteen parent-owned local rows were absorbed. Two independent
  instruction reviews per routine verified 407 unique physical instructions, five RST `$00`
  dispatchers, Dimitri's real local-call/RST `$18` stack path and shared hazard tail, both conditional
  smart calls, and the riding form's dual call/fallthrough return ownership with no defects. The
  project now has 3,365 readable hooks out of 11,664. Gates: lint 0, 30k verify 0 failures across
  4,584,109 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across
  11,707,471 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 153 (29 routines): completed the remaining eighteen Moosh
  state/substate and NPC-update roots, added Dimitri's root, durable state dispatcher, initialization,
  held-object substates, and landing helper, and rewrote the complete transformed-Link state machine.
  Forty parent-owned local rows were absorbed. Two independent instruction reviews per routine
  verified 727 unique physical instructions, six RST dispatch tables and their real stack effects,
  the shared Moosh cutscene tail, Dimitri's clamp/hazard paths, transformed Link's conditional calls,
  every smart-call classification, and all inline-data exclusions with no defects. The project now
  has 3,340 readable hooks out of 11,682. Gates: lint 0, 30k verify 0 failures across 4,584,109 calls
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,707,471 calls
  with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 152 (30 routines): completed the remaining five Maple
  target-selection/meeting helpers and twelve Ricky jump, cliff, substate, and speed helpers, then
  opened Moosh with its root, durable state dispatcher, and eleven movement/hazard states and
  helpers. Twenty-eight parent-owned local rows were absorbed. Two independent instruction reviews
  per routine verified all 510/510 physical instructions, RST dispatch frames, static tails, smart
  calls, and branch timing. Regeneration exposed that Moosh's callable `$7871` synthetic local
  needed a canonical `extra.sym` and ported-registry entry before its readable implementation could
  become a real hook; the registry was repaired before the gate. The project now has 3,312 readable
  hooks out of 11,722. Gates: lint 0, 30k verify 0 failures across 4,584,109 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,707,471 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 151 (31 routines): completed the final thirty bank-16
  serial/game-link packet, state, retry, file-transfer, and comparison roots plus the final
  non-serial changing-floor pattern loader. Three parent-owned local rows were absorbed; bank 16
  is now fully readable and `gen_bank16.c` is deleted. Two independent instruction reviews per
  routine verified 497 serial report rows collapsing to 375 unique physical instructions, the
  shared file-transfer body, both same-address aliases, all caller-frame escapes, and the loader's
  real HL/BC stack effects and `$578a` data cutoff. Self-audit also replaced three invalid
  offset expressions that the RAM-name macro cannot accept. The project now has 3,282 readable
  hooks out of 11,750. Gates: lint 0, 30k verify 0 failures across 4,584,109 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,707,471 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 150 (30 routines): added ten Maple encounter, collection,
  dialogue, and cleanup roots; ten Ricky cutscene state/dispatch helpers; and ten bank-16 serial
  interrupt-state, packet-timer, acknowledgement, and completion roots. Twenty-four parent-owned
  local/data rows were absorbed. Two independent instruction reviews per routine verified 596/596
  reported instructions, the shared dispatch aliases, real AF/DE and Maple deletion stack effects,
  and nonlocal serial/text call boundaries. A final generated-call audit corrected four remaining
  interpreter-call misclassifications and rejected two review edits that would have changed smart
  sound calls into interpreter calls. The project now has 3,251 readable hooks out of 11,753.
  Gates: lint 0, 30k verify 0 failures across 4,584,109 calls with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches across 11,707,471 calls with state `a62ae98192befee8`,
  normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 149 (30 routines): added ten Maple item collection,
  drop, bounds, animation, and bomb-search helpers; ten Ricky state-5/state-8 movement, punch,
  landing, hazard, and screen-edge helpers; and ten bank-16 serial packet send/receive helpers.
  Thirty-six parent-owned local/data rows were absorbed. Two independent instruction reviews per
  routine fixed three Ricky sound calls and six serial wait/completion calls that had been
  classified as interpreter calls even though the transliterator emitted smart calls; the latter
  also required entry-stack captures to preserve their intentional caller-frame escapes. The
  project now has 3,221 readable hooks out of 11,777. Gates: lint 0, 30k verify 0 failures across
  4,584,109 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches
  across 11,707,471 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 148 (26 routines): added ten Maple roots, ten Ricky roots,
  the final bank-6 Link cutscene root, and five bank-5 Link movement and adjacent-wall helpers.
  Fifteen parent-owned local rows were absorbed, including Ricky's internal dispatcher and eight
  Link cutscene labels. Bank 6 is now fully readable and `gen_bank06.c` is deleted. Two independent
  instruction reviews per routine fixed seven Maple and three Link movement call classifications,
  two Ricky dispatcher targets, the Ricky fallback's real stack boundary, and a three-byte jump
  endpoint. Gate diagnosis also caught and reverted an incorrect extra `$0014` return burn on the
  no-carry arm of the shared `rst $10` helper. The project now has 3,191 readable hooks out of
  11,813. Gates: lint 0, 30k verify 0 failures across 4,584,109 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,707,536 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 147 (25 routines): added ten bank-5 Link-state roots,
  ten bank-6 companion-cutscene, raft, and breakable-tile roots, and the final five executable
  bank-7 collision/item roots. One hundred sixteen parent-owned local rows were absorbed, while
  five initial-file/save-verification labels were removed as ROM data. Bank 7 is now fully
  readable and `gen_bank07.c` is deleted. Two independent instruction reviews per routine fixed
  two Link dispatcher targets, one reversed Dimitri branch, two unconditional-jump annotations,
  exact instruction boundaries/effect order in both breakable-tile callers, a stale bank-0
  companion smart call, and two callable RST return effects. The project now has 3,165 readable
  hooks out of 11,854. Gates: lint 0, 30k verify 0 failures across 4,584,109 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,469,013 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 146 (27 routines): added ten bank-5 Link-state, item-cancel,
  death, and warp roots; ten bank-6 Link cutscene state machines; and the final seven bank-7 seed,
  Bombchu, switch-hook, and Cane of Somaria item roots. One hundred forty-nine parent-owned
  local/data rows were absorbed, including the canonical `itemCode20` aliases for item codes
  21–24. Lane audit corrected a reversed `jr nc` arm in `warpTransition3`; two independent
  instruction reviews per routine found no further defects. The project now has 3,140 readable
  hooks out of 11,975. Gates: lint 0, 30k verify 0 failures across 4,610,406 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,812,463 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 145 (19 routines): added seven bank-5 Link-state and warp
  roots, eight bank-6 Link cutscene helpers, and four bank-7 Dimitri, Ricky/Moosh, tornado, and
  minecart-collision item roots. Forty-eight parent-owned local rows were absorbed. After the
  implementation lanes hit their session quota, the batch was conservatively reduced to the
  completed roots before regeneration. Independent review changed two unfinished warp targets to
  direct generated-C dispatch, corrected Dimitri's `$518c` offset-table pointer, and corrected the
  Ricky/Moosh break-tile pointers to `$5c03` and `$5c0c`; two independent instruction reviews per
  routine found no further defects. The project now has 3,113 readable hooks out of 12,124. Gates:
  lint 0, 30k verify 0 failures across 4,610,808 calls with state `3e450c2620a3f6a3`, full reference
  replay 0 state-hash mismatches across 11,813,882 calls with state `a62ae98192befee8`, normal and
  quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 144 (30 routines): added ten bank-5 warp-transition and
  Link-direction roots, ten bank-7 Bombchu targeting, switch-hook, and item roots, and ten bank-6
  Link/companion cutscene helpers. Thirty-four parent-owned local rows were absorbed. Independent
  review caught a duplicated `inc e` in the switch-hook six-byte position-copy loop and replaced a
  raw `$de3b` operand with `w1ReservedItemE_var3b`; two independent instruction reviews per routine
  found no further defects. The project now has 3,094 readable hooks out of 12,172. Gates: lint 0,
  30k verify 0 failures across 4,610,939 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 11,815,437 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 143 (30 routines): added the final companion force-mount,
  counter, tile-breaking, and early warp helpers in bank 5; added ten bank-7 Bombchu movement,
  collision, wall-climbing, angle, placement, and countdown helpers; and added ten bank-6
  boomerang, bomb, bracelet, feather, switch-hook, cane, and slingshot parent-item roots.
  Thirty-four parent-owned local rows were absorbed. Review corrected the switch-hook state's
  two-byte `ld (hl),$ff` range and all following addresses, restored a missing bank-6 `CYCT`
  override in the cane source, fixed a cold Feather branch target, and changed an unconditional
  bracelet `jr` from `CYCT` to `CYC`. Two independent instruction reviews per routine found no
  further defects. The project now has 3,064 readable hooks out of 12,206. Gates: lint 0, 30k
  verify 0 failures across 4,610,939 calls with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches across 11,815,437 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 142 (30 routines): added ten bank-5 companion animation,
  terrain-effect, priority, counter, drowning, and screen-entry helpers; added ten bank-7 item
  post-update, bracelet, throwing, and slingshot roots; and added ten bank-6 sword, shield,
  alternate-sword, flute/harp, seed-satchel, and shovel parent-item roots. Sixty-two parent-owned
  helper/data rows were absorbed or removed. Independent review caught the sword-poke helper's
  deliberate `pop hl` caller escape behind `CALL_ROM`; the final rewrite absorbs it with a real
  return frame and a three-way normal/escape/abnormal-continuation result. Two independent
  instruction reviews per routine found no further defects. The project now has 3,034 readable
  hooks out of 12,240. Gates: lint 0, 30k verify 0 failures across 4,610,939 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,815,437 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 141 (26 routines): added ten bank-5 companion position,
  respawn, cliff, mounting, and spawn helpers; added ten bank-7 item post-update and animation
  helpers across switch-hook, seed-shooter, minecart-collision, and shared post-update sources; and
  completed the final six bank-3F executable entries. Twelve parent-owned helper/data rows were
  absorbed or removed. `func_7cc7` is NOVERIFY because its `$33a2` call can switch threads, while
  its durable `$7cce` continuation remains verifiable. Integration upgraded the remaining old
  bank-0 companion save-position caller to `CALL_C`. Two independent instruction reviews per
  routine found no defects. Bank 3F is fully readable and `gen_bank3f.c` is deleted; the project
  now has 3,004 readable hooks out of 12,302. Gates: lint 0, 30k verify 0 failures across 4,610,939
  calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across
  11,815,617 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 140 (19 hook entries, 21 named shims): added ten bank-5
  companion weapon, direction, mounting, hazard, and dismount helpers; added nine bank-7 item
  entry shims covering the sword, Biggoron Sword/Fool's Ore alias, shovel, punch alias,
  sword beam, dust, and magnet ball; and completed the bank-3F monkey interaction root with a
  durable `$72f9` continuation. Forty-nine parent-owned local rows were absorbed into structured
  C. `interactionCode39_body` is NOVERIFY because its graphics initialization can switch threads,
  while its continuation remains verifiable. Integration upgraded the old bank-0 companion
  dismount caller to `CALL_C` and added its required entry-SP capture. Two independent instruction
  reviews per routine found no defects. The project now has 2,978 readable hooks out of 12,314.
  Gates: lint 0, 30k verify 0 failures across 4,610,939 calls with state `3e450c2620a3f6a3`, full
  reference replay 0 state-hash mismatches across 11,815,617 calls with state
  `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 139 (26 routines): added ten bank-5 companion movement,
  adjacent-wall, relative-tile, collision, and item-creation helpers; added five bank-7 sword-tile
  and damage helpers plus lateral bomb throwing and the shared bounce routine; and completed the
  bank-3F rabbit-spawn and Tuni Nut dispatcher/state cluster with a durable `$7b71` continuation.
  Twenty-nine parent-owned local rows were absorbed into structured C. `interactionCodeb1_body`
  and `tuniNut_state0` are NOVERIFY because their graphics initialization chain can switch threads,
  while the post-call continuation remains verifiable. Two independent instruction reviews per
  routine found no defects. The project now has 2,959 readable hooks out of 12,363. Gates: lint 0,
  30k verify 0 failures across 4,615,975 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 11,820,773 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 138 (25 routines): added nine bank-5 special-object
  initialization, damage, invincibility, active-tile, sidescroll-angle, and companion-collision
  helpers; completed the ten bank-7 bomb routines and two boomerang roots; and finished the three
  real remaining bank-3F rabbit roots plus the durable `$7825` post-graphics continuation. Sixty
  parent-owned local rows were absorbed into structured C. `interactionCode4b_body` is NOVERIFY
  because its graphics initializer can switch threads, while its continuation remains verifiable.
  Integration lint found a local-call return guard reading emulated PC/SP inside a static helper;
  splitting the helper and keeping the guard in each real hook preserved the `$782b`/SP contract.
  Two independent instruction reviews per routine found no defects on the final tree. The project
  now has 2,933 readable hooks out of 12,392. Gates: lint 0, 30k verify 0 failures across 4,615,975
  calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across
  11,820,773 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 137 (14 routines): added the complete bank-6 minecart
  state body behind its bank-5 cross-bank thunk, completed the remaining Ages seed movement,
  diagonal-collision, knockback, and gale-warp helpers, and added eight bank-3F rabbit
  dispatcher/state roots. Thirty-five parent-owned local rows were absorbed into structured C.
  The rabbit roots preserve real RST `$00` frames and use `hook_continue` for unknown callable
  targets while mirroring shared blocks still owned by generated parents. Two independent
  instruction reviews per routine found no defects on the final tree. The project now has 2,908
  readable hooks out of 12,452. Gates: lint 0, 30k verify 0 failures across 4,632,069 calls with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,822,424 calls
  with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 136 (16 routines): added the two bounded bank-6 minecart
  collision and collision-item helpers, eight bank-7 seed deletion/state/burning/animation/scent
  helpers, and six bank-3F monkey state and script dispatch roots. Twenty-five parent-owned local
  rows were absorbed into structured C. The larger cross-bank minecart state machine and the
  thread-switching monkey initializer were conservatively left generated. The rewrites preserve
  nested RST `$10`/`$00` return frames and use `hook_continue` for ordinary unknown dynamic table
  targets; two independent instruction reviews per routine found no defects. The project now has
  2,894 readable hooks out of 12,487. Gates: lint 0, 30k verify 0 failures across 4,632,069 calls
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,822,424
  calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 135 (20 routines): completed all seven remaining
  `commonCode1.s` roots, including solid-tile passability and conveyor movement; added the five
  remaining pre-minecart item-parent common roots, including the real AF caller escape in
  `clearParentItemIfCantUseSword`; and added eight bank-3F monkey disappearance/state roots with
  six RST `$00` dispatch tables. Twenty-six parent-owned local rows were absorbed into structured
  C. Cross-review caught all six unknown RST targets using scheduler-only `hook_handoff`; replacing
  them with `hook_continue(gb, HL, sp0_)` preserved the enclosing callable frame, and both
  independent reviews verified the correction with no further defects. The project now has 2,878
  readable hooks out of 12,512. Gates: lint 0, 30k verify 0 failures across 4,637,156 calls, full
  reference replay 0 state-hash mismatches across 11,828,513 calls with state
  `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 134 (27 routines): added eight bank-7 common-item
  knockback, offset, throwing, hazard, and clink helpers; nine bank-6 child-item allocation,
  capacity, and Link-state helpers; and ten bank-3F monkey animation, jump, gravity, flicker, and
  deletion helpers. Sixteen parent-owned local rows were absorbed into structured C and two
  title-screen sprite tables were removed from the executable registry. The bank-7 rewrite
  preserves the real BC stack, two synthetic local-call return frames, the conditional-call return
  at `$4b55`, and `itemUpdateSpeedZAndCheckHazards`'s intentional `pop hl; ret` escape from its
  caller. Pre-integration review corrected `$cc9c` in `isLinkInHole` from the plausible but wrong
  Link-force-state alias to `wActiveTileType`; two independent instruction reviews per routine
  found no further defects. The project now has 2,858 readable hooks out of 12,538. Gates: lint 0,
  30k verify 0 failures across 4,641,681 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 11,833,038 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 133 (29 routines): completed the remaining bank-7
  collision damage, knockback, sound, flame, and clink helpers, making `collisionEffects.s` fully
  readable; completed the bank-3F frame-resume, reload, refresh, and slot-4 graphics roots, making
  `loadGraphics.s` fully readable; and added five common-item animation/damage helpers plus ten
  bank-6 item-parent bit and input helpers. Four graphics continuations gained durable aliases,
  seven frame-waiting graphics roots and their four bank-0 wrappers are NOVERIFY, and the three
  bank-0 graphics wrapper calls now use `CALL_C`. Five graphics locals and three collision
  internal/data rows left the registry. Main review caught a locally emulated RST `$10` helper
  that burned its final `ret` but used only `pop_effect`; changing it to `ret_effect` restored the
  emulated PC as well as SP, and three cross-reviews found no analogous defect. The project now
  has 2,831 readable hooks out of 12,556. Gates: lint 0, 30k verify 0 failures across 4,641,681
  calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across
  11,833,038 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 132 (25 routines): completed every remaining numbered
  collision-effect handler in bank 7 and all remaining `parentItemUsage.s` roots in bank 6,
  absorbing eighteen parent-owned item locals; added the bank-3F loaded-object scan, all-object
  tile-index refresh, and enemy graphics/property loader with three durable frame-resume
  continuations. Four graphics roots are NOVERIFY because their direct-call chains reach the
  halt-wait scheduler, and bank 0's three parent-item wrappers now use `CALL_C` for the readable
  bank-6 dispatcher. A source audit corrected Batch 131's `interactWithTileBeforeLink_b06`
  unknown jump-table fallback from scheduler-only `hook_handoff` to callable `hook_continue`:
  the bank-0 wrapper reaches that body with `call $4000`, so its return frame must survive dynamic
  dispatch. Both independent instruction reviews found no further defects. The project now has
  2,802 readable hooks out of 12,564. Gates: lint 0, 30k verify 0 failures across 4,670,926 calls
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 11,833,890
  calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 131 (20 routines): completed the final bank-6
  `interactWithTileBeforeLink` dispatcher, making `interactableTiles.s` fully readable; added twelve
  more collision effects and seven bank-3F tree/part/interaction/item graphics roots and
  continuations. A separate audit confirmed `treasureAndDrops.s` has no generated routine left.
  Three post-frame-resume addresses gained durable aliases, and four graphics roots plus their two
  bank-0 wrappers are NOVERIFY because their direct-call chains reach the halt-wait scheduler.
  Integration upgraded six stale bank-0 interpreter calls. Both independent reviews found the
  same silent adjacent-RAM defect in `loadTreeGfx_body`: ROM `$cc19` is
  `wLoadedTreeGfxActive`, not `$cc18` `wLoadedTreeGfxIndex`. The project now has 2,777 readable hooks
  out of 12,587. Gates: lint 0, 30k verify 0 failures across 4,685,512 calls with state
  `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 12,136,202 calls with
  state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 130 (28 routines): completed `giveTreasure_body` and its
  remaining lookup, parameter-dispatch, mode, and duplicate-ring helpers; added twelve more bank-7
  collision effects and eight more bank-6 chest, sign, pushable-block, key-block, key-door, and
  overworld-keyhole handlers. Rewriting the treasure and tile parents removed sixteen generated
  internal/data rows. Integration promoted seven durable treasure locals and upgraded bank 0's
  `giveTreasure` edge to `CALL_C`, threading the enclosing SP through its static helper. Review
  fixed a three-cycle `jr` that had incorrectly used three bytes, restored the full `ret_effect`
  for a locally emulated RST helper, and changed the callable treasure-mode jump-table fallback to
  `hook_continue`. The project now has 2,757 readable hooks out of 12,587. Gates: lint 0, 30k
  verify 0 failures across 4,685,541 calls with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches across 12,569,417 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 129 (33 routines): added thirteen more bank-7 collision
  effects and shared damage tails, all twelve simple `giveTreasure_body` parameter modes in bank
  3F, and eight bank-6 informative-tile, key-door, Ghini-spawner, pushing, and key-count helpers.
  The treasure modes gained durable aliases so the still-generated parent jump table dispatches to
  their readable hooks instead of retaining inline generated bodies. Eight parent-owned
  interactable-tile locals left the executable registry. Integration upgraded the bank-0 roller
  text wrapper to `CALL_C`; independent review also replaced the tile jump table's unknown-target
  `hook_handoff` with `hook_continue`, preserving its callable return frame. The project now has
  2,729 readable hooks out of 12,603. Gates: lint 0, 30k verify 0 failures across 4,685,538 calls
  with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 12,569,454
  calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 128 (16 routines): completed the broken-tile and general
  item-drop decision paths, added ten more collision-effect entries in bank 7, and made palette
  refresh, bank-3F weapon graphics loading, and the per-object tile-index-base helper readable.
  Sixteen `.db` item-drop sets and seven parent-owned internal labels correctly left the executable
  registry. Promoting the tile-index helper required a durable alias at bank 3F `$4256`; without
  it, its generated caller retained the vanished non-hook local name and failed compilation. Two
  independent instruction reviews found and fixed four stale bank-0 `CALL_ROM` edges to the newly
  readable drop, availability, palette-refresh, and weapon-GFX targets. The project now has 2,696
  readable hooks out of 12,611. Gates: lint 0, 30k verify 0 failures across 4,685,537 calls with
  state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches across 12,569,439 calls
  with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 127 (27 routines): added parent-item initialization,
  shop input, parent clearing, pushing counters, tile-offset checks, and breakable-tile interaction
  creation in bank 6; the first nine collision-effect entries and shared damage tails in bank 7;
  and ring realignment, inventory insertion, treasure-display lookup, and gasha-maturity handling
  in bank 3F. Three callable local helpers gained durable aliases, while two internal inventory
  blocks and one inline data row correctly left the registry. Independent review found and fixed
  the bank-0 treasure-display wrapper's stale `CALL_ROM`, changing it to the smart `CALL_C` now
  required for its readable bank-3F target; a second review verified the correction and found no
  analogous callers. The project now has 2,680 readable hooks out of 12,634. Gates: lint 0, 30k
  verify 0 failures across 4,685,369 calls with state `3e450c2620a3f6a3`, full reference replay 0
  state-hash mismatches across 12,848,391 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 126 (19 routines): completed the bank-7
  `fileManagementFunction` dispatcher and its initialize/save/load/erase paths, the bank-3F object
  graphics insertion and interaction-data lookup paths, and five isolated treasure helpers in
  banks 3F and 16. All file variables use named RAM, conditional calls retain taken timing, and
  the file and graphics local entries preserve their real caller stacks. Rewriting the treasure
  roots absorbed four zero-caller branch labels (`haveItem`, `isUpgrade`, `dontHaveItem`, and
  `foundItem`), so the total hook registry correctly fell by four. Two independent instruction
  reviews found no defects. The project now has 2,653 readable hooks out of 12,637. Gates: lint 0,
  30k verify 0 failures across 4,685,336 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 13,033,804 calls with state `a62ae98192befee8`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 6 batch 125 (27 routines): opened the final sweep with the
  bank-7 save-file verification helpers (`clearFileAtHl`, copy/checksum/address lookup,
  `verifyFileAtHl`, `verifyFileCopies`, and their seven local entries) and bank-3F object-GFX
  lookup/bookkeeping helpers (find/allocate/index/use tracking plus enemy, part, and item lookup,
  including two local entries). The callable file-verification RST dispatcher preserves its
  pending return with `hook_continue`; no thread switch is involved. Promoting generated local
  labels required stable canonical aliases in `extra.sym` as well as `ported.txt` and
  `rewritten.txt`, otherwise regeneration removed their generated rows without creating readable
  hook declarations. Two independent instruction-level reviews found no remaining defects. The
  project now has 2,634 readable hooks out of 12,641. Gates: lint 0, 30k verify 0 failures across
  4,685,346 calls with state `3e450c2620a3f6a3`, full reference replay 0 state-hash mismatches
  across 13,033,919 calls with state `a62ae98192befee8`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 124 (6 routines): completed `updateTextbox` and its
  standard, option, and inventory dispatch entries, plus the final `agesFunc_10_70f6` and
  `agesFunc_10_7298` cutscene roots. `extraTextIndices` and its four generated children were
  confirmed as `.dw`/`.db` data; two bank-10 substates remain readable direct-only helpers rather
  than invented public hooks. Fixed RST tables now call every known readable target directly and
  reserve `hook_continue` for unknown dynamic fallbacks. The whole-movie verifier exposed five
  older instruction-level cycle defects across four routines: both black-bar routines burned
  taken two-byte jumps through skipped instructions, `checkEnemyKilled` had the same jump error
  and omitted its final `ret` burn, and `objectDataOp7` applied `ld c,a` without burning it. It
  also established that the infinite `introThreadStart` needs `HOOK_NOVERIFY` across its real
  scheduler stack handoff. Two independent reviews approved each
  late fix. Phase 5 is complete with 2,607 readable hooks out of 12,641. Gates: lint 0, 30k
  verify 0 failures across 4,685,346 calls with state `3e450c2620a3f6a3`, full reference replay
  0 state-hash mismatches across 13,033,919 calls with state `a62ae98192befee8`, whole-movie hook
  verification 0 failures across 44,996,366 calls with state `d2c9e4b3facf784b`, normal and quirk
  suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 123 (21 routines): completed the startup initializer,
  intro-cinematic roots, endgame-20 root and selected room-loading states, and the disable-LCD
  room-loader path. Seven stable post-call entries preserve returns across thread-capable calls.
  The `$4091` OAM-DMA bytes remain ROM source data copied into HRAM, while execution continues
  through the existing `$ff80` RAM hook; the `$59ab` jitter table and `$6fe3` room table also stay
  data-only. Review caught five callable RST dispatchers using `hook_handoff` for their unknown
  targets; restoring `hook_continue` with each entry SP preserves the pending caller return. Bank
  3 is now 636/636, `gen_bank03.c` is deleted, and the project has 2,601 readable hooks out of
  12,649. Two independent instruction-level reviews approved the corrected code. Gates: lint 0,
  30k verify 0 failures across 4,744,137 calls with state `3e450c2620a3f6a3`, full replay 0
  failures across 13,576,120 calls with state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 122 (23 routines): completed the bank-3 Timewarp,
  Ambi's Passage, Jabu-Jabu opening, linked-game, and Black Tower escape roots and their nested
  dispatchers. Three stable post-call entries preserve returns from thread-capable graphics and
  room-loading paths. The `$7c13` object table and deliberately corrupted `$7e54`/`$7e88`/`$7ea4`
  garbage labels left the executable registry; the `$7e3b` warp payload disappeared with its
  parent. Review caught five incorrect `CALL_C` target operands in the Timewarp cleanup path;
  restoring `$35e3`/`$35f4`/`$3605`/`$35c2`/`$35d2` ensures verify mode interprets the intended
  callees. Bank 3 is now 616/636 and the project has 2,580 readable hooks out of 12,647. Three
  independent instruction-level reviews approved the corrected code. Gates: lint 0, 30k verify
  0 failures across 4,744,137 calls with state `3e450c2620a3f6a3`, full replay 0 failures across
  13,601,537 calls with state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 121 (24 routines): completed the Black Tower completion,
  turn-to-stone, and Twinrova-reveal roots and nested state dispatchers, with eleven stable
  thread-resumption points. Review established that callable RST dispatchers must preserve their
  entry stack with `hook_continue` on an unrecognized dynamic target. Bank 3 reached 594/640 and
  the project reached 2,557 readable hooks out of 12,650. Gates: lint 0, 30k verify 0 failures
  across 4,744,137 calls with state `3e450c2620a3f6a3`, full replay 0 failures across 13,602,015
  calls with state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 120 (32 routines): completed bank 3's miscellaneous
  cutscene dispatcher plus the Nayru-singing, Black Tower explanation, and Nayru-to-Maku-Tree
  roots and nested state dispatchers. Thirteen stable post-call entries preserve returns from
  thread-capable room and graphics loaders. The generated `$6625` graphics-header table and
  `$6722` timing table were confirmed as `.db` data and disappeared from the executable registry
  when their parent was rewritten; bank 3 is now 570/631 and the project has 2,532 readable
  hooks out of 12,641. Two independent instruction-level reviews approved all cycle endpoints,
  branches, stack effects, calls, and registry changes. Gates: lint 0, 30k verify 0 failures
  across 4,744,137 calls, full replay 0 failures across 13,602,015 calls with state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 119 (17 routines): completed bank 12 with the
  `parseGivenObjectData` opcode dispatcher and added bank 3's fairy-room states, booted-from-palace
  state machine, shared room/object loader, and four stable thread-resumption entries. Five labels
  under `companionScript_subid00Script` were confirmed as script-VM bytecode rather than SM83 code
  and removed from the executable registry, so regeneration deleted both `gen_bank0c.c` and
  `gen_bank12.c`; banks 0C and 12 are now 101/101 and 41/41 readable entries, and bank 3 is
  538/625. Review also repaired `func_55f8`'s unconditional-JR byte endpoint from `$55fb` to
  `$55fa` and replaced raw RAM addresses with their intended fields. Two independent
  instruction-level reviews approved the corrected code. Gates: lint 0, 30k verify 0 failures
  across 4,753,441 calls, full replay 0 failures across 13,621,359 calls with state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 118 (6 routines): completed bank 1 with its final
  Seasons-only helper, required `$5d34` thread continuation, and three unreachable garbage roots.
  Their invalid calls and operand-byte jumps remain interpreted, while the genuine `$34ad`
  instruction boundary inside `updateAllObjects` received a stable Bank-0 continuation and direct
  C tail. Three embedded data-table labels left the executable registry and regeneration deleted
  `gen_bank01.c`; bank 0 is 653/653 and bank 1 is 294/294 readable entries. Three independent
  instruction-level reviews approved the final code. Gates: lint 0, 30k verify 0 failures across
  4,753,464 calls, full replay 0 failures across 13,621,809 calls with state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 117 (31 routines): added bank 1's game-logic and
  initialization roots, loading-room flow, standard-game-state dispatcher, cutscenes 00/01/03/04/05,
  and the remaining cutscene 15/18/19 room-loading paths. Ten thread-resumption entries received
  stable aliases, including the newly required continuation at `$4ca4`, and the two bank-0 callers
  now dispatch directly to readable C. Registration uses the canonical `func_4000_b01` name so the
  unrelated bank-16 `$4000` collision remains generated. Two independent instruction-level reviews
  approved every cycle range, branch, stack effect, and continuation. Gates: lint 0, 30k verify 0
  failures across 4,753,465 calls, full replay 0 failures across 13,627,189 calls with state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 116 (12 routines): reproduced the deliberately corrupted
  bank-2 rectangle-copy tail in readable C, including three stable row-loop aliases, the call into
  data at `$7de3`, and the final jump into the middle of a bank-0 instruction at `$05df`. The first
  remains `CALL_ROM`; the second continues in the interpreter within the current C frame. This
  completes all 553 bank-2 entries and regeneration deletes `gen_bank02.c`. Two independent
  instruction-level reviews approved every cycle range, branch, RST frame, stack effect, and
  corrupted transfer. Gates: lint 0, 30k verify 0 mismatches across 4,753,457 calls, full replay
  0 failures across 13,807,186 calls with state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-13: milestone 3 phase 5 batch 115 (15 routines): completed bank 2's normal executable
  roots with the DMG-mode guard and VBlank loop, indoor alternate-world tileset update, bank-2
  trampoline, menu fade-out/reload and save-reset flow, rectangle helper, and status-bar body.
  Five embedded dungeon-map and rectangle data rows left the executable registry, and six local
  entries received stable aliases. Review and integration corrected IO-register access through
  `mem_wr`, an offset RAM read that cannot use the token-pasting `W8` macro, stack-context
  propagation through newly direct callers, and static dispatch through the readable bank-call
  trampoline. Two independent instruction-level reviews approved the corrected code. Gates:
  lint 0, 30k verify 0 mismatches across 4,753,457 calls, full replay 0 failures across
  13,807,186 calls with state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 114 (15 routines): added bank 1's palette-fade root,
  shared fading update, locked-color dispatcher and leaves, and the two safe cutscene-15 dispatcher
  entries, plus bank 2's text-character copy chain and file-select root, initialization, and main
  mode dispatch. Six local entries received stable aliases; an embedded sprite-data row, a folded
  state-3 entry, and an obsolete synthetic cutscene continuation left the registry. Review caught
  a taken `jr nz` burning through its target rather than the branch's two-byte endpoint, a missing
  RST `$00` return frame, and file-select modes 6/7 targeting adjacent in-game wrappers instead of
  `$4526`/`$4b29`. Two independent instruction-level reviews approved the corrected code. Gates:
  lint 0, 30k verify 0 mismatches across 4,773,224 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 113 (18 routines): added bank 2's menu update,
  menu-specific dispatch, fade-in/fade-out, graphics-save, save-and-quit, and secret-list roots and
  state paths, plus the file-select error and input-wait tails. Nine local entries received stable
  aliases, bank-0 thread callers were retargeted to the readable roots, and thread-switching paths
  retain explicit handoff. Independent review caught an adjacent-but-wrong menu RAM field
  (`$cbcc` instead of `$cbcb`) and two same-size literal mistakes in `openMenu` (comparison 1 and
  sound `$56` instead of 3 and `$54`); exact ROM operands were restored before runtime testing.
  Two independent instruction-level reviews approved the corrected code. Gates: lint 0, 30k
  verify 0 mismatches across 4,802,991 calls, full replay state `64bddd0dfe384126`, normal and
  quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 112 (20 routines): completed bank 2's text-input root,
  character selection, name/secret lower-option tables, delete/back/start behavior, horizontal and
  vertical cursor movement, wraparound loops, output writes, and cursor-update tails. Nineteen
  local entries received stable aliases. Preflight identified `runTextInput@soundEffects` at
  `$46fe` as an eight-byte ROM data table rather than executable code, so it left the routine
  registry instead of receiving a hook. The translation preserves six independent RST `$10`/`$00`
  temporary frames and every exact loop re-entry. Two independent instruction-level reviews
  approved the code. Gates: lint 0, 30k verify 0 mismatches across 4,802,991 calls, full replay
  state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 111 (18 routines): added bank 2's complete erase-file
  dispatcher and four-state flow, file-select secret-entry dispatcher and validation path, and the
  in-game secret-entry dispatcher, verification, ring-secret load, success, and error paths.
  Fifteen local entries received stable aliases. All three RST `$00` tables preserve their outer
  caller frames, while deferred text-input continuations use explicit handoff; the secret decoder's
  `$cec0` scratch reads use the shared temporary-buffer name rather than unrelated enemy-placement
  union aliases. Two independent instruction-level reviews approved the code. Gates: lint 0, 30k
  verify 0 mismatches across 4,802,991 calls, full replay state `64bddd0dfe384126`, normal and quirk
  suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 110 (19 routines): added bank 1's room-brightening,
  room-music, common-graphics, entered-warp-position, and companion cleanup paths; bank 2's
  file-select graphics-state and text-input graphics/bootstrap paths; and bank 4's complete room
  tile/attribute generator plus the group-5 C3 and group-2 F7 room replacements. Seven local
  entries received stable aliases, eight embedded tile-data rows left the executable registry,
  and bank 4 is now 169/169 readable with `gen_bank04.c` deleted. Independent review caught three
  conditional branches aimed at `$5e71` that had incorrectly skipped the target's music-value load
  and four cycles; the shared target was restored before runtime testing. Two independent
  instruction-level reviews approved the corrected code. Gates: lint 0, 30k verify 0 mismatches
  across 4,802,991 calls, full replay state `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 109 (19 routines): added bank 1's dungeon-layout
  loading and clearing, active-room and first-layout address discovery, and dungeon-minimap update
  paths, plus bank 2's file-name and child-name entry dispatchers and states. Eleven local entries
  received stable aliases. The translation preserves SVBK and ROM-bank changes, RST `$00`/`$10`/
  `$18` stack effects, dungeon copy/search loops, and the original caller frame when handing off to
  the deferred text-input engine. Integration threaded the hook entry SP into the promoted bank-1
  call from bank 0. Two independent instruction-level reviews approved the corrected code. Gates:
  lint 0, 30k verify 0 mismatches across 4,802,991 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 108 (23 routines): added bank 2's equipped-item,
  item-icon, status-bar map, extra-heart address, and ring-tile rendering helpers, plus bank 4's
  changed-tile queue, VRAM write, subtile-address, interleaved-tile, and VBlank tile-write queue
  paths. Twelve local entries received stable aliases and the adjacent VRAM-address table left the
  executable registry. The translation preserves real AF/BC/DE/HL stacks, exact RST `$10`/`$18`
  return addresses, banked WRAM restoration, and cross-bank readable-call routing. Integration
  threaded the public hooks' entry SP through static bank-0 helpers after `CALL_C` made their
  nonlocal path reachable. Two independent instruction-level reviews approved the corrected code.
  Gates: lint 0, 30k verify 0 mismatches across 4,802,987 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 107 (19 routines): added bank 2's remaining
  file-select mode helpers, blank item-submenu sprites, dungeon-map scrollable-tilemap generator,
  compass tile selector, and floor-address calculation, plus bank `$3f` textbox initialization.
  Seven local entries received stable aliases and five adjacent sprite, map, and textbox data rows
  left the executable registry. The translation preserves all physical fallthroughs, banked WRAM
  switches, real AF/BC/HL stacks, RST `$10`/`$18` return addresses, and direct palette-loading
  tail. Two independent instruction-level reviews found no defects. Gates: lint 0, 30k verify 0
  mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and quirk suites
  8/8.
- 2026-09-12: milestone 3 phase 5 batch 106 (23 routines): added bank 4's toggle-block,
  underwater Jabu, entering-shutter, opened-chest, switch, and conditional single-tile substitution
  paths. Seventeen local entries received stable aliases and five embedded tile-data rows left the
  executable registry. The translation preserves the shutter and flag-test stacks, conditional
  call timing, the RST `$18` return address, local-entry state, and direct replacement tails. Final
  review also replaced six raw RAM/HRAM literals with their canonical room-layout and scratch-byte
  symbols. Two independent instruction-level reviews approved the corrected code. Gates: lint 0,
  30k verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and
  quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 105 (20 routines): added bank 2's subscreen-2
  essence and heart-piece renderer, ring-box capacity lookup, tilemap rectangle and treasure-display
  helpers, and inventory and harp sprite rendering paths. Thirteen local entries received stable
  aliases and two adjacent data rows left the executable registry. The translation preserves the
  real BC, DE, and HL stacks, exact RST `$10`/`$18` return addresses, branch timing, physical
  fallthroughs, and direct renderer tails. Two independent instruction-level reviews found no
  defects. Gates: lint 0, 30k verify 0 mismatches across 4,688,901 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 104 (18 routines): added bank 2's seed-and-harp
  submenu renderer, seed-index and placement helpers, equipped-ring marker, stored-item grid, and
  subscreen-1 treasure, ring-box, and ring drawing paths. Eleven local entries received stable
  aliases and four adjacent sprite/position tables left the executable registry. The translation
  preserves the real BC, DE, and HL stacks, all RST `$10`/`$18` return addresses, and the direct
  sprite/ring-renderer tails. Two independent instruction-level reviews found no defects. Gates:
  lint 0, 30k verify 0 mismatches across 4,688,901 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 103 (27 routines): added bank 2's inventory state-2
  and state-3 dispatchers and states, directional-input helpers, ring-box cursor traversal, submenu
  item selection, and all three cursor renderers. Seventeen local entries received stable aliases
  and ten data/table labels left the executable registry. Review corrected eight unconditional-JR
  burns and five endpoints, restored the `$588e` RST `$10` opcode burn, realigned the `$58b1`
  instruction sequence and call return PC, named the bank-4 tilemap address, and removed duplicate
  ported-root rows. Two independent final reviews approved the corrected code. Gates: lint 0, 30k
  verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and quirk
  suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 102 (21 routines): added bank 2's safe file-select
  mode 1, mode 3, and mode 5 state paths, text-speed menu input and cursor rendering, and nineteen
  independently hookable local entries; the mode-1 state that restarts the current thread remains
  generated. Three table/data labels left the executable registry. Review changed unconditional
  jump burns to `CYC` and split the shared cursor-adjust loop so `$4350` returns to the exact
  `$4344` collision block without repeating the `$4341` zero precheck. Two independent final
  reviews approved the corrected timing, dispatchers, RST stacks, and integration. Gates: lint 0,
  30k verify 0 mismatches across 4,688,901 calls, full replay state `64bddd0dfe384126`, normal and
  quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 101 (27 routines): added bank 2's Game Link menu
  dispatcher and seven states, Link/file sprite rendering, save/quit input and sprite helpers, and
  the fake-reset dispatcher and states. Eighteen local entries received stable aliases and one
  sprite-table label left the executable registry. Review corrected the Link animation source from
  `$cbbd` to the ROM's `$cbb6`, then shortened the `$4d27` unconditional-JR burn to its physical
  two-byte endpoint at `$4d29` instead of its `$4d2b` target. Two independent final reviews
  approved the corrected timing, calls, stacks, dispatchers, and registry integration. Gates: lint
  0, 30k verify 0 mismatches across 4,688,896 calls, full replay state `64bddd0dfe384126`, normal
  and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 100 (26 routines): completed bank 2's ring-menu
  rendering and selection helpers, including cursor sprites, equipped-ring markers, ring-box and
  list drawing, page counters, displayed text, and all eight independently hookable local paths.
  Six sprite/table labels left the executable registry. Review removed a duplicate RST `$18`
  return push and restored AF on the text helper's deliberate caller escape. Two independent
  final reviews approved the corrected stack, timing, data boundaries, and caller retargets.
  Gates: lint 0, 30k verify 0 mismatches across 4,688,896 calls, full replay state
  `64bddd0dfe384126`, normal and quirk suites 8/8.
- 2026-09-12: milestone 3 phase 5 batch 99 (24 routines): bank 2's ring-list selection and
  scrolling core, including ring text and descriptions, ring-box insertion/search, page changes,
  both scroll directions, and list-cursor movement. Twelve local entries received stable aliases;
  the direction-offset table left the executable registry. Review corrected the appraisal delay
  helper call to `CALL_C` so its deliberate double-pop return unwinds the caller, and restored two
  new cases in the already-readable ring-menu dispatchers. Two independent final reviews approved
  the exact stack, timing, RAM naming, and generated integration. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 98 (18 routines): bank 2's safe secret-list state,
  scrolling, cursor, text-selection, bulk-name loading, and secret-data lookup paths. Twelve local
  entries received stable aliases; the thread-starting initialization remains generated. Cursor
  sprite bytes and the linked-secret table left the executable registry, reducing its total by two.
  Reviews restored an unassigned `swap a` result, retained a promoted-local interpreter boundary,
  and replaced raw/union low-byte operands with the semantic secret-menu fields. Gates: lint 0,
  30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 97 (24 routines): bank 2's ring-menu dispatcher and
  complete appraisal flow, including initialization, prompts, payment, appraisal, refunds,
  redraws, and delayed exit. Five local entries received stable aliases. The promoted local
  dispatcher retained its interpreter boundary, and the delay helper preserves its real `pop af`
  plus grand-caller return. Two independent reviews approved the instruction timing, jump-table
  stacks, static tails, canonical RAM names, and registry closure. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 96 (24 routines): bank 2's inventory-menu dispatcher,
  initialization, item-text selection, equipping, and three subscreen input paths. Seventeen local
  entries received stable aliases. The targeted verifier found that two branches to the shared
  item-text tail skipped the tail's physical `jp`, leaving them four cycles short; the whole-movie
  replay then exposed a separate interrupt-boundary change where a newly global local helper needed
  `CALL_ROM` at its former interpreter boundary. Independent re-review approved both fixes. Gates:
  lint 0, 40k verify 0 mismatches (6,500,238 calls), standard 30k verify 0 mismatches (4,688,896
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 95 (16 routines): bank 1's fall-through-floor warp,
  cutscene-17 dispatcher and states, alternate-line wave-scroll initialization, and eight safe
  cutscene-15 leaves around its generated thread-switching state. Thirteen local entries received
  stable aliases and the cutscene-17 warp destination left the routine registry as data. Quality
  review corrected a `call nz` to use `CALL_C_CC` and replaced a numerically convenient status-bar
  alias with the canonical interaction-slot base; re-review approved both fixes. Gates: lint 0,
  30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 94 (21 routines): bank 1's remaining vertical scrolling
  substate and row update, object-category clearing helpers, compass-sound selection, and shocked-
  Link animation paths. Twelve local entries received stable aliases and one transition-data row
  left the routine registry. Quality review replaced six raw object-slot/buffer addresses with
  their canonical RAM symbols; the required re-review approved the focused fix and the full added
  code. Gates: lint 0, 30k verify 0 mismatches (4,688,896 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 93 (18 routines): bank 1's scrolling-transition states
  4–5, horizontal and vertical setup, per-frame scroll and Link-position updates, transition
  finalization, follower reset, and room-boundary setup. Nine local entries received stable
  aliases, while three data-only labels left the routine registry. Self-review caught an `srl h`
  whose helper result was not written back to H; two independent reviews then approved every
  branch, RST stack, direct tail, shared entry, and named RAM access. Gates: lint 0, 30k verify 0
  mismatches (4,688,896 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 92 (30 routines): bank 1's room-transition states 0–3,
  boundary and animation initialization, transition-direction and water checks, camera-position
  calculation, screen shake, scroll-register updates, and room-darkening checks. Seventeen local
  entries received stable aliases. Independent review corrected RST `$18` selection, shared-state
  entry routing, three no-carry branches, and the `$420d` fallthrough; the first gate then exposed
  the taken `jr z` at `$4117` using fallthrough timing, producing identical registers but a
  one-cycle verifier deficit. Gates after the fix: lint 0, 30k verify 0 mismatches (4,689,284
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 91 (28 routines): bank 2 status-bar hide/show and
  menu/palette-copy bodies, heart-beep timing, common graphics loading, equipped-item and treasure
  tile rendering, and the complete file-select/in-game heart-display path. Fourteen local entries
  received stable aliases and four older readable callers were retargeted after regeneration.
  Independent review rechecked every conditional branch side, the palette-loop SVBK writes, and
  the real BC/HL stack nesting; unlike batch 90's RST `$18` wrapper, the local RST `$10` helper at
  `$5479` does not own its return push, so the explicit `$547a` push is required. Gates: lint 0,
  30k verify 0 mismatches (4,717,640 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 90 (29 routines): bank 2 file-display addressing,
  file-select input/cursor movement, name-buffer handling, name/secret input cursor rendering,
  character graphics, entry-cursor positioning, heart/death-counter drawing, and acorn-cursor
  rendering. Eight local entries received stable aliases; regeneration removed twenty-two
  associated sprite/offset/table data labels from the routine registry. Review corrected two
  conditional calls that needed `CALL_C_CC` and an RST `$18` path that pushed its return PC twice.
  The first gate then exposed a taken `jr nc` at `$4817` that still executed its fallthrough
  `ld c,$00`; the first hook mismatch was frame 523 and the TAS state diverged at frame 540.
  Gates after the fix: lint 0, 30k verify 0 mismatches (4,717,623 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 89 (30 routines): bank 1's screen-scroll row and
  VBlank-queue helpers, tile-row copy and wave/big-buffer initialization, palette-transition
  pipeline, following-Link path maintenance, screen-reload clearing, and the object-category
  enable helpers. Six local/collision-safe entries received stable aliases. Regeneration removed
  the old generated `clearMemoryOnScreenReload`, so its existing readable caller was retargeted to
  the new hook. Independent review approved every RST return PC, real stack pair, SVBK transition,
  physical fallthrough, and shared tail. Gates: lint 0, 30k verify 0 mismatches (4,717,623 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 88 (21 routines): the complete text-option and
  inventory-text state machines, including nineteen newly aliased local entries and the full
  character/space/address/DMA fallthrough chain. The textbox cursor-delay table left the routine
  registry because it is data. Self-review corrected a taken branch that must skip the status
  update at `$4d8a` and restored taken timing on the `$4e25` control-code branch; independent
  review approved the final instruction boundaries, real RST stack, and direct tails. Gates:
  lint 0, 30k verify 0 mismatches (4,716,239 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 87 (18 routines): the standard-text textbox state
  machine, including every independently hookable state, its shared finish/exit paths, and the
  non-exitable-text branch. The thread-starting textbox root and `updateText` dispatcher remain
  generated. Review caught the shared RST `$10` carry path applying `inc h` during the preceding
  conditional-return burn and split the effects at their physical instruction boundaries. Gates:
  lint 0, 30k verify 0 mismatches (4,716,219 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 86 (26 routines): the Zelda-kidnapped dispatcher,
  state runner, and twenty-three safe substates/helpers. Twenty-four local entries received stable
  aliases. State F preserves its real call returning to the still-generated state 10 at `$7aca`,
  then hands off with the live stack; the `$7b48` mini-dispatch also remains generated. Quality
  review found fourteen raw generic-cutscene WRAM operands and replaced them with their existing
  `ram.h` names; lint did not flag the readability regression. Gates: lint 0, 30k verify 0
  mismatches (4,715,002 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 85 (17 routines): `func_782a` plus the Flame of Sorrow
  dispatcher, initialization, all twelve substates, and its local sound helper. Thirteen local
  entries received stable aliases. The state-1 path preserves a real call at `$78b5` followed by
  physical fallthrough into substate 2, while the adjacent Zelda-kidnapped root remains generated.
  Regeneration removed the `$7978` warp destination from the routine registry because it is data.
  Independent reviews approved both RST stacks, conditional returns, call frames, and static tails.
  Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 84 (25 routines): twelve safe timewarp states and
  dispatch bodies plus thirteen safe Black Tower escape states and helpers. Twenty-one local
  entries received stable aliases. The thread-switching timewarp and escape roots remain
  generated, and the selected timewarp paths tail directly into the existing generated
  `timewarpCutscene_incCBB3`. Independent reviews approved the shared `$72b0` body, escape-state
  fallthroughs, and `func_7e40`'s conditional call/jump timing. Gates: lint 0, 30k verify 0
  mismatches (4,715,002 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 83 (24 routines): the `func_701d` cutscene dispatcher
  and states, the dungeon-8 wall-retraction dispatcher and states, two dungeon-2 collapse helpers,
  and the complete safe `func_7168` state cluster through its physical stub return. Eighteen local
  entries received stable aliases. The shared `$71e4` continuation preserves `B = $51` from one
  entry and `B = $52` from the other, while the separate `$71f4` path keeps its real call frame.
  Independent spec and quality reviews approved all RST stack effects, direct tails, shared
  fallthroughs, and cycle boundaries. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 82 (21 routines): the Maku Tree disappearing handler,
  nine safe Nayru-warp states, four safe turn-to-stone states, and seven safe Twinrova-reveal
  states. Twenty local entries received stable aliases; states whose direct-call chains reach
  `refreshObjectGfx` or room loading remain generated. Byte-level review confirmed that the
  Twinrova state-2 non-linked branch deliberately skips the linked-only `func_6f9e` call and
  rejoins at `$6beb`. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 81 (15 routines): seven safe Black Tower explanation
  states, the three-entry `func_6733` graphics dispatcher and its two canonical bodies, and five
  safe Black Tower completion states. Fourteen local labels received stable aliases; the two
  explanation graphics indices sharing address `$6747` deliberately share one hook. The `$6725`
  state preserves its static jump to `func_6f44` and its non-taken fallthrough into `func_6733`.
  Independent review approved all RST stack effects, the WRAM-bank AF pair, conditional calls,
  branch timing, and static tails. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 80 (15 routines): the bank-3 pregame-intro root, its
  thirteen-way state dispatcher, and all thirteen state entries. The state-0 linked-game branch
  reuses the previously readable room-loading body, while state A preserves the real AF stack
  pair around its WRAM-bank switch. Fourteen local labels received stable hook aliases; independent
  review approved every instruction boundary, conditional return, RST dispatch stack effect, and
  static tail. Gates: lint 0, 30k verify 0 mismatches (4,715,002 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 79 (24 routines): the bank-2 gale-seed menu root and all
  four states, the map-menu root/state/input flow, minimap display-room selection, dungeon small-key
  and visited-floor bookkeeping, and the complete map room-text resolver/special-code cluster.
  Eight local state and special-code labels received real hook aliases. Regeneration removed the
  old unsuffixed `mapGetRoomText`, so its existing readable cave-popup caller was retargeted to the
  new hook. Review corrected two inverted no-carry cursor-wrap branches and a shared RST `$10`
  helper that wrote `L` during `add a,l`, one instruction before the real `ld l,a`. Gates: lint 0,
  30k verify 0 mismatches (4,714,516 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 78 (25 routines): closed bank 0 by rewriting
  `linkState07` and its two substate entries, nineteen independently reportable decompressor
  fragments, and the three remaining local draw/text tails. `pushDirectionData` was removed from
  the routine registry because its 64 bytes are data, while the existing handwritten
  `wRamFunction` remains the bank's sole external entry. The transliterator now removes stale
  bodyless generated-bank files and CMake tracks the source glob, so `gen_bank00.c` is genuinely
  gone and stays gone. Shared decompressor helpers propagate `CALL_C` nonlocal continuations back
  through the public hook with its original stack baseline. Bank 0 is 652/652 readable entries:
  651 hooks plus one handwritten RAM external. Gates: lint 0, 30k verify 0 mismatches (4,714,516
  calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk
  builds.

- 2026-09-12: milestone 3 phase 5 batch 77 (16 routines): all thirteen safe local states of the
  Nayru-singing cutscene plus the three safe global late states. The dynamic root, state D, state
  F, and their room-loading continuations remain generated because they reach thread switches.
  The first build caught three attempted `alu_set`/`alu_res` calls; SM83 `set` and `res` do not
  change flags, so the final code uses direct read/mask/write operations at the same opcode
  boundary. Gates: lint 0, 30k verify 0 mismatches (4,714,559 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 76 (15 routines): the bank-3 forest-fairy dispatcher and
  seven safe state/spawn entries, plus the completed-fairy dispatcher and all six of its states.
  Four room-loading states remain dynamic handoffs because they reach the thread-switching
  `refreshObjectGfx`. Post-regeneration review found that rewriting the parent had also removed
  those unsafe locals and two synthetic post-switch continuations from the generated table; seven
  real aliases in `extra.sym` and `ported.txt` preserve their interpreter entrypoints without
  marking them rewritten. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 75 (22 routines): the bank-2 minimap popup-data loader,
  popup-type dispatcher and all twelve selected popup handlers, dungeon-map direction and
  scrolling states, and the map-menu tilemap/sprite rendering tails. Existing readable arrow
  drawing now calls the new scroll checks directly. Cross-review found that the shared RST `$00`
  helper combined `add l` with `ld l,a`, the untaken `jr nc` with `inc h`, and `ld l,a` with
  `jp hl`; splitting every effect back onto its physical opcode restored exact interrupt-visible
  boundaries. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 74 (18 routines): the bank-1 palette-thread stop and
  refresh paths, remaining safe fade handlers, fading-palette calculation, BG palette mixing,
  named loop/helper entries, and the shared counter. Existing readable handlers were retargeted
  from their removed generated stop/counter symbols. Cross-review caught both `jr nc` timing
  directions in the color-mixing multiply loops reversed: no-carry is the taken path that skips
  the add. Gates: lint 0, 30k verify 0 mismatches (4,709,005 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 73 (22 routines): twelve bank-2 dungeon-map floor-list,
  tile drawing, scroll update, room-tile selection, and floor-visibility entries, plus ten bank-3
  miscellaneous-cutscene VRAM, lightning, scrolling, and OAM helpers. The dungeon scroll routine
  preserves the ROM's intentional missing `ret` and falls through into the room-tile selector.
  Cross-review caught an unconditional two-byte `jr` at `$68a2` burned through `$68a5`, one byte
  into unreachable code; its correct endpoint is `$68a4`. Gates: lint 0, 30k verify 0 mismatches
  (4,706,029 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal
  and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 72 (28 routines): the final two bank-4 VBlank copy
  handlers, eight tile-substitution routines, ten bank-1 palette-fade handlers, and eight bank-2
  time-portal, unvisited-map, flag, and Link-position routines. Integration retargeted older
  readable callers after `replaceTiles` and `dungeonMap_getLinkIconPosition` lost their generated
  symbols. Cross-review restored the final one-byte `ret` burn in the map-clear loop; the first
  full replay then exposed the shared RST `$10` carry path applying `inc h` and `ret` one opcode
  early, which diverged only at frame 286,260. Correcting those instruction boundaries made the
  full movie clean. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls), whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 71 (33 routines): ten more bank-4 VBlank copy
  handlers, fourteen bank-2 dungeon-map arrow, room-visited, sprite, warp-site/tree, and named
  loop/return entries, and all nine reportable entries in bank 1's toggle-block `cutscene02`
  state machine. The initial scout also selected `cutscene0b`, but an explicit
  `rewritten.txt` check found it was already readable and prevented a duplicate implementation
  and count. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls), whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 70 (24 routines): ten more bank-4 VBlank copy
  handlers, ten bank-2 dungeon-map item, ownership, floor-symbol, Link-icon, and cursor routines,
  and the four remaining report-clean endgame-09 root/stage dispatchers. Regeneration removed
  the generated `endgameCutsceneHandler_09` function, so its older readable bank-3 dispatcher
  had to retarget the static jump to `endgameCutsceneHandler_09_hook`. Gates: lint 0, 30k verify
  0 mismatches (4,706,029 calls), whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8
  in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 69 (18 routines): ten bank-4 VBlank copy handlers,
  bank 1's `cutscene1f`, and seven bank-3 secret-data loading, game-ID, masking, and length
  routines. Cross-review caught the missing real return-address push before
  `loadUnpackedSecretData`'s `rst $00` helper consumed its stack entry; the shared-address type-0
  and type-1 secret cases use one canonical hook plus a readable forwarding shim. Gates: lint 0,
  30k verify 0 mismatches (4,706,029 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 68 (31 routines): the last endgame-20 state and the
  endgame-0f root/state/substate dispatch path, eight bank-1 seed-tree/warp/toggle-block entries,
  and fifteen bank-3 secret encoding, decoding, formatting, and named-local entries. Integration
  promoted every reportable local block even when its report showed zero callers and retargeted
  the older readable bank-3 cutscene dispatcher after regeneration removed the generated
  `endgameCutsceneHandler_0f` symbol. Gates: lint 0, 30k verify 0 mismatches (4,706,029 calls),
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 67 (27 routines): nine late endgame-20 state/helper
  entries, the bank-1 summoned-Link/cutscene dispatcher and its reportable states/substates, and
  eight bank-3 game-secret generation, packing, cipher, checksum, and validation routines. The
  bank-1 integration promoted named substates to real hook entries and handled the same-address
  `cutscene13`/`tilesetLayoutGroup33` symbol using the generator's canonical alias. Gates: lint 0,
  30k verify 0 mismatches (4,723,535 calls), whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-12: milestone 3 phase 5 batch 66 (15 routines): six more endgame-20 state/helper
  entries, `doInventoryTextFirstPass`, and eight bank-1 warp/timewarp-solidity routines. Review
  restored two omitted one-byte `ret` burns; the reference replay then caught `ld hl,$d0d4`
  translated as `w7TextAddress` (`$d0d5`), and the corrected `w7TextAddress - 1` passed the former
  frame-32,040 failure. Gates: lint 0, 30k verify 0 mismatches (4,723,535 calls), whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 65 (17 routines): five core room/tileset/surfacing
  dispatchers, five safe textbox roots including the dynamic control-code dispatcher, and seven
  endgame-20 state/helper entries. Cross-review again split the bank-0 `rst $00` vector into its
  true instruction boundaries; integration updated older readable textbox callers, kept the
  unported `initTextboxStuff` static tail on its generated symbol, and used `WP(name)[1]` for
  adjacent bytes of banked RAM symbols. Gates: lint 0, 30k verify 0 mismatches, whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 64 (24 routines): the seven remaining top-level
  `scripting.s` roots, nine room-GFX/pirate-room drawing and dispatch routines, and eight more
  endgame stage-1/endgame-20 entries. Cross-review split two combined pairs in the bank-0
  `rst $00` helper so `add a,l`, `ld l,a`, and `jp hl` retain their individual interrupt
  boundaries; lint also required file-local `CALL_C` helpers to receive the owning shim's saved
  stack pointer rather than reading `gb->sp` themselves. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 63 (20 routines): nine bank-4 VBlank/warp resolver
  routines, the final two top-level `bank3Cutscenes.s` routines, and nine reportable endgame
  stage-1 state/OAM entries. Cross-review approved all instruction bodies and found two older
  readable callers that needed the newly exposed `cutscene_clearObjects_hook`; the integrated
  build also required a CMake reconfigure before the new `bank4.c` entered the globbed source
  list. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 62 (25 routines): nine bank-1 forest-scrambler,
  eye-puzzle, screen-edge, and warp-initiation routines; eight Capcom/title-screen intro
  dispatcher and state entries; and eight endgame stage-0/stage-1 entries. Cross-review caught
  an unconditional `jr` at `$618b` whose burn had used the jump target `$618f` instead of the
  two-byte instruction endpoint `$618d`; the corrected build passed every gate. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 61 (20 routines): ten more clean-seas/endgame
  stage-0 states, the three intro dispatcher/root routines, and seven bank-1 death-respawn and
  room-transition helpers. Review split two combined `rst $00` vector burns so register effects
  occur at their exact instruction boundaries and corrected state F's final `jp` endpoint from
  `$5645` to `$5644`; the integrated build also replaced assembly-only intro aliases with the
  generated thread-buffer base and updated the older bank-0 caller to `runIntro_hook`.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 60 (23 routines): eight clean-seas/endgame
  stage-0 entries, eight miscellaneous cutscene dispatcher/state entries, and seven bank-1
  transition, cutscene-trigger, era/season-display, and grass-animation helpers. Two named local
  helper bodies were promoted to real hook aliases so every reportable routine remains directly
  dispatchable. Cross-review approved all entries. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 59 (17 routines): the seven-entry credits
  handler root/state cluster, eight clean-seas state-machine entries, and the two Twinrova
  cutscene-body jump-table dispatchers. The three dynamic roots model their complete `rst $00`
  table-vector stack and carry paths before `hook_handoff`; cross-review approved all entries.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 58 (18 routines): seven deeper endgame
  substates, nine Ambi-passage/Jabu state-machine entries, and the Twinrova cutscene dispatcher
  and room-initialization state. The Twinrova root now models the complete bank-0 `rst $00`
  vector and hands off only at its dynamic `jp hl`. Integration replaced two more
  disassembly-only `wCutsceneIndex` spellings with raw `$c2ef` and exposed the shared Jabu tail
  through a lint-visible `_hook` wrapper. Gates: lint 0, 30k verify 0 mismatches, whole-movie
  state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 57 (20 routines): eight more local endgame
  states, nine Ambi-passage/Jabu/clean-seas cutscene leaves, and the final three entries from
  `ages/cutscenes.s`. Cross-review confirmed all twenty implementations; a parallel lane also
  selected three already-rewritten bank-3 helpers, whose duplicate definitions were caught and
  discarded before regeneration. The two static jumps into thread-capable `func_5d41` remain
  direct C tails as required; only an actual `ld sp,*` boundary uses `hook_handoff`. Gates: lint
  0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in
  both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 56 (22 routines): ten local endgame state
  helpers, nine miscellaneous bank-3 cutscene and graphics-copy helpers, and the final three safe
  bank-10 cutscene state entries. Cross-review corrected the carry path through the RAM-copied
  `rst $10` body so its not-taken `ret nc`, `inc h`, and final `ret` each burn their own
  instruction before applying effects. Integration also replaced a disassembly-only WRAM alias
  and absolute HRAM constants with the generated/raw address forms accepted by readable C.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 55 (26 routines): eight bank-3 local endgame
  state helpers, nine bank-3 Maku/Nayru/black-tower/pregame helpers, and nine more bank-10
  black-tower state entries. Cross-review caught five direct `jp` instructions whose CYC ranges
  used cycle count instead of their three-byte instruction length; all endpoints were corrected
  before replay. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 54 (23 routines): nine bank-3 endgame
  object-data/object-GFX helpers, seven bank-3 fairy/Nayru/Maku cutscene helpers, and seven more
  bank-10 temple-intro state entries. Cross-review caught a missing `$727c` static-jump burn in
  one bank-10 path. The reference gate then isolated a named-RAM typo where raw `$cbb7` had been
  written as `wTmpcbb4` (`$cbb4`); changing it to `wTmpcbb7` restored movie sync. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 53 (18 routines): the final five safe bank-3
  `linkedGameCutscenes.s` routines, four bank-3 temple camera/black-bar/palette helpers, and nine
  bank-10 local state/helper entries from the temple-intro and black-tower cutscene machines.
  The bank-10 disassembly-local entries received explicit C-safe `__` aliases in `extra.sym`;
  jump-table roots, data, and thread-switching tails remain deferred. Gates: lint 0, 30k verify
  0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 52 (20 routines): seven bank-3 linked-game
  interaction-spawn helpers, three bank-3 endgame tile/inventory helpers, and ten bank-3 temple
  and pre-title cinematic entries. Cross-review updated the older state-5 fallthrough to the new
  state-6 hook. The first whole-movie run then isolated an inverted carry test in
  `flashScreen_body`'s `jr nc`; correcting the C condition restored the reference hash. Gates:
  lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8
  in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 51 (26 routines): eight bank-3 linked-game
  cutscene helpers, ten bank-3 endgame countdown/OAM helpers, and eight bank-3 scrolling-tree and
  temple-intro state entries. Dynamic state-machine dispatchers and thread-switching helpers were
  skipped; the two state-0 triforce labels remain internal readable helpers because they have no
  independent callers. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 50 (23 routines): five bank-1 pirate-ship
  update/load/position/angle helpers, nine bank-1 small-cutscene and `cutscenes2.s` wrappers, and
  nine bank-3 riding-horse intro states and sprite helpers. The dynamic pirate room jump table,
  underwater-surfacing table traversal, and switching cutscene tails were conservatively
  deferred. The state-4 `$4f70` local body remains readable as an internal helper rather than a
  separate hook entry because it has no independent caller. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 49 (29 routines): the final seven bank-4
  `roomSpecificTileChanges.s` entries (including the separately addressable shared bodies), three
  bank-1 underwater-wave routines, ten bank-1 Ages cutscene wrappers, and nine bank-3 title and
  riding-horse intro entries. Registration exposed that disassembly-local `@` names cannot be C
  hook identifiers, so the two title-state local entries received explicit `__` aliases in
  `extra.sym`. Review also corrected an old unconditional `jr` annotation from `CYCT` to `CYC`.
  Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`),
  ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 48 (28 routines): ten bank-4 room tile-state
  handlers, eight bank-C global-flag/NPC-movement/delay script commands, and ten bank-3
  Twinrova and title-intro state helpers. Cross-review caught a conditional `call z` that used
  ordinary `CALL_C` timing; changing it to `CALL_C_CC` restored the taken edge's six-cycle call
  timing before replay. The scripting source now has only its dynamic jump-table dispatcher and
  data/bytecode labels deferred. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 47 (30 routines): ten bank-4 vine and overworld
  tile handlers including `replaceVineTiles`, ten bank-C collision/input/flag/display script
  checks, the final bank-3F textbox routine, and nine bank-3 Twinrova cutscene helpers. Dynamic
  RST dispatchers and one thread-switching cutscene state were deferred. Registering
  `replaceVineTiles` removed its generated symbol, so four older readable callers were updated
  to the `_hook` shim. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 46 (30 routines): ten bank-4 Jabu and overworld
  tile-replacement handlers, ten bank-C script tile/call/jump/collision commands, and ten
  bank-3F textbox bank-reading, display-speed, and numeric-substitution routines. The dynamic
  `handleTextControlCode` and `scriptCmd_jumpTable` entries were conservatively deferred. Once
  `readByteFromW7ActiveBankAndIncHl` became handwritten, an older smart caller had to change
  from the disappearing generated symbol to its `_hook` shim; the integrated build exposed and
  fixed that dependency. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 45 (29 routines): ten bank-4 room tile-state
  handlers, ten bank-C room/item/music script commands, and nine bank-3F textbox option,
  cursor, graphics-buffer, and text-stack routines. The local textbox option call at `$561f`
  keeps its real return-address push before entering the readable helper, and all three source
  groups passed independent cross-review without corrections. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 44 (29 routines): ten bank-4 room tile replacements,
  ten bank-C script flag/jump/sound helpers, and nine bank-3F character-display, option-state,
  and heart-piece textbox routines. Cross-review caught two more cycle-count-as-byte-count ranges,
  a missing fallthrough burn after `ret z`, a backward jump aimed at the wrong C label, and a
  smart `(hooked)` call initially written as `CALL_ROM` instead of `CALL_C`. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 43 (30 routines): ten post-load room-GFX handlers in
  bank 2, ten script text/counter/memory helpers in bank C, and ten textbox map-save, DMA,
  character-timer, arrow, clear, and scroll helpers in bank 3F. Cross-review caught a classic
  byte-count error before integration: the three-cycle `jr` at `$51b7` is two bytes, so its burn
  ends at `$51b9`, not `$51ba` (the first byte of the next routine). Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 42 (28 routines): nine tileset-transfer and
  room-layout helpers in bank 4, nine script counter/text/angle/NPC helpers in bank C, and ten
  textbox-selection and cutscene countdown helpers across banks 3F and 3. Review caught a
  four-cycle omission in the carry path of the local `rst $10` helper: after the vector's
  not-taken `ret nc`, it must burn `inc h` and the final `ret`. Integration also caught two
  source files being replaced instead of extended, because five prior bank-4 hook definitions
  disappeared at link time; the files now preserve both batches. Gates: lint 0, 30k verify 0
  mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and
  quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 41 (24 routines): all twelve bank-12
  `roomSpecificCode.s` leaf handlers, including their shared spinner-allocation body, plus twelve
  bank-C script state, object-spawn, coordinate, angle, speed, and Z-speed commands. A shared
  helper declared `static void ..._hook` fell outside `lint_game.py`'s hook-shim recognizer and
  exposed its `gb->sp` access; making the callable helper a normal hook-style function matched the
  existing shared-body pattern. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 40 (21 routines): the scripting input/menu and
  Link-movement control cluster in bank C; textbox DMA and mapping initialization in bank 3F;
  and small state, allocation, and screen-shake helpers from the bank-3 and Ages misc-cutscene
  sources. A review caught the initial `cutscenes.c` filename before integration: routines from
  `ages/cutscenes/miscCutscenes.s` belong in `miscCutscenes.c`, even though another phase-5 source
  is itself named `cutscenes.s`. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash
  clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 5 batch 39 (30 routines): room initialization and random enemy
  placement in bank 2; room-layout rectangle and vine helpers across banks 2 and 4; the first
  script stop/room-flag commands in bank C; and text-buffer clearing/setup in bank 3F. The two
  `rst $10` table advances in `createSeaEffectsPartIfApplicable` exposed a missing taken branch
  inside the shared add-A-to-HL vector: registers matched, but the first 30k verifier reported the
  routine eight cycles short. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean
  (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 38 (7 routines), completing the phase after the bank-2
  audit: `functionCaller_b02`, the enemy-kill list routines, random-buffer generation, random
  enemy placement, and timeportal spawning. The bank-0 trampoline now uses `CALL_C` while
  preserving the selected-ROM-bank push/pop and its dynamic table hand-off. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean (`64bddd0dfe384126`), ctest 8/8 in both
  normal and quirk builds. Phase 4 complete.

- 2026-09-11: milestone 3 phase 4 batch 37 (4 routines): the Link/companion
  riding dispatcher `func_410d`, the pre-object item dispatcher `updateItems`, and the bank-5 and
  bank-6 `linkApplyDamage` bridge/body. Static callers in bank 0 now use `CALL_C`; the item table
  remains a dynamic hand-off to its unreworked item-code targets. A formerly interpreted damage
  trampoline needed its entry-stack capture before being changed to `CALL_C`, since that macro's
  non-return fallback requires `sp0_`. Gates: lint 0, 30k verify 0 mismatches, whole-movie state
  hash clean (`64bddd0dfe384126`), ctest 8/8 in both normal and quirk builds. Phase 4 complete.

- 2026-09-11: milestone 3 phase 4 batch 36 (7 routines): `updateGameKeysPressed` and
  `updateSpecialObjects` in bank 5, `updateItemsPost`, `updateItemPost`, and `itemCodeNilPost`
  in bank 7, plus `parseStaticObjects` and `objectRunMovementScript_body` in banks 22 and 14.
  The three ROM jump-table dispatchers stay explicit in C, handing off their unreworked dynamic
  targets. Gates: lint 0, 30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in
  both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 35 (15 routines): movement-script and static-object
  loading in banks 14 and 22; minecart/raft bank-switch stubs; and the Link/companion animation,
  graphics-frame, animation-data, and ring-damage paths in bank 6. The bank-0 animation callers
  now use `CALL_C` for their rewritten bank-6 bodies. A first pass gave the two-byte `jr` at
  `$441c` its three-cycle width, which `burn_rom` rejected immediately at the jump boundary;
  every `CYC` range now follows opcode byte length. Gates: lint 0, 30k verify 0 mismatches,
  whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 34 (5 routines): object-data opcodes 0, 6, 7, 9, and A
  in `objectLoading.s`. `objectDataOp9` was checked against the ROM bytes after its first draft
  advanced `HL` one byte too far; the terminal `inc l` is not an auto-increment load. The
  coupled special-object and post-item dispatchers were deferred after their jump-table paths
  failed routine verification, rather than retaining an unverified rewrite. Gates: lint 0, 30k
  verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-11: milestone 3 phase 4 batch 33 (15 routines): the object-data parser's setup and
  pointer/allocation flow in `objectLoading.s`: `parseObjectData`, `func_55f8`,
  `parseGivenObjectData_hl`, the two skip-to-op-end loops, `checkSkipPointer`, `skipPointer`,
  `parsePointer`, `continueObjectLoopIfOpDone`, and opcodes 1, 2, 3, 4, 5, and 8. The central
  opcode dispatcher remains generated for the next batch; these routines tail into it directly
  after burning their own static jump bytes. Three raw address spellings initially did not have
  RAM declarations; using the canonical `wNumEnemies`, `wTmpcfc0`, and
  `wScreenTransitionDirection` names restored the required named-RAM convention. Gates: lint 0,
  30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk
  builds.

- 2026-09-11: milestone 3 phase 4 batch 32 (15 routines): the first object-system batch. Bank
  12's object-loading helpers (`read2Bytes`, `readCoordinates`,
  `decEnemyCounterIfApplicable`, `addPositionToPlacedEnemyPositions`,
  `assignRandomPositionToEnemy`, `checkEnemyKilled`), bank 11's part dispatcher
  (`label_11_212`, `updateParts`, `func_11_5e8a`), and bank 4's animation queue/data path
  (`initializeAnimations`, `updateAnimations`, `updateAnimationQueue`,
  `updateAnimationData`, `updateAnimationDataPointer`, `loadAnimationGfxIndex`). Two timing
  corrections were caught by the 30k hook verifier: a loop's taken `jr c` must burn through the
  branch's own end address, not its target (otherwise every part update was 45 cycles short), and
  the shared `rst $10` add-to-HL vector's `ret nc` is a taken conditional return on the no-carry
  path (three cycles short per animation update if burned as not-taken). Gates: lint 0, 30k verify
  0 mismatches, whole-movie state hash clean, ctest 8/8 in both normal and quirk builds.

- 2026-09-10: milestone 3 phase 3 batch 31 (2 routines): the last of the RAM-resident code - `hramOamDmaFunction` (the OAM DMA routine copied to HRAM, at 0xff80) and `wMusicReadFunction` (the music read function copied to $c000). Both use `I(addr, cycles)` with explicit cycle counts, as all RAM-resident code must, since `CYC` walks the ROM at that address and would decode garbage for code that actually lives in WRAM/HRAM. Their hooks replace the generated transliterations; `wMusicReadFunction` is called from the bank 39 sound driver's generated code (`gen_bank39.c`) and `hramOamDmaFunction` from the VBlank handler's rewritten `vblankInterrupt_hook`. With `wRamFunction` already hand-written in `src/game/ram_code.c`, every bank 0 routine is now readable C. Gates: 30k verify 0 mismatches, whole-movie state hash clean, ctest 8/8 both trees, lint 0 problems. Phase 4 (object system) is next.

- 2026-09-10: milestone 3 phase 3 batch 30 (6 routines): the six thread entry points -
  `textThreadStart`, `fileSelectThreadStart`, `thread_1b10` (the game-over screen),
  `introThreadStart`, `paletteFadeThreadStart`, `mainThreadStart`. Finishes bank 0's readable-C
  routines bar the RAM-resident code. Two real findings, plus a call-mechanism correction:

  - `ld ($cbb4),a` is 3 bytes and 4 cycles; writing `CYC(a, a + 4)` (the cycle count, not the
    byte count) burned one byte too many, double-counting the next instruction's own cycles.
    `thread_1b10` runs exactly once in the whole movie (the game-over screen), so the mis-burn
    only ever fires once - but it shifted global timing by a few cycles at that exact moment,
    and the state hash didn't show a diverging frame until 54,840 frames later. Bisected with
    `HOOK_SKIP` down to this one routine before finding the actual byte/cycle mixup; the same
    class of mistake as a `jp nn` mistakenly burned as 4 bytes instead of 3 in this same batch,
    caught immediately by `burn_rom`'s own unconditional-jump guard.
  - Five of these six routines call into still-transliterated code (`initTextbox`,
    `updateTextbox`, `b2_fileSelectScreen`, `runSaveAndQuitMenu`, `runIntro`, `paletteFadeHandler`,
    `runGameLogic`) using `CALL_ROM`, which always takes the interpreter; the ROM's own
    transliteration calls all of them with the smart `CALL(...)` macro (interpreter only in
    verify mode or when unhooked, the routine's own C otherwise). `CALL_C` is the rewrite's
    equivalent and is now used for all of them - `CALL_ROM` stays reserved for the one case
    the transliteration itself marks `/* unported */` (a call always taken through the
    interpreter, regardless of hook state).
  - A suspended thread resuming through `_countdownToRunThread`/`_initializeThread` lands on
    the ROM address right after its `call resumeThreadNextFrame` - the loop's own body for
    `paletteFadeThreadStart` and `introThreadStart` (their `jr` loops back to the routine's own
    entry point), but a few instructions *into* the loop for the other four, which no longer
    have a hook there once their parent is rewritten. The interpreter picks it up correctly
    either way; only the first two keep re-entering the readable C on every resume.

- 2026-09-10: milestone 3 phase 3 batch 29 (13 routines): `begin`, `resetGame`, `startGame`,
  `_mainLoop`, `_mainLoop_nextThread`, `_countdownToRunThread`, `_initializeThread`, and their
  five `ld sp`-boundary continuations (`resetGame__afterSp016c`,
  `_nextThread__afterSp091a`, `startGame__afterSp0925`,
  `_countdownToRunThread__afterSp0998`, `_initializeThread__afterSp09aa`). The whole thread
  scheduler and the VBlank wait loop it drives. Two real bugs and one harness gap found:

  - The loop that scans all four thread slots compares `hActiveThread` against the wrong byte:
    the ROM compares against the low byte of the buffer's *end* address
    (`wThreadStateBuffer + NUM_THREADS*8`, which is 0x00), not the buffer's own low byte
    (0xe0) - copying the wrong operand made the loop run until the 8-bit index happened to
    wrap around to 0xe0 again instead of exiting after four threads. Replay diverged from the
    very first frame; found by bisecting a WRAM diff between this build and the last clean one
    frame by frame down to movie frame 0.
  - `ld sp` is a real thread switch and needs a genuine `hook_handoff` there, the way the
    transliteration's own `HANDOFF_UP` does it - not a direct C call to the continuation. A
    direct call keeps every macro that assumes call/return semantics (`HALT`'s halt-bug path
    among them) tied to the wrong `sp0_`, and five addresses that only ever existed as the
    transliterator's synthetic local labels needed their own entries in a new
    `src/hooks/extra.sym` + `ported.txt` + `rewritten.txt` triple so `hook_handoff` had
    something to dispatch to.
  - Nested verify's fallback for a hook that never returns - burn the same number of cycles in
    the interpreter and compare - cannot make sense of a routine built around a `halt` loop
    that only exits when an interrupt handler (itself suppressed during verification) clears a
    flag; the generator already excludes any transliterated routine containing `halt` from
    nested verification, but that exclusion never reaches a hand-written routine reached only
    by direct C calls. A new `src/hooks/rewritten_noverify.txt` (checked by
    `tools/transliterate.py`, same `H` flag as the generator's own halt detection) marks the
    four routines whose call chain reaches the wait loop without a `hook_handoff` in between.
    Whole-movie replay against `tas/ages.ref` was clean throughout; only the 30k routine-diff
    needed this.

- 2026-09-10: milestone 3 phase 3 batch 28 (7 routines): `lcdInterrupt` and its behaviour
  branches (status bar toggle, the seven-entry `rst $00` jump table, the ring menu's two-stage
  textbox timing, the `lcdInterrupt_0bea` palette-final variant). Most of the writing time went
  into getting conditional branch direction right by hand (`jr nc`/`jr nz`/`jr z` taken-vs-not
  determines which C branch gets `CYC` and which gets `CYCT`, and it is easy to swap them without
  the compiler complaining); four branches came out backwards on the first pass and only the 30k
  verify caught it, by way of `lcdVector_hook`'s own comparison against the interpreter - the vector
  routines don't show a per-routine call count (see the porting notes), so they rely on their
  vector's check, not their own.

- 2026-09-10: milestone 3 phase 3 batch 27 (9 routines): `vblankInterrupt`, the VBlank function
  queue (`runVBlankFunctions`, `vblankFunctionRet` and the five queue handlers, including the
  HDMA one that runs 19,567 times in the movie). `jp hl` inside a rewrite is
  `hook_continue(gb, target, sp0)` followed by `return`, the way the transliteration's `HANDOFF`
  is - looping around it instead hung the replay at frame 500. Calls to another routine's C now
  go through `CALL_C`, which mirrors the generated `CALL`: the interpreter in verify mode or when
  the target is unhooked, the C function otherwise. Local labels of a rewritten routine cannot go
  in `rewritten.txt` (the generator drops them with their parent); they run in the interpreter.

- 2026-09-10: milestone 3 phase 3 batch 26 (20 routines): the thread state helpers
  (`threadStop`, `threadRestart`, `restartThisThread`, `stubThreadStart`, the three
  `resumeThread*`), `writeToSC` and the serial handlers, `timerInterrupt`, and the four
  interrupt vectors. Two rules came out of it, both in the porting notes: a static jump tail
  calls the target's C function directly instead of handing off, and `ld (nn),sp` must write
  its low byte one M-cycle before the instruction ends (`burn_store_sp`) or a frame sample can
  land in the gap - that was a one-byte HRAM difference at frame 4140 that verify could not see.

- 2026-09-10: milestone 3 phase 1 batch 25 finishes the plain routines of bank 0: the angle and
  z-speed helpers, weapon/treasure gfx, `enemyStandardUpdate`, `linkApplyDamage`,
  `checkUseItems`, the cutscene special-object code, the enemies-killed list, the random-position
  helpers, the four file routines, `initSound` and the intro cinematic. `checkTreasureObtained`
  was one cycle short until the `ld l,a` at 00:1749 got its own burn; the verify's cycle
  compare caught it, the callers only showed a cascaded off-by-one.
- 2026-09-10: milestone 3 phase 1 batch 24: playSound, the graphics decompressor, palette
  headers, the tileset gfx helper, sprite drawing (drawAllSprites family, terrain effects),
  the VBlank dirty-palette upload, the script engine's asm-call commands, loadRoomLayout
  (rst $00 jump table). `rewritten.txt` may name a routine by its generated C name
  (`playSound_b00`) when the symbol is shared across banks.
- 2026-09-10: milestone 3 phase 1 batches 20 to 23: 518 bank 0 routines rewritten in total
  (animation frame tails, sound timer, OAM sprites, draw queue, text thread entry, menus,
  object gfx loaders, `interBankCall`/`jpHl`/`jpBc`, status bar, treasures, global flags, the
  per-frame object update loops, tileset and room loading). Every register save is now a real
  stack push (`bank_push` included) after `updateAllObjects` showed the thread-state SP
  mismatch; `hook_handoff` tails for `jp hl` dispatch.
- 2026-09-10: milestone 3 phase 1 batches 4 to 19: 415 bank 0 routines rewritten in total
  (collision, positions, angles, speed, tiles, hazards, room flags, camera, treasure, object
  creation); `CALL_ROM` for calls into transliterated code, `burn_rom` guard for ranges that
  run past a jump.
- 2026-09-09 (night): milestone 3 phase 0 and phase 1 batches 1 to 3: 92 bank 0 routines
  rewritten, ROM-walked cycle burns, timing-phase state excluded from the gates.
- 2026-09-09 (evening): all labelled code hooked (12,829 entries); interpreter share on the
  first 20k frames down from 51.6M to 12k instructions; `tools/pc_hist.py` profiling.
- 2026-09-09 (later): bank 0 kernel ported (1494 routines); `halt`/`ei`/`reti` support in the
  generator and runtime; cross-bank reachability and direct bank N to bank 0 calls.
- 2026-09-09: 1356 routines across 16 banks generated and verified (routine diff, whole movie,
  frame hashes); sample-ring callback, duplicate-label and call-cc reachability fixes.
- 2026-09-09: 471 routines generated and verified; hand-off and nested-call rules found (see
  porting notes).
- 2026-09-08 (night): milestone 2 scaffolding. Hook table generated from the disassembly symbol
  file (`tools/gen_hooks.py`, `src/hooks/ported.txt`), transliteration skeletons
  (`tools/asm_skel.py`), replace and verify modes (`--verify-hooks`, `--no-hooks`), 33 routines
  ported and clean on the first 20,000 frames in both modes.

- 2026-09-08 (evening): full movie in sync with GBHawk; references recorded; ROM-gated TAS test;
  BizHawk reference tooling and CLAUDE.md added to the repo.

- 2026-09-08: real boot ROM supplied; GBHawk oracle running in Docker; six timing rules fixed.
- 2026-09-07: scaffold, SM83 core, PPU, APU, SDL3 app, TAS player, Blargg and acid2 passing.
