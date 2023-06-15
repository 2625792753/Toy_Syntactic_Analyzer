#pragma once

#include <fstream>
#include <iostream>
#include "meta.hpp"

#ifdef _DEBUG_
void rulelist_output()
{
    cout << "RULELISTS:" << endl;
    for (auto i : rulelist)
    {
        cout << i.first << " => " << i.second << endl;
    }
    cout << "PRIMARYRULR:: " << primary_rule.first << " => " << primary_rule.second << endl;
    cout << endl;
}
#endif

bool generate_rulelist(string path = rule_path)
{
    ifstream in(path, ios::in);
    if (!in.is_open())
    {
        cout << BAD_FILE_MSG(path) << endl;
        return false;
    }

    string single_rule;
    bool enter = true;
    while (in >> single_rule)
    {
        char reg = single_rule[0];
        int index = single_rule.find('=');
        if (index == string::npos || index == 0)
        {
            cout << "LL1 Expired.";
            return false;
        }

        string res = single_rule.substr(index + 1);
        rulelist.emplace(reg, res);
        if (enter)
        {
            primary_rule = pair<char, string>(reg, res);
            enter = false;
        }
    }

    return true;
}

bool load_input(string path = input_path)
{
    ifstream in(path, ios::in);
    if (!in.is_open())
    {
        cout << BAD_FILE_MSG(path) << endl;
        return false;
    }

    in >> input;
    input += '#';
    return true;
}