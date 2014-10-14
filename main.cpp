#include <iostream>
#include <iterator>
#include <string>

#include "ebnf_token.hpp"
#include "ebnf_parser.hpp"
#include "ebnf_scanner.hpp"

int main()
{
    using namespace std;

    auto parser = make_ebnf_parser(istreambuf_iterator<char>(cin),
                                   istreambuf_iterator<char>());

    auto ruleset = parser.run();

    for(auto& i : ruleset)
    {
        cout << i.first << ":\n" << i.second << endl;
    }
}
