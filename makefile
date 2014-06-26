MPLAB_DIR := /opt/microchip/MPLABX
XC8_DIR := /opt/microchip/xc8
CHIP := 16F1454
MSTACK := m-stack
CC := xc8

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.p1)))
OBJ_MSTACK := $(wildcard $(MSTACK)/obj/*.p1)

LD_FLAGS := --chip=$(CHIP) -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
CC_FLAGS := --pass1 --chip=$(CHIP) -Q -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
INCL := -I./m-stack/include -I./src 
DEFINE := 
LIBS := 
LIB_PATHS := 

#link all object files
mainbin: $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $(LIB_PATHS) -o$@ $(OBJ_FILES) $(OBJ_MSTACK) $(LIBS)
#	xc8-bin2hex mainbin

#compile all c files in source to object files
obj/%.p1: src/%.c
	$(CC) $(DEFINE)  $(CC_FLAGS) $(INCL) -c -o$@ $<
	
clean:
	rm obj/*
	rm mainbin.*
	rm l.obj
	rm funclist
