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
        Token(const Token &token);                                                  // Copy
        Token(TokenType emptyType);                                                 // Empty token
        Token(double constant);                                                     // Constant
        Token(const double *variable);                                              // Variable
        Token(Function function);                                                   // Function
        Token(BinaryOperator binaryOperator, int precedence, bool leftAssociative); // Binary operator
        Token(UnaryOperator unaryOperator);                                         // Unary operator
        ~Token();

        TokenType type;
        union Data
        {
            Data();                                                                    // Empty token
            Data(double constant);                                                     // Constant
            Data(const double *variable);                                              // Variable
            Data(Function function);                                                   // Function
            Data(BinaryOperator binaryOperator, int precedence, bool leftAssociative); // Binary operator
            Data(UnaryOperator unaryOperator);                                         // Unary operator
            ~Data();

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
        } data;
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
