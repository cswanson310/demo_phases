#include "node_transformer.h"
#include "parse_node.h"
#include "ast_node.h"
#include <stdexcept>

namespace node_transformer {

std::unique_ptr<ast_node::Node> parseToAst(const parse_node::Node& parseNode) {
    // This will use the virtual method we'll add to parse_node::Node
    return parseNode.createAstNode();
}

} // namespace node_transformer

