#include "foo_logical_node.h"
#include "logical_node.h"
#include <memory>

// Register the foo logical node factory at startup
REGISTER_LOGICAL_NODE(foo, [](const std::string& nodeType, const std::string& optimizationHint) {
    return std::make_unique<FooLogicalNode>(nodeType, optimizationHint);
});

