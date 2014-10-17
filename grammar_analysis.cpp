#include "grammar_analysis.hpp"

ruleset merge_trees(std::vector<std::pair<std::string, grammar_tree>> trees, ruleset::iterator& start)
{
    using namespace std;

    ruleset g;
    if(trees.size() == 0)
        return g;

    string start_name = trees[0].first;
    for(auto& p : trees)
    {
        string& rule_name = p.first;
        grammar_tree& t = p.second;

        auto rule_it = g.find(rule_name);
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
            g[std::move(rule_name)] = std::move(t);
        }
    }

    start = g.find(start_name);
    return g;
}

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


processed_ruleset resolve_identifiers(ruleset in)
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
