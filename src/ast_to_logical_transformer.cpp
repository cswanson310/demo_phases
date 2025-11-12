#include "ast_to_logical_transformer.h"
#include "ast_node.h"
#include "logical_node.h"
#include <stdexcept>

namespace ast_to_logical_transformer {

std::unique_ptr<logical_node::Node> astToLogical(const ast_node::Node& astNode) {
    // Use the virtual method to create the appropriate logical node
    return astNode.createLogicalNode();
}

} // namespace ast_to_logical_transformer

