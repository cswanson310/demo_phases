#pragma once
#include <string>
#include <memory>
#include <vector>
#include "parse_node.h"
#include "src/ast_params/sort_params.h"

struct SortNode : public ParseNode {
    std::vector<std::string> keys;
    bool asc;
    
    SortNode(const std::string& arg) {
        // Parse input like "field1,field2:desc" or "field1:asc"
        if (arg.find(":desc") != std::string::npos) {
            asc = false;
        } else {
            asc = true;
        }
        // For simplicity, just use two example keys
        keys = {"field1", "field2"};
    }
    
    std::string get_shape() const override {
        return "sort_shape";
    }
    
    // Returns type-specific AST parameters
    AstParams astParams() const override {
        SortParams params;
        params.sortKeys = keys;
        params.ascending = asc;
        return params;
    }
};
