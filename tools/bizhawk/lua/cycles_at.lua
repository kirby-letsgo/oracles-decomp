client.speedmode(6400)
client.SetSoundOn(false)
local a = tonumber(os.getenv("FROM")); local b = tonumber(os.getenv("TO"))
local f0 = io.open("/data/cycles_at.txt", "w")
while emu.framecount() <= b do
  local f = emu.framecount()
  if f >= a then f0:write(string.format("FRAME %d cycles %d ly %d\n", f, emu.totalexecutedcycles(), memory.read_u8(0xff44, "System Bus"))) end
  emu.frameadvance()
end
f0:close()
client.exit()
