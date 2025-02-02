#!/usr/bin/env python3
import argparse
import sys
import pathlib
from cffi import FFI

header = pathlib.Path("include/wt_sil_simple.h")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command", choices=("repl",))
    args = parser.parse_args()

    match args.command:
        case "repl":
            ffi = FFI()
            ffi.cdef(header.read_text())
            sil = ffi.dlopen("./libvetrniksil.so")
            s = ffi.new("wt_sil_state_t *")
            sil.wt_sil_init(s)
            sil.wt_sil_repl(s)
            sil.wt_sil_close(s)
        case other:
            sys.exit(f"unknown command: {other}")


if __name__ == "__main__":
    main()
