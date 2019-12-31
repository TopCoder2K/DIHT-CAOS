#!/usr/bin/python3

from socket import socket, AF_INET, SOCK_DGRAM
import sys

IP = "127.0.0.1"
PORT = int(sys.argv[1])
MAX_SIZE = 1024

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind((IP, PORT))

while True:
    data, addr = sock.recvfrom(MAX_SIZE)
    if len(data)==4:
        req = int.from_bytes(data, byteorder='little', signed=True)
        resp = (req*2).to_bytes(4, byteorder='little', signed=True)
        sock.sendto(resp, addr)
        
