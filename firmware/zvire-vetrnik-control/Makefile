BIN_FILE = .pio/build/blackpill_f401cc/firmware.bin
## 		VARIABLES
## Set environment variables or specify on the command line to override.
## Either of these should work:
## $ OTA_PASSWORD=test make ota
## $ make ota OTA_PASSWORD=test
##
OTA_HOST ?= localhost   ## IP address of Ethernet module.
OTA_PASSWORD ?= password##
SSH_HOST ?= ntb1        ## SSH host for upload_remote

##
## 		TARGETS
.PHONY: all
all:                    ## Compile, default target.
all: compile_commands.json compile  # upload

.PHONY: compile
compile:                ## Compile firmware.
compile: include/version.h
	pio run

.PHONY: clean
clean:                  ## Clean project.
	pio run --target clean
	-rm include/version.h

.PHONY: upload
upload:                 ## Upload firmware via local DFU.
	pio run --target upload

.PHONY: upload_remote
upload_remote:          ## Upload firmware via DFU on remote SSH_HOST
	@tar cf - $(BIN_FILE) | \
	ssh $(strip $(SSH_HOST)) '\
	    set -e; \
	    tmpdir=$$(mktemp -d -t tmp.XXXXXXXXXX); \
	    cleanup () { \
	        #ls -la "$$tmpdir"; \
	        rm -rf "$$tmpdir"; \
	    }; \
	    trap cleanup EXIT; \
	    cd "$$tmpdir"; \
	    tar xf -; \
	    set -o xtrace; \
	    dfu-util -d 0x0483:0xDF11 -a 0 -s 0x08000000:leave -D $(BIN_FILE) 2>&1 \
	    '

.PHONY: ota
ota:                    ## Upload firmware over-the-air
	curl \
	    --request POST \
	    --data-binary @$(BIN_FILE) \
	    http://arduino:$(strip $(OTA_PASSWORD))@$(strip $(OTA_HOST)):65280/sketch

.PHONY: check
check:                  ## Run static code analysis
	pio check

.PHONY: test
test:                   ## Run native tests.
	pio test -e native

compile_commands.json:  ## For youcompleteme
compile_commands.json: platformio.ini
	pio run --target compiledb

# This needs to be regenerated on every run to be able to
# detect -dirty state.
.PHONY: include/version.h
include/version.h:
	echo "#define VETRNIK_VERSION \"$(shell git describe --dirty --always --tags)\"" > include/version.h
	echo "#define VETRNIK_BUILD_TIME \"$(shell date --utc +"%Y%m%dT%H%M%SZ")\"" >> include/version.h


.PHONY: help
help:                   ## Show this help message.
	@grep -F -h "##" $(MAKEFILE_LIST) | sed -e '/unique_BhwaDzu7C/d;s/\\$$//;s/## \?//'
