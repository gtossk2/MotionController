# Put your stlink folder here so make burn will work.
STLINK=/usr/local/bin

# Put your source files here (or *.c, etc)
SRCS = src/*.c

# Your header files directionary
INCLUDE = ./include/

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=controller

STM_LIB_NAME = STM32F4xx_StdPeriph_Driver

# Put your STM32F4 library code directory here
STM_COMMON=./STM32F4/STM32F4-Discovery_FW_V1.1.0

# Subdir to build STM32F4 Library
STM_LIBRARY=$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/build

# Normally you shouldn't need to change anything below this line!
#######################################################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g -O2 -Wall -T./ldscripts/stm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += --specs=nosys.specs
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
#CFLAGS += -mfpu=fpv4-sp-d16
#CFLAGS += -ffreestanding -nostdlib
CFLAGS += -I.

# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc

# Include files
CFLAGS += -I$(INCLUDE)

# add startup file to build
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 
OBJS = $(SRCS:.c=.o)

.PHONY: proj

all: proj

proj: $(STM_LIB_NAME).a $(PROJ_NAME).elf

$(STM_LIB_NAME).a:
	cd $(STM_LIBRARY) && $(MAKE)

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ -L$(STM_LIBRARY) -lSTM32F4xx_StdPeriph_Driver 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

clean_stm:
	cd $(STM_LIBRARY) && $(MAKE) clean

# Flash the STM32F4
burn: proj
	openocd \
		-f interface/stlink-v2.cfg \
		-f target/stm32f4x_stlink.cfg \
		-c "init" \
		-c "reset init" \
		-c "stm32f2x unlock 0" \
		-c "flash probe 0" \
		-c "flash info 0" \
		-c "flash write_image erase $(PROJ_NAME).bin 0x8000000" \
		-c "reset run" -c shutdown || \
	st-flash write $(PROJ_NAME).bin 0x8000000


gdb:
	arm-none-eabi-gdb -q -iex 'add-auto-load-safe-path /' .gdbinit $(PROJ_NAME).elf

openocd:
	openocd -f board/stm32f4discovery.cfg
