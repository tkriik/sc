CC=		cc

STDFLAGS=	-std=c11 -pedantic
DFLAGS=		-D_DEFAULT_SOURCE
OFLAGS=		-O2
WFLAGS=		-Wall \
		-Wextra \
		-Wconversion \
		-Wfloat-equal \
		-Wshadow \
		-Wpointer-arith \
		-Wcast-align \
		-Wstrict-prototypes \
		-Wstrict-overflow=5 \
		-Wwrite-strings \
		-Waggregate-return \
		-Wcast-qual \
		-Wundef \
		-Wunreachable-code \
		-Wuninitialized \
		-Wformat=2 \
		-Wno-format-nonliteral \
		-Wno-unused-but-set-variable
FFLAGS=		-ftrapv
EXTRAFLAGS=	-Werror \
		-DNDEBUG
CFLAGS=		$(STDFLAGS) $(DFLAGS) $(OFLAGS) $(WFLAGS) $(FFLAGS) $(EXTRAFLAGS)

SRC=		main.c \
		bf.c \
		fnv1a.c

BIN=		sc

.PHONY: all debug

$(BIN): $(SRC)
	$(CC) -o $(BIN) $(CFLAGS) $(SRC) $(LDFLAGS)

all: $(BIN)

debug: OFLAGS=-Og
debug: EXTRAFLAGS=-g -fno-omit-frame-pointer
debug: all

clean:
	rm -f $(BIN)
