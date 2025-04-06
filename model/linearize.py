#!/usr/bin/env python3
from OMPython import ModelicaSystem
import pathlib
import logging
import argparse
import numpy as np
import pickle
from pprint import pprint
from linearize_result import Result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
        logging.getLogger("OMPython").setLevel(logging.DEBUG)

    wd = pathlib.Path(__file__).resolve().parent
    vetrnik_package = wd / "Vetrnik" / "package.mo"

    mod = ModelicaSystem(str(vetrnik_package),
                         "Vetrnik.SimplifiedOpenLoopLin",
                         ["Modelica"],
                         raiseerrors=True)

    # lambda_opt = 9.25  # read from plotCp graph
    # R = float(mod.getParameters("windTurbine.R")[0])  # rotor diameter

    operating_points = [
        # vwind     b
        # b values were determined experimentally to reach lambda_opt.
        (4,         0.048),
        (6,         0.073),
        (8,         0.100),
        (10,        0.122),
        (12,        0.145),
        (15,        0.185),

        # these b values result in operating points further away from lambda_opt:
        (6,         0.120),  # lambda=6.5
        (6,         0.030),  # lambda=12.7
        (10,        0.200),  # lambda=6.5
        (10,        0.020),  # lambda=15.2
        (15,        0.020),  # lambda=15.9
        (15,        0.300),  # lambda=6.5
    ]

    results = []

    for vwind, b in operating_points:
        print(f"vwind={vwind}, b={b}")
        # Set initial condition somewhere near omega_opt.
        # Otherwise, the turbine might not even start.
        # omega_opt = lambda_opt / R * vwind
        # I don't have an easy way to do that here, I have instead hardcoded
        # an initial condition in Vetrnik.SimplifiedOpenLoopLin.

        mod.setInputs([f"vwind={vwind}", f"b={b}"])

        mod.setLinearizationOptions(["stopTime=100.0", "stepSize=0.05"])

        sim_datafile = wd/'data'/'linearize.csv'
        (A, B, C, D) = mod.linearize(simflags=f" -override=outputFormat=csv -r={sim_datafile}")
        with open(sim_datafile, "r") as f:
            line = f.readline()
            delimiter = ","
            col = line.split(delimiter).index('"windTurbine.lambda"')
            # move to last line
            for line in f:
                pass
            tsr = float(line.split(delimiter)[col])  # lambda
        sim_datafile.unlink()  # delete temporary data file

        result = Result(
            A=np.array(A), B=np.array(B), C=np.array(C), D=np.array(D),
            vwind=vwind, b=b, tsr=tsr
        )
        results.append(result)

    print(mod.getLinearizationOptions())

    # Simulate one of them to make sure everything works
    # mod.setSimulationOptions(["outputFormat=csv", "stopTime=100.0"])
    # vwind, b = operating_points[-1]
    # mod.setInputs([f"vwind={vwind}", f"b={b}"])
    # mod.simulate(resultfile=str(wd / "data" / "linearize_simtest.csv"))

    p = {
        "results": results,
        "inputs": mod.getLinearInputs(),
        "outputs": mod.getLinearOutputs(),
        "states": mod.getLinearStates(),
    }
    pprint(p)
    with open("data/linearize.p", "wb") as f:
        pickle.dump(p, f)


if __name__ == "__main__":
    main()
