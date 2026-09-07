#
# Arduino Esplora LUFA Generic HID firmware
# Requires avr-gcc toolchain and submodule lufa/ (or LUFA_PATH).
#
#   git submodule update --init
#   make
#   make avrdude  (or flash via Arduino bootloader / Flip)
#

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = EsploraHID
SRC          = EsploraHID.c Descriptors.c EsploraBoard.c $(LUFA_SRC_USB)
LUFA_PATH    ?= ./lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ -I.
LD_FLAGS     =

# Arduino Esplora / Leonardo bootloader (avrdude)
AVRDUDE_PROGRAMMER ?= avr109
AVRDUDE_PORT       ?= COM3
AVRDUDE_FLAGS      = -b 57600

all:

DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

DMBS_PATH ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
