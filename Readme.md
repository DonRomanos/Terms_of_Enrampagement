[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status:](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)](https://travis-ci.com/DonRomanos/Challenge_Template.svg?branch=master)

# Challenge Template

This is a basic setup to create some of my programming Challenges. It contains a *CMakeLists.txt* and *conanfile.py* as well as a basic Benchmark and Tests. It sets up and uses the following tools / libraries:

* CMake
* Conan
* GTest
* Google Microbenchmark

**Note**: By default this uses C++20 because I want to play around with the newest things.

## How to use

Requirements:

* CMake
* Conan
* Python (for Conan)

Setting up a Conan profile for your environment

```shell
conan profile new WhateverCompilerSettingsYouWant --detect
```

For more Infos see: https://docs.conan.io/en/latest/reference/profiles.html

Installing the Libraries using Conan

```shell
mkdir build && cd build
conan install .. --build missing
conan build ..
```

Now you should see the library successfully compiling and running the tests.

### Disabling some of the Dependencies

If you don't have the dependencies or dont want to build with them for some reason you can disable them with these CMake Options, simply set them to 'OFF'.

```cmake
BUILD_REFERENCE
BUILD_TESTS
BUILD_BENCHMARK
```

### Alternative without Conan

Requirements:

* CMake

You can build the library without Conan if you manually install the gtest and google benchmark libraries.

Just tell Cmake where to find them by setting following CMake Variables

```cmake
gtest_DIR
benchmark_ROOT_DIR
```

You can do so via command line

```cmake
cmake -Dgtest_DIR=usr/local/... -Dbenchmark_ROOT_DIR=usr/...
```

or via the gui by adding a path entry with the name.

## Folder Structure

Should be self-explaining, I simplified the approach compared to what I had in the beginning. Now its much more flat.

## For the Future

Some ideas that can be tried out...

* At the moment I use the Visual Studio open CMakeCache feature on windows, however I can not switch the config with the current setup. Maybe this can be improved.
* Experiment a little bit with CMake calling conan, as then there would be only one entry point.

## Issues discovered during Developement

While setting up this repository I came across several problems which I want to note down here.

### Conan Generators dealing with Targets

Currently I'm using the 'cmake_find_package' generator which will create a 'findXXX.Cmake' file. This way I don't have to modify anything in the CMakeLists.txt compared to an approach without Conan. However since this file is automatically created it does not take into account all targets but creates only one for gtest for example.

Therefore I could not link against gtest::main but had to provide the main for myself. As the Gtest package comes with a Config.cmake I wonder if its possible to have the 'cmake_find_package' generator use that instead. But this is a separate story that should be adressed within Conan.

### Ctest swallowing output

At first I wanted to show the benchmark output as part of the tests. However ctest by default swallows all the output and can only be brought to bring it up for failing tests, as far as I could see. I could not work my way around it and decided to make a target and not a test executing the benchmark.

Another separate Story would be adding an option to add_test in cmake to show the standard output.
