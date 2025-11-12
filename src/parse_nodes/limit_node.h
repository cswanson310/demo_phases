#pragma once
#include "parse_node.h"
#include "limit_params.h"
#include <string>

// Forward declarations for createAstNode
namespace ast_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

struct LimitNode : public parse_node::TypedNode<LimitNode, LimitParams> {
    int limitValue;
    
    LimitNode(const std::string& arg) : limitValue(std::stoi(arg)) {}
    
    std::string get_shape() const override {
        return "limit_shape";
    }
    
    // Returns type-specific AST parameters
    LimitParams astParams() const override {
        return {.limitValue=limitValue};
    }
    
    // Implementation in .cpp to avoid template instantiation order issues
    std::unique_ptr<ast_node::Node> createAstNode() const override;
};

