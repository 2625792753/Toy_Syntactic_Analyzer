#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <stack>

#include "meta.hpp"

#include <iostream>
using namespace std;

set<char> non_terminators;
multimap<char, char> first;
set<char> nullalbe;
multimap<char, char> follow;
map<pair<char, char>, string> sheet;

void load_non_terminators()
{
    for (auto i : rulelist)
        non_terminators.emplace(i.first);

#ifdef _DEBUG_
    cout << "NON_DERMINATORS:: " << endl;
    for (auto i : non_terminators)
        cout << i << endl;
    cout << endl;
#endif
}

void nullable_process()
{
    int t = 0;
    nullalbe.emplace('#');

    while (nullalbe.size() > t)
    {
        t = nullalbe.size();

        for (auto i : rulelist)
        {
            char symbol = i.first;
            string expr = i.second;

            bool nullable_flag = true;
            for (auto c : expr)
            {
                if (!nullalbe.contains(c))
                {
                    nullable_flag = false;
                    break;
                }
            }

            if (nullable_flag)
                nullalbe.emplace(symbol);
        }
    }

#ifdef _DEBUG_

    cout << "NULLABLE_SET::" << endl;
    for (auto i : nullalbe)
        cout << i << endl;
    cout << endl;

#endif
}

void extend_rulelist()
{
    for (auto symbol : non_terminators)
    {
        auto pos = rulelist.equal_range(symbol);
        set<string> extra_expr;
        bool tail = true;
        while (pos.first != pos.second)
        {
            string expr = pos.first->second;
            for (int i = 0; i < expr.length() - 1; ++i)
            {
                if (nullalbe.contains(expr[i]))
                    extra_expr.emplace(expr.substr(i + 1));
                else
                {
                    tail = false;
                    break;
                }
            }

            if (tail && nullalbe.contains(expr.back()) && [symbol]() -> bool
                {
                    auto pos = rulelist.equal_range(symbol);
                    while (pos.first != pos.second)
                    {
                        if (pos.first->second == "#")
                            return false;

                        ++pos.first;
                    }

                    return true;
                }())
                extra_expr.emplace("#");

            ++pos.first;
        }

        for (auto expr : extra_expr)
            rulelist.emplace(symbol, expr);
    }

#ifdef _DEBUG_

    cout << "EXTENDED_RULELISTS::" << endl;
    for (auto rule : rulelist)
        cout << rule.first << " => " << rule.second << endl;
    cout << endl;

#endif
}

bool _loop_flag = false;

bool search_first(char target, set<char> visited)
{
    if (visited.contains(target))
    {
        _loop_flag = true;
        return false;
    }

    auto pos = rulelist.equal_range(target);
    while (pos.first != pos.second)
    {
        string expr = pos.first->second;
        char symbol = expr[0];

        multimap<char, string> need;
        if (!non_terminators.contains(symbol))
        {
            first.emplace(target, symbol);
            if (symbol != '#')
                sheet.emplace(pair<char, char>(target, symbol), expr);
        }
        else
        {
            visited.emplace(target);

            search_first(symbol, visited);
            need.emplace(symbol, expr);
        }

        for (auto s : need)
        {
            multimap<char, char> tmp;
            auto pos = first.equal_range(s.first);
            while (pos.first != pos.second)
            {
                tmp.emplace(target, pos.first->second);
                sheet.emplace(pair<char, char>(target, pos.first->second), s.second);

                ++pos.first;
            }

            first.merge(tmp);
        }

        ++pos.first;
    }

    return true;
}

