CC=gcc
DBG=-DDEBUG
LIBS=-lsqlite3


all: wlan_balancer.c mongoose.c
	$(CC) $(DBG) $(LIBS) -o wlan_balancer $^
	
