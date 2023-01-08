#include <vector>
#include <string>
#include <functional>
#include <numeric>

#include "document.h"
#include "search_server.h"

class SearchServer;

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries);

std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries); 