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
            this->data.constant = token.data.constant;
            break;

        case TokenType::Variable:
            this->data.variable = token.data.variable;
            break;

        case TokenType::Function:
            new (&this->data.function) auto(token.data.function);
            break;

        case TokenType::BinaryOperator:
            new (&this->data.binaryOperator.invoke) auto(token.data.binaryOperator.invoke);
            this->data.binaryOperator.precedence = token.data.binaryOperator.precedence;
            this->data.binaryOperator.leftAssociative = token.data.binaryOperator.leftAssociative;
            break;

        case TokenType::UnaryOperator:
            new (&this->data.unaryOperator) auto(token.data.unaryOperator);
            break;

        default:
            break;
        }
    }

    Token::Token(TokenType emptyType) : type(emptyType) {}
    Token::Token(double constant) : type(TokenType::Constant), data(constant) {}
    Token::Token(const double *variable) : type(TokenType::Variable), data(variable) {}
    Token::Token(Function function) : type(TokenType::Function), data(function) {}
    Token::Token(BinaryOperator binaryOperator, int precedence, bool leftAssociative) : type(TokenType::BinaryOperator), data(binaryOperator, precedence, leftAssociative) {}
    Token::Token(UnaryOperator unaryOperator) : type(TokenType::UnaryOperator), data(unaryOperator) {}
    Token::~Token()
    {
        switch (this->type)
        {
        case TokenType::Function:
            this->data.function.~function();
            break;

        case TokenType::BinaryOperator:
            this->data.binaryOperator.invoke.~function();
            break;

        case TokenType::UnaryOperator:
            this->data.unaryOperator.~function();
            break;

        default:
            break;
        }
    }

    Token::Data::Data() {}
    Token::Data::Data(double constant) : constant(constant) {}
    Token::Data::Data(const double *variable) : variable(variable) {}
    Token::Data::Data(Function function) : function(function) {}
    Token::Data::Data(BinaryOperator binaryOperator, int precedence, bool leftAssociative) : binaryOperator({binaryOperator, precedence, leftAssociative}) {}
    Token::Data::Data(UnaryOperator unaryOperator) : unaryOperator(unaryOperator) {}
    Token::Data::~Data() {}

    const Token AddToken(std::plus<double>{}, 2, true);
    const Token SubToken(std::minus<double>{}, 2, true);
    const Token MulToken(std::multiplies<double>{}, 3, true);
    const Token DivToken(std::divides<double>{}, 3, true);

    const Token PowToken(static_cast<double (*)(double, double)>(std::pow), 2, true);
    const Token ModToken(static_cast<double (*)(double, double)>(std::fmod), 2, true);

    const Token PosToken([](double x) -> double { return x; });
    const Token NegToken(std::negate<double>{});
}
