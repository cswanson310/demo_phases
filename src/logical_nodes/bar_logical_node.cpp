#include "bar_logical_node.h"
#include "logical_node.h"
#include <memory>

// Register the bar logical node factory at startup
REGISTER_LOGICAL_NODE(bar, [](const std::string& nodeType, const std::string& optimizationHint) {
    return std::make_unique<BarLogicalNode>(nodeType, optimizationHint);
});

