#pragma once
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "search_server.h"

using namespace std;

#define ASSERT(expr) AssertImpl((expr), #expr, __FUNCTION__, __FILE__,__LINE__)

#define ASSERT_HINT(expr, hint) AssertImpl((expr), #expr, __FUNCTION__, __FILE__,__LINE__, hint)

#define ASSERT_EQUAL(value1, value2) AssertImpl((value1 == value2), #value1 == #value2, __FUNCTION__, __FILE__,__LINE__)

// -------- Начало модульных тестов поисковой системы ----------

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
        cerr << file_name << "("s << line_number << "): "s << func_name << ": "s << "ASSERT("s << value1 << " != "s << value2 << ")"s << " failed."s;
        if (!hint.empty()) {
            cerr << " Hint: " << hint;
        }
        cerr << endl;
        abort();
    }
}


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
        SearchServer server("in the"s);
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

    SearchServer server(stop_words);
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

    SearchServer server(stop_words);
    server.AddDocument(doc_id, document, status, ratings);
    ASSERT(server.FindTopDocuments(request).empty()); 
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
    ASSERT(server.FindTopDocuments(request).empty());
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
    ASSERT(abs(relevance1 - ref_relevance1) < curr_accuracy);
    ASSERT(abs(relevance2 - ref_relevance2) < curr_accuracy);
    ASSERT(abs(relevance3 - ref_relevance3) < curr_accuracy);
}

//матчинг документов - вернуть все слова из запроса, которые есть в документе
//если встретилось хоть одно минус слово - тест отвалится
void TestMatchingDocs(){
    const int doc_id1 = 1;
    const string document1 = "new fresh big orange"s;
    const string stop_words = "and or if big";
    const string request = "fresh and big fish";

    SearchServer server(stop_words);
    server.AddDocument(doc_id1, document1, DocumentStatus::ACTUAL, {1, 2, 3});

    vector<string_view> matched_words = get<0>(server.MatchDocument(document1, doc_id1));
    for (const string_view& stop_word : SplitIntoWords(stop_words)) {
        assert(count(matched_words.begin(), matched_words.end(), stop_word) == 0);
    }
}

void TestMyTopDocuments(){
    const std::vector<int> doc_id = {1, 2, 3};
    const std::vector<std::string> documents = {"new fresh big orange"s, "new orange"s, "orange"s};
    const std::string stop_words = "and or if big";
    const std::string request = "fresh and big orange";
    
    SearchServer server(stop_words);
    std::for_each(std::begin(doc_id),
                 std::end(doc_id),
                 [&server, &documents](const int id){
                    server.AddDocument(id, documents.at(id-1), DocumentStatus::ACTUAL, {1, 2, 3});
                 });
    assert(server.FindTopDocuments(request).at(0).id == 1);
}

void TestFromStudencov(){
    SearchServer search_server("and with"s);
        
    int id = 0;
    for (
        const std::string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    const string query = "curly and funny -not"s;

    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        cout << words.size() << " words for document 1"s << endl; // ожидаем 1
        for (auto w : words) {
            std::cout << w << " ";
        }
        std::cout << std::endl;
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        std::cout << words.size() <<  " words for document 2"s << endl; // ожидаем 2
        for (auto w : words) {
            std::cout << w << " ";
        }
        std::cout << std::endl;
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        cout << words.size() << " words for document 3"s << endl; // ожидаем 0
        for (auto w : words) {
            std::cout << w << " ";
        }
        std::cout << std::endl;
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestStopWords();   
    TestMinusWords();  
    TestDescendingRelevance();
    TestPredicateFilter();
    TestDocumentsStatus();
    TestRelevanceCalc();
    TestMatchingDocs();
    TestMyTopDocuments();
    //TestFromStudencov();
}

// --------- Окончание модульных тестов поисковой системы -----------