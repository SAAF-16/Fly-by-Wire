SOURCES= pfcD.c transducers.c wes.c
OBJECTS= pfc tra wes pri
CC= gcc
CFLAGS= -w -o
ARGS= ./G18.txt
OBJDIR= bin log tmp
BIN=./bin/
arg2=


pfc:	pfcD.c ${OBJDIR}
		${CC} pfcD.c ${CFLAGS} ${BIN}pfc -lm

tra:	transducers.c ${OBJDIR}
		${CC} transducers.c ${CFLAGS} ${BIN}tra

wes:	wes.c ${OBJDIR}
		${CC} wes.c ${CFLAGS} ${BIN}wes

pri:	principale.c ${OBJDIR}
		${CC} principale.c ${CFLAGS} ${BIN}pri

all:	${OBJDIR} ${OBJECTS} 

${OBJDIR}:
		mkdir -p ${OBJDIR}

#make run ARGS="xxx"
run:	pri
		${BIN}pri ${ARGS}

clean:	
		rm ${BIN}*
		rmdir ${OBJDIR}