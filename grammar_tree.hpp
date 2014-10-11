#ifndef GRAMMAR_TREE_HPP_INCLUDED
#define GRAMMAR_TREE_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/variant.hpp>

using grammar_tree = boost::variant<struct grammar_terminal,
                                    struct grammar_token,
                                    struct grammar_rule,
                                    struct grammar_identifier,
                                    struct grammar_concat,
                                    struct grammar_alternates,
                                    struct grammar_optional,
                                    struct grammar_repeat>;

struct grammar_terminal
{
    std::string value;
};

struct grammar_token
{
    std::string name;
};

struct grammar_rule
{
    std::string name;
};

struct grammar_identifier
{
    std::string name;
};

struct grammar_concat
{
    std::vector<grammar_tree> children;
};

struct grammar_alternates
{
    std::vector<grammar_tree> children;
};

struct grammar_optional
{
    grammar_tree child;
};

struct grammar_repeat
{
    grammar_tree child;
};

#endif // GRAMMAR_TREE_HPP_INCLUDED
