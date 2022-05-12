# Andlua加密解密

## 解密lua

进入关键函数Lua_loader_buffer首先判断第一个字符为=

之后根据base64解码表映射解密base64，其中将第一个字符替换为0x1d，即为0x1d+0x2b=H
![在这里插入图片描述](https://img-blog.csdnimg.cn/58662f10ad6b4ba19485b7604d81377f.png)

解码base64后的结果如下所示

![](https://img-blog.csdnimg.cn/img_convert/f1c378a704ded7bd42e0fb986a44ab4e.png)

如果第一个字符为0x1c进行异或解密 

![](https://img-blog.csdnimg.cn/img_convert/9fd5de8b4798524c7398842356f98a8a.png)

最后再将第一个字符0x1c替换为0x78，进行inflate解码，解码完成后将第一个字符替换为28(0x1c)

![](https://img-blog.csdnimg.cn/img_convert/e464a7ba5b44eb9b40ba415408f043fc.png)

解密算法如下

```python
import base64
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
```

得到Luas的文件，利用unluac反编译，提示错误的signature

看了下lua文件头，修改第一个字符为0x1b

```
java -jar unluac.jar main.lua_dec 
```

成功反编译，不过得到类似如下的文件 字符串都被加密

```
_ENV["u\229\136\006\133\020\186"]("o\024\148;\177G")
_ENV["o\024\148;\177G"]("l\021\141*\169\\\199<\225\158-\229\016")
_ENV["o\024\148;\177G"]("m\023\130&\174F\248$\024\151|\149")
_ENV["o\024\148;\177G"]("q\239\150\022\186.\220\004\236d\026\136\004\166j\159")
_ENV["o\024\148;\177G"]("o\019\136.\164R\2064\255\145\r\160i\156")
_ENV["o\024\148;\177G"]("j\025\1472\1866")
_ENV["i\018\174-\219~\244\144"]["{\230\138.\157\020\129\r"
```



## lua字符串解密

跟踪lua 5.3源码。

```
lua_load()@lapi.c
  |-lua_lock()
  |-luaZ_init()
  |-luaD_protectedparser()
  | |-luaZ_initbuffer()
  | |-luaD_pcall()                    # 实际调用的是f_parser()
  | | |-zgetc()                       # 获取第一个字符，判断是否已经编译
  | | |                               #=================
  | | |-checkmode()                   # 1. 如果是二进制
  | | |-luaU_undump()                 #  1.1 加载预编译的二进制代码
  | | |-checkmode()                   # 2. 如果是文本
  | | |-luaY_parser()
  | | | |-mainfunc()
  | | |   |-luaX_next()
  | | |   | |-llex()
  | | |   |-statlist()                # 词法解析主函数
  | | |     |-statement()             # 根据相应的词法进行处理
  | | |       |-ifstat()              #
  | | |       |-whilestat()
  | | |                               #=================
  | | |-luaF_initupvals()
  | |-luaZ_freebuffer()               # 释放一系列缓存
  | |-... ...
  |-lua_unlock()
```

发现load_string函数被魔改

```c++
static TString* LoadString(LoadState* S) {
  size_t size = LoadByte(S);
  if (size == 0xFF)
    LoadVar(S, size);
  if (size == 0)
    return NULL;
  else if (--size <= LUAI_MAXSHORTLEN) { /* short string? */
    char buff[LUAI_MAXSHORTLEN];
    LoadVector(S, buff, size);
    return luaS_newlstr(S->L, buff, size);
  } else { /* long string */
    TString* ts = luaS_createlngstrobj(S->L, size);
    LoadVector(S, getstr(ts), size); /* load directly in final place */
    return ts;
  }
}
```

![](https://img-blog.csdnimg.cn/img_convert/e400f9c5559e59fbf002a0637c66d46c.png)

解密算法大致如下

```java
  int x=size-1;
    int v5=b.charAt(0)^x;
    int v6=x+v5;
    char v8=(char)x;
    for(int i=0;i<size-1;i++){
	    v8=(char)((char)x%255);
	    x+=v6;
	    b.setCharAt(i,(char)(((char)b.charAt(i))^((char)v8)));
    }
```

修改unluac LstringTypejava中的代码即可实现解密得到源码

```lua
require("import")
import("android.app.*")
import("android.os.*")
import("android.widget.*")
import("android.view.*")
import("layout")
activity.setTheme(R.Theme_Blue)
activity.setTitle("HelloWorld")
activity.setContentView(loadlayout(layout))
function start.onClick()
  print("thank")
  os.execute("/data/local/tmp/edh269")
end
```

## 参考链接

https://gohalo.me/post/lua-sourcecode.html