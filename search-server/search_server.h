#pragma once
#include "string_processing.h"
#include "document.h"
#include "process_queries.h"
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

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;

class SearchServer {
public:
    explicit SearchServer(const std::string& stop_words_text);
    
    template <typename StringContainer>
SearchServer(const StringContainer& stop_words);

    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);
    
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const;

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
    
std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;    
    
std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(std::execution::sequenced_policy, const std::string& raw_query, int document_id) const;
    
std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(std::execution::parallel_policy, const std::string& raw_query, int document_id) const;
    
const std::map<std::string, double>& GetWordFrequencies(int document_id) const;
    
private:
    struct DocumentData {
        int rating = 0;
        DocumentStatus status;
    };
    const std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string, double>> document_to_word_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;

    bool IsStopWord(const std::string& word) const;

    static bool IsValidWord(const std::string& word);

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    static int ComputeAverageRating(const std::vector<int>& ratings);

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(const std::string& text) const;

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    Query ParseQuery(const std::string& text) const;
    
    Query ParseQuery(std::execution::sequenced_policy, const std::string& text) const;
    
    Query ParseQuery(std::execution::parallel_policy, const std::string& text) const;

    double ComputeWordInverseDocumentFreq(const std::string& word) const;

template <typename DocumentPredicate>
std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
};

template <typename DocumentPredicate>
    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentPredicate document_predicate) const {
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

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words) : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw std::invalid_argument("Some of stop words are invalid"s);
        }
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
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

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
                 const std::vector<int>& ratings);

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query);

template <class ExecutionPolicy>
void MatchDocument(ExecutionPolicy&& policy, SearchServer& search_server, const std::string& query) {
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

void MatchDocument(const SearchServer& search_server, const std::string& query);

void RemoveDuplicates(SearchServer& search_server);