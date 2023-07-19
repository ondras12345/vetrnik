# Lisp
This directory contains the lisp code for zvire-vetrnik-control.

Type `make` to compile `out.lisp`, which is the file to be written in the SPI
flash.


## Uploading
```
# connect via telnet (do not use that nc trick, it does not work)
SPIflash erase init.lisp
SPIflahs dump init.lisp 0
SPIflash ed init.lisp 0
# paste out.lisp
.
reset
```
