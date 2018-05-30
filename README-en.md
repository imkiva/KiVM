KiVM
=============
[![Build Status](https://travis-ci.org/imkiva/KiVM.svg?branch=master)](https://travis-ci.org/imkiva/KiVM)
[![GitHub top language](https://img.shields.io/github/languages/top/imkiva/KiVM.svg)](https://github.com/imkiva/KiVM)
[![license](https://img.shields.io/github/license/imkiva/KiVM.svg?colorB=000000)](https://github.com/imkiva/KiVM)

Kiva's Java Virtual Machine.

### Features
- JNI Support
- JAR Class Loading Support
- Full OracleJDK/OpenJDK compatibility
- Garbage Collector [WIP]

### Build
1. Requirements
    * Linux or macOS. **Windows is not supported yet.**
    * Java Development Kit >= 8
    * CMake >= 3.8

2. Build instructions
    1. First, clone this repo.
    2. `cd` into your directory that contains KiVM source code.
    3. Type `cmake . && make` in your terminal app.
    4. Enjoy it!

### Run
```bash
$ export CLASSPATH=</path/to/rt.jar>:</path/to/charsets.jar>:...
$ ./java <main-class-name> [args...]
```

### Credit
* Inspired by [wind_jvm](https://github.com/wind2412/wind_jvm)
* Modified version of [libzippp](https://github.com/ctabin/libzippp)
