.PHONY: all
all:            ## Compile and upload, default target for Vim
all: compile_commands.json compile  # upload

.PHONY: compile
compile:        ## Compile firmware.
	pio run

.PHONY: clean
clean:          ## Clean project.
	pio run --target clean

.PHONY: upload
upload:         ## Upload firmware.
	pio run --target upload

.PHONY: bootloader
bootloader:     ## Burn fuses and bootloader.
	pio run --target bootloader

.PHONY: check
check:          ## Run static code analysis
	pio check

.PHONY: test
test:           ## Run native tests.
	pio test -e native

.PHONY: test_embedded
test_embedded:           ## Run native tests.
	pio test -e uno

.PHONY: disassemble
disassemble:    ## Open less with disassembly of the firmware
	~/.platformio/packages/toolchain-atmelavr/avr/bin/objdump -S .pio/build/ATmega8/firmware.elf | less

compile_commands.json: platformio.ini
	pio run --target compiledb

.PHONY: help
help:           ## Show this help.
	@grep -F -h "##" $(MAKEFILE_LIST) | sed -e '/unique_BhwaDzu7C/d;s/\\$$//;s/##//'
