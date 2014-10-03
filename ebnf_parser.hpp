#ifndef EBNF_PARSER_HPP_INCLUDED
#define EBNF_PARSER_HPP_INCLUDED

#include <utility>

#include "ebnf_token.hpp"
#include "ebnf_scanner.hpp"

template <typename InputIterator>
class ebnf_parser
{
    ebnf_parser(InputIterator first, InputIterator last): scanner(first, last), lookahead(std::move(scanner.lookahead())) {}

protected:
    ebnf_scanner<InputIterator> scanner;
    ebnf_token<InputIterator>& lookahead;

    void advance()
    {
        scanner.advance();
        lookahead = std::move(scanner.lookahead());
    }
};

template <typename InputIterator>
ebnf_parser<InputIterator> make_ebnf_parser(InputIterator first, InputIterator last)
{
    return ebnf_parser<InputIterator>(first, last);
}

#endif // EBNF_PARSER_HPP_INCLUDED
