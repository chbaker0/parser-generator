#ifndef EBNF_TOKEN_HPP_INCLUDED
#define EBNF_TOKEN_HPP_INCLUDED

#include <iostream>
#include <string>
#include <utility>

struct ebnf_token
{
    enum
    {
        Identifier = 0,
        TermString,
        OperatorDef,
        OperatorConcat,
        OperatorEnd,
        OperatorAlt,
        OperatorLOpt,
        OperatorROpt,
        OperatorLRep,
        OperatorRRep,
        OperatorLParen,
        OperatorRParen,
        OperatorExcept,
        EOI,
        Invalid
    };

    int type;
    std::string str;

    ebnf_token(): type(Invalid) {}
    ebnf_token(int type_in): type(type_in) {}
    ebnf_token(int type_in, std::string str_in): type(type_in), str(std::move(str_in)) {}
};

std::ostream& operator<<(std::ostream& s, const ebnf_token& t)
{
    return s << '(' << t.type << ", " << '"' << t.str << "\")";
}

#endif // EBNF_TOKEN_HPP_INCLUDED
