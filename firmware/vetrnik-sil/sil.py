#!/usr/bin/env python3
import argparse
import sys
import pathlib
import pprint
from cffi import FFI
from typing import Any
from inspect import getmembers

ffi = FFI()
lib_header = pathlib.Path("wt_sil_preprocessed.h")


def cdata_dict(cd):
    if isinstance(cd, ffi.CData):
        try:
            return ffi.string(cd)
        except TypeError:
            try:
                return [cdata_dict(x) for x in cd]
            except TypeError:
                return {k: cdata_dict(v) for k, v in getmembers(cd)}
    else:
        return cd


class SILState:
    """Wrapper for (wt_sil_state_t *)."""

    def __init__(self, state_struct):
        self._s = state_struct

    def to_dict(self) -> dict[str, Any]:
        ret = {
            "pwr_status": cdata_dict(self._s.pwr_status),
            "ctrl_strategy": self._s.ctrl_strategy,
            "stats": cdata_dict(self._s.stats),
            "pwr_REL": cdata_dict(self._s.pwr_REL),
            "outputs": cdata_dict(self._s.outputs),
            "lcd": self.lcd_str(),
            "lcd_backlight": self._s.lcd_backlight,
            "ctrl_contactor_state": self._s.ctrl_contactor_state,
            "pump": self._s.pump,
        }
        return ret

    def lcd_str(self) -> str:
        ret = ""
        for row in self._s.lcd:
            ret += f"|{ffi.string(row).decode('ascii')}|\n"
        return ret[:-1]

    def __str__(self) -> str:
        return pprint.pformat(self.to_dict())


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

    ffi.cdef(lib_header.read_text())
    sil = ffi.dlopen("./libvetrniksil.so")
    s = ffi.new("wt_sil_state_t *")
    sil.wt_sil_init(s)

    try:
        match args.command:
            case "repl":
                # retry on error:
                r = False
                while not r:
                    r = sil.wt_sil_repl(s)
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
