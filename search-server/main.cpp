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

#define RUN_TEST(func) RunTestImpl((func), #func)

template <typename Function>
void RunTestImpl(const Function& test, const string& test_name) {
    test();
    cerr << test_name << " OK"s << endl;
}

#define ASSERT(expr) AssertImpl((expr), #expr, __FUNCTION__, __FILE__,__LINE__)
#define ASSERT_HINT(expr, hint) AssertImpl((expr), #expr, __FUNCTION__, __FILE__,__LINE__, hint)
#define ASSERT_EQUAL(value1, value2) AssertImpl((value1 == value2), #value1, #value2, __FUNCTION__, __FILE__,__LINE__)
#define ASSERT_EQUAL_HINT(value1, value2, hint) AssertImpl((value1 == value2), #value1, #value2, __FUNCTION__, __FILE__,__LINE__, hint)

void AssertImpl(const bool& value, const string& expr, const string& func_name, const string& file_name, const unsigned& line_number, const string& hint = ""s ){
    if (!value) {
        cerr << file_name << "("s << line_number << "): "s << func_name << ": "s << "ASSERT("s << expr << ")"s << " failed."s;
        if (!hint.empty()) {
            cerr << " Hint: " << hint;
        }
        cerr << endl;
        abort();
    }
}

void AssertImpl(const bool& value, const string& value1, const string& value2, const string& func_name, const string& file_name, const unsigned& line_number, const string& hint = ""s ){
    if (!value) {
        cerr << file_name << "("s << line_number << "): "s << func_name << ": "s << "ASSERT("s << value1 << " == "s << value2 << ")"s << " failed."s;
        if (!hint.empty()) {
            cerr << " Hint: " << hint;
        }
        cerr << endl;
        abort();
    }
}

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
    int id = 0;
    double relevance = 0.0;
    int rating = 0;

    Document() = default;
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
        return FindTopDocuments(raw_query, [f_status](int , DocumentStatus status, int ) { return status == f_status;});
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
    
    set<string> GetMinusWordsFromLine(const string& text) {
        return ParseQuery(text).minus_words;
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
        ASSERT_EQUAL(found_docs.size(), 1);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
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
    ASSERT_EQUAL(doc1[0].id, 1);    
    ASSERT_EQUAL(doc2[0].id, 2);
    ASSERT_EQUAL(server.GetDocumentCount(), 2); 
}


//тест на исключение стоп слов
void TestAddStopWords(){
    const int doc_id = 1;
    const string document = "new and fresh big orange or apple"s;
    const string stop_words = "and or if big";
    const DocumentStatus status = DocumentStatus::ACTUAL;
    const vector<int> ratings = {-8, 3, 12, 5, 0};

    const string request = "and or if big";

    SearchServer server;
    server.SetStopWords(stop_words);
    server.AddDocument(doc_id, document, status, ratings);
    ASSERT(server.FindTopDocuments(request).empty()); 
}

//документы с минус словами не должны включаться в результаты поиска
void TestAddMinusWords(){
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
    ASSERT(server.FindTopDocuments(request).empty());
}

