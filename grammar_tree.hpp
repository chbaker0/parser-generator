#ifndef GRAMMAR_TREE_HPP_INCLUDED
#define GRAMMAR_TREE_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/container/vector.hpp>

/*
    Extensible tree for representing a formal grammar.

    Boost.Variant simplifies implementation and avoids runtime polymorphism
*/

// Typename alias for a tree node
using grammar_tree = boost::variant<struct grammar_terminal,
                                    struct grammar_token,
                                    struct grammar_rule,
                                    struct grammar_identifier,
                                    struct grammar_concat,
                                    struct grammar_alternates,
                                    struct grammar_optional,
                                    struct grammar_repeat>;
// Node type for a terminal string in a grammar
struct grammar_terminal
{
    std::string value;
};

// Node type for a token ID
struct grammar_token
{
    std::string name;
};

// Node type for a rule name
struct grammar_rule
{
    std::string name;
};

// Node type for an unresolved identifier; meant to be an intermediate node
// before semantic analysis to identify rule names and token IDs
struct grammar_identifier
{
    std::string name;
};

// Node type for series of symbols
// Using Boost.Container vector since std::vector
// doesn't support incomplete types.
struct grammar_concat
{
    boost::container::vector<grammar_tree> children;
};

// Node type for group of possible symbols to satisfy a rule
struct grammar_alternates
{
    boost::container::vector<grammar_tree> children;
};

// Node type for an optional symbol
struct grammar_optional
{
    grammar_tree child;
};

// Node type for arbitrary repetition of a symbol
struct grammar_repeat
{
    grammar_tree child;
};

#endif // GRAMMAR_TREE_HPP_INCLUDED
