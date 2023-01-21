#include <vector>
#include <string>
#include <functional>
#include <numeric>

#include "search_server.h"
#include "document.h"

template <typename ExecutionPolicy>
std::vector<std::vector<Document>> ProcessQueries(
    ExecutionPolicy&& policy,
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> results(queries.size());
    std::transform(std::execution::par, std::begin(queries), std::end(queries), std::begin(results),
                   [&search_server, policy](const std::string& query){
                       return search_server.FindTopDocuments(policy, query);
                   });
    return results;
}

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries); 

template <typename ExecutionPolicy>
std::vector<Document> ProcessQueriesJoined(
    ExecutionPolicy&& policy,
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        std::vector<std::vector<Document>> results = std::move(ProcessQueries(policy, search_server, queries));
        size_t documents_count = transform_reduce(std::execution::par,
                                            std::begin(results),
                                            std::end(results),
                                            std::size_t(0),
                                            std::plus<>{},
                                            [](const auto& results_vector) {
                                                return results_vector.size();
                                            });
        std::vector<Document> flat_results;
        flat_results.reserve(documents_count);
        for (auto result : results) {
            std::move(result.begin(), result.end(), std::back_inserter(flat_results));
        }
        return flat_results;                                  
}

std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);