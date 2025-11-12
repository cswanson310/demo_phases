#include "foo_ast_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "src/logical_nodes/foo_logical_node.h"
#include <memory>

// Implementation of createLogicalNode
// Foo uses type-specific FooLogicalParams with template specialization
std::unique_ptr<logical_node::Node> FooAstNode::createLogicalNode() const {
    return logical_node::create<FooLogicalParams>(logicalParams());
}

// The create<FooAstParams> specialization is already in the header
// No static registration needed since we use template specialization

// If you want runtime polymorphic registration, you can add it here
// But the template approach is more type-safe and doesn't require alwayslink
