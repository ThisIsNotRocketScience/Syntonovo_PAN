CC=g++
AR=ar
CFLAGS=-I/opt/vc/include -I.. -Wno-write-strings -g
LDFLAGS=-L/opt/vc/lib -L. -lbrcmEGL -lbrcmGLESv2 -lpthread -I/opt/vc/include -L/opt/vc/lib -lbcm_host  -lm -Wno-write-strings

.PHONY: default all clean

default: finalpanguimain

all: default

pangui: finalpanguimain.o Combine.o imgui_es2.o SerialSearcher.o finalpangui.o panstructs.o serial.o
	$(CC) -o finalpanguimain *.o $(LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	-rm -f finalpanguimain *.o
