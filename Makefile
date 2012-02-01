# Project: server

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES = 
LINKSRV  = ser_tcp.o $(RES)
LINKCLI = cli_tcp.o $(RES)
OBJ = cli_tcp.o ser_tcp.o filetransfer.o
LIBS = -lwsock32
INCS =  
CXXINCS =  
BINSRV = server
BINCLI = client
CXXFLAGS = $(CXXINCS)  
RM = rm -f

all: server client


clean:
	${RM} $(OBJ) $(BINSRV) $(BINCLI)

server: ser_tcp.o filetransfer.o $(RES)
	$(CPP) $(LINKSRV) -o $(BINSRV) $(LIBS)

client: cli_tcp.o filetransfer.o $(RES)
	$(CPP) $(LINKCLI) -o $(BINCLI) $(LIBS)

cli_tcp.o: cli_tcp.cpp
	$(CPP) -c cli_tcp.cpp -o cli_tcp.o $(CXXFLAGS)

ser_tcp.o: ser_tcp.cpp
	$(CPP) -c ser_tcp.cpp -o ser_tcp.o $(CXXFLAGS)

filetransfer.o: filetransfer.cpp
	$(CPP) -c filetransfer.cpp -o filetransfer.o $(CXXFLAGS)
