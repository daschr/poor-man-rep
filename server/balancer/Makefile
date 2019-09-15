CC=gcc
DBG=-DDEBUG
LIBS=-lsqlite3 -ljansson


all: wlan_balancer.c mongoose.c
	$(CC) -O3 $(DBG) $(LIBS) -o wlan_balancer $^
	
