#.SILENT:

COMP = cc
LIBS = -lm -ldl
FLGS = -std=c99 -Wall -Wextra -Werror -Wconversion
SFML = -lcsfml-system -lcsfml-window -lcsfml-graphics

VGARGS = --leak-check=full --suppressions=etc/valgrind/display.supp

BIN = bin/example
SRC = src/main.c src/rl_display_sfml.c

all: $(BIN) $(GLFW_BIN)

run: $(BIN)
	$(BIN)

clean:
	rm -rf $(BIN) $(GLFW_BIN)

$(BIN): $(SRC)
	$(COMP) $(FLGS) $^ -o $@ $(LIBS) $(SFML)

check: $(BIN)
	valgrind $(VGARGS) $(BIN)
