#pragma once
#include <string>
#include <memory>
#include <concepts>

// Forward declarations
namespace ast_node {
    struct Node;
}

namespace parse_node {

// Concept: A type that represents parameters for creating AST nodes
template<typename T>
concept AstParamType = requires {
    // Must be default constructible
    typename T;
};

// Concept: A parse node that can produce AST parameters
template<typename T>
concept ParseNodeType = requires(const T& node) {
    // Must have a get_shape() method returning string
    { node.get_shape() } -> std::convertible_to<std::string>;
    
    // Must have an astParams() method that returns some param type
    { node.astParams() } -> AstParamType;
};

// Concept: A parameter type that can be used to create an AST node
template<typename ParamT>
concept CreatableAstParam = requires(const ParamT& params) {
    // The param type must work with ast_node::create
    { ast_node::create(params) } -> std::convertible_to<std::unique_ptr<ast_node::Node>>;
};

} // namespace parse_node

