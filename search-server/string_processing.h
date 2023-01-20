#pragma once
#include <string_view>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

std::vector<std::string_view> SplitIntoWords(const std::string_view& str_orig);

template <typename StringContainer>
std::set<std::string_view> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string_view> non_empty_strings;
    for (const std::string_view& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}