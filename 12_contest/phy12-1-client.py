#!/usr/bin/python3

import sys
import socket
import time
import os
import socket
import signal

time.sleep(1)  # ensure port_num.txt created and server started

if not os.path.exists("port_num.txt"):
    err = "Can't find port_num.fifo in " + os.getcwd() + "\n"
    print(err, file=sys.stderr)
    sys.exit(10)

port_num = None
pid = int(sys.argv[3])
    
with open("port_num.txt", "r") as f:
    contents = f.read()
    port_num = int(contents)

if not port_num:
    err = "No information about port number\n"
    print(err, file=sys.stderr)
    sys.exit(10)

dat = None
corr = None

with open(sys.argv[1], "r") as f:
    dat = f.read()
    corr = dat.upper()

print("Interactor started, connecting to port {} (pid={})\n".format(port_num,pid), file=sys.stderr)

try:
    sock = socket.create_connection(("localhost", port_num))
except Exception as e:
    msg = "Can't connect to {}: {}\n".format(port_num, e)
    print(msg, file=sys.stderr)
    sys.exit(4)

ans = None

print("Connected to server\n", file=sys.stderr)

try:
    sock.send(dat.encode())
    ans = ""
    while True:
        block = sock.recv(65536).decode()
        if block:
            ans += block
        else:
            break
except Exception as e:
    msg = "Error while comm to {}: {}\n".format(port_num, e)
    print(msg, file=sys.stderr)
    sys.exit(4)

print("Sent: {}\nGot: {}\n".format(dat, ans), file=sys.stderr)

# Normalize response
import re
ans = ans.replace("\r", "").replace("\n", " ").strip()
ans = re.sub(r"\s+", " ", ans).lower()


with open(sys.argv[2], "w") as f:
    f.write(ans)

time.sleep(1)  # wait a little before shutdown
sock.close()

    
print("Shutting down server by SIGTERM\n", file=sys.stderr)
os.kill(pid, signal.SIGTERM)
print("Interaction done.\n", file=sys.stderr)
sys.exit(0)
