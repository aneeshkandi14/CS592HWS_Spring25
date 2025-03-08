#!/usr/bin/env python3
# 80137d50: 14600002 7c42


import sys

infile = sys.argv[1]
outfile = sys.argv[2]

i = open(infile, "r")
o = open(outfile, "wb")

for line in i.readlines():
    line = line.strip()
    line = line.replace("\t", "")[:32]
    data = bytes.fromhex(line)
    o.write(data)

i.close()
o.close()
