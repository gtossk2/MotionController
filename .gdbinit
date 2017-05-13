#OpenOCD GDB server, enable semihosting, program the flash, and wait for command
target extended localhost:3333
monitor arm semihosting enable
monitor reset halt
load controller.elf
file controller.elf
monitor reset init

