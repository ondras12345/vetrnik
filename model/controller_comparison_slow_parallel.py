#!/usr/bin/env python3
"""
Perform SIL simulation of all LISP controllers with the "slow", nonlinear
model Vetrnik.SILcsv.
This will take about 4 hours on an N core CPU, where N is number of
LISP controllers to be tested.
"""

import argparse
import logging
import pathlib
import shutil
import pandas as pd
import time
import io
from concurrent.futures import ThreadPoolExecutor
from OMPython import ModelicaSystem
from controller_comparison import get_controllers, build_lisp, select_controller

LISP_DIR = pathlib.Path("../firmware/vetrnik-control/lisp")
LISP_CONTROLLER_SYMLINK = LISP_DIR / "32-control-MPPT.lisp"
LISP_CONTROLLER_SYMLINK_ORIG = LISP_DIR / "32-control-MPPT.lisp.orig"
LISP_CONTROLLER_DIR = LISP_DIR / "controllers"

MODEL_DIR = pathlib.Path(__file__).resolve().parent
DATA_DIR = MODEL_DIR / "data"


def simulate_model(controller, wind_profile, result_file):
    vetrnik_package = MODEL_DIR / "Vetrnik" / "package.mo"
    mod = ModelicaSystem(
        vetrnik_package, "Vetrnik.SILcsv", ["Modelica"],
        variableFilter=r"(rpm|duty|windTurbine.Cp|capacitor.v|uFilter.y|Rl.LossPower)",
        # it complained about the Jacobian, because OMPython issues
        # --generateSymbolicLinearization by default
        # raiseerrors=True,
    )

    stop_time: float = wind_profile.time.iloc[-1]
    time_interval: float = 0.1
    mod.setSimulationOptions([
        f"stopTime={stop_time}",
        f"stepSize={time_interval}",
        "outputFormat=csv",
    ])
    mod.setInputs("vwind="+repr(list(wind_profile.itertuples(index=False, name=None))))
    mod.simulate(resultfile=result_file)


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

        result_dir = DATA_DIR / "controller_comparison_slow"
        if result_dir.exists():
            raise Exception("result_dir already exists")
        result_dir.mkdir()
        wind_profile.to_csv(result_dir / "vwind.csv", index=False)

        with ThreadPoolExecutor(max_workers=len(controllers), thread_name_prefix="Worker") as executor:
            futures = []
            for controller in controllers:
                print("\ncontroller:", controller.name)
                select_controller(controller)
                shutil.copy(LISP_DIR / "out.lisp", result_dir / f"out-{controller.name}.lisp")
                result_file = result_dir / (controller.name + ".csv")

                print("starting simulation...")
                futures.append(executor.submit(simulate_model, controller, wind_profile, result_file))
                # give it time to start and load its lisp file
                # before changing it
                while not result_file.exists():
                    time.sleep(0.5)
                time.sleep(3)
                print("simulation running")

            # progress bar
            done = [False]
            while not all(done):
                time.sleep(30)
                done = [future.done() for future in futures]
                try:
                    with open(result_file, "rb") as f:
                        f.seek(-350, io.SEEK_END)
                        f.readline()  # throw one line away (might be incomplete
                        li = f.readline().decode('ascii')
                        ti = float(li.split(",")[0])
                    print(f"time for {controller.name}: {ti}\tdone={done}")
                except Exception as e:
                    # don't crash the whole program due to a problem in the
                    # progress bar code...
                    print(e)
            print("all done")
    finally:
        # clean up: restore the old symlink
        LISP_CONTROLLER_SYMLINK.unlink(missing_ok=True)
        LISP_CONTROLLER_SYMLINK_ORIG.rename(LISP_CONTROLLER_SYMLINK)
        build_lisp()
    print("done")
