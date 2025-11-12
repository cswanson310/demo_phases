#include "bar_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/bar_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
// Bar uses type-specific BarLogicalParams with template specialization
std::unique_ptr<logical_node::Node> BarAstNode::createLogicalNode() const {
    return logical_node::create<BarLogicalParams>(logicalParams());
}

// The create<BarAstParams> specialization is already in the header
// No static registration needed since we use template specialization

