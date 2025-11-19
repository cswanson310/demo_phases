#include "set_metadata_node.h"
#include "parse_node.h"
#include <memory>

// Register the set_metadata node factory at startup
REGISTER_PARSE_NODE(set_metadata, [](const std::string& argString) {
    return std::make_unique<SetMetadataNode>(argString);
});

