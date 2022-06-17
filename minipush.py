import tty
from typing import TextIO
import serial
import select
import os
import time
import sys

SERIAL_BAUD = 921_600

class UartConnection:
    name_short: str = "MT"
    serial: serial.Serial

    def __init__(self, serial_path: str):
        self.serial = serial.Serial(serial_path, SERIAL_BAUD)
    
    def send_string(self, string: str) -> int:
        return self.send_bytes(bytes(string, "ascii"))

    def send_bytes(self, bytes_to_send: bytes) -> int:
        return self.serial.write(bytes_to_send)

    def read(self, max_len: int) -> bytes:
        return self.serial.read(max_len)

    def read_buffer(self) -> bytes:
        return self.read(self.serial.in_waiting)

    def read_buffer_string(self) -> str:
        return self.read_buffer().decode("ascii")
    
    def start_interactive(self, input_file: TextIO, output_file: TextIO):
        try:
            tty.setcbreak(input_file.fileno())
            while True:
                rfd, _, _ = select.select([self.serial, input_file], [], [])

                if self.serial in rfd:
                    r = self.read_buffer_string()
                    output_file.write(r)
                    output_file.flush()

                if input_file in rfd:
                    r = input_file.read(1)
                    self.send_string(r)
        except KeyboardInterrupt:
            print("Got Ctrl+C. Exiting...")
            exit(0)
        except OSError as e:
            print("Got OSError. Resetting...")
        finally:
            os.system("stty sane")

class Minipush(UartConnection):
    payload_path: str
    def __init__(self, serial_path: str, payload_path: str):
        super().__init__(serial_path)
        self.payload_path = payload_path
    
    def wait_for_payload_request(self):
        print("Waiting for connection...")
        recieved_bytes = self.read(1)
        count = 0
        while True:
            for byte in recieved_bytes:
                if byte == 3:
                    count += 1
                else:
                    count = 0
                    print(chr(byte), end="")
                if count == 3:
                    return
            recieved_bytes = self.read(1)
    
    def send_payload(self):
        print("[MP] Sending payload...")

        with open(self.payload_path, "rb") as f:
            payload = f.read()

        payload_size = len(payload)
        self.send_bytes(bytes([payload_size & 0xFF,payload_size >>8 & 0xFF,payload_size >>16& 0xFF,payload_size >>24& 0xFF]))
        if self.read(2) != b"OK":
            print("[MP] Sending failed")
            exit(1)

        i = 0
        while True:
            chunk_lower = 512*i
            if chunk_lower > payload_size:
                break
            chunk_higher = min(512*(i+1), payload_size)
            payload_chunk = payload[chunk_lower:chunk_higher]
            self.send_bytes(payload_chunk)
            i += 1
        print("[MP] Sending complete")
    
    def run(self):
        self.wait_for_payload_request()
        self.send_payload()
        self.start_interactive(sys.stdin, sys.stdout)


def main():
    minipush = Minipush(
        serial_path=sys.argv[1],
        payload_path=sys.argv[2]
    )
    time.sleep(1)
    minipush.run()

if __name__ == '__main__':
    main()