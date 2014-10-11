#ifndef EBNF_PARSER_HPP_INCLUDED
#define EBNF_PARSER_HPP_INCLUDED

#include <exception>
#include <utility>

#include "ebnf_token.hpp"
#include "ebnf_scanner.hpp"

template <typename InputIterator>
class ebnf_parser
{
public:
    ebnf_parser(InputIterator first, InputIterator last): scanner(first, last), lookahead(std::move(scanner.lookahead())) {}

    void run()
    {
        parse_ebnf();
    }

    class parse_error : public std::exception
    {
        const char* what() const noexcept override
        {
            return "Parse error.";
        }
    };

protected:
    ebnf_scanner<InputIterator> scanner;
    ebnf_token lookahead;

    void advance()
    {
        scanner.advance();
        lookahead = std::move(scanner.lookahead());
    }

    void parse_group()
    {
        if(lookahead.type == ebnf_token::Identifier)
        {
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLOpt)
        {
            advance();
            parse_exp();
            if(lookahead.type != ebnf_token::OperatorROpt)
                throw parse_error();
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLParen)
        {
            advance();
            parse_exp();
            if(lookahead.type != ebnf_token::OperatorRParen)
                throw parse_error();
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLRep)
        {
            advance();
            parse_exp();
            if(lookahead.type != ebnf_token::OperatorRRep)
                throw parse_error();
            advance();
        }
    }

    void parse_concat()
    {
        parse_group();
        while(lookahead.type == ebnf_token::Identifier ||
              lookahead.type == ebnf_token::OperatorLOpt ||
              lookahead.type == ebnf_token::OperatorLParen ||
              lookahead.type == ebnf_token::OperatorLRep)
        {
            parse_group();
        }
    }

    void parse_exp()
    {
        parse_concat();
        while(lookahead.type == ebnf_token::OperatorAlt)
        {
            advance();
            parse_concat();
        }
    }

    void parse_rule()
    {
        advance();
        if(lookahead.type != ebnf_token::OperatorDef)
            throw parse_error();
        advance();
        parse_exp();
    }

    void parse_ebnf()
    {
        if(lookahead.type == ebnf_token::Identifier)
        {
            parse_rule();
            if(lookahead.type != ebnf_token::OperatorEnd)
                throw parse_error();
            advance();
        }
        else
            throw parse_error();
    }
};

template <typename InputIterator>
ebnf_parser<InputIterator> make_ebnf_parser(InputIterator first, InputIterator last)
{
    return ebnf_parser<InputIterator>(first, last);
}

#endif // EBNF_PARSER_HPP_INCLUDED
