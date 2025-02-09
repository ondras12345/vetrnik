.PHONY: all
all: listings windCp.csv diagrams

LISTINGS=windCp WindTurbine VariableDamper vetrnikController
.PHONY: listings
listings: $(patsubst %,listings/%.listing, $(LISTINGS))

# diagram svgs must be exported manually in OMEdit
.PHONY: diagrams
diagrams: $(patsubst %.svg,%-converted.pdf, $(wildcard diagrams/*.svg))

listings/%.listing: Vetrnik/%.mo
	./exportlisting.py -o $@ $(patsubst Vetrnik/%.mo, %, $<)

windCp.csv: Vetrnik/windCp.mo windCp.py
	./windCp.py

# inkscape does not work well for exporting pdfs from svg diagrams,
# some of the connect() lines are missing
diagrams/%-converted.pdf: diagrams/%.svg
	cairosvg $< -o $@

.PHONY: clean
clean:
	rm -f windCp.csv listings/*.listing diagrams/*-converted.pdf
