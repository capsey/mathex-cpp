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

#include "mathex"
#include <functional>

namespace mathex {
    using BinaryOperator = std::function<double(double, double)>;
    using UnaryOperator = std::function<double(double)>;

    enum class TokenType {
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

    class Token {
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
        union Data {
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
            struct {
                BinaryOperator invoke;
                int precedence;
                bool leftAssociative;

                double operator()(double a, double b) {
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
