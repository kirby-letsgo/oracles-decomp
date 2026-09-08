local out = io.open("/data/trace_lcdc.txt", "w")
client.speedmode(6400)
client.SetSoundOn(false)
local maxf = tonumber(os.getenv("MAXFRAMES") or "14400")
local prev = -1
event.onmemorywrite(function(addr, val)
  local on = (val & 0x80) ~= 0
  if on ~= (prev == 1) or prev == -1 then
    out:write(string.format("%d LCDC %02x at %d\n", emu.framecount(), val, emu.totalexecutedcycles() // 4))
    prev = on and 1 or 0
  end
end, 0xff40)
while emu.framecount() < maxf do emu.frameadvance() end
out:close()
client.exit()
