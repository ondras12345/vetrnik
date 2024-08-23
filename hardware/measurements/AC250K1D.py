#!/usr/bin/env python3

import serial
import logging
import argparse

_LOGGER = logging.getLogger(__name__)


class AC250K1D:
    @staticmethod
    def _checksum(packet: str | bytes) -> int:
        if not isinstance(packet, bytes):
            packet = packet.encode("ascii")
        return sum(packet) % 256

    def __init__(self, serial_port: str, address: int, timeout: float = 1.0):
        self.address = address
        self._ser = serial.Serial(serial_port, 9600, timeout=timeout)

    def _send(self, message: str):
        packet = f"{self.address:02X}{message}"
        packet = f"{packet}{self._checksum(packet):02X}"
        packet = f"@{packet}\r".encode("ascii")
        _LOGGER.debug("senging %r", packet)
        self._ser.write(packet)

    def _receive(self) -> str:
        # wait for '#' (packet init)
        self._ser.read_until(b"#")
        packet = b"#"
        packet += self._ser.read_until(b"\r")
        _LOGGER.debug("received %r", packet)
        # verify address
        packet = packet.decode("ascii")
        addr = packet[1:3]
        if addr != f"{self.address:02X}":
            raise ValueError(f"received message from wrong address: 0x{addr}")
        return packet[3:-1]

    def query(self, message: str):
        self._send(message)
        return self._receive()

    def command(self, cmd: str) -> bool:
        response = self.query(cmd)
        if response == "OK":
            return True
        elif response == "Err":
            return False
        else:
            raise RuntimeError(f"Device reported error: {response}")

    @property
    def voltage(self) -> int:
        """Get REAL output voltage (not SET)."""
        return int(self.query("NAP???")[3:])

    @voltage.setter
    def voltage(self, value: int) -> None:
        """Set output voltage."""
        if not self.command(f"NAP{value:03d}"):
            raise RuntimeError("failed to set voltage")

    @property
    def output(self) -> bool:
        # returns OUT1 / OUT0
        r = self.query("out?")
        if r == "OUT1":
            return True
        elif r == "OUT0":
            return False
        else:
            raise RuntimeError(f"unexpected response: {repr(r)}")

    @output.setter
    def output(self, value: bool) -> None:
        if not self.command(f"OUT{'1' if value else '0'}"):
            raise RuntimeError("failed to set output")

    def get_id(self) -> str:
        return self.query("ID?")

    def close(self):
        self._ser.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p", "--port", default="/dev/ttyUSB-AC250K1D",
        help="serial port (default %(default)s)"
    )
    parser.add_argument(
        "-a", "--address", type=int, default=1,
        help="AC250K1D address (default %(default)s)"
    )
    parser.add_argument(
        "-v", "--voltage", type=int,
        help="set voltage"
    )
    parser.add_argument(
        "-1", "--on", action="store_true",
        help="turn output on"
    )
    parser.add_argument(
        "-0", "--off", action="store_true",
        help="turn output off"
    )
    parser.add_argument("-d", "--debug", action="store_true")

    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    with AC250K1D(args.port, args.address) as psu:
        print(f"Info: {psu.get_id()}")
        print(f"Output: {psu.output}")
        print(f"Voltage: {psu.voltage} V")

        if args.voltage is not None:
            psu.voltage = args.voltage
        if args.on:
            psu.output = True
        elif args.off:
            psu.output = False


if __name__ == "__main__":
    main()
