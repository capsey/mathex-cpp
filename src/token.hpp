#include "mathex"
#include <functional>

namespace mathex
{
    using BinaryOperator = std::function<double(double, double)>;
    using UnaryOperator = std::function<double(double)>;

    enum class TokenType
    {
        None = 0,
        LeftParenthesis,
        RightParenthesis,
        Comma,
        Constant,
        Variable,
        Function,
        BinaryOperator,
        UnaryOperator,
    };

    class Token
    {
    public:
        Token(double constant);
        Token(const double *variable);
        Token(Function function);
        Token(BinaryOperator binaryOperator, int precedence, bool leftAssociative);
        Token(UnaryOperator unaryOperator);
        ~Token();

        TokenType type;
        union
        {
            double constant;
            const double *variable;
            Function function;
            struct
            {
                BinaryOperator invoke;
                int precedence;
                bool leftAssociative;

                double operator()(double a, double b)
                {
                    return this->invoke(a, b);
                }
            } binaryOperator;
            UnaryOperator unaryOperator;
        };
    };

    extern const Token AddToken; // Addition operator.
    extern const Token SubToken; // Substraction operator.
    extern const Token MulToken; // Multiplication operator.
    extern const Token DivToken; // Division operator.

    extern const Token PowToken; // Exponentiation operator.
    extern const Token ModToken; // Modulus operator.

    extern const Token PosToken; // Unary identity operator.
    extern const Token NegToken; // Unary negation operator.
}
