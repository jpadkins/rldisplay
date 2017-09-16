#.SILENT:

COMP = cc
LIBS = -lm -ldl -isystem lib
FLGS = -std=c99 -Wall -Wextra -Werror -Wconversion
SFML = -lcsfml-system -lcsfml-window -lcsfml-graphics

VGARGS = --leak-check=full --suppressions=etc/valgrind/display.supp

BIN = bin/example
SRC = $(wildcard src/*.c)

all: $(BIN)

run: $(BIN)
	$(BIN)

clean:
	rm -rf $(BIN)

$(BIN): $(SRC)
	$(COMP) $(FLGS) $^ -o $@ $(LIBS) $(SFML)

check: $(BIN)
	valgrind $(VGARGS) $(BIN)
