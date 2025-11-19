#pragma once
#include <string>
#include <memory>
#include "parse_node.h"
#include "src/ast_params/set_metadata_params.h"

struct SetMetadataNode : public ParseNode {
    std::string metaName;
    std::string expression;
    
    SetMetadataNode(const std::string& arg) {
        // Parse input like "name:expression" or just use defaults
        size_t colonPos = arg.find(':');
        if (colonPos != std::string::npos) {
            metaName = arg.substr(0, colonPos);
            expression = arg.substr(colonPos + 1);
        } else {
            metaName = "default_meta";
            expression = arg;
        }
    }
    
    std::string get_shape() const override {
        return "set_metadata_shape";
    }
    
    // Returns type-specific AST parameters
    AstParams astParams() const override {
        SetMetadataParams params;
        params.metaName = metaName;
        params.expression = expression;
        return params;
    }
};

