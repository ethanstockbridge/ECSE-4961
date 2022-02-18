"""
Description: Generate file of a specified size, consisting of random bytes
Usage: python file_gen.py
"""

import os

fname = input("Output file name: ")
megabytes = int(input("Output file size (MB): "))

MBtoB = 1024*1024

with open(fname, 'wb') as fout:
    fout.write(os.urandom(MBtoB*megabytes))