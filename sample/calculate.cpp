#include <cmath>
#include <iostream>
#include <mathex>
#include <numeric>

double x = 3;
double y = 5;
double z = 8;

int main()
{
    // Create a configuration with default flags
    mathex::Config config;

    // Add a constant variable and a function to the configuration
    config.addVariable("x", x);
    config.addVariable("y", y);
    config.addVariable("z", z);

    config.addConstant("e", 2.71);
    config.addConstant("pi", 3.14);

    config.addFunction("sum", [](double args[], int num_args, double &result) -> mathex::Error
                       {
        if (num_args <= 1)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = std::accumulate(args, args + num_args, 0.0);
        return mathex::Success; });

    config.addFunction("abs", [](double args[], int num_args, double &result) -> mathex::Error
                       {
        if (num_args != 1)
        {
            return mathex::Error::IncorrectArgsNum;
        }

        result = fabs(args[0]);
        return mathex::Success; });

    // Evaluate expressions using the configuration
    double result;
    mathex::Error error = config.evaluate("2 * sum(2pi, -abs(x), y + 1, z / 2)", result);

    if (error == mathex::Success)
    {
        std::cout << "Result: " << result << std::endl;
    }
    else
    {
        std::cerr << "Evaluation Error: " << static_cast<int>(error) << std::endl;
    }

    return static_cast<int>(error);
}
