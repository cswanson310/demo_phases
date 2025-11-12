#include <string>
#include <functional>
#include <memory>
#include "logical_params.h"

#pragma once

namespace logical_node {

struct Node {
    virtual ~Node() {}
    virtual std::string debugName() const = 0;
    virtual std::string explain() const = 0;  // Like EXPLAIN in SQL
};

// Factory function signature: takes AST node params -> creates Logical node
using Factory = std::function<std::unique_ptr<Node>(const std::string& nodeType, const std::string& optimizationHint)>;

// Register a logical node factory for a given type
void registerNode(std::string name, Factory factory);

// Create a logical node from AST node parameters
std::unique_ptr<Node> create(const LogicalParams& params);

// Helper class for automatic registration at startup
class NodeRegistrar {
public:
    NodeRegistrar(std::string name, Factory factory) {
        registerNode(name, factory);
    }
};

// Macro to make registration easier
#define REGISTER_LOGICAL_NODE(name, factory) \
    static ::logical_node::NodeRegistrar _logical_registrar_##name(#name, factory)

}

