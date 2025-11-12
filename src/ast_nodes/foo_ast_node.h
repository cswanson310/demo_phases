#pragma once
#include "ast_node.h"
#include "foo_ast_params.h"
#include "foo_logical_params.h"
#include <string>

// Forward declare logical_node namespace
namespace logical_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

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
    
    // Foo has its own logical params type
    FooLogicalParams logicalParams() const {
        return FooLogicalParams{
            .nodeType = "foo",
            .optimizationHint = "can_be_pushed_down",
            .costMultiplier = fooData > 0 ? 2 : 1
        };
    }
    
    // Implement createLogicalNode - declared in .cpp to avoid circular deps
    std::unique_ptr<logical_node::Node> createLogicalNode() const override;
};

// Specialize the create function for FooAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<FooAstParams>(const FooAstParams& params) {
        return std::make_unique<FooAstNode>(params);
    }
}
