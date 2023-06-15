#define _DEBUG_

#include "meta.hpp"
#include "util.hpp"
#include "syntactic_analyzer.hpp"

int main(int argc, char const *argv[])
{
    if (generate_rulelist("../../rules.txt"))
    {
#ifdef _DEBUG_
        cout << "DEBUG_INFO:: " << endl;
        rulelist_output();
#endif
        generate_all_vessels();
    }

    if (load_input("../../input.txt"))
    {
        if (syntactic_analyze(input))
            cout << "yes";
        else
            cout << "no";
    }
}
