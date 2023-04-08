# CellNta

## Table of Contents
- [About](#about)
- [Building](#building)
- [Related](#related)

## About
N-Dimensional cellular automaton. 
Currently project contain a lot of bugs and most of the features still not implemented

## Building
```shell
git clone https://github.com/Rejeq/CellNta
cd CellNta
mkdir build && cd build
cmake ..
cmake --build . -j 4
```

#### Testing
To build with the testing, add `-DCELLNTA_TESTING=ON` cmake flag  
To run tests: `ctest .` in the build directory

#### Dependendies
The dependencies will be automatically loaded and compiled by default,
but if you have problems, you can install everything manually,
just disable fetching by adding cmake flag: `CELLNTA_FETCHING=OFF`

List of dependencies:
- [Eigen3](https://eigen.tuxfamily.org/index.php) - math library
- [GLEW](https://glew.sourceforge.net/) - openGL loader
- [Boost](https://www.boost.org/) - used only dynamic_bitset, so needed only headers
- [Spdlog](https://github.com/gabime/spdlog) - logging system, optional, add `-DCELLNTA_LOGGING=OFF` to disable
- [Tracy](https://github.com/wolfpld/tracy) - optional, profiling tools
- [GoogleTest](https://github.com/google/googletest) - optional, testing

For Desktop gui:
- [SDL2](https://www.libsdl.org/) - window manager
- [ImGui](https://github.com/ocornut/imgui) - gui library

Most of these libraries can be automatically defined on your system, so you only need to install them.  
But for some libraries, like imgui, you need to specify the `-D[LIB]_DIR`
variable (see ./cmake/Find`[LIB]`.cmake for the exact name).

You can use a package manager, such as [vcpkg](https://vcpkg.io/en/index.html),
and install the dependencies with the following command:
```shell
vcpkg install sdl2 glew boost spdlog eigen3
```
Afterwards, when configuring with cmake, add the toolchain file flag:
`-DCMAKE_TOOLCHAIN_FILE='[path to vcpkg]/scripts/buildsystems/vcpkg.cmake'`

## Related
- [NDCell](https://github.com/HactarCE/NDCell) - similar project
- [TrueNgine](https://github.com/GSBicalho/TrueNgine) - a N-Dimensional Renderer
- [Perspective Projections](https://web.archive.org/web/20201111215855/https://ef.gy/linear-algebra:perspective-projections) - article about how the Nd camera works
