#pragma once
#include <string>
#include <functional>
#include <memory>
#include <concepts>

namespace logical_node {

// Concept for parameter types that can create logical nodes
template<typename T>
concept LogicalParamType = std::default_initializable<T> && std::copyable<T>;

struct Node {
    virtual ~Node() = default;
    virtual std::string debugName() const = 0;
    virtual std::string explain() const = 0;  // Like EXPLAIN in SQL
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

#define REGISTER_LOGICAL_NODE_TYPED(name, ParamType, factory) \
    static ::logical_node::NodeRegistrar<ParamType> _logical_registrar_##name(#name, factory)

} // namespace logical_node
