-- Dumps per-frame key bytes and periodic full-WRAM hashes while a movie plays.
local out = io.open("/data/dump.txt", "w")
client.speedmode(6400)
client.SetSoundOn(false)
local maxf = tonumber(os.getenv("MAXFRAMES") or "300000")
local function fnv(bytes)
  local h = 0xcbf29ce4
  for i = 1, #bytes do h = ((h ~ bytes[i]) * 16777619) & 0xffffffff end
  return h
end
do
  local w = memory.read_bytes_as_array(0x0000, 0x8000, "WRAM")
  local h = memory.read_bytes_as_array(0x0000, 0x7f, "HRAM")
  local f0 = io.open("/data/wram0.txt", "w")
  for i = 1, #w do f0:write(string.format("%02x", w[i])) end
  f0:write("\n")
  for i = 1, #h do f0:write(string.format("%02x", h[i])) end
  f0:write("\n")
  f0:close()
end
while emu.framecount() <= maxf do
  local f = emu.framecount()
  local grp = memory.read_u8(0x0c2d, "WRAM")
  local room = memory.read_u8(0x0c30, "WRAM")
  local fc = memory.read_u8(0x0c00, "WRAM")
  local rng1 = memory.read_u8(0x14, "HRAM")
  local rng2 = memory.read_u8(0x15, "HRAM")
  local lag = emu.islagged() and 1 or 0
  if f % 60 == 0 then
    local w = memory.read_bytes_as_array(0x0300, 0x0d00, "WRAM")
    out:write(string.format("%d %02x %02x %02x %02x %02x %d %08x\n", f, grp, room, fc, rng1, rng2, lag, fnv(w)))
  else
    out:write(string.format("%d %02x %02x %02x %02x %02x %d\n", f, grp, room, fc, rng1, rng2, lag))
  end
  if f % 5000 == 0 then out:flush() end
  emu.frameadvance()
end
out:close()
client.exit()
