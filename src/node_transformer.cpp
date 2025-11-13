#include "include/node_transformer.h"
#include "include/parse_node.h"
#include "include/ast_node.h"

std::unique_ptr<AstNode> parseToAst(const ParseNode& parseNode) {
    // This relies on the virtual createAstNode() method implemented by each concrete parse node
    return parseNode.createAstNode();
}
