#include <cmath>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <mathex>

mathex::Config *config = nullptr;
double result;

void suite_setup(void)
{
    config = new mathex::Config();
}

void suite_teardown(void)
{
    delete config;
    config = nullptr;
}

TestSuite(mx_evaluate, .init = suite_setup, .fini = suite_teardown);

Test(mx_config, addVariable)
{
    double x = 5;
    double y = 3;

    cr_assert(config->addVariable("x", x) == mathex::Success, "successfully inserted first variable");
    cr_assert(config->addVariable("y", y) == mathex::Success, "successfully inserted second variable");
    cr_assert(config->addVariable("y", y) == mathex::Error::AlreadyDefined, "cannot redefine a variable");
    cr_assert(config->addVariable("رطانة", x) == mathex::Error::IllegalName, "did not accept id with illegal characters");

    cr_assert(config->evaluate("x + y", result) == mathex::Success, "variables used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 8, 4), "calculations with variables are correct");

    x = 3;
    y = 10;

    cr_assert(config->evaluate("x + y", result) == mathex::Success, "changing value of a variable changes evaluated value");
    cr_assert(ieee_ulp_eq(dbl, result, 13, 4), "calculations with variables are correct");

    cr_assert(config->remove("x") == mathex::Success);
    cr_assert(config->remove("y") == mathex::Success);
    cr_assert(config->remove("رطانة") == mathex::Error::Undefined);
    cr_assert(config->evaluate("x + y", result) == mathex::Error::Undefined);
}

Test(mx_config, addConstant)
{
    cr_assert(config->addConstant("e", 2.71) == mathex::Success, "successfully inserted first constant");
    cr_assert(config->addConstant("pi", 3.14) == mathex::Success, "successfully inserted second constant");
    cr_assert(config->addConstant("pi", 0) == mathex::Error::AlreadyDefined, "cannot redefine a constant");
    cr_assert(config->addConstant("رطانة", 0) == mathex::Error::IllegalName, "did not accept id with illegal characters");

    cr_assert(config->evaluate("e + pi", result) == mathex::Success, "constants used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 5.85, 4), "calculations with constants are correct");

    cr_assert(config->remove("e") == mathex::Success);
    cr_assert(config->remove("pi") == mathex::Success);
    cr_assert(config->remove("رطانة") == mathex::Error::Undefined);
    cr_assert(config->evaluate("e + pi", result) == mathex::Error::Undefined);
}

Test(mx_config, addFunction)
{
    auto foo_wrapper = [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 0)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = -1.25;
        return mathex::Success;
    };

    auto abs_wrapper = [](double args[], int argc, double &result) -> mathex::Error
    {
        if (argc != 1)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = fabs(args[0]);
        return mathex::Success;
    };

    cr_assert(config->addFunction("foo", foo_wrapper) == mathex::Success, "successfully inserted first function");
    cr_assert(config->addFunction("abs", abs_wrapper) == mathex::Success, "successfully inserted second function");
    cr_assert(config->addFunction("abs", abs_wrapper) == mathex::Error::AlreadyDefined, "cannot redefine a function");
    cr_assert(config->addFunction("رطانة", foo_wrapper) == mathex::Error::IllegalName, "did not accept id with illegal characters");

    cr_assert(config->evaluate("abs(foo()) + 1.12", result) == mathex::Success, "functions used in expressions without errors");
    cr_assert(ieee_ulp_eq(dbl, result, 2.37, 4), "calculations with functions are correct");

    cr_assert(config->remove("foo") == mathex::Success);
    cr_assert(config->remove("abs") == mathex::Success);
    cr_assert(config->remove("رطانة") == mathex::Error::Undefined);
    cr_assert(config->evaluate("abs(foo()) + 1.12", result) == mathex::Error::Undefined);
}