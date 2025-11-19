#include "limit_node.h"
#include "parse_node.h"
#include <memory>

// Register the limit node factory at startup
REGISTER_PARSE_NODE(limit, [](const std::string& argString) {
    return std::make_unique<LimitNode>(argString);
});
