#include "node_transformer.h"
#include "parse_node.h"
#include "ast_node.h"
#include "ast_params.h"
#include "param_type.h"
#include "src/ast_nodes/limit_ast_node.h"
#include "src/ast_nodes/sort_ast_node.h"
#include "src/ast_nodes/set_metadata_ast_node.h"
#include <variant>

// Generate createAstNode specializations for all node types
// Automatically generated from ast_node_types.def
// To add a new node type, just add one line to ast_node_types.def
#define AST_NODE_TYPE(ParamType, AstNodeType) \
    template <> \
    std::unique_ptr<AstNode> createAstNode(const ParamType& params) { \
        return std::make_unique<AstNodeType>(params); \
    }
#include "ast_node_types.def"
#undef AST_NODE_TYPE

// Main transformation function - uses std::visit for compile-time dispatch
std::unique_ptr<AstNode> parseToAst(const ParseNode& parseNode) {
    AstParams params = parseNode.astParams();
    
    // std::visit dispatches to the correct createAstNode specialization
    // based on the actual type held in the variant
    return std::visit([](const auto& p) {
        return createAstNode(p);
    }, params);
}
