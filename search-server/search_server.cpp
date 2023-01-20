#include "search_server.h"

SearchServer::SearchServer(const std::string& stop_words_text): 
    raw_stop_words_(stop_words_text),
    stop_words_(MakeUniqueNonEmptyStrings(SplitIntoWords(raw_stop_words_))) {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw std::invalid_argument("Some of stop words are invalid"s);
        }
    }  

void SearchServer::AddDocument(int document_id, const std::string_view& document, DocumentStatus status, const std::vector<int>& ratings) {
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw std::invalid_argument("Invalid document_id"s);
        }
        raw_documents_text_.insert(std::string(document));
        const auto words = SplitIntoWordsNoStop(*find(raw_documents_text_.begin(), raw_documents_text_.end(), std::string(document)));
        
        const double inv_word_count = 1.0 / words.size();
        for (const std::string_view& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            document_to_word_freqs_[document_id][word] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
        document_ids_.insert(document_id);
}  
    
std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query, DocumentStatus status) const {
    return FindTopDocuments(std::execution::seq, raw_query, [status](int, DocumentStatus document_status, int) {
        return document_status == status;
    });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query) const {
    return FindTopDocuments(std::execution::seq, raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
        return documents_.size();
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::string_view& raw_query, int document_id) const {
        auto query = std::move(ParseQuery(raw_query));    
        auto& plus = query.plus_words;
        auto& minus = query.minus_words;
        const auto& words_freq = word_to_document_freqs_;
        std::vector<std::string_view> matched_words;

        bool minus_check = std::any_of(std::execution::seq,
                                       std::begin(minus),
                                       std::end(minus),
                                       [document_id, &words_freq](const std::string_view& word){
                                           return (words_freq.at(word).count(document_id));
                                       });
        if (!minus_check){
            matched_words.reserve(plus.size());
            for (const std::string_view& word : plus) {
                if (word_to_document_freqs_.count(word) == 0 || stop_words_.count(word)) {
                    continue;
                }
                if (word_to_document_freqs_.at(word).count(document_id)) {
                    matched_words.push_back(word);
                }
            }
        }
    
    return {matched_words, documents_.at(document_id).status};
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::sequenced_policy, const std::string_view& raw_query, int document_id) const {
    return MatchDocument(raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::parallel_policy, const std::string_view& raw_query, int document_id) const {
        auto query = std::move(ParseQuery(std::execution::par, raw_query));    
        auto& plus = query.plus_words;
        auto& minus = query.minus_words;
        const auto& words_freq = word_to_document_freqs_;
        std::vector<std::string_view> matched_words;
        bool minus_check = std::any_of(std::execution::seq,
                                       std::begin(minus),
                                       std::end(minus),
                                       [document_id, &words_freq](const std::string_view& word){
                                           return (words_freq.at(word).count(document_id));
                                       });
    
        if (!minus_check && plus.size()) {
            matched_words.reserve(plus.size());
            std::copy_if(std::execution::seq,
                         std::begin(plus),
                         std::end(plus),
                         std::back_inserter(matched_words),
                         [this, &words_freq, document_id]
                         (const std::string_view& word){
                             return (words_freq.at(word).count(document_id));
                         }
            );
       
        if (matched_words.size()) {
            std::sort(std::execution::seq,
                     std::begin(matched_words),
                     std::end(matched_words));
            auto it = std::unique(std::execution::seq,
                                 std::begin(matched_words),
                                 std::end(matched_words));
            matched_words.resize(std::distance(matched_words.begin(), it));
        }
     }   
 
    return {matched_words, documents_.at(document_id).status};
}

bool SearchServer::IsStopWord(const std::string_view& word) const {
        return stop_words_.count(word);
}

bool SearchServer::IsValidWord(const std::string_view& word) {
        return std::none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
        });
}

std::vector<std::string_view> SearchServer::SplitIntoWordsNoStop(const std::string_view& text) const {
        std::vector<std::string_view> words;
        for (const std::string_view& word : SplitIntoWords(text)) {
            if (!IsValidWord(word)) {
                throw std::invalid_argument("Word "s + std::string(word) + " is invalid"s);
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

SearchServer::QueryWord SearchServer::ParseQueryWord(const std::string_view& text) const {
        if (text.empty()) {
            throw std::invalid_argument("Query word is empty"s);
        }
        std::string_view word = text;
        bool is_minus = false;
        if (word[0] == '-') {
            is_minus = true;
            word = word.substr(1);
        }
        if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
            throw std::invalid_argument("Query word is invalid"s);
        }

        return {word, is_minus, IsStopWord(word)};
}

SearchServer::Query SearchServer::ParseQuery(const std::string_view& text) const {
        SearchServer::Query result;
        for (const std::string_view& word : SplitIntoWords(text)) {
            const auto query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    result.minus_words.push_back(query_word.data);
                } else {
                    result.plus_words.push_back(query_word.data);
                }
            }
        }
    {
            std::sort(std::execution::seq,
                     std::begin(result.plus_words),
                     std::end(result.plus_words));
            auto it = std::unique(std::execution::seq,
                                 std::begin(result.plus_words),
                                 std::end(result.plus_words));
            result.plus_words.resize(std::distance(result.plus_words.begin(), it));
    }
    {
            std::sort(std::execution::seq,
                     std::begin(result.minus_words),
                     std::end(result.minus_words));
            auto it = std::unique(std::execution::seq,
                                 std::begin(result.minus_words),
                                 std::end(result.minus_words));
            result.minus_words.resize(std::distance(result.minus_words.begin(), it));
    }
        return result;
}

SearchServer::Query SearchServer::ParseQuery(std::execution::sequenced_policy, const std::string_view& text) const {
        return ParseQuery(text);
}

SearchServer::Query SearchServer::ParseQuery(std::execution::parallel_policy, const std::string_view& text) const {
        SearchServer::Query result;
        for (const std::string_view& word : SplitIntoWords(text)) {
            const auto query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    result.minus_words.push_back(query_word.data);
                } else {
                    result.plus_words.push_back(query_word.data);
                }
            }
        }
                    
        return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string_view& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

void AddDocument(SearchServer& search_server, int document_id, const std::string_view& document, DocumentStatus status,
                 const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка добавления документа "s << document_id << ": "s << e.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string_view& raw_query) {
    std::cout << "Результаты поиска по запросу: "s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка поиска: "s << e.what() << std::endl;
    }
}

void MatchDocument(SearchServer& search_server, const std::string_view& query) {
    MatchDocument(std::execution::seq, search_server, query);
}

void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}

const std::map<std::string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static std::map<std::string_view, double> result;
    if ( (document_to_word_freqs_.count(document_id) != 0) && (document_to_word_freqs_.at(document_id).size() != 0 ) ) {
        return document_to_word_freqs_.at(document_id);
    } else {
        return result;
    }
}

void RemoveDuplicates(SearchServer& search_server) {
    std::set<int> duplicates_id;
    std::map<std::set<std::string_view>, std::set<int>> work_base;
        for (const int id : search_server) {
            std::set<std::string_view> words_;
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