//матчинг документов - вернуть все слова из запроса, которые есть в документе
//если встретилось хоть одно минус слово - тест отвалится
//тестируем и стоп слова и минус слова
//поверяем корректность слов в списке
void TestMatchingDocs(){
    const int doc_id1 = 1;
    const int doc_id2 = 2;
    const string document1 = "new and fresh big apple"s;
    const string document2 = "new and fresh big fish"s;
    const string stop_words = "and or if big"s;
    const string request = "fresh and nice big -fish"s;

    SearchServer server;
    server.SetStopWords(stop_words);
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {1, 3, 2});

    auto [matched_words, status]= server.MatchDocument(document1, doc_id1);
    for (const string& stop_word : SplitIntoWords(stop_words)) {
        ASSERT_EQUAL(count(matched_words.begin(), matched_words.end(), stop_word), 0);
    }

    for (const auto& item : server.FindTopDocuments(request)) {
        ASSERT(item.id != doc_id2);
    }

    vector<string> ref_matched_words = {"apple"s, "fresh"s, "new"s};
    for (int i = 0; i < static_cast<int>(matched_words.size()); ++i) {
        ASSERT_EQUAL(matched_words[i], ref_matched_words[i]);
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
    const auto response = server.FindTopDocuments(request);
    relevance1 = response.at(0).relevance;
    relevance2 = response.at(1).relevance;
    relevance3 = response.at(2).relevance;
    ASSERT((relevance1 > relevance2) && (relevance2 > relevance3));
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
    const auto response = server.FindTopDocuments(request, predicate);
    ASSERT_EQUAL(response.at(0).id, 3);
    ASSERT_EQUAL(response.size(), 1);
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

    ASSERT_EQUAL(server.FindTopDocuments(request, DocumentStatus::BANNED).at(0).id, 2);
}

void TestRelevanceCalc(){
    const int doc_id1 = 1; 
    const int doc_id2 = 2; 
    const int doc_id3 = 3; 
    const string document1 = "new fresh big orange"s; 
    const string document2 = "tasty fish"s; 
    const string document3 = "big wheel for my car"s; 

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3}); 
    server.AddDocument(doc_id2, document2, DocumentStatus::ACTUAL, {3, 4, 5}); 
    server.AddDocument(doc_id3, document3, DocumentStatus::ACTUAL, {6, 7, 8}); 

    const string request = "fresh and big fish"; 

    double relevance1 = 0.0, ref_relevance = 0.0;
    vector<double> ref_TF, ref_IDF;
    vector<string> splitted_request = SplitIntoWords(request);
    vector<vector<string>> base = {SplitIntoWords(document1), SplitIntoWords(document2), SplitIntoWords(document3)};

    for (const string& word : splitted_request) {
        int word_count = 0;
        for (const auto& item : base) {
            if (count(item.begin(), item.end(), word) > 0) {
                ++word_count;
            }
        }
        if (word_count == 0) {
            ref_IDF.push_back(0.0);
            continue;    
        } 
        ref_IDF.push_back(log(base.size() * 1.0 / word_count));
    }
    int doc_id = 1;

    for (int i = 0; i < static_cast<int>(splitted_request.size()); ++i) {
        ref_relevance += (count(base.at(doc_id).begin(), base.at(doc_id).end(), splitted_request.at(i)) * 1.0 / base.at(doc_id).size()) * ref_IDF[i];
    }

    const double curr_accuracy = 1e-6;
    relevance1 = server.FindTopDocuments(request).at(0).relevance;
    ASSERT(abs(relevance1 - ref_relevance) < curr_accuracy);
}

//проверяем корректность вычисления рейтинга
void TestCalcDocumentRating(){
    const int doc_id1 = 1; 
    const string document1 = "new fresh big orange"s; 
    const string request = "fresh and big fish"; 
    const vector<int> rating = {1, 3, 2};

    SearchServer server;
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, rating);
    const double ref_rating = (1 + 3 + 2) * 1.0 / 3;
    ASSERT_EQUAL(server.FindTopDocuments(request).at(0).rating, ref_rating);
}   



// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestAddStopWords();   
    TestAddMinusWords();  
    TestMatchingDocs();
    TestDescendingRelevance();
    TestPredicateFilter();
    TestDocumentsStatus();
    TestRelevanceCalc();
    TestCalcDocumentRating();          
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;

    SearchServer search_server;
    search_server.SetStopWords("и в на"s);
    search_server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    search_server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
    search_server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});
    cout << "ACTUAL by default:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s)) {
        PrintDocument(document);
    }
    cout << "BANNED:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, DocumentStatus::BANNED)) {
        PrintDocument(document);
    }
    cout << "Even ids:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }
}