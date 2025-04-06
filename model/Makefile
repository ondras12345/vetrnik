.PHONY: all
all: listings data/windCp.csv diagrams data/linearize_b_tfs.p

LISTINGS=windCp WindTurbine VariableDamper vetrnikController
.PHONY: listings
listings: $(patsubst %,listings/%.listing, $(LISTINGS))

# diagram svgs must be exported manually in OMEdit
.PHONY: diagrams
diagrams: $(patsubst %.svg,%-converted.pdf, $(wildcard diagrams/*.svg))

listings/%.listing: Vetrnik/%.mo
	./exportlisting.py -o $@ $(patsubst Vetrnik/%.mo, %, $<)

data/windCp.csv: Vetrnik/windCp.mo windCp.py
	./windCp.py

# inkscape does not work well for exporting pdfs from svg diagrams,
# some of the connect() lines are missing
diagrams/%-converted.pdf: diagrams/%.svg
	cairosvg $< -o $@

data/linearize.p: linearize.py $(wildcard Vetrnik/*.mo)
	./linearize.py

data/linearize_b_tfs.p: linearize_analyze.py data/linearize.p
	./linearize_analyze.py

.PHONY: clean
clean:
	rm -f data/windCp.csv data/linearize*.p data/linearize*.mat listings/*.listing diagrams/*-converted.pdf
