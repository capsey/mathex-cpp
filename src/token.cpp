#include "mathex"
#include "token.hpp"
#include <functional>
#include <cmath>

namespace mathex
{
    Token::Token(double constant) : type(TokenType::Constant), constant(constant) {}
    Token::Token(const double *variable) : type(TokenType::Variable), variable(variable) {}
    Token::Token(Function function) : type(TokenType::Function), function(function) {}
    Token::Token(BinaryOperator binaryOperator, int precedence, bool leftAssociative) : type(TokenType::BinaryOperator), binaryOperator({binaryOperator, precedence, leftAssociative}) {}
    Token::Token(UnaryOperator unaryOperator) : type(TokenType::UnaryOperator), unaryOperator(unaryOperator) {}

    Token::~Token()
    {
        switch (this->type)
        {
        case TokenType::Function:
            this->function.~function();
            break;

        case TokenType::BinaryOperator:
            this->binaryOperator.invoke.~function();
            break;

        case TokenType::UnaryOperator:
            this->unaryOperator.~function();
            break;
        }
    }

    const Token AddToken(std::plus<double>{}, 2, true);
    const Token SubToken(std::minus<double>{}, 2, true);
    const Token MulToken(std::multiplies<double>{}, 3, true);
    const Token DivToken(std::divides<double>{}, 3, true);

    const Token PowToken(static_cast<double (*)(double, double)>(std::pow), 2, true);
    const Token ModToken(static_cast<double (*)(double, double)>(std::fmod), 2, true);

    const Token PosToken([](double x) -> double { return x; });
    const Token NegToken(std::negate<double>{});
}
