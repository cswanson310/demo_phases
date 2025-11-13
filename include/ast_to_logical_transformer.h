#pragma once
#include <memory>
#include "ast_node.h"
#include "logical_node.h"

// Transforms a polymorphic AstNode to its corresponding LogicalNode
std::unique_ptr<LogicalNode> astToLogical(const AstNode& astNode);
