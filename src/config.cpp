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
#include <algorithm>
#include <cctype>
#include <memory>
#include <type_traits>

namespace mathex
{
    Config::Config(Flags flags /* = DefaultFlags */) : m_Flags(flags) {}

    Config::~Config() {}

    Error Config::addVariable(const std::string &name, const double &value)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        if (this->m_Tokens.find(name) != this->m_Tokens.end())
        {
            return Error::AlreadyDefined;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(&value));
        return Error::Success;
    }

    Error Config::addConstant(const std::string &name, double value)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        if (this->m_Tokens.find(name) != this->m_Tokens.end())
        {
            return Error::AlreadyDefined;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(value));
        return Error::Success;
    }

    Error Config::addFunction(const std::string &name, Function apply)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        if (this->m_Tokens.find(name) != this->m_Tokens.end())
        {
            return Error::AlreadyDefined;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(apply));
        return Error::Success;
    }

    Error Config::remove(const std::string &name)
    {
        if (this->m_Tokens.erase(name) == 0)
        {
            return Error::Undefined;
        }

        return Error::Success;
    }

    bool Config::readFlag(Flags flag)
    {
        return static_cast<Flags>(static_cast<std::underlying_type<Flags>::type>(this->m_Flags) & static_cast<std::underlying_type<Flags>::type>(flag)) != Flags::None;
    }
}
