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

mathex::Config *config = nullptr;
double result;

const double x = 5;
const double y = 3;
const double z = 6;
const double pi = 3.14;

void suite_setup(void)
{
    config = new mathex::Config(mathex::DefaultFlags + mathex::Flags::Exponentiation);
    config->addConstant("x", x);
    config->addConstant("y", y);
    config->addConstant("z", z);
    config->addConstant("pi", pi);

    config->addFunction("foo", [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 2)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = args[0];
        return mathex::Success;
    });

    config->addFunction("bar", [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 0)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = 5.43;
        return mathex::Success;
    });

    config->addFunction("f", [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 1)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = args[0] * args[0];
        return mathex::Success;
    });

    config->addFunction("g", [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 1)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = 3 * args[0] - 1;
        return mathex::Success;
    });

    config->addFunction("h", [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 2)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = args[0] * args[0] + args[1];
        return mathex::Success;
    });
}

void suite_teardown(void)
{
    delete config;
    config = nullptr;
}

TestSuite(evaluate, .init = suite_setup, .fini = suite_teardown);

Test(evaluate, simple_expressions)
{
    cr_expect(config->evaluate("5 + 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 8, 4));

    cr_expect(config->evaluate("10 - 4", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 6, 4));

    cr_expect(config->evaluate("2 * 6", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 12, 4));

    cr_expect(config->evaluate("15 / 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 5, 4));

    cr_expect(config->evaluate("4 + 6 * 2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 16, 4));

    cr_expect(config->evaluate("(7 + 3) * 4", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 40, 4));

    cr_expect(config->evaluate("8 + 12 / 4 - 3 * 2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 5, 4));

    cr_expect(config->evaluate("10 / 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 10.0 / 3, 4));

    cr_expect(config->evaluate("-5 + 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, -2, 4));

    cr_expect(config->evaluate("1000000 * 1000000", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 1000000000000, 4));
}

Test(evaluate, erroneous_expressions)
{
    cr_expect(config->evaluate("5 5", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("() + 3", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("8 +", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("/ 5", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("* 7 + 2", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("* 7 + 2", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("4 + 6 -", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("3 + * 5", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("4 + 6 + * 2", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("5 + abc - 3", result) == mathex::Error::Undefined);
    cr_expect(config->evaluate("sin(90)", result) == mathex::Error::Undefined);
}

Test(evaluate, number_format)
{
    cr_expect(config->evaluate("30", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 30, 4));

    cr_expect(config->evaluate("2.5", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 2.5, 4));

    cr_expect(config->evaluate(".1", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 0.1, 4));

    cr_expect(config->evaluate("1.", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 1.0, 4));

    cr_expect(config->evaluate(".", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("1..4", result) == mathex::Error::SyntaxError);
    cr_expect(config->evaluate("2.6.", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("5e4", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 50000, 4));

    cr_expect(config->evaluate("5.3e4", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 53000, 4));

    cr_expect(config->evaluate("2.4e-2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 0.024, 4));

    cr_expect(config->evaluate("2.4e+2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 240, 4));

    cr_expect(config->evaluate("2.6e", result) == mathex::Error::Undefined);
    cr_expect(config->evaluate("3.4ee6", result) == mathex::Error::Undefined);
    cr_expect(config->evaluate("1.6e4.3", result) == mathex::Error::SyntaxError);
}

Test(evaluate, variables)
{
    cr_expect(config->evaluate("x + 5", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 10, 4));

    cr_expect(config->evaluate("a * 2", result) == mathex::Error::Undefined);

    cr_expect(config->evaluate("x + y - z", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(config->evaluate("-x + 7", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(config->evaluate("2 * pi * x", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 31.4, 4));

    cr_expect(config->evaluate("x^3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 125, 4));

    cr_expect(config->evaluate("x + x - x / 2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 7.5, 4));

    cr_expect(config->evaluate("x^2 + y * z - z / y", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 41, 4));

    cr_expect(config->evaluate("2^x + 3 * x - 5", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 42, 4));

    cr_expect(config->evaluate("x + a", result) == mathex::Error::Undefined);
}

Test(evaluate, changing_variables)
{
    double var;
    config->addVariable("var", var);

    var = 3;
    cr_expect(config->evaluate("var + 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 6, 4));

    var = 5;
    cr_expect(config->evaluate("var + 3", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 8, 4));

    config->remove("var");
}

Test(evaluate, functions)
{
    cr_expect(config->evaluate("foo(2, 5)", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 2, 4));

    cr_expect(config->evaluate("f(x) + 5", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 30, 4));

    cr_expect(config->evaluate("2 * g(y) - f(x)", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, -9, 4));

    cr_expect(config->evaluate("h(x, y) + z", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 34, 4));

    cr_expect(config->evaluate("bar() + 2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 7.43, 4));

    cr_expect(config->evaluate("f(x) + d(x)", result) == mathex::Error::Undefined);
    cr_expect(config->evaluate("f()", result) == mathex::Error::IncorrectArgsNum);
    cr_expect(config->evaluate("f(3, )", result) == mathex::Error::SyntaxError);

    cr_expect(config->evaluate("f(x) + f(y) - f(z) / 2", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 16, 4));

    cr_expect(config->evaluate("3^2 + f(2x - g(3^1))", result) == mathex::Success);
    cr_expect(ieee_ulp_eq(dbl, result, 13, 4));
}
