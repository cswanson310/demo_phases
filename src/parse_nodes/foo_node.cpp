#include "foo_node.h"
#include "include/parse_node.h"
#include "src/ast_nodes/foo_ast_node.h"  // Include the full AST node definition here
#include <memory>

using namespace std::string_literals;

// Implementation of createAstNode - uses the template specialization from foo_ast_node.h
std::unique_ptr<ast_node::Node> FooNode::createAstNode() const {
    return ast_node::create<FooAstParams>(astParams());
}

// This static object's constructor runs before main()
// registering the "foo" node type automatically
REGISTER_NODE(foo, [](std::string argString) {
    return std::make_unique<FooNode>(argString);
});