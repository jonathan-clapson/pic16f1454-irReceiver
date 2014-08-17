MPLAB_DIR := /opt/microchip/MPLABX
XC8_DIR := /opt/microchip/xc8

CHIP := 16F1454

CC := xc8
C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.p1)))
LD_FLAGS := 
DEFINES := 
CC_FLAGS := --pass1 --chip=$(CHIP) -Q -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
INCL := -I./include -I./src
LIBS := 
LIB_PATHS := 

#link all object files
objects: $(OBJ_FILES)

#compile all c files in source to object files
obj/%.p1: src/%.c
	$(CC) $(CC_FLAGS) $(INCL) $(DEFINES) -c -o$@ $<
	
test:
	echo $(C_FILES)	
	
clean:
	rm obj/*.d obj/*.pre obj/*.p1
