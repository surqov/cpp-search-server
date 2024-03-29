#pragma once
#include "search_server.h"
#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
            if (fooled_ || requests_.size() >= min_in_day_) {
                requests_.pop_front();
                fooled_ = true;
            }
            std::vector<Document> result = search_server_.FindTopDocuments(raw_query, document_predicate);
            requests_.push_back({raw_query, !result.empty()});
            return result;
        }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        QueryResult() = default;
        const std::string request;
        bool founded;
    };

    std::deque<QueryResult> requests_;

    const static int min_in_day_ = 1440;

    const SearchServer& search_server_;

    bool fooled_ = false;
}; 