#include "limit_node.h"
#include "parse_node.h"
#include "ast_node.h"
#include "src/ast_nodes/limit_ast_node.h"
#include <memory>

// Register the limit node factory at startup
REGISTER_NODE(limit, [](const std::string& argString) {
    return std::make_unique<LimitNode>(argString);
});

// Implementation of createAstNode - must be in .cpp to ensure
// the template specialization is fully defined before use
std::unique_ptr<ast_node::Node> LimitNode::createAstNode() const {
    return ast_node::create<LimitParams>(astParams());
}

