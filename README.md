# LibFTL

The Fast TransLator (FTL) library can be used to generate executable x86 code
dynamically at runtime. It is intended to be used by instruction set simulators
as a dynamic translator for swiftly converting foreign target binaries to host
executable code on-the-fly at runtime.

[![Build Status](https://travis-ci.org/janweinstock/ftl.svg?branch=master)](https://travis-ci.org/janweinstock/ftl)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/20251/badge.svg)](https://scan.coverity.com/projects/janweinstock-ftl)

----
## Installation

Clone repository and update submodules if you want to build unit tests:
```
git clone git@github.com:janweinstock/ftl.git
cd ftl
git submodule update --init
```
Building `ftl` requires `cmake >= 3.8`. During configuration you must state
whether to build the unit tests and the example programs:
* `-DFTL_BUILD_TESTS=[ON|OFF]`: build unit tests (default `OFF`)
* `-DFTL_BUILD_EXAMPLES=[ON|OFF]`: build example programs (default `OFF`)
```
mkdir -p BUILD/RELEASE/BUILD
cd BUILD/RELEASE/BUILD
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=.. \
      -DFTL_BUILD_TESTS=ON -DFTL_BUILD_EXAMPLES=ON
make -j 4
make test
make install
```
If everything went well, the following artifacts will have been installed:
* ftl headers in `BUILD/RELEASE/include/ftl.h`
* ftl library in `BUILD/RELEASE/include/lib/libftl.a`
* ftl examples in `BUILD/RELEASE/examples/`

----
## Integration
You can use `ftl` as a submodule within `cmake` projects. For example:
```
set(FTL_BUILD_TESTS OFF)
set(FTL_BUILD_EXAMPLES OFF)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/ftl)
...
target_link_libraries(my_target ftl)
```

----
## License

This project is licensed under the Apache-2.0 license - see the
[LICENSE](LICENSE) file for details.
