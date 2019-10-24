CC=g++
AR=ar
CFLAGS=-I/opt/vc/include  -Wno-write-strings -g -fpermissive 
LDFLAGS=-L/opt/vc/lib -L. -lbrcmEGL -lbrcmGLESv2 -lpthread -I/opt/vc/include -L/opt/vc/lib -lbcm_host  -lm -Wno-write-strings

.PHONY: default all clean

default: pangui

all: default

pangui: finalpanguimain.o Combine.o imgui_es2.o SerialSearcher.o finalpangui.o linux_serial.o BottomEncoder.o ModSourceScreen.o ModTargetModal.o ScreenSetup.o HomeScreen.o VCF2StructureScreen.o ExtraScreenDefinitions.o ParameterModal.o BuildScreens.o NewModulationModal.o
	$(CC) -o finalpanguimain *.o $(LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	-rm -f finalpanguimain finalpangui.o BottomEncoder.o ModSourceScreen.o ModTargetModal.o ScreenSetup.o HomeScreen.o VCF2StructureScreen.o
