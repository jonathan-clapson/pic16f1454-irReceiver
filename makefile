MPLAB_DIR := /opt/microchip/MPLABX
XC8_DIR := /opt/microchip/xc8
CHIP := 16F1454
CC := xc8

SUBMK = hal/ hal/m-stack cstdlib

PROGRAMMER_IP := 192.168.1.122
PROGRAMMER_USER := jonathan

C_FILES := $(wildcard src/*.c)
#really need to find all these automagically
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.p1)))
OBJ_MSTACK := $(wildcard hal/m-stack/obj/*.p1)
OBJ_HAL := $(wildcard hal/obj/*.p1)
OBJ_CSTDLIB := $(wildcard cstdlib/obj/*.p1)

LD_FLAGS := --chip=$(CHIP) -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
CC_FLAGS := --pass1 --chip=$(CHIP) -Q -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
INCL := -Ihal/include -Ihal/m-stack/include -Icstdlib/include -Isrc
DEFINES := 
LIBS := 
LIB_PATHS := 

all: picir
.PHONY: all
.PHONY: hal_dirs
.PHONY: download
.PHONY: clean

#link all object files
picir: submk $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $(LIB_PATHS) -o$@ $(OBJ_FILES) $(OBJ_MSTACK) $(OBJ_HAL) $(OBJ_CSTDLIB) $(LIBS)
#	xc8-bin2hex picir

#compile all c files in source to object files
obj/%.p1: src/%.c
	$(CC) $(DEFINE)  $(CC_FLAGS) $(INCL) $(DEFINES) -c -o$@ $<
	
submk:
	for dir in $(SUBMK); do \
		$(MAKE) -C $$dir; \
	done
	
download:
	scp picir.hex $(PROGRAMMER_USER)@$(PROGRAMMER_IP):~/
	ssh $(PROGRAMMER_USER)@$(PROGRAMMER_IP) -t 'exec bash -ilc "k14 lvp program picir.hex"'
	
clean:
	for dir in $(SUBMK); do \
		$(MAKE) clean -C $$dir; \
	done
	rm obj/*
	rm mainbin.*
	rm l.obj
	rm funclist
