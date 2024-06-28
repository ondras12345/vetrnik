# Lisp
This directory contains the lisp code for vetrnik-control.

Type `make` to compile `out.lisp`, which is the file to be written to the SPI
flash.


## Uploading
Simple:
```
# connect via telnet (do not use that nc trick, it does not work)
erase init.lisp
hexdump init.lisp 0
ed init.lisp 0
# paste out.lisp
.
reset
```

Advanced:
```
filltext init.lisp
# will print end address
ed init.lisp $end_addr+1
# paste out.lisp
.
reset
# (or lisp_reset & lisp_read with addr)
```
