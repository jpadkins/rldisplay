# Roguelike Display

Provides an interface for creating and managing a window as well as
rendering glyph data for text-centric graphical applications such as
roguelike games. The interface is generic across backends, with CSFML
being the initial backend window management/rendering library used
(to facilitate rapid development). Next on my list is GLFW (or maybe SDL2)
with modern OpenGL/FreeType using GLAD to bind the function pointers.

Once stabilized, a header-only version of the library will be generated.
