#pragma once
#include <memory>

// Forward declarations
namespace ast_node {
    struct Node;
}

namespace logical_node {
    struct Node;
}

namespace ast_to_logical_transformer {

// Transform an AST node to a logical node
// This uses the AST node's logical params and creates the appropriate logical node
std::unique_ptr<logical_node::Node> astToLogical(const ast_node::Node& astNode);

// Convenience overload for unique_ptr
inline std::unique_ptr<logical_node::Node> astToLogical(const std::unique_ptr<ast_node::Node>& astNode) {
    return astToLogical(*astNode);
}

} // namespace ast_to_logical_transformer

