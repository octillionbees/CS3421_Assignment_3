OBJFILES= parse.o Clock.o CPU.o DatMem.o InstMem.o
TARGET= cs3421_emul

${TARGET}: ${OBJFILES}
	g++ -g -Wall -Wextra -o ${TARGET} ${OBJFILES}

Clock.o: Clock.h
CPU.o: CPU.h
DatMem.o: DatMem.h
InstMem.o: InstMem.h
parse.o: Clock.h CPU.h DatMem.h InstMem.h

%.o: %.cpp
	g++ -g -c $<

.PHONY: clean
clean:
	rm -f ${OBJFILES} ${TARGET}