#pragma once
#include <string>
#include "ast_params.h"

// Parameters for creating LimitAstNode from LimitNode
// Also used for creating LimitLogicalNode from LimitAstNode
struct LimitParams {
    int limitValue = 0;  // The number of rows to limit
};

