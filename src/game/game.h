#pragma once
#include "game/asm.h"
#include "game/ram.h"

#define IO_IF 0xff0f
#define IO_LCDC 0xff40
#define IO_SCY 0xff42
#define IO_SCX 0xff43
#define IO_LYC 0xff45
#define IO_WY 0xff4a
#define IO_WX 0xff4b
#define IO_HDMA2 0xff52
#define IO_HDMA3 0xff53
#define IO_HDMA4 0xff54
#define IO_HDMA5 0xff55
#define IO_LY 0xff44
#define IO_DMA 0xff46
#define IO_BGP 0xff47
#define IO_OBP0 0xff48
#define IO_OBP1 0xff49
#define IO_STAT 0xff41
#define IO_VBK 0xff4f
#define IO_RP 0xff56
#define IO_SVBK 0xff70
#define IO_IE (hSoundData3 + 13)
#define MBC_ROM_BANK 0x2222

#define flagLocationGroupTable SYM(flagLocationGroupTable)
#define speedTable_bank3 SYM(initSound__readFunction_b39)
#define linkDirectionOffsets SYM(_getLinkPositionPlusDirectionOffset__positionOffsets)
#define hazardCollisionTable SYM(hazardCollisionTable)
#define rupeeValues SYM(getRupeeValue__rupeeValues)
#define specialCollisions_allowHoles SYM(checkGivenCollision_allowHoles__specialCollisions)
#define specialCollisions_disallowHoles SYM(checkGivenCollision_disallowHoles__specialCollisions)
#define specialCollisions_disallowSmallBridges SYM(checkCollisionPosition_disallowSmallBridges__specialCollisions)
#define bitTable 0x00f8
#define chestDataGroupTable_bank16 SYM(chestDataGroupTable)
#define tileUpdateRoomFlagsOnBreakTable SYM(tileUpdateRoomFlagsOnBreakTable)
#define tileIncreaseGashaMaturityOnBreakTable SYM(tileIncreaseGashaMaturityOnBreakTable)
#define adjacentRoomsData SYM(_adjacentRoomsData)
#define ROM_addToGashaMaturity SYM(addToGashaMaturity)
#define ROM_checkTreasureObtained SYM(checkTreasureObtained)
#define ROM_b3f_decideItemDrop_body SYM(decideItemDrop_body)
#define ROM_b3f_checkItemDropAvailable_body SYM(checkItemDropAvailable_body)
#define ROM_b3f_giveTreasure_body SYM(giveTreasure_body)
#define ROM_b3f_loseTreasure_body SYM(loseTreasure_body)
#define ringTierTable_bank3f SYM(ringTierTable)
#define ROM_getFreeInteractionSlot SYM(getFreeInteractionSlot)
#define ROM_getTileMappingData SYM(getTileMappingData)
#define ROM_setTile SYM(setTile)
#define ROM_playSound SYM(playSound_b00)
#define ROM__interactionActuallyRunScript SYM(_interactionActuallyRunScript)
#define ROM__interactionSaveScriptAddress SYM(_interactionSaveScriptAddress)
#define ROM_b07_checkEnemyAndPartCollisions SYM(checkEnemyAndPartCollisions)
#define ROM_b05_specialObjectUpdatePositionGivenVelocity SYM(specialObjectUpdatePositionGivenVelocity)
#define interactionAnimationTable_bank16 SYM(interactionAnimationTable)
#define adjacentTileDirOffsets SYM(interactionCheckAdjacentTileIsSolid_viaDirection__dirOffsets)
#define linkAngleToDirectionButtons SYM(convertLinkAngleToDirectionButtons__data)
#define wallCollisionDirections SYM(checkLinkPushingAgainstWall__collisionDirections)
#define enemyAnimationTable_bank0d SYM(enemyAnimationTable)
#define partAnimationTable_bank16 SYM(partAnimationTable)
#define dungeonRoomPropertiesGroupTable_bank01 SYM(dungeonRoomPropertiesGroupTable)
#define ROM_getFreePartSlot SYM(getFreePartSlot)
#define ROM_markEnemyAsKilledInRoom SYM(markEnemyAsKilledInRoom_b00)
#define ROM_checkGlobalFlag SYM(checkGlobalFlag)
#define ROM_setGlobalFlag SYM(setGlobalFlag)
#define ROM_b0b_objectCreateFloatingImage SYM(objectCreateFloatingImage)
#define ROM_b11_createEnergySwirlGoingIn_body SYM(createEnergySwirlGoingIn_body)
#define ROM_b11_createEnergySwirlGoingOut_body SYM(createEnergySwirlGoingOut_body)
#define ROM_b06_specialObjectNextAnimationFrame SYM(specialObjectNextAnimationFrame)
#define ROM_b06_specialObjectSetAnimation_body SYM(specialObjectSetAnimation_body)
#define ROM_b03_clearFadingPalettes_body SYM(clearFadingPalettes_body)
#define ROM_b03_flashScreen_body SYM(flashScreen_body)
#define ROM_b01_loadDungeonLayout_b01 SYM(loadDungeonLayout_b01)
#define ROM_b06_functionCaller SYM(functionCaller_b06)
#define ROM_b06_tryToBreakTile_body SYM(tryToBreakTile_body)
#define wPaletteFadeOffset (wThreadStateBuffer + 31)
#define dungeonsUsingToggleBlocks SYM(dungeonsUsingToggleBlocks)
#define objectTable2_bank12 SYM(objectTable2)
#define ROM_b02_getIndexOfGashaSpotInRoom_body SYM(getIndexOfGashaSpotInRoom_body)
#define ROM_b04_setInterleavedTile_body SYM(setInterleavedTile_body)
#define ROM_b09_checkNpcShouldExistAtGameStage_body SYM(checkNpcShouldExistAtGameStage_body)
#define wInteractionSlots (w1ReservedInteraction0 + 0x200)

