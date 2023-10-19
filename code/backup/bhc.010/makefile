bhc.bin : bhc.hex;

bhc.hex : bhc.abs;

bhc.abs : bhc.obj, ce.obj;
	bl51 bhc.obj, ce.obj, node.lib to bhc.abs ${LFLAGS} XDATA(4200H)

bhc.obj : bhc.c;

ce.obj	: ce.c;

