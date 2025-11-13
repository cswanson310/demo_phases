#include "set_metadata_node.h"
#include "parse_node.h"
#include "ast_node.h"
#include "src/ast_nodes/set_metadata_ast_node.h"
#include <memory>

// Register the set_metadata node factory at startup
REGISTER_PARSE_NODE(set_metadata, [](const std::string& argString) {
    return std::make_unique<SetMetadataNode>(argString);
});

// Implementation of createAstNode
std::unique_ptr<AstNode> SetMetadataNode::createAstNode() const {
    return ::createAstNode<SetMetadataParams>(astParams());
}

