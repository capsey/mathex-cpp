#include "token.hpp"
#include "mathex"
#include <cmath>
#include <functional>

namespace mathex
{
    Token::Token(const Token &token)
    {
        this->type = token.type;

        switch (this->type)
        {
        case TokenType::Constant:
            this->constant = token.constant;
            break;

        case TokenType::Variable:
            this->variable = token.variable;
            break;

        case TokenType::Function:
            new (&this->function) auto(token.function);
            break;

        case TokenType::BinaryOperator:
            new (&this->binaryOperator.invoke) auto(token.binaryOperator.invoke);
            this->binaryOperator.precedence = token.binaryOperator.precedence;
            this->binaryOperator.leftAssociative = token.binaryOperator.leftAssociative;
            break;

        case TokenType::UnaryOperator:
            new (&this->unaryOperator) auto(token.unaryOperator);
            break;

        default:
            break;
        }
    }

    Token::Token(TokenType emptyType) : type(emptyType) {}
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

        default:
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
