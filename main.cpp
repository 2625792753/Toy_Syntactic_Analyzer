#define _DEBUG_

#include "meta.hpp"
#include "util.hpp"
#include "syntactic_analyzer.hpp"

int main(int argc, char const *argv[])
{
    if (generate_rulelist(argv[1]))
    {
#ifdef _DEBUG_
        cout << "DEBUG_INFO:: " << endl;
        rulelist_output();
#endif
        generate_all_vessels();
    }
}
