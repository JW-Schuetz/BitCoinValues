#
# LINUX program for polling the actual bitcoin-value.
# Is still running on my RasBerry-Pi.
#

BIN = bitcoinkurs
OBJ = bitcoinkurs.o dopoll.o parse.o writedata.o timerhandling.o\
      exitfun.o setopt.o init.o signalhandler.o smtputil.o\
	  doapihistory.o
CC  = gcc
LNK = -lc -lcurl -lrt

%.o: %.c	bitcoinkurs.h makefile
			${CC} -ggdb -O0 -c $<

$(BIN):		$(OBJ)
			${CC} ${LNK} $(OBJ) -o $(BIN)
			mv $(BIN) ../$(BIN)

clean:
		rm -f *.o; rm -f $(BIN)