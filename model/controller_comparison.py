#!/usr/bin/env python3
import argparse
import logging
import pathlib
import shutil
import subprocess
import numpy as np
import pandas as pd
from OMPython import ModelicaSystem
from dataclasses import dataclass

LISP_DIR = pathlib.Path("../firmware/vetrnik-control/lisp")
LISP_CONTROLLER_SYMLINK = LISP_DIR / "32-control-MPPT.lisp"
LISP_CONTROLLER_SYMLINK_ORIG = LISP_DIR / "32-control-MPPT.lisp.orig"
LISP_CONTROLLER_DIR = LISP_DIR / "controllers"

MODEL_DIR = pathlib.Path(__file__).resolve().parent
DATA_DIR = MODEL_DIR / "data"


@dataclass
class Controller:
    name: str
    file: pathlib.Path


def get_controllers() -> list[Controller]:
    """Get list of available LISP controllers."""
    return [Controller(name=p.stem, file=p)
            for p in LISP_CONTROLLER_DIR.glob("*.lisp")]

def build_lisp() -> None:
    (LISP_DIR / "out.lisp").unlink()
    subprocess.run("make", cwd=LISP_DIR)


def select_controller(controller: Controller) -> None:
    LISP_CONTROLLER_SYMLINK.unlink(missing_ok=True)
    LISP_CONTROLLER_SYMLINK.symlink_to(controller.file.relative_to(LISP_DIR))
    build_lisp()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    args = parser.parse_args()
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        logging.getLogger("OMPython").setLevel(logging.DEBUG)

    LISP_CONTROLLER_SYMLINK_ORIG.unlink(missing_ok=True)
    LISP_CONTROLLER_SYMLINK.rename(LISP_CONTROLLER_SYMLINK_ORIG)
    try:
        controllers = get_controllers()

        wind_profile_csv = DATA_DIR / "vwind-ramps.csv"
        wind_profile = pd.read_csv(wind_profile_csv)

        vetrnik_package = MODEL_DIR / "Vetrnik" / "package.mo"
        mod = ModelicaSystem(
            vetrnik_package, "Vetrnik.SILsimplifiedCsv", ["Modelica"],
            variableFilter=r"(vwind|rpm|duty|simplifiedOpenLoop.windTurbine.Cp|simplifiedOpenLoop.load.lossPower)",
            #raiseerrors=True  # it complained about the Jacobian, because
                               # OMPython issues --generateSymbolicLinearization by default
        )
        stop_time: float = wind_profile.time.iloc[-1]
        time_interval: float = 0.1
        mod.setSimulationOptions([
            f"stopTime={stop_time}",
            f"stepSize={time_interval}",
        ])
        mod.setInputs("vwind="+repr(list(wind_profile.itertuples(index=False, name=None))))

        # model_tempdir = pathlib.Path(mod.getWorkDirectory())
        # shutil.copy(wind_profile_csv, model_tempdir)

        mod.setSimulationOptions("outputFormat=csv")

        result_dir = DATA_DIR / "controller_comparison"
        shutil.rmtree(result_dir, ignore_errors=True)
        result_dir.mkdir()
        for controller in controllers:
            print("\n\ncontroller:", controller)
            select_controller(controller)
            result_file = result_dir / (controller.name + ".csv")
            mod.simulate(resultfile=result_file)
    finally:
        # clean up: restore the old symlink
        LISP_CONTROLLER_SYMLINK.unlink(missing_ok=True)
        LISP_CONTROLLER_SYMLINK_ORIG.rename(LISP_CONTROLLER_SYMLINK)
        build_lisp()
