#pragma once
#include "parse_node.h"
#include "foo_ast_params.h"
#include <string>

// Forward declare the ast_node namespace and create function
namespace ast_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

// FooNode uses the CRTP pattern with TypedNode
struct FooNode : public parse_node::TypedNode<FooNode, FooAstParams> {
    std::string data;
    
    FooNode(const std::string& inputData = "") : data(inputData) {}
    
    std::string get_shape() const override {
        return "foo_shape";
    }
    
    FooAstParams astParams() const override {
        FooAstParams params;
        params.nodeType = "foo";
        params.debugInfo = "FooNode from parse layer with data: " + data;
        params.fooSpecificData = 42;
        return params;
    }
    
    // Implement createAstNode using the type-specific template specialization
    // Implementation is in .cpp file to avoid template instantiation order issues
    std::unique_ptr<ast_node::Node> createAstNode() const override;
};

// Verify the concept is satisfied
static_assert(parse_node::ParseNodeType<FooNode>, "FooNode must satisfy ParseNodeType concept");
