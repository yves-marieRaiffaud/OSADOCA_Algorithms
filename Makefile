EXECUTABLE := main

CC := /usr/bin/gcc
XENO_CONFIG := /usr/xenomai/bin/xeno-config

CFLAGS :=  $(shell $(XENO_CONFIG)   --posix --alchemy --cflags)
LDFLAGS :=  $(shell $(XENO_CONFIG)  --posix --alchemy --ldflags)

OPT := -Werror --std=c99
MATHLIB := -lm
TCPIP := -I/root/OSADOCA/TCP_IP
ADDITIONALFILES := -I/root/OSADOCA/AdditionalFiles
CONTROLALGO := -I/root/OSADOCA/Navigation_Algorithms

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(OPT) $(ADDITIONALFILES) $(MATHLIB) $(TCPIP) $(CONTROLALGO)

clean:
	rm -f $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

