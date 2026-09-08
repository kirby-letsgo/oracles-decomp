local out = io.open("/data/timing_" .. (os.getenv("TNAME") or "x") .. ".txt", "w")
client.speedmode(6400)
client.SetSoundOn(false)
while emu.framecount() < 200 do emu.frameadvance() end
local w = memory.read_bytes_as_array(0x0000, 0xc0, "WRAM")
for i = 1, #w do out:write(string.format("%02x%s", w[i], (i % 16 == 0) and "\n" or " ")) end
out:close()
client.exit()
