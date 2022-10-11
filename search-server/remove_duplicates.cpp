#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::set<int> duplicates_id;
    std::vector<std::pair<int, std::string>> work_base;
        for (const int id : search_server) {
            std::set<std::string> words;
            for (const auto item : search_server.GetWordFrequencies(id)) {
                words.insert(item.first);
            }
            std::string out = std::accumulate(words.begin(), words.end(), std::string(""));
            work_base.push_back({id, out});
        }
    
    std::sort(work_base.begin(), work_base.end(), [] (const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });
    
    for (auto item1 = std::begin(work_base), item2 = item1 + 1;
        item2 != work_base.end();
        ++item1, ++item2) {
        
        if (((*item1).second.size() != (*item2).second.size()) || ((*item1).second != (*item2).second)) {
            continue;
        }

        duplicates_id.insert(std::max((*item1).first, (*item2).first));
    }
    
        for (const int id : duplicates_id) {
            std::cout << "Found duplicate document id " << id << std::endl;
            search_server.RemoveDocument(id);
        }
}
