CC=gcc
AR=ar
CFLAGS=-I/opt/vc/include -I..
LDFLAGS=-L/opt/vc/lib -L. -lbrcmEGL -lbrcmGLESv2 -ltftgl -lbcm2835  -I/opt/vc/include -L/opt/vc/lib -lbcm_host 

.PHONY: default all clean

default: pansim
all: default
	
pangui: pangui.o Combine.o
	$(CC) -o pansim pansim.o $(LDFLAGS)
		
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clean:
	-rm -f pansim pansim.o
