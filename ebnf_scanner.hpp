#ifndef EBNF_SCANNER_HPP_INCLUDED
#define EBNF_SCANNER_HPP_INCLUDED

#include <cassert>
#include <cctype>
#include <string>

#include "ebnf_token.hpp"

/*
    Lexical analyzer for EBNF input.

    Supports only one token lookahead; EBNF is LL(1) so only one is necessary.
    Templated to run from any type of input iterator
*/

template <typename InputIterator>
class ebnf_scanner
{
public:
    ebnf_scanner(InputIterator first, InputIterator last): head(first), last(last)
    {
        advance();
    }

    // Get a reference to lookahead token
    // Token is non-const; caller is expected to only modify token if
    // caller won't need it anymore.
    ebnf_token& lookahead()
    {
        return lah;
    }

    // Scan for next token
    void advance()
    {
        while(head != last && std::isspace(*head)) ++head;
        if(head == last)
        {
            lah = ebnf_token(ebnf_token::EOI);
            return;
        }

        // Stand-in lexer
        if(*head == '"')
        {
            lah = ebnf_token(ebnf_token::TermString);
            bool escaped = false;
            while(++head != last && (!escaped && *head != '"'))
            {
                if(escaped)
                {
                    if(*head == '"')
                        lah.str.push_back('"');
                    else if(*head == 'n')
                        lah.str.push_back('\n');
                    else if(*head == '\\')
                        lah.str.push_back('\\');
                    escaped = false;
                }
                else
                {
                    if(*head == '\\')
                        escaped = true;
                    else
                        lah.str.push_back(*head);
                }
            }
            assert(head != last && !escaped && *head == '"');
            ++head;
        }
        else if(*head == '\'')
        {
            lah = ebnf_token(ebnf_token::TermString);
            bool escaped = false;
            while(++head != last && (!escaped && *head != '\''))
            {
                if(escaped)
                {
                    if(*head == '\'')
                        lah.str.push_back('\'');
                    else if(*head == 'n')
                        lah.str.push_back('\n');
                    else if(*head == '\\')
                        lah.str.push_back('\\');
                    escaped = false;
                }
                else
                {
                    if(*head == '\\')
                        escaped = true;
                    else
                        lah.str.push_back(*head);
                }
            }
            assert(head != last && !escaped && *head == '\'');
            ++head;
        }
        else if(std::isalnum(*head) || *head == '_')
        {
            lah = ebnf_token(ebnf_token::Identifier);
            do
            {
                lah.str.push_back(*head++);
            } while(head != last && (std::isalnum(*head) || *head == '_'));
        }
        else if(*head == '=')
            lah = ebnf_token(ebnf_token::OperatorDef), ++head;
        else if(*head == ',')
            lah = ebnf_token(ebnf_token::OperatorConcat), ++head;
        else if(*head == ';')
            lah = ebnf_token(ebnf_token::OperatorEnd), ++head;
        else if(*head == '|')
            lah = ebnf_token(ebnf_token::OperatorAlt), ++head;
        else if(*head == '[')
            lah = ebnf_token(ebnf_token::OperatorLOpt), ++head;
        else if(*head == ']')
            lah = ebnf_token(ebnf_token::OperatorROpt), ++head;
        else if(*head == '{')
            lah = ebnf_token(ebnf_token::OperatorLRep), ++head;
        else if(*head == '}')
            lah = ebnf_token(ebnf_token::OperatorRRep), ++head;
        else if(*head == '(')
            lah = ebnf_token(ebnf_token::OperatorLParen), ++head;
        else if(*head == ')')
            lah = ebnf_token(ebnf_token::OperatorRParen), ++head;
        else
            lah = ebnf_token(ebnf_token::Invalid), ++head;
    }

protected:
    InputIterator head, last;
    ebnf_token lah;
};

template <typename InputIterator>
ebnf_scanner<InputIterator> make_ebnf_scanner(InputIterator first, InputIterator last)
{
    return ebnf_scanner<InputIterator>(first, last);
}

#endif // EBNF_SCANNER_HPP_INCLUDED
