#ifndef GRAMMAR_ANALYSIS_HPP_INCLUDED
#define GRAMMAR_ANALYSIS_HPP_INCLUDED

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "grammar_tree.hpp"

struct rule
{
    grammar_tree t;
    bool is_generated;

    rule(): t(), is_generated(true) {}
    rule(grammar_tree t_in): t(std::move(t_in)), is_generated(false) {}
    rule(grammar_tree t_in, bool is_generated_in): t(std::move(t_in)), is_generated(is_generated_in) {}
};

struct processed_ruleset
{
    std::vector<rule> rules;
    std::unordered_map<std::string, std::size_t> id_table;
};

ruleset merge_trees(std::vector<std::pair<std::string, grammar_tree>> trees, ruleset::iterator& start);
processed_ruleset resolve_identifiers(ruleset in);
processed_ruleset factor_inner_alternates(processed_ruleset in);

#endif // GRAMMAR_ANALYSIS_HPP_INCLUDED
