#
# Copyright (c) 2015 Sergi Granell (xerpi)
# based on Cirne's vita-toolchain test Makefile
#

TARGET		:= smsppsp
PSPAPP = smsplus/psp
SOUND=smsplus/sound
Z80=smsplus/cpu
PSP_APP_NAME=SMSPlusVITA
PSP_APP_VER=1.3.1

#SOURCES		:= src $(ZLIB)
SOURCES		:= smsplus

INCLUDES=-Ismsplus -Ismsplus/cpu -Ismsplus/sound -Ismsplus/unzip

BUILD_Z80=$(Z80)/z80.o
BUILD_EMUL=$(SOURCES)/sms.o $(SOURCES)/pio.o $(SOURCES)/memz80.o \
					 $(SOURCES)/render.o $(SOURCES)/vdp.o $(SOURCES)/tms.o \
					 $(SOURCES)/system.o $(SOURCES)/error.o $(SOURCES)/fileio.o \
					 $(SOURCES)/state.o $(SOURCES)/loadrom.o
BUILD_MZ=$(SOURCES)/unzip/ioapi.o $(SOURCES)/unzip/unzip.o
BUILD_SOUND=$(SOUND)/sound.o $(SOUND)/sn76489.o $(SOUND)/emu2413.o \
            $(SOUND)/ym2413.o $(SOUND)/fmintf.o $(SOUND)/stream.o
BUILD_PORT=$(PSPAPP)/main.o $(PSPAPP)/emumain.o $(PSPAPP)/menu.o


OBJS=$(BUILD_SOUND) $(BUILD_Z80) $(BUILD_MZ) \
     $(BUILD_EMUL) $(BUILD_PORT)

LIBS= -lpsplib -lpng -lz -lvita2d -lm_stub -lSceDisplay_stub -lSceGxm_stub 	\
	-lSceCtrl_stub -lSceAudio_stub -lSceRtc_stub -lScePower_stub -lSceAppUtil_stub

DEFINES = -DPSP -DPSP_APP_NAME=\"$(PSP_APP_NAME)\" -DPSP_APP_VER=\"$(PSP_APP_VER)\" \
					-DLSB_FIRST -DALIGN_DWORD -Dint32=int32_t -Dint16=int16_t -Du32=uint32_t \
					-Du64=uint64_t -DScePspDateTime=SceDateTime



PREFIX  = arm-none-eabi
AS	= $(PREFIX)-as
CC      = $(PREFIX)-gcc
CXX			=$(PREFIX)-g++
READELF = $(PREFIX)-readelf
OBJDUMP = $(PREFIX)-objdump
CFLAGS  = -O2 -Wall -specs=psp2.specs $(INCLUDES) $(DEFINES) -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
CXXFLAGS = $(CFLAGS) -mword-relocations -fno-rtti -Wno-deprecated -Wno-comment -Wno-sequence-point
ASFLAGS = $(CFLAGS)



all: $(TARGET).velf

$(TARGET).velf: $(TARGET).elf
	psp2-fixup -q -S $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).elf $(TARGET).velf $(OBJS) $(DATA)/*.h

copy: $(TARGET).velf
	@cp $(TARGET).velf ~/PSPSTUFF/compartido/$(TARGET).elf
