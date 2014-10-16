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

struct resolve_name_visitor : boost::static_visitor<>
{
    template <typename Dummy>
    void operator()(Dummy& d) const
    {

    }
    void operator()(grammar_concat& t) const
    {
        for(auto& i : t.children)
            boost::apply_visitor(resolve_name_visitor(i, id_table), i);
    }
    void operator()(grammar_alternates& t) const
    {
        for(auto& i : t.children)
            boost::apply_visitor(resolve_name_visitor(i, id_table), i);
    }
    void operator()(grammar_optional& t) const
    {
        boost::apply_visitor(resolve_name_visitor(t.child, id_table), t.child);
    }
    void operator()(grammar_repeat& t) const
    {
        boost::apply_visitor(resolve_name_visitor(t.child, id_table), t.child);
    }
    void operator()(grammar_identifier& i) const
    {
        auto it = id_table.find(i.name);
        if(it != id_table.end())
            variant_node = grammar_rule_id{it->second};
    }

    grammar_tree& variant_node;
    const std::unordered_map<std::string, std::size_t>& id_table;
    resolve_name_visitor(grammar_tree& variant_node_in, const std::unordered_map<std::string, std::size_t>& id_table_in): variant_node(variant_node_in), id_table(id_table_in) {}
};


processed_ruleset inline resolve_identifiers(ruleset in)
{
    processed_ruleset result;

    std::size_t cur = 0;
    for(auto& i : in)
        result.id_table[i.first] = cur++;

    result.rules.reserve(cur);
    for(auto& i : in)
        result.rules.push_back(std::move(i.second));

    for(auto& i : result.rules)
    {
        boost::apply_visitor(resolve_name_visitor(i.t, result.id_table), i.t);
    }

    return result;
}

#endif // GRAMMAR_ANALYSIS_HPP_INCLUDED
