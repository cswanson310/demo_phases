#include "limit_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/limit_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
// Limit uses type-specific LimitLogicalParams with template specialization
std::unique_ptr<logical_node::Node> LimitAstNode::createLogicalNode() const {
    return logical_node::create<LimitParams>(logicalParams());
}

// The create<LimitAstParams> specialization is already in the header
// No static registration needed since we use template specialization

