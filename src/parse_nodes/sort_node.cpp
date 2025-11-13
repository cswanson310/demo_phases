#include "sort_node.h"
#include "parse_node.h"
#include "ast_node.h"
#include "src/ast_nodes/sort_ast_node.h"
#include <memory>

// Register the sort node factory at startup
REGISTER_PARSE_NODE(sort, [](const std::string& argString) {
    return std::make_unique<SortNode>(argString);
});

// Implementation of createAstNode
std::unique_ptr<AstNode> SortNode::createAstNode() const {
    return ::createAstNode<SortParams>(astParams());
}
