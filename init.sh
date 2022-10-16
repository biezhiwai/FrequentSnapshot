#!/usr/bin/env bash
# need to install miniconda3 before run this script.

sudo apt-get install libnuma-dev
# install perf
sudo apt install flex bison libssl-dev libelf-dev
git clone --depth=1 https://github.com/microsoft/WSL2-Linux-Kernel.git
cd WSL2-Linux-Kernel/tools/perf
make
cp ./perf /usr/bin
cd  /etc/ld.so.conf.d
echo "/home/cxk/miniconda3/lib" | sudo tee -a python3.conf
sudo ldconfig