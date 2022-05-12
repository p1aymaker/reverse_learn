package unluac.parse;

import java.nio.ByteBuffer;


public abstract class LStringType extends BObjectType<LString> {

  public static LStringType50 getType50() {
    return new LStringType50();
  }
  
  public static LStringType53 getType53() {
    return new LStringType53();
  }
  
  protected ThreadLocal<StringBuilder> b = new ThreadLocal<StringBuilder>() {
    
    @Override
    protected StringBuilder initialValue() {
      return new StringBuilder();  
    }

  };
  
}

class LStringType50 extends LStringType {
  @Override
  public LString parse(final ByteBuffer buffer, BHeader header) {
    BSizeT sizeT = header.sizeT.parse(buffer, header);
    final StringBuilder b = this.b.get();
    b.setLength(0);
    sizeT.iterate(new Runnable() {
      
      @Override
      public void run() {
        b.append((char) (0xFF & buffer.get()));
      }
      
    });
    String s = b.toString();
    
    //System.out.println("-- parsed <string> \"" + s + "\"");
    return new LString(sizeT, s);
  }
}

class LStringType53 extends LStringType {
  @Override
  public LString parse(final ByteBuffer buffer, BHeader header) {
    BSizeT sizeT;
    int size = 0xFF & buffer.get();
    if(size == 0xFF) {
      sizeT = header.sizeT.parse(buffer, header);
    } else {
      sizeT = new BSizeT(size);
    }
    final StringBuilder b = this.b.get();

    b.setLength(0);
    sizeT.iterate(new Runnable() {
      
      boolean first = true;
      
      @Override
      public void run() {
        if(!first) {
          b.append((char) (0xFF & buffer.get()));
        } else {
          first = false;
        }
      }
      
    });
    b.append('\0');
    int x=size-1;
    int v5=b.charAt(0)^x;
    int v6=x+v5;
    char v8=(char)x;
    for(int i=0;i<size-1;i++){
	    v8=(char)((char)x%255);
	    x+=v6;
	    b.setCharAt(i,(char)(((char)b.charAt(i))^((char)v8)));
    }

    String s =b.toString();
    System.out.println("-- parsed <string> \"" + s + "\"");
    return new LString(sizeT, s);
  }
}
