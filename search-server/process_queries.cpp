#include "process_queries.h"

std::vector<std::vector<Document>> ProcessQueries(
   const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> results(queries.size());
    std::transform(std::execution::seq, queries.begin(), queries.end(), results.begin(),
                   [&search_server](const std::string& query){
                       return search_server.FindTopDocuments(query);
                   });
    return results;
}


std::vector<Document> ProcessQueriesJoined(
const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        std::vector<std::vector<Document>> results = ProcessQueries(search_server, queries);
        size_t documents_count = transform_reduce(std::execution::seq,
                                            results.begin(),
                                            results.end(),
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