#define musicAssignmentGroupTable_bank04 SYM(musicAssignmentGroupTable)
#define roomPackData_bank04 SYM(roomPackData)
#define animationGroupTable_bank04 SYM(animationGroupTable)
#define uniqueGfxHeaderTable_bank04 SYM(uniqueGfxHeaderTable)
#define ROM_b04_applyWarpDest_b04 SYM(applyWarpDest_b04)
#define ROM_b05_companionDismount SYM(companionDismount)
#define ROM_b05_saveLinkLocalRespawnAndCompanionPosition SYM(saveLinkLocalRespawnAndCompanionPosition)
#define ROM_loadTilesetData SYM(loadTilesetData)
#define ROM_loadTilesetGraphics SYM(loadTilesetGraphics)
#define ROM_loadTilesetAndRoomLayout SYM(loadTilesetAndRoomLayout)
#define ROM_loadUniqueGfxHeaderEntry SYM(loadUniqueGfxHeaderEntry)
#define IO_P1 0xff00
#define IO_SB 0xff01
#define IO_SC 0xff02
#define IO_TMA 0xff06
#define IO_HDMA1 0xff51
#define IO_TIMA 0xff05
#define IO_TAC 0xff07
#define IO_KEY1 0xff4d
#define interactionAnimationFrameTable_bank16 SYM(interactionOamDataTable)
#define enemyAnimationFrameTable_bank0d SYM(enemyOamDataTable)
#define partAnimationFrameTable_bank16 SYM(partOamDataTable)
#define ROM_jpBc SYM(jpBc)
#define ROM_jpHl 0x00a0
#define ROM_add16BitRefs SYM(add16BitRefs)
#define ROM_checkTileCollisionAt_allowHoles SYM(checkTileCollisionAt_allowHoles)
#define ROM_objectUpdateSpeedZAndBounce SYM(objectUpdateSpeedZAndBounce)
#define ROM_getRandomNumber_noPreserveVars SYM(getRandomNumber_noPreserveVars)
#define ROM_updateInteractionsAndDrawAllSprites SYM(updateInteractionsAndDrawAllSprites)
#define ROM_b06_interactWithTileBeforeLink SYM(interactWithTileBeforeLink_b06)
#define ROM_b3f_reloadObjectGfx SYM(reloadObjectGfx_b3f)
#define ROM_b3f_loadWeaponGfx SYM(loadWeaponGfx_b3f)
#define ROM_b3f_loadTreasureDisplayData SYM(loadTreasureDisplayData_b3f)
#define ROM_b3f_checkTreasureObtained_body SYM(checkTreasureObtained_body)
#define ROM_b3f_enemyLoadGraphicsAndProperties SYM(enemyLoadGraphicsAndProperties)
#define ROM_b01_makeActiveObjectFollowLink SYM(makeActiveObjectFollowLink_b01)
#define ROM_b05_linkApplyDamage_b5 SYM(linkApplyDamage_b5)
#define ROM_b06_specialObjectCode_companionCutscene SYM(specialObjectCode_companionCutscene_b06)
#define ROM_b06_specialObjectCode_linkInCutscene SYM(specialObjectCode_linkInCutscene_b06)
#define ROM_b12_parseGivenObjectData SYM(parseGivenObjectData_b12)
#define ROM_b08_checkObjectIsCloseToPosition SYM(checkObjectIsCloseToPosition_b08)
#define ROM_b07_fileManagementFunction SYM(fileManagementFunction)
#define ROM_b03_runIntroCinematic SYM(runIntroCinematic)
#define ROM_b0c_runScriptCommand SYM(runScriptCommand)
#define ROM_b39_updateSound SYM(b39_updateSound)
#define ROM_b39_playSound SYM(b39_playSound)
#define ROM_b39_updateMusicVolume SYM(b39_updateMusicVolume)
#define ROM_writeToSC SYM(writeToSC)
#define ROM_nextThread SYM(_nextThread)
#define ROM_resumeThreadInAFrames SYM(resumeThreadInAFrames)
#define ROM_b16_serialFunc_4000 SYM(func_4000_b16)
#define ROM_b16_serialFunc_44ac SYM(func_44ac)
#define ROM_vblankInterrupt SYM(vblankInterrupt)
#define ROM_lcdInterrupt SYM(lcdInterrupt)
#define ROM_timerInterrupt SYM(timerInterrupt)
#define ROM_serialInterrupt SYM(serialInterrupt)
#define ROM_pollInput SYM(pollInput)
#define ROM_refreshDirtyPalettes SYM(refreshDirtyPalettes)
#define ROM_init SYM(init)
#define ROM_mainLoop SYM(_mainLoop)
#define ROM_mainLoop_nextThread SYM(_mainLoop_nextThread)
#define ROM_countdownToRunThread SYM(_countdownToRunThread)
#define ROM_restartSound SYM(restartSound)
#define ROM_stopTextThread SYM(stopTextThread)
#define ROM_checkReloadStatusBarGraphics SYM(checkReloadStatusBarGraphics)
#define ROM_resumeThreadNextFrame SYM(resumeThreadNextFrame)
#define ROM_initTextbox SYM(initTextbox)
#define ROM_updateTextbox SYM(updateTextbox)
#define ROM_b2_fileSelectScreen SYM(b2_fileSelectScreen)
#define ROM_runSaveAndQuitMenu SYM(runSaveAndQuitMenu)
#define ROM_runIntro SYM(runIntro)
#define ROM_paletteFadeHandler SYM(paletteFadeHandler)
#define ROM_checkLockBG7Color3ToBlack SYM(checkLockBG7Color3ToBlack)
#define ROM_runGameLogic SYM(runGameLogic)
#define ROM_initializeThread SYM(_initializeThread)
#define ROM_vblankFunctionsStart SYM(vblankFunctionsStart)
#define ROM_runVBlankFunctions SYM(runVBlankFunctions)
#define ROM_lcdInterrupt_clearLYC SYM(lcdInterrupt_clearLYC)
#define ROM_lcdInterrupt_setLcdcToA7 SYM(lcdInterrupt_setLcdcToA7)
#define ROM_lcdInterrupt_clearWXY SYM(lcdInterrupt_clearWXY)
#define ROM_lcdInterrupt_ringMenu SYM(lcdInterrupt_ringMenu)
#define ROM_lcdInterrupt_0bea SYM(lcdInterrupt_0bea)
#define ROM_vblankFunctionRet SYM(vblankFunctionRet)
#define ROM_updateDirtyPalettes SYM(updateDirtyPalettes)
#define ROM_hramOamDmaFunction hOamFunc
#define ROM_b04_vblankRunBank4Function SYM(vblankRunBank4Function_b04)
#define ROM_initialThreadStatesBase (SYM(restartThisThread) + 9)
#define ROM__scriptCmd_asmRetFunc SYM(_scriptCmd_asmRetFunc)
#define ROM_drawAllSprites_drawObject SYM(drawAllSpritesUnconditionally__drawObject)
#define ROM_loadRoomLayout_loadLayoutData SYM(loadRoomLayout__loadLayoutData)
#define ROM_loadRoomLayout_loadLargeRoomLayoutHlpr SYM(loadRoomLayout__loadLargeRoomLayoutHlpr)
#define ROM_loadRoomLayout_decompressLayoutMode2Helper SYM(loadRoomLayout__decompressLayoutMode2Helper)
#define ROM_loadRoomLayout_decompressLayoutMode1Helper SYM(loadRoomLayout__decompressLayoutMode1Helper)
#define paletteHeaderTable_bank01 SYM(paletteHeaderTable)
#define roomLayoutGroupTable_bank04 SYM(roomLayoutGroupTable)
#define puddleAnimationFrames_bank14 SYM(puddleAnimationFrames)
#define greenGrassAnimationFrame0_bank14 SYM(greenGrassAnimationFrame0)
#define data_1058 SYM(data_1058)
#define wRamFunctionAddr (wc4b6 + 1)
#define ROM_updateEnemy SYM(updateEnemy)
#define ROM_enemyStandardUpdate SYM(enemyStandardUpdate)
#define ROM_updateInteraction SYM(updateInteraction)
#define ROM_updateEnemies SYM(updateEnemies)
#define ROM_updateInteractions SYM(updateInteractions)
#define ROM_setEnemyTargetToLinkPosition SYM(setEnemyTargetToLinkPosition)
#define ROM_updateCamera SYM(updateCamera)
#define ROM_drawAllSprites SYM(drawAllSprites)
#define ROM_loadTileset SYM(loadTileset)
#define ROM_loadTilesetLayout SYM(loadTilesetLayout)
#define ROM_loadTilesetLayout_helper SYM(loadTilesetLayout__helper)
#define ROM_loadRoomLayout SYM(loadRoomLayout)
#define ROM_copyMemoryReverse SYM(copyMemoryReverse)
#define ROM_refreshObjectGfx SYM(refreshObjectGfx)
#define ROM_checkSpawnTimeportalInteraction SYM(checkSpawnTimeportalInteraction_b00)
#define interactionCodeTable SYM(interactionCodeTable)
#define ROM_b05_updateSpecialObjects SYM(updateSpecialObjects)
#define ROM_b07_updateItems SYM(updateItems)
#define ROM_b11_updateParts SYM(updateParts)
#define ROM_b01_func_4000 SYM(func_4000_b01)
#define ROM_b05_func_410d SYM(func_410d)
#define ROM_b06_updateGrabbedObjectPosition SYM(updateGrabbedObjectPosition)
#define ROM_b07_updateItemsPost SYM(updateItemsPost)
#define ROM_b01_checkUpdateFollowingLinkObject SYM(checkUpdateFollowingLinkObject)
#define ROM_b04_updateChangedTileQueue SYM(updateChangedTileQueue)
#define ROM_b04_updateAnimations SYM(updateAnimations)
#define ROM_b04_setPastCliffPalettesToRed SYM(setPastCliffPalettesToRed)
#define ROM_b04_initializeAnimations SYM(initializeAnimations)
#define ROM_b02_func_02_7a77 SYM(func_02_7a77)
#define ROM_b02_checkLoadPastSignAndChestGfx SYM(checkLoadPastSignAndChestGfx)
#define ROM_b04_loadTilesetData_body SYM(loadTilesetData_body)
#define ROM_b02_updateTilesetFlagsForIndoorRoomInAltWorld SYM(updateTilesetFlagsForIndoorRoomInAltWorld)
#define ROM_b04_applyAllTileSubstitutions SYM(applyAllTileSubstitutions)
#define ROM_b01_clearSolidObjectPositions SYM(clearSolidObjectPositions)
#define ROM_b02_calculateRoomStateModifier SYM(calculateRoomStateModifier)
#define ROM_b12_runRoomSpecificCode SYM(runRoomSpecificCode)
#define ROM_b02_createSeaEffectsPartIfApplicable SYM(createSeaEffectsPartIfApplicable)
#define ROM_b01_checkLoadPirateShip SYM(checkLoadPirateShip)
#define ROM_b02_loadRememberedCompanion SYM(loadRememberedCompanion)
#define ROM_b02_checkAndSpawnMaple SYM(checkAndSpawnMaple)
#define ROM_b12_parseObjectData SYM(parseObjectData)
#define ROM_b16_parseStaticObjects SYM(parseStaticObjects)
#define ROM_b16_loadStaticObjects_body SYM(loadStaticObjects_body)
#define ROM_b03_disableLcdAndLoadRoom_body SYM(disableLcdAndLoadRoom_body)
#define ROM_b10_playWaveSoundAtRandomIntervals_body SYM(agesFunc_10_7298__playWaveSoundAtRandomIntervals_body)
#define ROM_b03_func_03_7841 SYM(func_03_7841)
#define ROM_b03_func_03_7849 SYM(func_03_7849)
#define ROM_b03_endgameCutsceneHandler_body SYM(endgameCutsceneHandler_body)
#define ROM_b02_fileSelect_redrawDecorationsAndSetWramBank4 SYM(fileSelect_redrawDecorationsAndSetWramBank4)
#define ROM_b06_loadLinkAndCompanionAnimationFrame_body SYM(loadLinkAndCompanionAnimationFrame_body)
#define data_5951_bank3f SYM(data_5951)
#define ROM_pushDirectionData SYM(pushDirectionData)
#define ROM_b02_functionCaller SYM(functionCaller_b02)
#define ROM_b09_objectOscillateZ_body SYM(objectOscillateZ_body)
#define ROM_b0b_objectCreateExclamationMark_body SYM(objectCreateExclamationMark_body)
#define wildTokayObjectTable_bank12 SYM(wildTokayObjectTable)
#define objectTable1_bank12 SYM(objectTable1)
#define linkInteractPositionOffsets SYM(linkInteractWithAButtonSensitiveObjects__positionOffsets)
#define ROM_threadStop SYM(threadStop)
#define ROM_threadRestart SYM(threadRestart)
#define ROM_clearAllParentItems SYM(clearAllParentItems)
#define ROM_dropLinkHeldItem SYM(dropLinkHeldItem)
#define ROM_clearMemory SYM(clearMemory)
#define ROM_putLinkOnGround SYM(putLinkOnGround)
#define ROM_retrieveTextCharacter_func_18fd SYM(retrieveTextCharacter__func_18fd)
#define ROM_b3f_interactionLoadGraphics SYM(interactionLoadGraphics)
#define ROM_b3f_refreshObjectGfx_body SYM(refreshObjectGfx_body)
#define ROM_b3f_loadObjectGfxHeaderToSlot4_body SYM(loadObjectGfxHeaderToSlot4_body)
#define ROM_b3f_loadTreeGfx_body SYM(loadTreeGfx_body)
#define textGfxSourceTable SYM(retrieveTextCharacter__data)
#define gfx_font_start_bank1c SYM(gfx_font_jp)
#define gfx_font_jp_bank1c (SYM(gfx_font_tradeitems) + 288)
#define wcc20 (wcc1f + 1)
#define ROM_b01_initializeRoomBoundaryAndLoadAnimations SYM(initializeRoomBoundaryAndLoadAnimations)
#define ROM_b04_func_04_6ed1 SYM(func_04_6ed1)
#define ROM_b04_func_04_6f31 SYM(func_04_6f31)
#define ROM_b03_secretFunctionCaller_body SYM(secretFunctionCaller_body)
#define ROM_b02_updateMenus SYM(b2_updateMenus)
#define ROM_b02_runBank2Function SYM(runBank2Function)
#define gfxRegisterStates SYM(gfxRegisterStates)
#define vblankDmaFunctionOffset SYM(vblankDmaFunctionOffset)
#define uncmpGfxHeaderTable_bank01 SYM(uncmpGfxHeaderTable)
#define gfxHeaderTable_bank01 SYM(gfxHeaderTable)
#define tilesetHeaderTable_bank01 SYM(tilesetLayoutDictionary02)
#define tilesetGfxHeaderTable_bank01 SYM(tilesetLayoutDictionaryTable)
#define ROM_loadTilesetHlpr SYM(loadTilesetHlpr)
#define ROM_interBankCall 0x008a
#define ROM_loadPaletteHeader SYM(loadPaletteHeader)
#define ROM_queueDmaTransfer SYM(queueDmaTransfer)
#define ROM_decompressGraphics SYM(decompressGraphics)
#define ROM_addSpritesToOam SYM(addSpritesToOam)
#define ROM_addSpritesToOam_withOffset SYM(addSpritesToOam_withOffset)
#define ROM_b02_applyRoomSpecificTileChangesAfterGfxLoad SYM(applyRoomSpecificTileChangesAfterGfxLoad)
#define ROM_b04_generateW3VramTilesAndAttributes SYM(generateW3VramTilesAndAttributes)
#define ROM_b12_checkLinkCanSurface_isUnderwater SYM(checkLinkCanSurface_isUnderwater)
#define ROM_b0e_objectLoadMovementScript_body SYM(objectLoadMovementScript_body)
#define ROM_b0e_objectRunMovementScript_body SYM(objectRunMovementScript_body)
#define creditsSpriteTable_bank16 SYM(data_4556)
#define simpleScriptCommandTable (SYM(interactionRunSimpleScript__runCommand) + 3)
#define OBJ_COLLIDE_RADIUS_Y 0x26
#define ROM_getActiveRoomFromDungeonMapPosition SYM(getActiveRoomFromDungeonMapPosition)
#define ROM_getRoomInDungeon SYM(getRoomInDungeon)
#define ROM_loadUncompressedGfxHeader SYM(loadUncompressedGfxHeader)
#define ROM_b06_showInfoTextForTile SYM(showInfoTextForTile)
#define ROM_b01_updateCameraPosition SYM(updateCameraPosition)
#define ROM_b01_updateGfxRegs2Scroll SYM(updateGfxRegs2Scroll)
#define ROM_b01_updateScreenShake SYM(updateScreenShake)
#define ROM_b01_calculateCameraPosition SYM(calculateCameraPosition)
#define ROM_b01_setScreenTransitionState02 SYM(setScreenTransitionState02)
#define ROM_b01_initWaveScrollValues_body SYM(initWaveScrollValues_body)
#define ROM_b01_loadBigBufferScrollValues_body SYM(loadBigBufferScrollValues_body)

