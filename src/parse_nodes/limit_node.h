#pragma once

#include <string>
#include <memory>
#include "parse_node.h"
#include "src/ast_params/limit_params.h"

struct LimitNode : public ParseNode {
    int limitValue;
    
    LimitNode(const std::string& arg) : limitValue(std::stoi(arg)) {}
    
    std::string get_shape() const override {
        return "limit_shape";
    }
    
    // Returns type-specific AST parameters
    AstParams astParams() const override {
        LimitParams params;
        params.limitValue = limitValue;
        return params;
    }
};
