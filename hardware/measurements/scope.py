#!/usr/bin/env python3
# pip3 install pyserial
# #pip3 install pyvisa  # not necessary ??
# pip3 install pyvisa-py
# pip3 install pandas

import numpy as np
import pandas as pd
import pyvisa
from pyvisa import constants
import argparse
import logging


_LOGGER = logging.getLogger(__name__)

_FORMATS = {
    0: "BYTE",
    1: "WORD",
    2: "ASCII",
}

_TYPES = {
    0: "NORM",
    1: "PEAK",  # PEAK detect
    2: "AVER",  # AVERage
}


def scope_get_settings(scope):
    settings = {}
    sl = [
        ":TIMEBASE:RANGE",
        ":TIMEBASE:DELAY",
        ":TIMEBASE:REFERENCE",
        ":TIMEBASE:MODE",
        ":TIMEBASE:POSITION",
        ":TIMEBASE:REFERENCE",
        ":TIMEBASE:SCALE",
        ":CHANNEL1:DISPLAY",
        ":CHANNEL1:PROBE",
        ":CHANNEL1:RANGE",
        ":CHANNEL1:OFFSET",
        ":CHANNEL1:BWLIMIT",
        ":CHANNEL1:COUPLING",
        ":CHANNEL1:INVERT",
        ":CHANNEL1:UNITS",
        ":CHANNEL2:DISPLAY",
        ":CHANNEL2:PROBE",
        ":CHANNEL2:RANGE",
        ":CHANNEL2:OFFSET",
        ":CHANNEL2:BWLIMIT",
        ":CHANNEL2:COUPLING",
        ":CHANNEL2:INVERT",
        ":CHANNEL2:UNITS",
        ":TRIGGER:MODE",
        ":TRIGGER:COUPLING",
        ":TRIGGER:LEVEL",
        ":TRIGGER:REJECT",
        ":TRIGGER:SLOPE",
        ":TRIGGER:SOURCE",
        ":TRIGGER:HFREJECT",
        ":TRIGGER:HOLDOFF",
        ":TRIGGER:SWEEP",
        ":ACQUIRE:TYPE",
        ":FUNCTION:CENTER",
        ":FUNCTION:DISPLAY",
        ":FUNCTION:OPERATION",
        ":FUNCTION:RANGE",
        ":FUNCTION:REFERENCE",
        ":FUNCTION:SCALE",
        ":FUNCTION:SOURCE",
        ":FUNCTION:SPAN",
        ":FUNCTION:WINDOW",
    ]

    for s in sl:
        settings[s] = scope.query(f"{s}?")

    return settings


def scope_setup_read(scope, N_points=2000):
    """Set up scope for reading waveform data.
    :param N_points: one of 100, 250, 500, 1000, 2000, "MAX"
    """
    #scope.write(":WAVEFORM:FORMAT ASCII")  # does not work well with PyVISA
    scope.write(":WAVEFORM:FORMAT WORD")
    scope.write(":WAVEFORM:BYTEORDER LSBFIRST")
    scope.write(":WAVEFORM:UNSIGNED 0")
    scope.write(f":WAVEFORM:POINTS {N_points}")



def scope_read_channel(scope, channel: str):
    """Read waveform data from scope.
    :param channel: one of 'CHANNEL1', 'CHANNEL2', 'POD1', 'POD2', 'FUNCTION'
    """
    _LOGGER.info("reading channel %s", channel)
    scope.write(f":WAVEFORM:SOURCE {channel}")

    pre = scope.query(":WAVEFORM:PREAMBLE?").split(",")
    _LOGGER.debug("raw preamble: %r", pre)
    preamble = {
        "FORMAT": _FORMATS.get(int(pre[0]), "?"),
        "TYPE": _TYPES.get(int(pre[1]), "?"),
        "POINTS": int(pre[2]),
        "COUNT": int(pre[3]),  # average count or 1 if PEAK or NORM
        "XINCREMENT": float(pre[4]),  # s
        "XORIGIN": float(pre[5]),  # s
        "XREFERENCE": float(pre[6]),  # s
        "YINCREMENT": float(pre[7]),  # V
        "YORIGIN": float(pre[8]),  # V
        "YREFERENCE": float(pre[9]),  # V
    }
    _LOGGER.info("preamble: %s", preamble)

    # ASCII still has header with length, thus this won't work:
    #wav = scope.query_ascii_values(":WAVEFORM:DATA?")

    wav = scope.query_binary_values(
        ":WAVEFORM:DATA?",
        datatype="h", is_big_endian=False, container=np.array
    )

    wav = pd.DataFrame({"voltage_raw": wav})
    # see also https://github.com/kiwih/agilent-rs232/blob/master/agilent-rs232.py
    wav["time [s]"] = (wav.index - preamble["XREFERENCE"]) * preamble["XINCREMENT"] + preamble["XORIGIN"]
    wav[f"{channel} [V]"] = (wav.voltage_raw - preamble["YREFERENCE"]) * preamble["YINCREMENT"] + preamble["YORIGIN"]
    wav.drop(columns=["voltage_raw"], inplace=True)

    return wav


def scope_read_channels(scope, settings):
    """Read all displayed channels and merge to one dataframe"""

    scope_setup_read(scope)

    wav = pd.DataFrame({"time [s]": []})
    for channel in ["CHANNEL1", "CHANNEL2", "FUNCTION"]:
        if settings[f":{channel}:DISPLAY"] == "1":
            wav_ch = scope_read_channel(scope, channel)
            wav = pd.merge_ordered(wav, wav_ch, on="time [s]")

    return wav


def scope_save_measurement(scope, name: str):
    settings = scope_get_settings(scope)
    txt_file = name + '.txt'
    with open(txt_file, "w") as f:
        for key, value in settings.items():
            f.write(f"{key} {value}\n")
    print(f"Scope settings written to {txt_file}")

    wav = scope_read_channels(scope, settings)
    #print(wav)
    csv_file = name + '.csv'
    wav.to_csv(csv_file, index = False)
    print(f"Data written to {csv_file}")


def scope_save_screenshot(scope, name: str):
    res = scope.query_binary_values(
        ":DISPlay:DATA? BMP",
        datatype='B', container=bytearray
    )
    bmp_file = name + '.bmp'
    with open(bmp_file, "wb") as f:
        f.write(res)
    print(f"Screen image written to {bmp_file}")


def scope_open(rm, resource: str = "ASRL/dev/ttyUSB-scope::INSTR"):
    scope = rm.open_resource(
        resource, baud_rate=57600,
        flow_control=constants.VI_ASRL_FLOW_DTR_DSR,
        write_termination = '\n', read_termination='\n',
        timeout=45000
    )
    return scope


def main():
    logging.basicConfig(level=logging.INFO)

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-D", "--device",
        help="Serial port the scope is attached to, default: %(default)s",
        default="/dev/ttyUSB-scope"
    )
    parser.add_argument(
        "-b", "--bmp", action="store_true",
        help="Also get a BMP screenshot"
    )
    parser.add_argument(
        "name",
        help="Name (without extension) of the files to write the output to"
    )

    args = parser.parse_args()

    rm = pyvisa.ResourceManager()

    scope = scope_open(rm, f"ASRL{args.device}::INSTR")

    print(scope.query("*IDN?"))

    scope_save_measurement(scope, args.name)

    if args.bmp:
        scope_save_screenshot(scope, args.name)

    rm.close()


if __name__ == '__main__':
    main()


# Notes:
# - write :DIGITIZE to get a new single shot capture
