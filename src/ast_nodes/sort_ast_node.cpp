#include "sort_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/sort_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
std::unique_ptr<LogicalNode> SortAstNode::createLogicalNode() const {
    return ::createLogicalNode<SortParams>(logicalParams());
}
