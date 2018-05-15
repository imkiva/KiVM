KiVM
=============
[English](README-en.md)

[![Build Status](https://travis-ci.org/imkiva/KiVM.svg?branch=master)](https://travis-ci.org/imkiva/KiVM)
[![GitHub top language](https://img.shields.io/github/languages/top/imkiva/KiVM.svg)](https://github.com/imkiva/KiVM)
[![license](https://img.shields.io/github/license/imkiva/KiVM.svg?colorB=000000)](https://github.com/imkiva/KiVM)

Kiva's Java Virtual Machine.

### 特性
- 支持标准的 JNI 接口
- 支持从 jar 文件中加载类
- 完全兼容 OracleJDK/OpenJDK
- 垃圾收集器 [正在进行中]

### 编译
1. 要求
    * 操作系统要求为 Linux 或者 macOS. **Windows 由于我不懂 windows 的相关特性，所以暂不支持.**
    * JDK >= 8
    * CMake >= 3.8

2. 过程
    1. 首先，同步这个项目仓库
    2. 在终端中切换到同步下来的源码目录
    3. 在终端中输入 `cmake . && make`
    4. 等待编译完成

### 鸣谢
* 被 [wind_jvm](https://github.com/wind2412/wind_jvm) 项目启发
* 使用了修改版本的 [libzippp](https://github.com/ctabin/libzippp)
