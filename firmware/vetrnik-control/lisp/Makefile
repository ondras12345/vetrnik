## 		VARIABLES
## Set environment variables or specify on the command line to override.
## Either of these should work:
## $ MQTT_HOST=localhost make deploy
## $ make deploy MQTT_HOST=localhost
##
MQTT_HOST ?= localhost        ## IP address or hostname of MQTT broker.
MQTT_PORT ?= 1883             ## MQTT broker port
MQTT_TOPIC_PREFIX ?= vetrnik/ ## topic prefix as configured on vetrnik-control


sources := $(sort $(wildcard [0-9]*.lisp))


##
## 		TARGETS
.PHONY: all
all:                    ## Compile everything (default target).
all: out.lisp spec.md

out.lisp: ${sources}
	#cat ${sources} | ../tools/lisp-minify > $@
	@for file in ${sources} ; do \
	    echo "; BEGIN $$file" ; \
	    ../tools/lisp-minify $$file ; \
	    echo "" ; \
	done > $@

spec.md: spec.md.jinja spec.yaml spec.py test.lisp.jinja
	./spec.py

.PHONY: clean
clean:                  ## Remove generated files.
	rm -f out.lisp spec.md tests/test_*.lisp

.PHONY: deploy
deploy:                 ## Load to memory on the target device.
deploy: out.lisp
	if [ "$$(wc -L out.lisp | awk '{print $$1}')" -ge 508 ] ; then >&2 echo "out.lisp contains a line longer than 508 chars" ; exit 1 ; fi
	mosquitto_pub -h $(MQTT_HOST) -p $(MQTT_PORT) -t $(strip $(MQTT_TOPIC_PREFIX))cmnd/cli -q 2 -m "lisp_reset"
	sleep 3
	mosquitto_pub -h $(MQTT_HOST) -p $(MQTT_PORT) -t $(strip $(MQTT_TOPIC_PREFIX))cmnd/lisp -q 2 --stdin-line < out.lisp

.PHONY: help
help:                   ## Show this help message.
	@grep -F -h "##" $(MAKEFILE_LIST) | sed -e '/unique_BhwaDzu7C/d;s/\\$$//;s/## \?//'
