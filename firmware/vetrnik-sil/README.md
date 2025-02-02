# Software-in-the-loop
## Build
```sh
sudo apt install build-essential cmake python3-cffi

cmake .
make
```

## Usage
```console
$ ./sil.py repl
> (* 2 3)
6
> (lcdb)
nil
> (lcdb t)
t
> (lcdb)
t
>

$ ./sil.py run ../vetrnik-control/lisp/tests/test_*.lisp
running file ../vetrnik-control/lisp/tests/test_ctrlg.lisp
.
running file ../vetrnik-control/lisp/tests/test_lcdb.lisp
.
.
running file ../vetrnik-control/lisp/tests/test_map.lisp
.
.
.
[...]
```
