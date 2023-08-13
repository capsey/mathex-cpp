/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <cmath>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <mathex>
#include <stdexcept>

mathex::Config *config = nullptr;
double result;

void suite_setup(void) {
    config = new mathex::Config();
}

void suite_teardown(void) {
    delete config;
    config = nullptr;
}

TestSuite(config, .init = suite_setup, .fini = suite_teardown);

Test(config, addVariable) {
    double x = 5;
    double y = 3;

    cr_assert_none_throw(config->addVariable("x", x));
    cr_assert_none_throw(config->addVariable("y", y));
    cr_assert_throw(config->addVariable("y", y), mathex::AlreadyDefined);
    cr_assert_throw(config->addVariable("رطانة", x), std::invalid_argument);

    cr_assert(config->evaluate("x + y", result) == mathex::Success, "variables used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 8, 4), "calculations with variables are correct");

    x = 3;
    y = 10;

    cr_assert(config->evaluate("x + y", result) == mathex::Success, "changing value of a variable changes evaluated value");
    cr_assert(ieee_ulp_eq(dbl, result, 13, 4), "calculations with variables are correct");

    cr_assert(config->remove("x"));
    cr_assert(config->remove("y"));
    cr_assert_not(config->remove("رطانة"));
    cr_assert(config->evaluate("x + y", result) == mathex::Error::Undefined);
}

Test(config, addConstant) {
    cr_assert_none_throw(config->addConstant("e", 2.71));
    cr_assert_none_throw(config->addConstant("pi", 3.14));
    cr_assert_throw(config->addConstant("pi", 0), mathex::AlreadyDefined);
    cr_assert_throw(config->addConstant("رطانة", 0), std::invalid_argument);

    cr_assert(config->evaluate("e + pi", result) == mathex::Success, "constants used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 5.85, 4), "calculations with constants are correct");

    cr_assert(config->remove("e"));
    cr_assert(config->remove("pi"));
    cr_assert_not(config->remove("رطانة"));
    cr_assert(config->evaluate("e + pi", result) == mathex::Error::Undefined);
}

Test(config, addFunction) {
    auto foo_wrapper = [](double args[], int argc, double &result) -> mathex::Error {
        if (argc != 0) {
            return mathex::Error::IncorrectArgsNum;
        }

        result = -1.25;
        return mathex::Success;
    };

    auto abs_wrapper = [](double args[], int argc, double &result) -> mathex::Error {
        if (argc != 1) {
            return mathex::Error::IncorrectArgsNum;
        }

        result = fabs(args[0]);
        return mathex::Success;
    };

    cr_assert_none_throw(config->addFunction("foo", foo_wrapper));
    cr_assert_none_throw(config->addFunction("abs", abs_wrapper));
    cr_assert_throw(config->addFunction("abs", abs_wrapper), mathex::AlreadyDefined);
    cr_assert_throw(config->addFunction("رطانة", foo_wrapper), std::invalid_argument);

    cr_assert(config->evaluate("abs(foo()) + 1.12", result) == mathex::Success, "functions used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 2.37, 4), "calculations with functions are correct");

    cr_assert(config->remove("foo"));
    cr_assert(config->remove("abs"));
    cr_assert_not(config->remove("رطانة"));
    cr_assert(config->evaluate("abs(foo()) + 1.12", result) == mathex::Error::Undefined);
}
