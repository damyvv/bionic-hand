#!/bin/bash
cd ..
mkdir -p .vscode
ip route show | grep eth0 | grep -v via | cut -d' ' -f1 | sed 's/0\/20/1/' | tr -d '\n' > .vscode/wsl-ip