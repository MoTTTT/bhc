lcdpad.bin : lcdpad.hex

lcdpad.hex : lcdpad.abs

lcdpad.abs : lcdpad.obj
	bl51 lcdpad.obj,node.lib to lcdpad.abs ${LFLAGS}

lcdpad.obj : lcdpad.c;

