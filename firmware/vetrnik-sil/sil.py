#!/usr/bin/env python3
import argparse
import sys
import pathlib
from cffi import FFI

header = pathlib.Path("wt_sil_preprocessed.h")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser(
        "repl",
        help="run an interactive read-eval-print-loop"
    )
    parser_run = subparsers.add_parser(
        "run",
        help="run a lisp file"
    )
    parser_run.add_argument("file", nargs="+")

    args = parser.parse_args()

    ffi = FFI()
    ffi.cdef(header.read_text())
    sil = ffi.dlopen("./libvetrniksil.so")
    s = ffi.new("wt_sil_state_t *")
    sil.wt_sil_init(s)

    try:
        match args.command:
            case "repl":
                sil.wt_sil_repl(s)
            case "run":
                for f in args.file:
                    print(f"running file {f}")
                    result = sil.wt_sil_run_file(s, f.encode("utf8"))
                    if result != 0:
                        sys.exit(result)

            case other:
                sys.exit(f"unknown command: {other}")
    finally:
        sil.wt_sil_close(s)


if __name__ == "__main__":
    main()
