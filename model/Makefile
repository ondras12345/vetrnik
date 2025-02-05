.PHONY: all
all: listings windCp.csv

LISTINGS=windCp WindTurbine VariableDamper
.PHONY: listings
listings: $(patsubst %,listings/%.listing, $(LISTINGS))

listings/%.listing: Vetrnik/%.mo
	./exportlisting.py -o $@ $(patsubst Vetrnik/%.mo, %, $<)

windCp.csv: Vetrnik/windCp.mo windCp.py
	./windCp.py

.PHONY: clean
clean:
	rm -f windCp.csv listings/*.listing
