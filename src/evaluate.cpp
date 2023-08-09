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
#include "token.hpp"
#include <cctype>
#include <cmath>
#include <memory>
#include <queue>
#include <stack>

#define OPERAND_EXPECTED (last_token == TokenType::None || last_token == TokenType::LeftParenthesis || last_token == TokenType::Comma || last_token == TokenType::BinaryOperator || last_token == TokenType::UnaryOperator)
#define UNARY_OPERATOR_EXPECTED (last_token == TokenType::None || last_token == TokenType::LeftParenthesis || last_token == TokenType::Comma || last_token == TokenType::UnaryOperator)
#define BINARY_OPERATOR_EXPECTED (last_token == TokenType::Constant || last_token == TokenType::Variable || last_token == TokenType::RightParenthesis)

namespace mathex {
    enum class States {
        INTEGER_PART,  // Integer portion of decimal fraction. (before decimal point)
        FRACTION_PART, // Fractional portion of decimal fraction. (after decimal point)
        EXP_START,     // Separator of mantissa and exponent in scientific notation. (including sign)
        EXP_VALUE,     // Exponent of scientific notation.
    };

    Error Config::evaluate(const std::string &expression, double &result) {
        // https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail

        TokenType last_token = TokenType::None;

        std::stack<Token> ops_stack;
        std::queue<Token> out_queue;
        std::stack<double> res_stack;

        int arg_count = 0;
        std::stack<int> arg_stack;
        std::queue<int> arg_queue;

        for (size_t i = 0; i < expression.length(); i++) {
            if (expression[i] == ' ') {
                continue;
            }

            if (isdigit(expression[i]) || expression[i] == '.') {
                // Two operands in a row are not allowed
                // Operand should only either be first in expression or right after operator
                if (!OPERAND_EXPECTED) {
                    return Error::SyntaxError;
                }

                if (arg_count == 0) {
                    arg_count++;
                }

                double value = 0;
                double decimal_place = 10;
                double exponent = 0;
                bool exponent_sign = true;

                States state = States::INTEGER_PART;
                size_t j;

                for (j = i; j < expression.length(); j++) {
                    switch (state) {
                    case States::INTEGER_PART: {
                        if (isdigit(expression[j])) {
                            value = (value * 10) + (double)(expression[j] - '0');
                            continue;
                        }

                        if (expression[j] == '.') {
                            state = States::FRACTION_PART;
                            continue;
                        }

                        if ((expression[j] == 'e' || expression[j] == 'E') && this->readFlag(Flags::ScientificNotation)) {
                            state = States::EXP_START;
                            continue;
                        }
                    } break;

                    case States::FRACTION_PART: {
                        if (expression[j] == '.') {
                            return Error::SyntaxError;
                        }

                        if (isdigit(expression[j])) {
                            value += (double)(expression[j] - '0') / decimal_place;
                            decimal_place *= 10;
                            continue;
                        }

                        if ((expression[j] == 'e' || expression[j] == 'E') && this->readFlag(Flags::ScientificNotation)) {
                            state = States::EXP_START;
                            continue;
                        }
                    } break;

                    case States::EXP_START: {
                        if (expression[j] == '.') {
                            return Error::SyntaxError;
                        }

                        if (isdigit(expression[j])) {
                            exponent = (exponent * 10) + (double)(expression[j] - '0');
                            state = States::EXP_VALUE;
                            continue;
                        }

                        if (expression[j] == '+' || expression[j] == '-') {
                            exponent_sign = (expression[j] == '+');
                            state = States::EXP_VALUE;
                            continue;
                        }
                    } break;

                    case States::EXP_VALUE: {
                        if (expression[j] == '.') {
                            return Error::SyntaxError;
                        }

                        if (isdigit(expression[j])) {
                            exponent = (exponent * 10) + (double)(expression[j] - '0');
                            state = States::EXP_VALUE;
                            continue;
                        }
                    } break;
                    }

                    // If reached here means number literal has ended
                    break;
                }

                // Cannot have scientific notation separator without specifying exponent
                if (state == States::EXP_START) {
                    j--;
                }

                // ".1" => 0.1 and "1." => 1.0 but "." != 0.0
                if (j - i == 1 && expression[i] == '.') {
                    return Error::SyntaxError;
                }

                if (exponent != 0) {
                    value *= std::pow(exponent_sign ? 10.0 : 0.1, exponent);
                }

                Token token(value);
                out_queue.push(token);

                last_token = TokenType::Constant;
                i = j - 1;
                continue;
            }

            if (isalpha(expression[i]) || expression[i] == '_') {
                if (last_token == TokenType::Constant && this->readFlag(Flags::ImplicitMultiplication)) {
                    // Implicit multiplication
                    while (!ops_stack.empty()) {
                        if (ops_stack.top().type == TokenType::BinaryOperator) {
                            if (!(ops_stack.top().data.binaryOperator.precedence > MulToken.data.binaryOperator.precedence || (ops_stack.top().data.binaryOperator.precedence == MulToken.data.binaryOperator.precedence && MulToken.data.binaryOperator.leftAssociative))) {
                                break;
                            }
                        } else if (ops_stack.top().type != TokenType::UnaryOperator) {
                            // Precedence of unary operator is always greater than of any binary operator
                            break;
                        }

                        out_queue.push(std::move(ops_stack.top()));
                        ops_stack.pop();
                    }

                    ops_stack.push(MulToken);
                } else {
                    // Two operands in a row are not allowed
                    // Operand should only either be first in expression or right after operator
                    if (!OPERAND_EXPECTED) {
                        return Error::SyntaxError;
                    }
                }

                if (arg_count == 0) {
                    arg_count++;
                }

                size_t j;

                for (j = i + 1; j < expression.length(); j++) {
                    if (!isalnum(expression[j]) && expression[j] != '_') {
                        break;
                    }
                }

                auto fetched = this->m_Tokens.find(expression.substr(i, j - i));

                if (fetched == this->m_Tokens.end()) {
                    return Error::Undefined;
                }

                switch (fetched->second->type) {
                case TokenType::Function: {
                    if (expression[j] != '(') {
                        return Error::SyntaxError;
                    }
                    ops_stack.push(*fetched->second);
                } break;

                case TokenType::Variable:
                case TokenType::Constant: {
                    out_queue.push(*fetched->second);
                } break;

                default: {
                    // This clause should not be possible, since you can
                    // only insert variable or function into the config.
                } break;
                }

                last_token = fetched->second->type;
                i = j - 1;
                continue;
            }

            const Token *token = nullptr;

            if (expression[i] == '+') {
                if (this->readFlag(Flags::Addition) && BINARY_OPERATOR_EXPECTED) {
                    // Used as binary operator
                    token = &AddToken;
                } else if (this->readFlag(Flags::Identity) && UNARY_OPERATOR_EXPECTED) {
                    // Used as unary operator
                    token = &PosToken;
                } else {
                    return Error::SyntaxError;
                }
            } else if (expression[i] == '-') {
                if (this->readFlag(Flags::Substraction) && BINARY_OPERATOR_EXPECTED) {
                    // Used as binary operator
                    token = &SubToken;
                } else if (this->readFlag(Flags::Negation) && UNARY_OPERATOR_EXPECTED) {
                    // Used as unary operator
                    token = &NegToken;
                } else {
                    return Error::SyntaxError;
                }
            } else if (expression[i] == '*' && this->readFlag(Flags::Multiplication)) {
                // There should always be an operand on the left hand side of the operator
                if (!BINARY_OPERATOR_EXPECTED) {
                    return Error::SyntaxError;
                }

                token = &MulToken;
            } else if (expression[i] == '/' && this->readFlag(Flags::Division)) {
                // There should always be an operand on the left hand side of the operator
                if (!BINARY_OPERATOR_EXPECTED) {
                    return Error::SyntaxError;
                }

                token = &DivToken;
            } else if (expression[i] == '^' && this->readFlag(Flags::Exponentiation)) {
                // There should always be an operand on the left hand side of the operator
                if (!BINARY_OPERATOR_EXPECTED) {
                    return Error::SyntaxError;
                }

                token = &PowToken;
            } else if (expression[i] == '%' && this->readFlag(Flags::Modulus)) {
                // There should always be an operand on the left hand side of the operator
                if (!BINARY_OPERATOR_EXPECTED) {
                    return Error::SyntaxError;
                }

                token = &ModToken;
            }

            if (token != nullptr) {
                if (token->type == TokenType::BinaryOperator) {
                    while (!ops_stack.empty()) {
                        if (ops_stack.top().type == TokenType::BinaryOperator) {
                            if (!(ops_stack.top().data.binaryOperator.precedence > token->data.binaryOperator.precedence || (ops_stack.top().data.binaryOperator.precedence == token->data.binaryOperator.precedence && token->data.binaryOperator.leftAssociative))) {
                                break;
                            }
                        } else if (ops_stack.top().type != TokenType::UnaryOperator) {
                            // Precedence of unary operator is always greater than of any binary operator
                            break;
                        }

                        out_queue.push(std::move(ops_stack.top()));
                        ops_stack.pop();
                    }
                }

                ops_stack.push(*token);
                last_token = token->type;
                continue;
            }

            if (expression[i] == '(') {
                if (last_token == TokenType::Function) {
                    arg_stack.push(arg_count);
                    arg_count = 0;
                } else {
                    // Two operands in a row are not allowed
                    // Operand should only either be first in expression or right after operator
                    if (!OPERAND_EXPECTED) {
                        return Error::SyntaxError;
                    }

                    if (arg_count == 0) {
                        arg_count++;
                    }
                }

                Token token(TokenType::LeftParenthesis);
                ops_stack.push(token);

                last_token = TokenType::LeftParenthesis;
                continue;
            }

            if (expression[i] == ')') {
                // Empty expressions are not allowed
                if (last_token == TokenType::None || last_token == TokenType::Comma) {
                    return Error::SyntaxError;
                }

                if (last_token != TokenType::LeftParenthesis) {
                    if (ops_stack.empty()) {
                        // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                        if (!this->readFlag(Flags::ImplicitParentheses)) {
                            return Error::SyntaxError;
                        }

                        continue;
                    }

                    while (ops_stack.top().type != TokenType::LeftParenthesis) {
                        out_queue.push(std::move(ops_stack.top()));
                        ops_stack.pop();

                        if (ops_stack.empty()) {
                            // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                            if (!this->readFlag(Flags::ImplicitParentheses)) {
                                return Error::SyntaxError;
                            }

                            break;
                        }
                    }
                }

                if (!ops_stack.empty()) {
                    ops_stack.pop(); // Discard left parenthesis

                    if (!ops_stack.empty() && ops_stack.top().type == TokenType::Function) {
                        out_queue.push(std::move(ops_stack.top()));
                        ops_stack.pop();

                        arg_queue.push(arg_count);
                        arg_count = arg_stack.top();
                        arg_stack.pop();
                    } else if (last_token == TokenType::LeftParenthesis) {
                        // Empty parentheses are not allowed, unless for zero-argument functions
                        return Error::SyntaxError;
                    }
                }

                last_token = TokenType::RightParenthesis;
                continue;
            }

