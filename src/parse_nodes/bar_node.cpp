#include "bar_node.h"
#include "include/parse_node.h"
#include "src/ast_nodes/bar_ast_node.h"
#include <memory>

using namespace std::string_literals;

// Implementation of createAstNode - uses the template specialization from bar_ast_node.h
std::unique_ptr<ast_node::Node> BarNode::createAstNode() const {
    return ast_node::create<BarAstParams>(astParams());
}

// This static object's constructor runs before main()
// registering the "bar" node type automatically
REGISTER_NODE(bar, [](std::string argString) {
    return std::make_unique<BarNode>(argString);
});

