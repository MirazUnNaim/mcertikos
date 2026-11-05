#!/bin/bash
cd "$(dirname "$0")"
make LAB=1 && make qemu LAB=1
