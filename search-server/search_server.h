#pragma once
#include "string_processing.h"
#include "document.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <numeric>
#include <execution>
#include <string_view>

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;
const size_t KEYS = 5;

class SearchServer {
public:
    SearchServer() = default;
    
    SearchServer(const std::string& stop_words_text);
    
    template <typename StringContainer>
SearchServer(const StringContainer& stop_words);
    
    ~SearchServer() = default;

    void AddDocument(int document_id, const std::string_view& document, DocumentStatus status, const std::vector<int>& ratings);
    
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string_view& raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(const std::string_view& raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::string_view& raw_query) const;
    
    template <typename DocumentPredicate, class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentPredicate document_predicate) const;

    template <class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentStatus status) const;

    template <class ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query) const;

    int GetDocumentCount() const;
    
auto begin() const{
    return document_ids_.begin();
}

auto end() const {
    return document_ids_.end();
}

template <class ExecutionPolicy>
void RemoveDocument(ExecutionPolicy&& policy, int document_id) {
    if (std::find(policy,
                 std::begin(document_ids_),
                 std::end(document_ids_),
                 document_id) == document_ids_.end()) {
        return;
    }
    
    const auto& id_words = document_to_word_freqs_.at(document_id);
    std::vector<std::string> words_(id_words.size());
    std::transform(policy,
                   std::begin(id_words),
                   std::end(id_words),
                   std::begin(words_),
                   [](const auto& pair) {
                       return pair.first;
                   });
    
    std::for_each(policy, 
                  std::begin(words_),
                  std::end(words_),
                  [this, document_id](const auto& word){
                     word_to_document_freqs_[word].erase(document_id);
                  });
    document_ids_.erase(document_id);
    documents_.erase(document_id);
    document_to_word_freqs_.erase(document_id);
}
    
void RemoveDocument(int document_id);
    
std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::string_view& raw_query, int document_id) const;    
    
std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::sequenced_policy, const std::string_view& raw_query, int document_id) const;
    
std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::execution::parallel_policy, const std::string_view& raw_query, int document_id) const;
    
const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;
    
private:
    struct DocumentData {
        int rating = 0;
        DocumentStatus status;
    };
    const std::string raw_stop_words_;
    std::set<std::string> raw_documents_text_;
    
    const std::set<std::string_view> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> document_to_word_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;

    bool IsStopWord(const std::string_view& word) const;

    static bool IsValidWord(const std::string_view& word);

    std::vector<std::string_view> SplitIntoWordsNoStop(const std::string_view& text) const;

    static int ComputeAverageRating(const std::vector<int>& ratings);

    struct QueryWord {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(const std::string_view& text) const;

    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };

    Query ParseQuery(const std::string_view& text) const;
    
    Query ParseQuery(std::execution::sequenced_policy, const std::string_view& text) const;
    
    Query ParseQuery(std::execution::parallel_policy, const std::string_view& text) const;

    double ComputeWordInverseDocumentFreq(const std::string_view& word) const;

template <typename DocumentPredicate>
std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
    
template <typename DocumentPredicate, class ExecutionPolicy>
std::vector<Document> FindAllDocuments(ExecutionPolicy&& policy, const Query& query, DocumentPredicate document_predicate) const;           
};

template <typename DocumentPredicate, class ExecutionPolicy>
    std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&&, const std::string_view& raw_query, DocumentPredicate document_predicate) const {
        const auto query = ParseQuery(raw_query); 
  
        auto matched_documents = FindAllDocuments(query, document_predicate);

        std::sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < ACCURACY) {
                return lhs.rating > rhs.rating;
            } else {
                return lhs.relevance > rhs.relevance;
            }
        });
        
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }
    
template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query, DocumentPredicate document_predicate) const {
    return FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(policy, raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

template <typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, const std::string_view& raw_query) const {
    return FindTopDocuments(policy, raw_query, DocumentStatus::ACTUAL);
}


template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words) : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw std::invalid_argument("Some of stop words are invalid"s);
        }
}

template <typename DocumentPredicate, class ExecutionPolicy>
std::vector<Document> SearchServer::FindAllDocuments([[maybe_unused]] ExecutionPolicy&& policy, const Query& query, DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string_view& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const std::string_view& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.emplace_back(document_id, relevance, documents_.at(document_id).rating);
        }
        return matched_documents;
    }
template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
    return FindAllDocuments(std::execution::seq, query, document_predicate);
}

void AddDocument(SearchServer& search_server, int document_id, const std::string_view& document, DocumentStatus status,
                 const std::vector<int>& ratings);

void FindTopDocuments(const SearchServer& search_server, const std::string_view& raw_query);

template <class ExecutionPolicy>
void MatchDocument(ExecutionPolicy&& policy, SearchServer& search_server, const std::string_view& query) {
    try {
        std::cout << "Матчинг документов по запросу: "s << query << std::endl;
        std::for_each(policy,
                        std::begin(search_server),
                        std::end(search_server),
                        [search_server, query](const int document_id) {
                            const auto [words, status] = search_server.MatchDocument(query, document_id);
                            PrintMatchDocumentResult(document_id, words, status);
                        });
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка матчинга документов на запрос "s << query << ": "s << e.what() << std::endl;
    }
}

void MatchDocument(const SearchServer& search_server, const std::string_view& query);

void RemoveDuplicates(SearchServer& search_server);