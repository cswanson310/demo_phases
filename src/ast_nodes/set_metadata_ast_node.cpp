#include "set_metadata_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/set_metadata_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
std::unique_ptr<LogicalNode> SetMetadataAstNode::createLogicalNode() const {
    return ::createLogicalNode<SetMetadataParams>(logicalParams());
}
