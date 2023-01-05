#include "search_server.h"

SearchServer::SearchServer(const std::string& stop_words_text): SearchServer(SplitIntoWords(stop_words_text)){ }

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw std::invalid_argument("Invalid document_id"s);
        }
        const auto words = SplitIntoWordsNoStop(document);

        const double inv_word_count = 1.0 / words.size();
        for (const std::string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            document_to_word_freqs_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids_.insert(document_id);
}  
    
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
        return documents_.size();
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
        const auto query = ParseQuery(raw_query);

        std::vector<std::string> matched_words;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return {matched_words, documents_.at(document_id).status};
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(std::execution::sequenced_policy, const std::string& raw_query, int document_id) const {
    return MatchDocument(raw_query, document_id);
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(std::execution::parallel_policy, const std::string& raw_query, int document_id) const {
        const auto query = ParseQuery(std::execution::par, raw_query);    
        const auto& plus = query.plus_words;
        const auto& minus = query.minus_words;
        const auto& words_freq = word_to_document_freqs_;
        std::vector<std::string> matched_words;
    
        bool minus_check = std::any_of(std::execution::par,
                                       std::begin(minus),
                                       std::end(minus),
                                       [document_id, &words_freq](const std::string& word){
                                           return (words_freq.at(word).count(document_id));
                                       });
        if (!minus_check) {
            std::copy_if(std::execution::par,
                         std::begin(plus),
                         std::end(plus),
                         std::back_inserter(matched_words),
                         [&words_freq, document_id]
                         (const std::string& word){
                             return (words_freq.at(word).count(document_id));
                         }
            );
        }
 
        return {matched_words, documents_.at(document_id).status};
}

bool SearchServer::IsStopWord(const std::string& word) const {
        return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
        std::vector<std::string> words;
        for (const std::string& word : SplitIntoWords(text)) {
            if (!IsValidWord(word)) {
                throw std::invalid_argument("Word "s + word + " is invalid"s);
            }
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const std::string& text) const {
        if (text.empty()) {
            throw std::invalid_argument("Query word is empty"s);
        }
        std::string word = text;
        bool is_minus = false;
        if (word[0] == '-') {
            is_minus = true;
            word = word.substr(1);
        }
        if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
            throw std::invalid_argument("Query word "s + text + " is invalid");
        }

        return {word, is_minus, IsStopWord(word)};
}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
        SearchServer::Query result;
        for (const std::string& word : SplitIntoWords(text)) {
            const auto query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    result.minus_words.insert(query_word.data);
                } else {
                    result.plus_words.insert(query_word.data);
                }
            }
        }
        return result;
}

SearchServer::Query SearchServer::ParseQuery(std::execution::sequenced_policy, const std::string& text) const {
        return ParseQuery(text);
}

SearchServer::Query SearchServer::ParseQuery(std::execution::parallel_policy, const std::string& text) const {
        SearchServer::Query result;
        const std::vector<std::string> SplittedText = std::move(SplitIntoWords(text));
        std::for_each(std::execution::par,
                      std::begin(SplittedText),
                      std::end(SplittedText),
                      [this, &result](const std::string& word) {
                            const auto query_word = std::move(ParseQueryWord(word));
                            if (!query_word.is_stop) {
                                if (query_word.is_minus) {
                                    result.minus_words.insert(query_word.data);
                                } else {
                                    result.plus_words.insert(query_word.data);
                                }
                            }
                      }
        );
                    
    
        return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status,
                 const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    std::cout << "Результаты поиска по запросу: "s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка поиска: "s << e.what() << std::endl;
    }
}

void MatchDocument(SearchServer& search_server, const std::string& query) {
    MatchDocument(std::execution::seq, search_server, query);
}

void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}

const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static std::map<std::string, double> result;
    if ( (document_to_word_freqs_.count(document_id) != 0) && (document_to_word_freqs_.at(document_id).size() != 0 ) ) {
        return document_to_word_freqs_.at(document_id);
    } else {
        return result;
    }
}

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