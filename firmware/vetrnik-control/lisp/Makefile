sources := $(sort $(wildcard [0-9]*.lisp))

out.lisp: ${sources}
	#cat ${sources} | ../tools/lisp-minify > $@
	@for file in ${sources} ; do \
	    echo "; BEGIN $$file" ; \
	    ../tools/lisp-minify $$file ; \
	    echo "" ; \
	done > $@


.PHONY: clean
clean:
	-rm out.lisp
