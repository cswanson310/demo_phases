#pragma once
#include "ast_node.h"
#include "foo_ast_params.h"
#include <string>

struct FooAstNode : public ast_node::Node {
    std::string nodeType;
    std::string debugInfo;
    int fooData;
    
    FooAstNode(const FooAstParams& params)
        : nodeType(params.nodeType)
        , debugInfo(params.debugInfo)
        , fooData(params.fooSpecificData) {}
    
    std::string debugName() const override {
        return "FooAstNode[" + nodeType + "]: " + debugInfo + " (data=" + std::to_string(fooData) + ")";
    }
    
    LogicalParams logicalParams() const override {
        return LogicalParams{
            .nodeType = "foo",
            .optimizationHint = "can_be_pushed_down"
        };
    }
};

// Specialize the create function for FooAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<FooAstParams>(const FooAstParams& params) {
        return std::make_unique<FooAstNode>(params);
    }
}
