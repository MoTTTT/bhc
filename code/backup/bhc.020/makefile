
bhc.bin : bhc.hex;

bhc.hex : bhc.abs;

bhc.abs : bhc.obj, ce.obj;
	bl51 bhc.obj, ce.obj, node.lib to bhc.abs ${LFLAGS} XDATA(4200H)

bhc.obj : bhc.c, ui.c;

ce.obj	: ce.c;

clean : 
	del *.hex
	del *.abs
	del *.obj
	del *.lst
	del *.bin

all : 
	make clean
	make bhc.bin
