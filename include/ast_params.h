#pragma once
#include <variant>

// Include actual parameter type definitions
#include "limit_params.h"
#include "sort_params.h"
#include "set_metadata_params.h"

// Dummy type to handle trailing comma from X-macro
// This should never be instantiated - it only exists to make the preprocessor happy
struct __AstParams_TrailingComma_Sentinel {};

// Variant holding all possible AST parameter types
// Automatically generated from ast_node_types.def
// To add a new node type, just add one line to ast_node_types.def
#define AST_NODE_TYPE(ParamType, AstNodeType) ParamType,
using AstParams = std::variant<
#include "ast_node_types.def"
    __AstParams_TrailingComma_Sentinel  // Absorbs trailing comma
>;
#undef AST_NODE_TYPE
