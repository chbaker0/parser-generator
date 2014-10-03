#include <iostream>
#include <iterator>
#include <string>

#include "ebnf_token.hpp"
#include "ebnf_scanner.hpp"

int main()
{
    using namespace std;

    auto scanner = make_ebnf_scanner(istreambuf_iterator<char>(cin),
                                     istreambuf_iterator<char>());

    while(scanner.lookahead().type != ebnf_token::EOI)
    {
        cout << scanner.lookahead() << endl;
        scanner.advance();
    }
}
