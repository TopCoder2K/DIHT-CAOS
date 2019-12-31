import socket
import sys


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(("127.0.0.1", int(sys.argv[1])))


string = input()
while string != "done":
    string = input()
    sock.send(string.encode())
    # sock.shutdown(socket.SHUT_WR)
    print(sock.recv(228).decode())

sock.close()
