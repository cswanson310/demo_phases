#pragma once
#include "parse_node.h"
#include "limit_params.h"
#include <string>

// Forward declarations
struct AstNode;
template<typename ParamType>
std::unique_ptr<AstNode> createAstNode(const ParamType& params);

struct LimitNode : public TypedParseNode<LimitNode, LimitParams> {
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
    std::unique_ptr<AstNode> createAstNode() const override;
};
