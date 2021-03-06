#ifndef GRAMMAR_TREE_HPP_INCLUDED
#define GRAMMAR_TREE_HPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>

/*
    Extensible tree for representing a formal grammar.

    Boost.Variant simplifies implementation and avoids runtime polymorphism
*/

// Typename alias for a tree node
using grammar_tree = boost::variant<struct grammar_terminal,
                                    struct grammar_empty,
                                    struct grammar_token,
                                    struct grammar_token_id,
                                    struct grammar_rule,
                                    struct grammar_rule_id,
                                    struct grammar_identifier,
                                    struct grammar_concat,
                                    struct grammar_alternates,
                                    boost::recursive_wrapper<struct grammar_optional>,
                                    boost::recursive_wrapper<struct grammar_repeat>>;

using ruleset = std::unordered_map<std::string, grammar_tree>;

// Node type for a terminal string in a grammar
struct grammar_terminal
{
    std::string value;
};

// Node type for terminal empty string
struct grammar_empty
{
};

// Node type for a token name
struct grammar_token
{
    std::string name;
};

// Node type for a token ID
struct grammar_token_id
{
    std::size_t id;
};

// Node type for a rule name
struct grammar_rule
{
    std::string name;
};

// Node type for a rule ID
struct grammar_rule_id
{
    std::size_t id;
};

// Node type for an unresolved identifier; meant to be an intermediate node
// before semantic analysis to identify rule names and token IDs
struct grammar_identifier
{
    std::string name;
};

// Node type for series of symbols
// Using Boost.Container deque since std::deque
// doesn't support incomplete types.
struct grammar_concat
{
    boost::container::deque<grammar_tree> children;
};

// Node type for group of possible symbols to satisfy a rule
struct grammar_alternates
{
    boost::container::deque<grammar_tree> children;
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

inline std::ostream& operator<<(std::ostream& os, const grammar_tree& t)
{
    struct printer : public boost::static_visitor<>
    {
        std::ostream& os;

        void operator()(const grammar_terminal& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Terminal \"" << n.value << "\")\n";
        }
        void operator()(const grammar_empty& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Empty)\n";
        }
        void operator()(const grammar_token& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Token " << n.name << ")\n";
        }
        void operator()(const grammar_token_id& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Token " << n.id << ")\n";
        }
        void operator()(const grammar_rule& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Rule " << n.name << ")\n";
        }
        void operator()(const grammar_rule_id& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Rule " << n.id << ")\n";
        }
        void operator()(const grammar_identifier& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Identifier " << n.name << ")\n";
        }
        void operator()(const grammar_concat& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(\n";
            for(auto& i : n.children)
            {
                boost::apply_visitor(printer(os, tab+1), i);
            }
            os << ")\n";
        }
        void operator()(const grammar_alternates& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Alternates\n";

            for(auto& i : n.children)
            {
                boost::apply_visitor(printer(os, tab+1), i);
            }

            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << ")\n";
        }
        void operator()(const grammar_optional& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Optional\n";

            boost::apply_visitor(printer(os, tab+1), n.child);

            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << ")\n";
        }
        void operator()(const grammar_repeat& n) const
        {
            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << "(Repeat\n";

            boost::apply_visitor(printer(os, tab+1), n.child);

            for(unsigned int i = 0; i < tab; ++i) os << ' ';
            os << ")\n";
        }

        unsigned int tab;

        printer(std::ostream& _os, unsigned int tab_in): os(_os), tab(tab_in) {}
    } p(os, 0);

    boost::apply_visitor(p, t);
    return os;
}

#endif // GRAMMAR_TREE_HPP_INCLUDED
