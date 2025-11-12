#pragma once
#include <string>
#include <functional>
#include <memory>
#include <concepts>

// Forward declarations for concepts
namespace ast_node {
    struct Node;
}

namespace parse_node {

// Concept: A type that can be used as AST parameters
template<typename T>
concept AstParamType = std::default_initializable<T> && std::copyable<T>;

// Concept: Defines what a parse node must look like
template<typename T>
concept ParseNodeType = requires(const T& node) {
    // Must have a get_shape() method
    { node.get_shape() } -> std::convertible_to<std::string>;
    
    // Must have an astParams() method that returns an AstParamType
    { node.astParams() } -> AstParamType;
    
    // Must be derived from a base (for polymorphism if needed)
    requires std::is_class_v<T>;
};

// Base interface for parse nodes (can be used polymorphically)
struct Node {
    virtual ~Node() = default;
    virtual std::string get_shape() const = 0;
};

// CRTP base class for parse nodes with type-safe params
template<typename Derived, typename ParamType>
    requires AstParamType<ParamType>
struct TypedNode : Node {
    using AstParams = ParamType;
    
    // Derived must implement this
    virtual ParamType astParams() const = 0;
    
    // Helper to create AST node from this parse node's params
    // This will be specialized per node type
    std::unique_ptr<ast_node::Node> createAstNode() const;
};

// Factory registration for polymorphic usage
using Parser = std::function<std::unique_ptr<Node>(std::string)>;
void registerNode(std::string name, Parser parser);
std::unique_ptr<Node> createFromInput(std::string name, std::string argString);

// Helper class for automatic registration
class NodeRegistrar {
public:
    NodeRegistrar(std::string name, Parser parser) {
        registerNode(name, parser);
    }
};

#define REGISTER_NODE(name, parser) \
    static ::parse_node::NodeRegistrar _registrar_##name(#name, parser)

} // namespace parse_node
