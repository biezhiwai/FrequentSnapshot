all:ckp_simulator
.PHONY:all

ckp_simulator:
	gcc -D FULL_UPDATE util.c naive.c cou.c zigzag.c pingpong.c mk.c LL.c system.c main.c -o ./bin/ckp_simulator -pthread -lnuma -Wall -lrt -g
clean:
	rm  ./bin/*
