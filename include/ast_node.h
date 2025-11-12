#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <concepts>
#include "logical_params.h"

// Forward declare the logical_node namespace
namespace logical_node {
    struct Node;
}

namespace ast_node {

// Concept for logical parameter types
template<typename T>
concept LogicalParamType = std::default_initializable<T> && std::copyable<T>;

// Base interface for AST nodes
struct Node {
    virtual ~Node() = default;
    virtual std::string debugName() const = 0;
    virtual LogicalParams logicalParams() const = 0;
};

// CRTP base class for AST nodes with type-safe params (optional, for more type safety)
template<typename Derived, typename InputParamType, typename OutputParamType>
    requires LogicalParamType<OutputParamType>
struct TypedNode : Node {
    using LogicalParamsType = OutputParamType;
    
    // Derived must implement this
    virtual OutputParamType logicalParams() const = 0;
};

// Generic create function that can work with any param type
// This is a template that will be specialized for each param type
template<typename ParamType>
std::unique_ptr<Node> create(const ParamType& params);

// Registration system (for runtime polymorphism if needed)
template<typename ParamType>
using Factory = std::function<std::unique_ptr<Node>(const ParamType&)>;

template<typename ParamType>
void registerNode(std::string name, Factory<ParamType> factory);

// Helper class for automatic registration
template<typename ParamType>
class NodeRegistrar {
public:
    NodeRegistrar(std::string name, Factory<ParamType> factory);
};

#define REGISTER_AST_NODE_TYPED(name, ParamType, factory) \
    static ::ast_node::NodeRegistrar<ParamType> _ast_registrar_##name(#name, factory)

} // namespace ast_node
