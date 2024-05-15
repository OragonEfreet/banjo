# Building From Sources {#build}

\brief How to build the documentation from source code

If you want to build the project from source, follow the instructions of this page.

## Requirements

The project is provided with a [CMake](https://cmake.org/download/) configuration.
Download and install the software.

You will also need a C99-compliant development environment (Visual Studio, GCC, ...).

## Build

1. Clone or download this repository.
2. Open CMake.
   - Hit "Browse Source..." and select the root directory of your clone/download (where _CMakeLists.txt_ is).
   - Hit "Browse Build..." and select the output directory. It can be any new/empty directory.
3. Run "Configure"
   - Select your development environment.
4. Run "Generate"

Navigate to the output directory and build the project.
For fully visual IDEs, the "Open Project" button opens your software for you.

### Options

The build system provides various options used to customize the build:

* `BANJO_FEAT_PEDANTIC` [_OFF_]: If _ON_, when a part of the code implementation has the choice between being performant or safe, it will choose safety.
* `BANJO_FEAT_LOG_COLOR` [_ON_]:  The logging system will output colored.
* `BANJO_BUILD_EXAMPLES` [_ON_]:  Example programs are built.

## Examples

- On GNU Make projects, the executable examples are in the _examples/_ folder of the output directory.
- On Visual Studio, they are located in the _Examples_ solution folder.

## Test

- On GNU Make projects, run `make test` to run the tests.
- In Visual Studio, build the `RUN TESTS` target.