#define OBJ_STATE 0x04
#define OBJ_SUBSTATE 0x05
#define OBJ_COUNTER1 0x06
#define OBJ_COUNTER2 0x07
#define OBJ_Y 0x0a
#define OBJ_YH 0x0b
#define OBJ_XH 0x0d
#define OBJ_ZH 0x0f
#define OBJ_VISIBLE 0x1a
#define OBJ_COLLISION_RADIUS_Y 0x26
#define OBJ_VAR2A 0x2a
#define OBJ_ANGLE 0x09
#define OBJ_Z 0x0e
#define OBJ_SPEED 0x10
#define OBJ_SPEED_Z 0x14
#define OBJ_RELATED1 0x16
#define OBJ_RELATED2 0x18
#define OBJ_ID 0x01
#define OBJ_SUBID 0x02
#define OBJ_VAR03 0x03
#define OBJ_DIRECTION 0x08
#define OBJ_SPEED_TMP 0x11
#define OBJ_SPEED_X 0x12
#define OBJ_OAM_FLAGS_BACKUP 0x1b
#define OBJ_OAM_FLAGS 0x1c
#define OBJ_OAM_TILE_INDEX_BASE 0x1d
#define OBJ_VAR18 0x18
#define OBJ_VAR19 0x19
#define OBJ_OAM_DATA_ADDRESS 0x1e
#define OBJ_ANIM_COUNTER 0x20
#define OBJ_ANIM_PARAMETER 0x21
#define OBJ_ANIM_POINTER 0x22
#define OBJ_COLLISION_TYPE 0x24
#define OBJ_ENEMY_COLLISION_MODE 0x25
#define OBJ_COLLISION_RADIUS_X 0x27
#define OBJ_DAMAGE 0x28
#define OBJ_HEALTH 0x29
#define OBJ_INVINCIBILITY_COUNTER 0x2b
#define OBJ_KNOCKBACK_ANGLE 0x2c
#define OBJ_KNOCKBACK_COUNTER 0x2d
#define OBJ_STUN_COUNTER 0x2e
#define OBJ_VAR2F 0x2f
#define OBJ_USE_TEXT_ID 0x30
#define OBJ_PRESSED_A_BUTTON 0x31
#define OBJ_TEXT_ID 0x32
#define OBJ_VAR32 0x32
#define OBJ_VAR33 0x33
#define OBJ_VAR34 0x34
#define OBJ_SCRIPT_RET 0x35
#define OBJ_VAR35 0x35
#define OBJ_VAR36 0x36
#define OBJ_VAR37 0x37
#define OBJ_VAR38 0x38
#define OBJ_VAR39 0x39
#define OBJ_VAR3A 0x3a
#define OBJ_VAR3B 0x3b
#define OBJ_VAR3C 0x3c
#define OBJ_VAR3D 0x3d
#define OBJ_VAR3E 0x3e
#define OBJ_VAR3F 0x3f
#define OBJ_ENABLED 0x00
#define OBJ_X 0x0c
#define OBJ_SCRIPT_PTR 0x18
#define INTERACTION_BASE 0x40
#define ENEMY_BASE 0x80
#define PART_BASE 0xc0
#define ENEMY_SLOTS (w1ReservedInteraction0_var3f + 1)
#define PART_SLOTS w7TextDisplayState

