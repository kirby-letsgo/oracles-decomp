local out = io.open("/data/inputpoll.txt", "w")
client.speedmode(6400)
client.SetSoundOn(false)
local startf = tonumber(os.getenv("START")); local endf = tonumber(os.getenv("END"))
local function inrange() local f = emu.framecount(); return f >= startf and f <= endf end
event.oninputpoll(function() if inrange() then out:write(string.format("POLL frame %d cycles %d ly %d lcdc %02x\n", emu.framecount(), emu.totalexecutedcycles() // 4, memory.read_u8(0xff44, "System Bus"), memory.read_u8(0xff40, "System Bus"))) end end)
event.onmemorywrite(function(addr, val) if inrange() then out:write(string.format("LCDC %02x frame %d cycles %d\n", val, emu.framecount(), emu.totalexecutedcycles() // 4)) end end, 0xff40)
while emu.framecount() <= endf do
  if inrange() then out:write(string.format("FRAMEEND %d lag %s cycles %d\n", emu.framecount(), tostring(emu.islagged()), emu.totalexecutedcycles() // 4)) end
  emu.frameadvance()
end
out:close()
client.exit()
