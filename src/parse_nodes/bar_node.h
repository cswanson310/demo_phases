#pragma once
#include "parse_node.h"
#include "bar_ast_params.h"
#include <string>
#include <vector>

// Forward declare the ast_node namespace and create function
namespace ast_node {
    struct Node;
    template<typename ParamType>
    std::unique_ptr<Node> create(const ParamType& params);
}

// BarNode demonstrates a different node type with different parameters
struct BarNode : public parse_node::TypedNode<BarNode, BarAstParams> {
    std::string data;
    std::vector<std::string> items;
    
    BarNode(const std::string& inputData = "") : data(inputData) {
        // Parse comma-separated items from input
        if (!data.empty()) {
            items.push_back(data);
            items.push_back(data + "_processed");
        }
    }
    
    std::string get_shape() const override {
        return "bar_shape";
    }
    
    BarAstParams astParams() const override {
        BarAstParams params;
        params.nodeType = "bar";
        params.debugInfo = "BarNode from parse layer with " + std::to_string(items.size()) + " items";
        params.barItems = items;
        params.barFlag = !items.empty();
        params.barCostEstimate = items.size() * 10.5;
        return params;
    }
    
    // Implement createAstNode using the type-specific template specialization
    // Implementation is in .cpp file to avoid template instantiation order issues
    std::unique_ptr<ast_node::Node> createAstNode() const override;
};

// Verify the concept is satisfied
static_assert(parse_node::ParseNodeType<BarNode>, "BarNode must satisfy ParseNodeType concept");

