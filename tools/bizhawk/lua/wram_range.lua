client.speedmode(6400)
client.SetSoundOn(false)
local a = tonumber(os.getenv("FROM")); local b = tonumber(os.getenv("TO"))
local f0 = io.open("/data/wram_range.txt", "w")
while emu.framecount() <= b do
  local f = emu.framecount()
  if f >= a then
    f0:write("FRAME " .. f .. "\n")
    local w = memory.read_bytes_as_array(0x0000, 0x8000, "WRAM")
    for i = 1, #w do f0:write(string.format("%02x", w[i])) end
    f0:write("\n")
    local h = memory.read_bytes_as_array(0x0000, 0x7f, "HRAM")
    for i = 1, #h do f0:write(string.format("%02x", h[i])) end
    f0:write("\n")
  end
  emu.frameadvance()
end
f0:close()
client.exit()