bool generate_all_vessels()
{
    load_non_terminators();

    nullable_process();

    extend_rulelist();

    // FIRST set generation
    for (auto r : rulelist)
        search_first(r.first, set<char>());

    if (_loop_flag)
    {
        cout << "ERROR:: Generation faild. Loop detected." << endl;
        return false;
    }

    // simplification
    for (auto i : non_terminators)
    {
        set<char> symbols;
        auto pos = first.equal_range(i);
        while (pos.first != pos.second)
        {
            symbols.emplace(pos.first->second);
            ++pos.first;
        }

        first.erase(i);
        for (auto s : symbols)
            first.emplace(i, s);
    }

    // FOLLOW set generation
    // 1.Conclude rules which related to First sets
    follow.emplace(primary_rule.first, '#');
    for (auto i : rulelist)
    {
        string expr = i.second;
        set<char> need;
        for (int j = 0; j < expr.length() - 1; ++j)
        {
            char tar = expr[j];
            if (non_terminators.contains(tar))
            {
                // find non_terminator and reclusively solve nullable non_terminators
                need.emplace(tar);
            }

            char next = expr[j + 1];
            if (non_terminators.contains(next))
            {
                auto pos = first.equal_range(next);
                while (pos.first != pos.second)
                {
                    for (auto n : need)
                        follow.emplace(n, pos.first->second);

                    ++pos.first;
                }
            }
            else
            {
                for (auto n : need)
                    follow.emplace(n, next);
            }

            if (!nullalbe.contains(next))
                need.clear();
        }
    }

    // simplification
    for (auto i : non_terminators)
    {
        set<char> symbols;
        auto pos = follow.equal_range(i);
        while (pos.first != pos.second)
        {
            symbols.emplace(pos.first->second);
            ++pos.first;
        }

        follow.erase(i);
        for (auto s : symbols)
            follow.emplace(i, s);
    }

    // 2.Conclude rules which merge follow sets
    multimap<char, char> follow_form;
    for (auto r : rulelist)
    {
        char from = r.first;
        string expr = r.second;

        for (int i = expr.length() - 1; i >= 0; --i)
        {
            if (non_terminators.contains(expr[i]))
                follow_form.emplace(expr[i], from);

            if (!nullalbe.contains(expr[i]))
                break;
        }
    }

    // simplification on follow_form
    for (auto i : non_terminators)
    {
        set<char> symbols;
        auto pos = follow_form.equal_range(i);
        while (pos.first != pos.second)
        {
            symbols.emplace(pos.first->second);
            ++pos.first;
        }

        follow_form.erase(i);
        for (auto s : symbols)
            follow_form.emplace(i, s);
    }

    // Final concluding
    int size = follow.size();
    do
    {
        size = follow.size();
        for (auto f : follow_form)
        {
            char to = f.first;
            char from = f.second;

            auto pos = follow.equal_range(from);
            set<char> tmp;
            while (pos.first != pos.second)
            {
                tmp.emplace(pos.first->second);
                ++pos.first;
            }

            for (char obj : tmp)
            {
                follow.emplace(to, obj);
            }
        }

        // simplification on each run
        for (auto i : non_terminators)
        {
            set<char> symbols;
            auto pos = follow.equal_range(i);
            while (pos.first != pos.second)
            {
                symbols.emplace(pos.first->second);
                ++pos.first;
            }

            follow.erase(i);
            for (auto s : symbols)
                follow.emplace(i, s);
        }
    } while (follow.size() > size);

    // Sheet generation
    for (auto r : rulelist)
    {
        if (r.second == "#")
        {
            auto pos = follow.equal_range(r.first);
            while (pos.first != pos.second)
            {
                sheet.emplace(pair<char, char>(r.first, pos.first->second), r.second);
                ++pos.first;
            }
        }
    }

#ifdef _DEBUG_
    cout << "FIRST_SET:: " << endl;
    for (auto i : non_terminators)
    {
        auto pos = first.equal_range(i);
        cout << i << '\t';

        while (pos.first != pos.second)
        {
            cout << pos.first->second << ' ';
            ++pos.first;
        }

        cout << endl;
    }

    cout << "FOLLOW_SET:: " << endl;
    for (auto i : non_terminators)
    {
        auto pos = follow.equal_range(i);
        cout << i << '\t';

        while (pos.first != pos.second)
        {
            cout << pos.first->second << ' ';
            ++pos.first;
        }

        cout << endl;
    }

    cout << "SHEET::" << endl;
    for (auto i : sheet)
        cout << i.first.first << '\t' << i.first.second << '\t' << i.second << endl;

    cout << endl;
    cout << endl;
#endif

    return true;
}

bool syntactic_analyze(string input)
{
    int index = 0;
#ifdef _DEBUG_
    cout << "ANALYZE_TRACING::" << endl;
#endif

    stack<char> analyze_stack;
    analyze_stack.emplace(primary_rule.first);
    while (!analyze_stack.empty() || index < input.length() - 1)
    {
        char trace = input[index];
        auto step = sheet.find(pair<char, char>(analyze_stack.top(), trace));
        if (step != sheet.end())
        {
            analyze_stack.pop();
            for (int i = step->second.length() - 1; i >= 0; --i)
            {
                char c = step->second[i];
                if (c != '#')
                    analyze_stack.push(c);
            }
        }
        else
        {
            if (analyze_stack.top() == input[index])
            {
                ++index;
                analyze_stack.pop();
            }
            else
                return false;
        }
#ifdef _DEBUG_
        // Tracing
        cout << [analyze_stack]() -> string
        {
            string tmp;
            auto stack_copy = new stack<char>(analyze_stack);
            while (!stack_copy->empty())
            {
                tmp += stack_copy->top();
                stack_copy->pop();
            }
            delete (stack_copy);
            reverse(tmp.begin(), tmp.end());
            return tmp;
        }()
                                         << '\t' << input.substr(index) << endl;
#endif
    }

    return analyze_stack.empty() && index == input.length() - 1;
}