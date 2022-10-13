#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::set<int> duplicates_id;
    std::map<std::set<std::string>, std::set<int>> work_base;
        for (const int id : search_server) {
            std::set<std::string> words_;
            for (const auto& [words_from_doc, d] : search_server.GetWordFrequencies(id)) {
                words_.insert(words_from_doc);
            }
            const int last_item = *(work_base[words_].end());
            work_base[words_].insert(id);
            if (work_base.at(words_).size() > 1) {
                duplicates_id.insert(std::max(id, last_item));
            }
        }
           
        for (const int id : duplicates_id) {
            std::cout << "Found duplicate document id " << id << std::endl;
            search_server.RemoveDocument(id);
        }
}
