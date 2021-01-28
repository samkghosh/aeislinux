#Makefile to compile Communication Library
HOME=/home/projects
MYSQLINC=/usr/include/mysql
MYSQLLIB=/usr/lib64/mysql


CC=g++ 
AR=ar -rcs 
LIBSDIR=$(HOME)/general/lib
LIBS= -lcommlibs -llogs -ldb -lmysqlclient_r -lpthread -leasy
//CFLAGS=-c -D TESTOUT
CFLAGS=-w -c -fpermissive 
INC=$(HOME)/general/inc $(MYSQLINC)

#OBJS= Tcpip.o Message.o utils.o 

OBJS=ArcaneSyncService.o base64.o msgRcvr.o receiveFile.o scheduleReport.o sha1.o statusmgmt.o usermgmt.o iniFunc.o


ArcaneSyncService.o: ArcaneSyncService.cpp

base64.o:	base64.cpp

msgRcvr.o: msgRcvr.cpp

receiveFile.o: receiveFile.cpp

scheduleReport.o: scheduleReport.cpp

sha1.o: sha1.cpp

statusmgmt.o: statusmgmt.cpp

usermgmt.o: usermgmt.cpp

#webcom.o: webcom.cpp

iniFunc.o: iniFunc.cpp



all: $(OBJS)
	$(CC) -o aeisserver  $(OBJS) -L$(MYSQLLIB) -L$(LIBSDIR) $(LIBS)

.c.o:
	$(CC) -c -I$(INC) $< 2>>err

.cpp.o:
	$(CC) $(CFLAGS) -I$(INC) -I$(MYSQLINC) $< 
#$(CC) $(CFLAGS) -I$(INC) $< 2>>err

clean:
	rm -rf *.o







