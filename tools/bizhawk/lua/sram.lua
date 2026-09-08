local out = io.open("/data/sram0.txt", "w")
client.SetSoundOn(false)
local doms = memory.getmemorydomainlist()
for i = 0, #doms do if doms[i] then out:write("DOMAIN " .. doms[i] .. " " .. memory.getmemorydomainsize(doms[i]) .. "\n") end end
memory.usememorydomain("CartRAM")
local n = memory.getmemorydomainsize("CartRAM")
out:write("SIZE " .. n .. "\n")
for i = 0, n - 1 do out:write(string.format("%02x", memory.read_u8(i))); if i % 32 == 31 then out:write("\n") end end
out:close()
client.exit()
