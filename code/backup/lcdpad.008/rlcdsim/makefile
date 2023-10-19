lcdpad.bin : lcdpad.obj lcdpad.abs lcdpad.hex lcdpad.bin

lcdpad.bin : lcdpad.hex

lcdpad.hex : lcdpad.abs

lcdpad.abs : lcdpad.obj
	bl51 lcdpad.obj,node.lib to lcdpad.abs ${LFLAGS} XDATA(4200H)

lcdpad.obj : lcdpad.c;

