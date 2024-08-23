#!/usr/bin/env python3

import pyvisa

# TODO remove dle1041_ prefix
# TODO does not respond after first open, try write("") in open()

def dle1041_open(rm, port: str = "/dev/ttyUSB-dle1041"):
    return rm.open_resource(
        f"ASRL{port}::INSTR", baud_rate=9600,
        read_termination="\r\n", write_termination="\n"
    )

def dle1041_parse_col(col):
    """Parse a pandas column with dle1041 measurements and return column with floats"""
    import pandas as pd
    import numpy as np
    col = col.str.strip().str.split(" ", n=1, expand=True)[0]
    col = col.replace("OVLOAD", np.nan)
    col = pd.to_numeric(col)
    return col

if __name__ == "__main__":
    rm = pyvisa.ResourceManager()
    meter = dle1041_open(rm)
    print(meter.query("*IDN?"))
    meter.write("VDC")
    print(meter.query("READ?"))
    rm.close()
