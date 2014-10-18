#include <iostream>
#include <iterator>
#include <string>

#include "ebnf_token.hpp"
#include "ebnf_parser.hpp"
#include "ebnf_scanner.hpp"
#include "grammar_analysis.hpp"

int main()
{
    using namespace std;

    auto parser = make_ebnf_parser(istreambuf_iterator<char>(cin),
                                   istreambuf_iterator<char>());

    auto rule_list = parser.run();

    for(auto& i : rule_list)
    {
        cout << i.first << ":\n" << i.second << endl;
    }

    ruleset::iterator start;
    auto ruleset = merge_trees(std::move(rule_list), start);

    for(auto& i : ruleset)
    {
        cout << i.first << ":\n" << i.second << endl;
    }

    auto processed = resolve_identifiers(std::move(ruleset));

    for(auto& n : processed.id_table)
    {
        cout << n.first << ": " << n.second << endl;
    }

    for(size_t i = 0; i < processed.rules.size(); ++i)
    {
        cout << i << ":\n" << processed.rules[i].t << endl;
    }

    factor_inner_alternates(processed);

    cout << "Factored alternates:\n\n";

    for(size_t i = 0; i < processed.rules.size(); ++i)
    {
        cout << i << ":" << (processed.rules[i].is_generated ? " Generated\n" : "\n") << processed.rules[i].t << endl;
    }
}
