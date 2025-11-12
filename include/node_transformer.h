#pragma once
#include <memory>
#include <string>

// Forward declarations
namespace parse_node {
    struct Node;
}

namespace ast_node {
    struct Node;
}

namespace node_transformer {

// Transform a parse node to an AST node
// This uses the parse node's type-specific params and creates the appropriate AST node
std::unique_ptr<ast_node::Node> parseToAst(const parse_node::Node& parseNode);

// Convenience overload for unique_ptr
inline std::unique_ptr<ast_node::Node> parseToAst(const std::unique_ptr<parse_node::Node>& parseNode) {
    return parseToAst(*parseNode);
}

} // namespace node_transformer

