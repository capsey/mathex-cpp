# Mathematical Expressions for C++

[![tests status](https://github.com/capsey/mathex-cpp/actions/workflows/tests.yml/badge.svg)](https://github.com/capsey/mathex-cpp/actions/workflows/tests.yml)

- [Mathematical Expressions for C++](#mathematical-expressions-for-c)
  - [What is Mathex?](#what-is-mathex)
  - [How to use?](#how-to-use)
  - [Building from source](#building-from-source)

## What is Mathex?

Mathematical Expressions (or Mathex for short) is a library for C++ that evaluates mathematical expressions from strings at runtime according to [Mathex Specification](https://github.com/capsey/mathex). It aims to provide fast, easy, customizable and, most importantly, safe evaluation with no dependencies.

The library is completely C++11 standard compliant.

## How to use?

Using Mathex is super easy - just include one header, initialize and evaluate. That's it.

```c
#include <iostream>
#include <mathex>
#include <string>

int main()
{
    // Use `Config` class and `DefaultFlags` to get default settings.
    // For what settings are available, check out documentation.
    mathex::Config config(mathex::DefaultFlags);

    // Config class contains your settings along with custom
    // variables and functions you inserted.
    double x = 1.5;
    config.addVariable("x", x);

    // These variables and functions are then available for users
    // to use in expressions.
    std::string input = "2x + 5";
    double result;

    // Mathex writes result of evaluation into the reference you provide.
    // Here we pass a reference to a `result` variable we declared above.
    if (config.evaluate(input, result) == mathex::Success)
    {
        std::cout << input << " is " << result << std::endl; // Outputs `2x + 5 is 8`
    }
}
```

Don't forget to link Mathex when you compile your program:

```shell
c++ program.c -lmathex++
```

## Building from source

To build the library, you need to clone the repository using Git and build the binary using GNU Make:

```shell
git clone https://github.com/capsey/mathex-c.git
cd ./mathex-c
make # to specify compiler: CXX=clang++ make
```

It will use default C++ compiler on your system (`c++`). If you want to use specific compiler, export environment variable `CXX` with your desired compiler before running `make`.

After compilation, library binary will be in `bin` directory. The header file is located in `include` directory.