void burn_rom(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken);
void burn_store_sp(GB *gb, uint16_t a, uint16_t addr);
#define CALL_C_(a, fn, target, ra) do { push_effect(gb, (uint16_t)(ra)); uint16_t sp_ = gb->sp; if (hook_in_verify || !hook_enabled_at(gb, target)) asm_call(gb, (target), (ra)); else { fn(gb); if (!(gb->pc == (uint16_t)(ra) && gb->sp == (uint16_t)(sp_ + 2))) { hook_continue(gb, gb->pc, sp0_); return; } } } while (0)
// a `jp` into another routine: its C if that routine is hooked in the running game, else the interpreter
#define TAIL(l) do { if (hook_enabled_at(gb, SYM(l))) { l##_hook(gb); return; } hook_continue(gb, SYM(l), gb->sp); return; } while (0)
#define CALL_C(a, fn, target, ra) do { CYC((a), (a) + 3); CALL_C_((a), fn, (target), (ra)); } while (0)
#define CALL_C_CC(a, fn, target, ra) do { CYCT((a), (a) + 3); CALL_C_((a), fn, (target), (ra)); } while (0)
#define CALL_L_(fn, ra) do { push_effect(gb, (uint16_t)(ra)); uint16_t sp_ = gb->sp; fn(gb); if (!(gb->pc == (uint16_t)(ra) && gb->sp == (uint16_t)(sp_ + 2))) { hook_continue(gb, gb->pc, sp0_); return; } } while (0)
#define CALL_L(a, fn, ra) do { CYC((a), (a) + 3); CALL_L_(fn, (ra)); } while (0)
#define CALL_L_CC(a, fn, ra) do { CYCT((a), (a) + 3); CALL_L_(fn, (ra)); } while (0)
#define CYC(from, to) burn_rom(gb, 0, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0, (from), (to), true)
#define CALL_ROM_CC(a, target) do { CYCT((a), (a) + 3); push_effect(gb, (uint16_t)((a) + 3)); asm_call(gb, (target), (uint16_t)((a) + 3)); } while (0)
#define CALL_ROM(a, target) do { CYC((a), (a) + 3); push_effect(gb, (uint16_t)((a) + 3)); asm_call(gb, (target), (uint16_t)((a) + 3)); } while (0)

