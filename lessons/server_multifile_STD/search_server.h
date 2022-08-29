#pragma once
#include "document.h"
#include "string_processing.h"
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>
#include <numeric>
#include <iostream>
#include <vector>

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:   
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);

    explicit SearchServer(const std::string& stop_words_text);

    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const;

    int GetDocumentCount() const;

    int GetDocumentId(int index) const;

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;
private: 
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    
    const std::set<std::string> stop_words_;
    
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    
    std::map<int, DocumentData> documents_;
    
    std::vector<int> document_ids_;
    
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
    
    double ComputeWordInverseDocumentFreq(const std::string& word) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
};

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query);
    
void MatchDocuments(const SearchServer& search_server, const std::string& query);