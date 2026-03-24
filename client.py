import socket
import time
import argparse

#IP = "127.0.0.1"
#PORT = 5000
#MESSAGE = "CLIENT_SET|X|10"
#INTERVAL = 5  # seconds

parser = argparse.ArgumentParser(description="Send messages periodically to a destination")
parser.add_argument("--ip", required=True, help="IP destination")
parser.add_argument("--port", type=int, required=True, help="Port destination")
parser.add_argument("--msg", required=True, help="Message to send")
parser.add_argument("--interval", type=float, default=5, help="Interval in seconds")

args = parser.parse_args()

IP = args.ip
PORT = args.port
MESSAGE = args.msg
INTERVAL = args.interval

while True:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((IP, PORT))
            s.sendall(MESSAGE.encode())
            print("CLIENT: Message sended")

            try:
                data = s.recv(1024)
                print("CLIENT: ", data.decode())
            except socket.timeout:
                print("ERROR: Server data not received")

    except Exception as e:
        print("Error:", e)

    time.sleep(INTERVAL)