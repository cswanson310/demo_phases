#include "include/ast_to_logical_transformer.h"
#include "include/ast_node.h"
#include "include/logical_node.h"

std::unique_ptr<LogicalNode> astToLogical(const AstNode& astNode) {
    // This relies on the virtual createLogicalNode() method implemented by each concrete AST node
    return astNode.createLogicalNode();
}
