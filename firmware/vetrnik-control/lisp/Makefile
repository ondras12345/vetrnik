sources := $(sort $(wildcard [0-9]*.lisp))

.PHONY: all
all: out.lisp spec.md

out.lisp: ${sources}
	#cat ${sources} | ../tools/lisp-minify > $@
	@for file in ${sources} ; do \
	    echo "; BEGIN $$file" ; \
	    ../tools/lisp-minify $$file ; \
	    echo "" ; \
	done > $@

spec.md: spec.md.jinja spec.yaml spec.py
	./spec.py

.PHONY: clean
clean:
	-rm out.lisp
	-rm spec.md
