MPLAB_DIR := /opt/microchip/MPLABX
XC8_DIR := /opt/microchip/xc8

CHIP := 16F1454
#the folder that contains usb_config.h
CONFIGPATH := ../src/

CC := xc8
C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.p1)))
LD_FLAGS := 
CC_FLAGS := --pass1 --chip=$(CHIP) -Q -G --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto
INCL := -I./include -I./src -I$(CONFIGPATH)
LIBS := -ldsp-elf
LIB_PATHS := -Lmicrochip/lib

#link all object files
objects: $(OBJ_FILES)

#compile all c files in source to object files
obj/%.p1: src/%.c
	$(CC) $(CC_FLAGS) $(INCL) -c -o$@ $<
	
test:
	echo $(C_FILES)	
	
clean:
	rm obj/*.d obj/*.pre obj/*.p1

#xc8 --pass1  --chip=16F1454 -Q -G  --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free  -I".." -I"../../../usb/include" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -obuild/PIC16F1459/production/_ext/1268166442/usb.p1  ../../../usb/src/usb.c 

#xc8 --pass1  --chip=16F1454 -Q -G  --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 -I".." -I"../../../usb/include" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -obuild/PIC16F1459/production/_ext/1268166442/usb_hid.p1  ../../../usb/src/usb_hid.c 
#xc8 --pass1  --chip=16F1454 -Q -G  --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 -I".." -I"../../../usb/include" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -obuild/PIC16F1459/production/_ext/1472/usb_descriptors.p1  ../usb_descriptors.c 
#xc8 --pass1  --chip=16F1454 -Q -G  --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 -I".." -I"../../../usb/include" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -obuild/PIC16F1459/production/_ext/1472/main.p1  ../main.c 
#xc8  --chip=16F1454 -G -mdist/PIC16F1459/production/MPLAB.X.production.map  --double=24 --float=24 --opt=default,-asm,-asmfile,-speed,+space,+debug --addrqual=ignore --mode=free -P -N255 -I".." -I"../../../usb/include" --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,+osccal,-resetbits,-download,-stackcall,+clib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     -odist/PIC16F1459/production/MPLAB.X.production.elf  build/PIC16F1459/production/_ext/1268166442/usb.p1 build/PIC16F1459/production/_ext/1268166442/usb_hid.p1 build/PIC16F1459/production/_ext/1472/usb_descriptors.p1 build/PIC16F1459/production/_ext/1472/main.p1     