            if (expression[i] == ',') {
                // Previous argument has to be non-empty
                if (!BINARY_OPERATOR_EXPECTED) {
                    return Error::SyntaxError;
                }

                // Comma is only valid inside function parentheses
                if (arg_stack.empty()) {
                    return Error::SyntaxError;
                }

                if (ops_stack.empty()) {
                    // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                    if (!this->readFlag(Flags::ImplicitParentheses)) {
                        return Error::SyntaxError;
                    }

                    continue;
                }

                while (ops_stack.top().type != TokenType::LeftParenthesis) {
                    out_queue.push(std::move(ops_stack.top()));
                    ops_stack.pop();

                    if (ops_stack.empty()) {
                        // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                        if (!this->readFlag(Flags::ImplicitParentheses)) {
                            return Error::SyntaxError;
                        }

                        break;
                    }
                }

                arg_count++;
                last_token = TokenType::Comma;
                continue;
            }

            // Any character that was not captured by previous checks is considered invalid
            return Error::SyntaxError;
        }

        // Expression cannot end if operand is expected next
        if (OPERAND_EXPECTED) {
            return Error::SyntaxError;
        }

        while (!ops_stack.empty()) {
            if (ops_stack.top().type == TokenType::LeftParenthesis) {
                // Mismatched parenthesis (ignore if implicit parentheses are enabled)
                if (!this->readFlag(Flags::ImplicitParentheses)) {
                    return Error::SyntaxError;
                }

                continue;
            }

            if (ops_stack.top().type == TokenType::Function) {
                // Implicit parentheses for zero argument functions are not allowed
                if (arg_count == 0) {
                    return Error::SyntaxError;
                }

                arg_queue.push(arg_count);
                arg_count = arg_stack.top();
                arg_stack.pop();
            }

            out_queue.push(std::move(ops_stack.top()));
            ops_stack.pop();
        }

        while (!out_queue.empty()) {
            switch (out_queue.front().type) {
            case TokenType::Constant: {
                res_stack.push(out_queue.front().data.constant);
            } break;

            case TokenType::Variable: {
                res_stack.push(*out_queue.front().data.variable);
            } break;

            case TokenType::BinaryOperator: {
                double b = res_stack.top();
                res_stack.pop();
                double a = res_stack.top();
                res_stack.pop();

                res_stack.push(out_queue.front().data.binaryOperator(a, b));
            } break;

            case TokenType::UnaryOperator: {
                double x = res_stack.top();
                res_stack.pop();

                res_stack.push(out_queue.front().data.unaryOperator(x));
            } break;

            case TokenType::Function: {
                int args_num = arg_queue.front();
                arg_queue.pop();

                double func_result;
                Error error;

                if (args_num > 0) {
                    std::unique_ptr<double[]> args(new double[args_num]);

                    for (int i = 0; i < args_num; i++) {
                        args[(size_t)args_num - (size_t)i - 1] = res_stack.top();
                        res_stack.pop();
                    }

                    error = out_queue.front().data.function(args.get(), args_num, func_result);
                } else {
                    error = out_queue.front().data.function(nullptr, 0, func_result);
                }

                if (error != Error::Success) {
                    return error;
                }

                res_stack.push(func_result);
            } break;

            default: {
            } break;
            }

            out_queue.pop();
        }

        result = res_stack.top();
        res_stack.pop();

        // Exactly one value has to be left in results stack
        return res_stack.empty() ? Error::Success : Error::SyntaxError;
    }
}
