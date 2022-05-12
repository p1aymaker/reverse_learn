import base64
import zlib
fp=open("main.lua","rb")
buffer=fp.read()
buffer=b'H'+buffer[1:]
buffer=base64.b64decode(buffer)
init=0
buffer_dec1=[]
for i in buffer:
    init=init^i
    buffer_dec1.append(init)
buffer_dec1=bytes(buffer_dec1)
buffer_dec2=b'\x78'+buffer_dec1[1:]
decompressed = zlib.decompress(buffer_dec2)
print(decompressed[0])
with open("main.lua_dec","wb") as f:
    f.write(b'\x1b'+decompressed[1:])