CC=g++
AR=ar
CFLAGS=-I/opt/vc/include -I.. -Wno-write-strings
LDFLAGS=-L/opt/vc/lib -L. -lbrcmEGL -lbrcmGLESv2 -ltftgl -lbcm2835  -I/opt/vc/include -L/opt/vc/lib -lbcm_host  -lm -Wno-write-strings

.PHONY: default all clean

default: pangui
all: default
	
pangui: pangui.o Combine.o imgui_es2.o
	$(CC) -o pangui pangui.o Combine.o imgui_es2.o $(LDFLAGS)
		
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clean:
	-rm -f pangui pangui.o
