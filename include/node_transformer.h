#pragma once
#include <memory>
#include "parse_node.h"
#include "ast_node.h"

// Transforms a polymorphic ParseNode to its corresponding AstNode
std::unique_ptr<AstNode> parseToAst(const ParseNode& parseNode);
