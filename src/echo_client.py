#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

HOST = '172.128.1.100'
PORT = 9876

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello, world')
    data = s.recv(1024)

print('Received', repr(data))
