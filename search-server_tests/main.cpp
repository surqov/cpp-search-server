#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}
    
struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }    
    
    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, 
            DocumentData{
                ComputeAverageRating(ratings), 
                status
            });
    }

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus f_status = DocumentStatus::ACTUAL) const {           
        return FindTopDocuments(raw_query, [f_status](int document_id, DocumentStatus status, int rating) { return status == f_status;});
    }
    
    template <typename Predicate>
    vector<Document> FindTopDocuments(const string& raw_query, Predicate predicate) const {            
        const Query query = ParseQuery(raw_query);
        vector<Document> matched_documents;
        matched_documents = FindAllDocuments(query, predicate);
        
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < ACCURACY) {
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

    int GetDocumentCount() const {
        return documents_.size();
    }
   
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
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
    
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }
    
    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0); // завернул в accumulate
        
        return rating_sum / static_cast<int>(ratings.size());
    }
    
    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };
    
    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
    }
    
    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };
    
    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }
    
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }
      
    template <typename Predicate>  
    vector<Document> FindAllDocuments(const Query& query, Predicate predicate) const {
        map<int, double> document_to_relevance;
 
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;                 
            }
        }
        
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> bar, matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        
        copy_if(matched_documents.begin(), matched_documents.end(), std::back_inserter(bar), [predicate, this](const auto& item) {return (predicate(item.id, documents_.at(item.id).status, item.rating));});   
  
        return bar;
    }
};

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating
         << " }"s << endl;
}

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        assert(server.FindTopDocuments("in"s).empty());
    }
}

//тест на добавление документа
void TestAddDocument(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const string document1 = "new fresh big orange"s;
    const string document2 = "nice stifler's mom"s;
    const string stop_words = "and or if big";
    const DocumentStatus status = DocumentStatus::ACTUAL;
    const vector<int> ratings = {-8, 3, 12, 5, 0};

    const string request1 = "fresh and big fish";
    const string request2 = "mom";

    SearchServer server;
    server.SetStopWords(stop_words);
    server.AddDocument(doc_id1, document1, status, ratings);
    server.AddDocument(doc_id2, document2, status, ratings);
    auto doc1 = server.FindTopDocuments(request1);
    auto doc2 = server.FindTopDocuments(request2);
    assert(doc1[0].id == 1);    
    assert(doc2[0].id == 2);
    assert(server.GetDocumentCount() == 2); 
}


//тест на исключение стоп слов
void TestStopWords(){
    const int doc_id = 1;
    const string document = "new and fresh big orange or apple"s;
    const string stop_words = "and or if big";
    const DocumentStatus status = DocumentStatus::ACTUAL;
    const vector<int> ratings = {-8, 3, 12, 5, 0};

    const string request = "and or if big";

    SearchServer server;
    server.SetStopWords(stop_words);
    server.AddDocument(doc_id, document, status, ratings);
    assert(server.FindTopDocuments(request).empty()); 
}

//документы с минус словами не должны включаться в результаты поиска
void TestMinusWords(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const int doc_id3 = 3;
    const string document1 = "new fresh big orange"s;
    const string document2 = "nice stifler mom"s;
    const string document3 = "so thats it"s;
    
    const string request = "fresh and -big fish";

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {3, 4, 5});
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8});
    assert(server.FindTopDocuments(request).empty());
}

//матчинг документов - вернуть все слова из запроса, которые есть в документе
//если встретилось хоть одно минус слово - тест отвалится
void TestMatchingDocs(){
    const int doc_id1 = 1;
    const string document1 = "new fresh big orange"s;
    const string stop_words = "and or if big";
    const string request = "fresh and big fish";

    SearchServer server;
    server.SetStopWords(stop_words);
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});

    vector<string> matched_words = get<0>(server.MatchDocument(document1, doc_id1));
    for (const string& stop_word : SplitIntoWords(stop_words)) {
        assert(count(matched_words.begin(), matched_words.end(), stop_word) == 0);
    }
}

//документы по убыванию релевантности
void TestDescendingRelevance(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const int doc_id3 = 3;
    const string document1 = "new fresh big orange"s;
    const string document2 = "tasty fish"s;
    const string document3 = "big wheel for my car"s;
    
    const string request = "fresh and big fish";

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {3, 4, 5});
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8});

    double relevance1, relevance2, relevance3;
    relevance1 = server.FindTopDocuments(request).at(0).relevance;
    relevance2 = server.FindTopDocuments(request).at(1).relevance;
    relevance3 = server.FindTopDocuments(request).at(2).relevance;
    assert(relevance1 > relevance2 > relevance3);
}

void TestPredicateFilter(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const int doc_id3 = 3;
    const string document1 = "new fresh big orange"s;
    const string document2 = "tasty fish"s;
    const string document3 = "big wheel for my car"s;
    
    const string request = "fresh and big fish";

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {3, 4, 5});
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8});
    auto predicate = [](int document_id, DocumentStatus , int ){ return document_id % 3 == 0;};
    assert(server.FindTopDocuments(request, predicate).at(0).id == 3);
}

void TestDocumentsStatus(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const int doc_id3 = 3;
    const string document1 = "new fresh big orange"s;
    const string document2 = "tasty fish"s;
    const string document3 = "big wheel for my car"s;
    
    const string request = "fresh and big fish";

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::BANNED, {3, 4, 5});
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8});

    assert(server.FindTopDocuments(request, DocumentStatus::BANNED).at(0).id == 2);
}

void TestRelevanceCalc(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const int doc_id3 = 3;
    const string document1 = "new fresh big orange"s;
    const string document2 = "tasty fish"s;
    const string document3 = "big wheel for my car"s;
    
    const string request = "fresh and big fish";

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {3, 4, 5});
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8});

    double relevance1, relevance2, relevance3;
    double ref_relevance1 = 0.549306, 
            ref_relevance2 = 0.376019, 
            ref_relevance3 = 0.081093;
    const double curr_accuracy = 1e-6;
    relevance1 = server.FindTopDocuments(request).at(0).relevance;
    relevance2 = server.FindTopDocuments(request).at(1).relevance;
    relevance3 = server.FindTopDocuments(request).at(2).relevance;
    assert(abs(relevance1 - ref_relevance1) < curr_accuracy);
    assert(abs(relevance2 - ref_relevance2) < curr_accuracy);
    assert(abs(relevance3 - ref_relevance3) < curr_accuracy);
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestStopWords();
    TestMinusWords();
    TestMatchingDocs();
    TestDescendingRelevance();
    TestPredicateFilter();
    TestDocumentsStatus(); 
    TestRelevanceCalc();       
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}