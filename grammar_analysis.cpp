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

    for(auto& i : in)
        result.rules.push_back(std::move(i.second));

    for(auto& i : result.rules)
    {
        boost::apply_visitor(resolve_name_visitor(i.t, result.id_table), i.t);
    }

    return result;
}

struct factor_alternates_visitor : boost::static_visitor<>
{
    template <typename Dummy>
    void operator()(Dummy&) const
    {

    }
    void operator()(grammar_alternates& alt) const
    {
        std::size_t new_id = grammar.rules.size();
        grammar.rules.emplace_back(std::move(alt), true);
        alt.children.push_back(grammar_rule_id{new_id});
        variant_node = grammar_rule_id{new_id};
    }
    void operator()(grammar_concat& concat) const
    {
        for(grammar_tree& t : concat.children)
            boost::apply_visitor(factor_alternates_visitor(t, grammar), t);
    }
    void operator()(grammar_optional& opt) const
    {
        boost::apply_visitor(factor_alternates_visitor(opt.child, grammar), opt.child);
    }
    void operator()(grammar_repeat& rep) const
    {
        boost::apply_visitor(factor_alternates_visitor(rep.child, grammar), rep.child);
    }

    grammar_tree& variant_node;
    processed_ruleset& grammar;

    factor_alternates_visitor(grammar_tree& p, processed_ruleset& g): variant_node(p), grammar(g) {}
};

void factor_inner_alternates(processed_ruleset& in)
{
    for(std::size_t i = 0; i < in.rules.size(); ++i)
    {
        grammar_alternates *alt = boost::get<grammar_alternates>(&in.rules[i].t);
        if(alt)
            for(grammar_tree& t : alt->children)
                boost::apply_visitor(factor_alternates_visitor(t, in), t);
        else
            boost::apply_visitor(factor_alternates_visitor(in.rules[i].t, in), in.rules[i].t);
    }
}
