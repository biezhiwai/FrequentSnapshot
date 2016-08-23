all:ckp_simulator
.PHONY:all

ckp_simulator:
	gcc -m64 -D _FILE_OFFSET_BITS=64 -D TICK_UPDATE -D OFF_DUMP util.c naive.c cou.c zigzag.c pingpong.c mk.c LL.c system.c main.c -o ./bin/ckp_simulator -pthread -lnuma -Wall -lrt -g
clean:
	rm  ./bin/*
