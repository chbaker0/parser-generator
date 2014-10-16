#ifndef EBNF_PARSER_HPP_INCLUDED
#define EBNF_PARSER_HPP_INCLUDED

#include <exception>
#include <string>
#include <unordered_map>
#include <utility>

#include "ebnf_token.hpp"
#include "ebnf_scanner.hpp"
#include "grammar_tree.hpp"

/*
    EBNF grammar parser

    Utilizes ebnf_scanner class for lexical analysis.
    Parses using LL(1) recursive descent for O(n) parsing time.
*/

template <typename InputIterator>
class ebnf_parser
{
public:
    ebnf_parser(InputIterator first, InputIterator last): scanner(first, last), lookahead(std::move(scanner.lookahead())) {}

    using grammar = std::unordered_map<std::string, grammar_tree>;

    grammar run()
    {
        grammar g;
        parse_ebnf(g);
        return g;
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

    grammar_tree parse_group()
    {
        grammar_tree t;

        if(lookahead.type == ebnf_token::Identifier)
        {
            t = grammar_identifier{lookahead.str};
            advance();
        }
        else if(lookahead.type == ebnf_token::TermString)
        {
            t = grammar_terminal{lookahead.str};
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLOpt)
        {
            advance();
            t = grammar_optional{parse_exp()};
            if(lookahead.type != ebnf_token::OperatorROpt)
                throw parse_error();
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLParen)
        {
            advance();
            t = parse_exp();
            if(lookahead.type != ebnf_token::OperatorRParen)
                throw parse_error();
            advance();
        }
        else if(lookahead.type == ebnf_token::OperatorLRep)
        {
            advance();
            t = grammar_repeat{parse_exp()};
            if(lookahead.type != ebnf_token::OperatorRRep)
                throw parse_error();
            advance();
        }

        return t;
    }

    grammar_tree parse_unit()
    {
        grammar_tree t = parse_group();

        grammar_concat t_rest;
        while(lookahead.type == ebnf_token::Identifier ||
              lookahead.type == ebnf_token::TermString ||
              lookahead.type == ebnf_token::OperatorLOpt ||
              lookahead.type == ebnf_token::OperatorLParen ||
              lookahead.type == ebnf_token::OperatorLRep)
        {
            t_rest.children.push_back(parse_group());
        }
        if(t_rest.children.size())
        {
            t_rest.children.push_front(std::move(t));
            return t_rest;
        }
        else
            return t;
    }

    grammar_tree parse_exp()
    {
        grammar_tree t = parse_unit();

        if(lookahead.type == ebnf_token::OperatorAlt)
        {
            grammar_alternates a;
            a.children.push_back(std::move(t));

            do
            {
                advance();
                a.children.push_back(parse_unit());
            } while(lookahead.type == ebnf_token::OperatorAlt);

            t = std::move(a);
        }

        return t;
    }

    void parse_rule(grammar& g)
    {
        std::string rule = std::move(lookahead.str);

        advance();
        if(lookahead.type != ebnf_token::OperatorDef)
            throw parse_error();
        advance();

        grammar_tree t = parse_exp();

        auto rule_it = g.find(rule);
        if(rule_it != g.end())
        {
            grammar_alternates *old = boost::get<grammar_alternates>(&rule_it->second);
            grammar_alternates *new_alt = boost::get<grammar_alternates>(&t);

            if(old)
            {
                if(new_alt)
                    for(auto& i : new_alt->children)
                        old->children.push_back(std::move(i));
                else
                    old->children.push_back(std::move(t));
            }
            else
            {
                grammar_alternates temp;
                temp.children.push_back(std::move(rule_it->second));
                if(new_alt)
                    for(auto& i : new_alt->children)
                        temp.children.push_back(std::move(i));
                else
                    temp.children.push_back(std::move(t));
                rule_it->second = std::move(temp);
            }
        }
        else
        {
            g[std::move(rule)] = std::move(t);
        }

        if(lookahead.type != ebnf_token::OperatorEnd)
            throw parse_error();
    }

    void parse_ebnf(grammar& g)
    {
        while(lookahead.type == ebnf_token::Identifier)
        {
            parse_rule(g);
            advance();
        }
    }
};

template <typename InputIterator>
ebnf_parser<InputIterator> make_ebnf_parser(InputIterator first, InputIterator last)
{
    return ebnf_parser<InputIterator>(first, last);
}

#endif // EBNF_PARSER_HPP_INCLUDED
