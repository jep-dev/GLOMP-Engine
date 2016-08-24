# FOGL
Asynchronous model/view/controller engine/framework; under construction.

#### Requirements:
* Python 2.7 or 3.4
* OpenGL 3.30 Core or better
* OpenGL Utility Library (GLU)
* [GL3W](https://github.com/skaslev/gl3w) -- OpenGL extension loading
* [GLFW](https://github.com/glfw/glfw) -- windows with OpenGL and event handling
* [Boost](https://github.com/boostorg/boost) -- versatile C++ support library
* [Doxygen](https://github.com/doxygen/doxygen) (optional) -- see Documentation section


#### Documentation
Comments for declarations are kept in the project headers, and use the QT variant of Doxygen formatting. To generate HTML and/or LaTex from these comments, simply run `doxygen` from the `doc` directory, with or without modifying the Doxyfile to your liking.

#### Building:

1. Run `git clone https://github.com/skaslev/gl3w ../gl3w`

2. In `../gl3w/`, run `gl3w_gen.py` to generate OpenGL headers

3. Run `git clone https://github.com/boostorg/boost ../boost`

4. In `../boost/`, run `bootstrap.sh`, then `b2`

5. In `./`, Update `Makefile` as necessary, then run `make`

6. Run one of the following:

 a. `ln -s ../gl3w/libgl3w.so your_path` where `your_path` is a directory in your `LD_LIBRARY_PATH`

 b. Add `:../gl3w/` to the tail of your `LD_LIBRARY_PATH`

The Makefile targets gl3w for convenience, but building gl3w separately is always an option. OpenGL and GLU are not provided by gl3w; Boost and other dependencies are not targeted and must be met or made independently.

All of the requirements support Linux, Windows, and OS X. This project only supports Linux, but will support Windows XP or better before release. 

#### Todo:
- [ ] include/util/types.hpp (152) Distinguish between permutations with and without duplicates; the current permutation test relies on the exclusive-or of two parameter packs, not necessarily sets
- [ ] include/system.hpp (13) Remove file reading function and more in favor of in-place ifstream, etc.
- [ ] Makefile (45) Automate submodule targets; requires iteration over pairs (module directories with submodule targets)


