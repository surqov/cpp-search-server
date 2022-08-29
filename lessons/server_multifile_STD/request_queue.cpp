#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server){}

struct RequestQueue::QueryResult {
    const std::string request = ""s;
    bool founded = false;
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        if (RequestQueue::fooled_ || RequestQueue::requests_.size() >= RequestQueue::min_in_day_) {
            RequestQueue::requests_.pop_front();
            RequestQueue::fooled_ = true;
        }
        std::vector<Document> result = search_server_.FindTopDocuments(raw_query, document_predicate);
        RequestQueue::requests_.push_back({raw_query, !result.empty()});
        return result;
    }
    
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        return RequestQueue::AddFindRequest(raw_query, [status](int, DocumentStatus document_status, int) {
            return document_status == status;
        });
    }

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
        return RequestQueue::AddFindRequest(raw_query, DocumentStatus::ACTUAL);
    }

int RequestQueue::GetNoResultRequests() const {
        return std::count_if(RequestQueue::requests_.begin(), RequestQueue::requests_.end(), [](const auto item){return item.founded == false;});
    }

std::deque<RequestQueue::QueryResult> RequestQueue::requests_;
const int RequestQueue::min_in_day_ = 1440;
bool RequestQueue::fooled_ = false;