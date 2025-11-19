#include "limit_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/limit_logical_node.h"
#include <memory>

// Specialize the create function for LimitParams
template<>
inline std::unique_ptr<LogicalNode> createLogicalNode<LimitParams>(const LimitParams& params) {
    return std::make_unique<LimitLogicalNode>(params);
}

// Specialize the create function for LimitParams
template<>
inline std::unique_ptr<AstNode> createAstNode(const LimitParams& params) {
    return std::make_unique<LimitAstNode>(params);
}

// Implementation of createLogicalNode
std::unique_ptr<LogicalNode> LimitAstNode::createLogicalNode() const {
    return ::createLogicalNode<LimitParams>(logicalParams());
}
