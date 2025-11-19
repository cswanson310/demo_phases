#pragma once

#include <memory>
#include <stdexcept>
#include "param_type.h"
#include "parse_node.h"
#include "src/ast_nodes/limit_ast_node.h"
#include "src/ast_nodes/sort_ast_node.h"
#include "src/ast_nodes/set_metadata_ast_node.h"
#include "ast_node.h"

// Transforms a polymorphic ParseNode to its corresponding AstNode
std::unique_ptr<AstNode> parseToAst(const ParseNode& parseNode);
template <ParamType T>
std::unique_ptr<AstNode> createAstNode(const T& params) {
    throw std::runtime_error("hi");
}

// Template specialization declarations (definitions in node_transformer.cpp)
template <>
std::unique_ptr<AstNode> createAstNode(const LimitParams& params);

template <>
std::unique_ptr<AstNode> createAstNode(const SortParams& params);

template <>
std::unique_ptr<AstNode> createAstNode(const SetMetadataParams& params);

