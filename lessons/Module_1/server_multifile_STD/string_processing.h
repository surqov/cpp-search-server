#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <set>

std::vector<std::string> SplitIntoWords(const std::string& text);

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings);