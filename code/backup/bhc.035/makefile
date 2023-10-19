all : 
	make clean
	make bhc.obj
	make ce.obj
	make bhc.abs
	make bhc.hex
	make bhc.bin
	d:\emulator\dl -D27256 bhc.bin

bhc.bin : bhc.obj bhc.abs bhc.hex bhc.bin

bhc.hex : bhc.abs;

bhc.abs : bhc.obj ce.obj;
	bl51 bhc.obj, ce.obj, node.lib to bhc.abs ${LFLAGS}

bhc.obj : bhc.c ui.c;

ce.obj	: ce.c;

clean : 
	del *.hex
	del *.abs
	del *.obj
	del *.lst
	del *.bin

