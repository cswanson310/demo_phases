#include "limit_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/limit_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
std::unique_ptr<LogicalNode> LimitAstNode::createLogicalNode() const {
    return ::createLogicalNode<LimitParams>(logicalParams());
}
