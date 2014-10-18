#ifndef EBNF_TOKEN_HPP_INCLUDED
#define EBNF_TOKEN_HPP_INCLUDED

#include <iostream>
#include <string>
#include <utility>

/*
    Structure representing a token for the EBNF parser; not general purpose

    Avoids string usage when possible by representing most operators as
    different types.
*/
struct ebnf_token
{
    // One type for each possible type of operator in EBNF, plus
    // EOI for end of input and Invalid for god knows what
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

// Support stream formatting
std::ostream& operator<<(std::ostream& s, const ebnf_token& t)
{
    return s << '(' << t.type << ", " << '"' << t.str << "\")";
}

#endif // EBNF_TOKEN_HPP_INCLUDED
