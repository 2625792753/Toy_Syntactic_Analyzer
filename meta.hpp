#pragma once

#include <map>
#include <string>

#define BAD_FILE_MSG(PATH) "FILE AT PATH:: " + PATH + "DEPRECATED."

using namespace std;

string rule_path;
multimap<char, string> rulelist;
pair<char, string> primary_rule;

string input_path;
string input;