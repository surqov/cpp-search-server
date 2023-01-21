#include "process_queries.h"

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        return ProcessQueries(std::execution::seq, search_server, queries);
}

std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
        return ProcessQueriesJoined(std::execution::seq, search_server, queries);
}