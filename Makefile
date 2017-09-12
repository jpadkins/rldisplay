#.SILENT:

COMP = cc
FLGS = -std=c99 -Wall -Wextra -Werror -Wconversion

GLFW_LIBS = -lglfw -ldl
FT_2_LIBS = `pkg-config --cflags freetype2`
SFML_LIBS = -lcsfml-system -lcsfml-window -lcsfml-graphics

SFML = bin/example_sfml
GLFW = bin/example_glfw
BINS = $(SFML) $(GLFW)

SFML_SRC = src/main.c src/rl_display_sfml.c
GLFW_SRC = src/main.c src/rl_display_glfw.c src/glad.c

all: $(BINS)

run_sfml: $(SFML)
	$(SFML)

run_glfw: $(GLFW)
	$(GLFW)

clean:
	rm -rf $(BINS)

$(SFML): $(SRC_SFML)
	$(COMP) $(FLGS) $^ -o $@ $(SFML_SRC) $(SFML_LIBS)

$(GLFW): $(SRC_GLFW)
	$(COMP) $(FLGS) $^ -o $@ $(GLFW_SRC) $(GLFW_LIBS) $(FT_2)

check_sfml: $(SFML)
	valgrind --leak-check=full $(SFML)

check_glfw: $(GLFW)
	valgrind --leak-check=full $(GLFW)
