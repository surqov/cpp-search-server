#include <vector>
#include <string>
#include <functional>
#include <numeric>
#include <string_view>
#include "search_server.h"
#include "document.h"

class SearchServer;

std::vector<std::vector<Document>> ProcessQueries(
        const SearchServer& search_server,
        const std::vector<std::string>& queries);


std::vector<Document> ProcessQueriesJoined(
        const SearchServer& search_server,
        const std::vector<std::string>& queries);