#pragma once
#include "document.h"
#include "search_server.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <deque>
#include <utility>

using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);  
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;
private:
    struct QueryResult;
    static std::deque<QueryResult> requests_;
    const static int min_in_day_;
    const SearchServer& search_server_;
    static bool fooled_;
}; 