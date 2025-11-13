#pragma once
#include "parse_node.h"
#include "sort_params.h"
#include <string>

// Forward declarations
struct AstNode;
template<typename ParamType>
std::unique_ptr<AstNode> createAstNode(const ParamType& params);

struct SortNode : public TypedParseNode<SortNode, SortParams> {
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
    SortParams astParams() const override {
        return SortParams{
            .sortKeys = keys,
            .ascending = asc
        };
    }
    
    // Implementation in .cpp to avoid template instantiation order issues
    std::unique_ptr<AstNode> createAstNode() const override;
};
