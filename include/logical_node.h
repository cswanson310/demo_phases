#pragma once
#include <string>
#include <functional>
#include <memory>
#include <concepts>

struct LogicalNode {
    virtual ~LogicalNode() = default;
    virtual std::string debugName() const = 0;
    virtual std::string explain() const = 0;  // Like EXPLAIN in SQL
};

// Generic create function that can work with any param type
// This is a template that will be specialized for each param type
template<typename ParamType>
std::unique_ptr<LogicalNode> createLogicalNode(const ParamType& params);

// Factory type (for runtime polymorphism if needed)
template<typename ParamType>
using LogicalNodeFactory = std::function<std::unique_ptr<LogicalNode>(const ParamType&)>;
