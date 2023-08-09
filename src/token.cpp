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

#include "token.hpp"
#include "mathex"
#include <cmath>
#include <functional>

namespace mathex {
    Token::Token(const Token &token) {
        this->type = token.type;

        switch (this->type) {
        case TokenType::Constant: {
            this->data.constant = token.data.constant;
        } break;

        case TokenType::Variable: {
            this->data.variable = token.data.variable;
        } break;

        case TokenType::Function: {
            new (&this->data.function) auto(token.data.function);
        } break;

        case TokenType::BinaryOperator: {
            new (&this->data.binaryOperator.invoke) auto(token.data.binaryOperator.invoke);
            this->data.binaryOperator.precedence = token.data.binaryOperator.precedence;
            this->data.binaryOperator.leftAssociative = token.data.binaryOperator.leftAssociative;
        } break;

        case TokenType::UnaryOperator: {
            new (&this->data.unaryOperator) auto(token.data.unaryOperator);
        } break;

        default: {
        } break;
        }
    }

    Token::Token(TokenType emptyType) : type(emptyType) {}
    Token::Token(double constant) : type(TokenType::Constant), data(constant) {}
    Token::Token(const double *variable) : type(TokenType::Variable), data(variable) {}
    Token::Token(Function function) : type(TokenType::Function), data(function) {}
    Token::Token(BinaryOperator binaryOperator, int precedence, bool leftAssociative) : type(TokenType::BinaryOperator), data(binaryOperator, precedence, leftAssociative) {}
    Token::Token(UnaryOperator unaryOperator) : type(TokenType::UnaryOperator), data(unaryOperator) {}
    Token::~Token() {
        switch (this->type) {
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
