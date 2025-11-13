#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <concepts>

// Forward declarations
struct LogicalNode;

// Base interface for AST nodes
struct AstNode {
    virtual ~AstNode() = default;
    virtual std::string debugName() const = 0;
    
    // Virtual method to create the corresponding logical node
    // Each concrete AST node implements this using its type-specific logical params
    virtual std::unique_ptr<LogicalNode> createLogicalNode() const = 0;
};

// CRTP base class for AST nodes with type-safe params (optional, for more type safety)
template<typename Derived, typename InputParamType, typename OutputParamType>
struct TypedAstNode : AstNode {
    using LogicalParamsType = OutputParamType;
    
    // Derived must implement this
    virtual OutputParamType logicalParams() const = 0;
};

// Generic create function that can work with any param type
// This is a template that will be specialized for each param type
template<typename ParamType>
std::unique_ptr<AstNode> createAstNode(const ParamType& params);
