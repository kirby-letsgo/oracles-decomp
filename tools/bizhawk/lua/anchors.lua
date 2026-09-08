local out = io.open("/data/anchors.txt", "w")
client.speedmode(6400)
client.SetSoundOn(false)
local startf = tonumber(os.getenv("START")); local endf = tonumber(os.getenv("END"))
local pcs = {}
for a in string.gmatch(os.getenv("PCS") or "", "%S+") do pcs[#pcs + 1] = tonumber(a, 16) end
local hit = {}
local function inrange() local f = emu.framecount(); return f >= startf and f <= endf end
local allset = {}
for a in string.gmatch(os.getenv("ALL") or "", "%S+") do allset[tonumber(a, 16)] = true end
local registered = false
local function register()
  event.oninputpoll(function() if inrange() then out:write(string.format("POLL %d\n", emu.totalexecutedcycles() // 4)) end end)
  event.onmemorywrite(function(addr, val) if inrange() and (val & 0x80) == 0 then out:write(string.format("LCDC %02x %d\n", val, emu.totalexecutedcycles() // 4)) end end, 0xff40)
  for _, pc in ipairs(pcs) do
    event.onmemoryexecute(function()
      if (allset[pc] or not hit[pc]) and inrange() then hit[pc] = true; out:write(string.format("ANCHOR %04x %d\n", pc, emu.totalexecutedcycles() // 4)) end
    end, pc)
  end
end
while emu.framecount() <= endf do
  if not registered and emu.framecount() >= startf - 1 then register(); registered = true end
  emu.frameadvance()
end
out:close()
client.exit()