static inline uint16_t rd16(GB *gb, uint16_t a) { return (uint16_t)(mem_rd(gb, a) | (mem_rd(gb, a + 1) << 8)); }
static inline void wr16(GB *gb, uint16_t a, uint16_t v) { mem_wr(gb, a, (uint8_t)v); mem_wr(gb, a + 1, v >> 8); }
static inline uint16_t cpu_sp(GB *gb) { return gb->sp; }
static inline uint16_t cpu_pc(GB *gb) { return gb->pc; }
static inline int bitlen8(uint8_t v) { int n = 0; while (v) { n++; v >>= 1; } return n; }
static inline int popcount8(uint8_t v) { int n = 0; while (v) { n += v & 1; v >>= 1; } return n; }

void fillMemory(GB *gb, uint16_t dst, uint8_t value, unsigned n);
void clearMemory(GB *gb, uint16_t dst, unsigned n);
void fillMemoryBc(GB *gb, uint16_t dst, uint8_t value, unsigned n);
void clearMemoryBc(GB *gb, uint16_t dst, unsigned n);
void copyMemory(GB *gb, uint16_t dst, uint16_t src, unsigned n);
void copyMemoryBc(GB *gb, uint16_t dst, uint16_t src, unsigned n);
uint8_t getRandomNumber(GB *gb);
uint8_t getRandomNumber_noPreserveVars(GB *gb);
void clearOam(GB *gb);
bool disableLcd(GB *gb);
void clearVram(GB *gb);
void initializeVramMap0(GB *gb);
void initializeVramMap1(GB *gb);
void initializeVramMaps(GB *gb);
void setStatusBarNeedsRefreshBit1(GB *gb);
