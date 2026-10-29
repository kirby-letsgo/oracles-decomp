-- Read the current save directly: loading a state or switching files can remove
-- essences as well as add them. No persistent progress or game writes needed.
-- wEssencesObtained (USA and EU): Ages $c6bf, Seasons $c6bb.
local ESSENCES = oracles.game() == "seasons" and 0xc6bb or 0xc6bf
local show = oracles.setting("Show essences", true)

local function rgb(r, g, b) return r + g * 32 + b * 1024 end
local BACK = rgb(2, 3, 5)
local EDGE = rgb(9, 11, 13)
local EMPTY = rgb(5, 6, 8)
local GOLD = rgb(31, 25, 7)
local LIGHT = rgb(31, 31, 21)

local function pip(x, y, have)
  oracles.draw_rect(x, y, 3, 3, have and GOLD or EDGE)
  oracles.draw_rect(x + 1, y + 1, 1, 1, have and LIGHT or EMPTY)
end

oracles.on_frame(function()
  if not show() then return end
  -- room_x is absent in menus, cutscenes, room transitions and side-view areas.
  local camera = oracles.camera()
  if not camera or camera.room_x == nil then return end

  local bits = oracles.rd(ESSENCES)
  local width = oracles.screen()
  local side = (width - 160) // 2
  -- 33x5 pixels: one row, no count or large backing panel. In widescreen
  -- it fits entirely inside the side strip; otherwise centre it at the bottom.
  local x = side >= 33 and (side - 33) // 2 or (width - 33) // 2
  local y = 137
  oracles.draw_rect(x, y, 33, 5, BACK)
  for i = 0, 7 do pip(x + 1 + i * 4, y + 1, ((bits >> i) & 1) == 1) end
end)

oracles.log("essence-tracker ready")
