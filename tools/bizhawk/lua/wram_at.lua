client.speedmode(6400)
client.SetSoundOn(false)
local target = tonumber(os.getenv("FRAME"))
while emu.framecount() < target do emu.frameadvance() end
local f0 = io.open("/data/wram_at.txt", "w")
f0:write("FRAME " .. emu.framecount() .. "\n")
local w = memory.read_bytes_as_array(0x0000, 0x8000, "WRAM")
for i = 1, #w do f0:write(string.format("%02x", w[i])) end
f0:write("\n")
local h = memory.read_bytes_as_array(0x0000, 0x7f, "HRAM")
for i = 1, #h do f0:write(string.format("%02x", h[i])) end
f0:write("\n")
f0:close()
client.exit()
