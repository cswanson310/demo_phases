#include "sort_node.h"
#include "parse_node.h"
#include <memory>

// Register the sort node factory at startup
REGISTER_PARSE_NODE(sort, [](const std::string& argString) {
    return std::make_unique<SortNode>(argString);
});
