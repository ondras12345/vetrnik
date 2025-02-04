.PHONY: all
all: listings windCp.csv

.PHONY: listings
listings: $(patsubst %.mo,listings/%.listing,$(wildcard *.mo))

listings/%.listing: %.mo
	./exportlisting.py -o $@ $(basename $<)

windCp.csv: windCp.mo windCp.py
	./windCp.py

.PHONY: clean
clean:
	rm -f windCp.csv listings/*.listing
