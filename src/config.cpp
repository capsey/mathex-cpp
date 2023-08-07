#include "mathex"
#include "token.hpp"
#include <algorithm>
#include <cctype>
#include <memory>

namespace mathex
{
    Config::Config(Flags flags = DefaultFlags) : m_Flags(flags) {}

    Config::~Config() {}

    Error Config::addVariable(const std::string &name, const double &value)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(&value));
    }

    Error Config::addConstant(const std::string &name, double value)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(value));
    }

    Error Config::addFunction(const std::string &name, Function apply)
    {
        if (name.empty() || isdigit(name[0]) || !std::all_of(name.begin(), name.end(), [](const char &c) { return isalnum(c) || c == '_'; }))
        {
            return Error::IllegalName;
        }

        this->m_Tokens[name] = std::unique_ptr<Token>(new Token(apply));
    }

    Error Config::remove(const std::string &name)
    {
        if (this->m_Tokens.erase(name) == 0)
        {
            return Error::Undefined;
        }

        return Error::Success;
    }
}
