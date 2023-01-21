#pragma once
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <random>
#include <execution>

#include "search_server.h"
#include "process_queries.h"
#include "concurrent_map.h"
#include "test_framework.h"

using namespace std;

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
    
    std::cerr << "Test Add Document - OK\n"s;
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
    
    auto [words_, status_] = server.MatchDocument(request, 1);
    assert(words_.empty());
    std::cerr << "Test Stop Words - OK\n"s;
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
    
    std::cerr << "Test Minus Words - OK\n"s;
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
    
    std::cerr << "Test Descending Relevance - OK\n"s;
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
    
    std::cerr << "Test Predicate Filter - OK\n"s;
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
    
    std::cerr << "Test Document Status - OK\n"s;
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
    
    std::cerr << "Test Relevance Calculation - OK\n"s;
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
    
    std::cerr << "Test Matchins Docs - OK\n"s;
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
    
    std::cerr << "Test Top Documents - OK\n"s;
}

void TestMatchedSize(){
    SearchServer search_server("and with"s);
        
    int id = 0;
    for (
        const std::string& text : {
            "funny pet and nasty rat with"s, //1
            "funny pet with and with curly hair"s, //2
            "funny pet and not very nasty rat"s, //3
            "pet with rat and rat and rat"s, //4
            "nasty rat with curly hair"s, //5
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }
    
    const string query = "curly and funny with -not"s;
    
    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        //for (auto i : search_server.FindTopDocuments(query)) std::cout << i << std::endl;
        assert(words.size() == 1); // ожидаем 1
        //for (auto w : words) {
            //std::cout << w << " ";
        //}
        //std::cout << std::endl;
    }
    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        assert(words.size() == 2); // ожидаем 2
        //for (auto w : words) {
            //std::cout << w << " ";
        //}
        //std::cout << std::endl;
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        assert(words.size() == 0); // ожидаем 0
        //for (auto w : words) {
            //std::cout << w << " ";
        //}
        //std::cout << std::endl;
    }
    
    std::cerr << "Test Matched Size - OK\n"s;
}

void TestMyProcessQueries(){
    SearchServer search_server("and with"s);
    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }
    const vector<string> queries = {
        "nasty rat -not"s,
        "not very funny nasty pet"s,
        "curly hair"s
    };
    {
        LOG_DURATION("Process Queries seq"s);
        assert(std::abs(ProcessQueriesJoined(std::execution::seq, search_server, queries)[0].relevance - 0.183492) < ACCURACY);
        
    }
    {
        LOG_DURATION("Process Queries par"s);
        assert(std::abs(ProcessQueriesJoined(std::execution::par, search_server, queries)[2].relevance - 0.167358) < ACCURACY);
        
    }
    
    
    std::cerr << "Test Process Queries - OK? \n"s;  
}

void RunConcurrentUpdates(ConcurrentMap<int, int>& cm, size_t thread_count, int key_count) {
    auto kernel = [&cm, key_count](int seed) {
        vector<int> updates(key_count);
        iota(begin(updates), end(updates), -key_count / 2);
        shuffle(begin(updates), end(updates), mt19937(seed));

        for (int i = 0; i < 2; ++i) {
            for (auto key : updates) {
                ++cm[key].ref_to_value;
            }
        }
    };

    vector<future<void>> futures;
    for (size_t i = 0; i < thread_count; ++i) {
        futures.push_back(async(kernel, i));
    }
}

void TestConcurrentUpdate() {
    constexpr size_t THREAD_COUNT = 3;
    constexpr size_t KEY_COUNT = 50000;

    ConcurrentMap<int, int> cm(THREAD_COUNT);
    RunConcurrentUpdates(cm, THREAD_COUNT, KEY_COUNT);

    const auto result = cm.BuildOrdinaryMap();
    ASSERT_EQUAL(result.size(), KEY_COUNT);
    for (auto& [k, v] : result) {
        AssertEqual(v, 6, "Key = " + to_string(k));
    }
}

void TestConcurrentReadAndWrite() {
    ConcurrentMap<size_t, string> cm(5);

    auto updater = [&cm] {
        for (size_t i = 0; i < 50000; ++i) {
            cm[i].ref_to_value.push_back('a');
        }
    };
    auto reader = [&cm] {
        vector<string> result(50000);
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = cm[i].ref_to_value;
        }
        return result;
    };

    auto u1 = async(updater);
    auto r1 = async(reader);
    auto u2 = async(updater);
    auto r2 = async(reader);

    u1.get();
    u2.get();

    for (auto f : {&r1, &r2}) {
        auto result = f->get();
        ASSERT(all_of(result.begin(), result.end(), [](const string& s) {
            return s.empty() || s == "a" || s == "aa";
        }));
    }
}

void TestConcurrentSpeedup() {
    {
        ConcurrentMap<int, int> single_lock(1);

        LOG_DURATION("Single lock");
        RunConcurrentUpdates(single_lock, 4, 50000);
    }
    {
        ConcurrentMap<int, int> many_locks(3000);

        LOG_DURATION("3'000 locks");
        RunConcurrentUpdates(many_locks, 4, 50000);
    }
}

string GenerateWord(mt19937& generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution('a', 'z')(generator));
    }
    return word;
}

vector<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    words.erase(unique(words.begin(), words.end()), words.end());
    return words;
}

string GenerateQuery(mt19937& generator, const vector<string>& dictionary, int word_count, double minus_prob = 0) {
    string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        if (uniform_real_distribution<>(0, 1)(generator) < minus_prob) {
            query.push_back('-');
        }
        query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
    }
    return query;
}

vector<string> GenerateQueries(mt19937& generator, const vector<string>& dictionary, int query_count, int max_word_count) {
    vector<string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery(generator, dictionary, max_word_count));
    }
    return queries;
}

template <typename ExecutionPolicy>
void TestWithExecutionPolicy(string_view mark, const SearchServer& search_server, const vector<string>& queries, ExecutionPolicy&& policy) {
    LOG_DURATION(mark);
    double total_relevance = 0;
    for (const string_view query : queries) {
        for (const auto& document : search_server.FindTopDocuments(policy, query)) {
            total_relevance += document.relevance;
        }
    }
}

#define TEST(policy) TestWithExecutionPolicy(#policy, search_server, queries, execution::policy)

void TestWithExecutionPolicy_runner() {
    mt19937 generator;
    const auto dictionary = GenerateDictionary(generator, 1000, 10);
    const auto documents = GenerateQueries(generator, dictionary, 10'000, 70);
    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, {1, 2, 3});
    }
    const auto queries = GenerateQueries(generator, dictionary, 100, 70);
    std::cerr << "Execution With "s;
    TEST(seq);
    std::cerr << "Execution With "s;
    TEST(par);
    std::cerr << "Test ExecutionPolicy - OK? \n"s;  
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
    TestMatchedSize();
    TestMyProcessQueries();
    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestConcurrentReadAndWrite);
    RUN_TEST(tr, TestConcurrentSpeedup);
    TestWithExecutionPolicy_runner();
}

// --------- Окончание модульных тестов поисковой системы -----------