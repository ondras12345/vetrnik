#!/usr/bin/env python3
import argparse
import tempfile
import pathlib
import sys
import os
from OMPython import OMCSessionZMQ


def strip_annots(omc, model_name: str) -> str:
    omc.sendExpression('loadModel(Modelica)')
    omc.sendExpression(f'loadFile("Vetrnik/package.mo")')
    with tempfile.TemporaryDirectory() as tmp:
        tmpfile = pathlib.Path(tmp) / "listing.mo"
        print("tmpfile =", tmpfile, file=sys.stderr)
        if not omc.sendExpression(f'saveTotalModel("{tmpfile}", Vetrnik.{model_name})'):
            raise Exception("saveTotalModel failed")
        omc.sendExpression('clear()')
        if not omc.sendExpression(f'loadFile("{tmpfile}")'):
            raise Exception("loadFile failed")
    out = omc.sendExpression(f'list(Vetrnik.{model_name})')
    if not out:
        raise Exception("list() failed")
    return out


def main():
    parser = argparse.ArgumentParser(
        description="strip annotations from a Modelica model file"
    )
    parser.add_argument(
        "model_name",
        help="Modelica model name. "
             "It must be located in a file called {model_name}.mo"
    )
    parser.add_argument("-o", dest="output_file", type=argparse.FileType("w"),
                        default="-", help="output file")
    args = parser.parse_args()

    omc = OMCSessionZMQ()
    out = strip_annots(omc, args.model_name)
    with args.output_file as f:
        print(out, file=f)


if __name__ == "__main__":
    main()
