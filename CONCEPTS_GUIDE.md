# C++20 Concepts-Based Node Pipeline

## Overview

This project now uses **C++20 concepts** to define compile-time requirements for parse nodes and AST nodes. Each node type can define its own parameter type, enabling type-safe transformations through the pipeline.

## Key Innovation

**Problem:** The old design required all nodes to use the same `AstParams` struct. This was limiting because different node types might need different parameters.

**Solution:** Use C++20 concepts to allow each node type to define its own parameter type (like `FooAstParams`, `BarAstParams`, etc.) while still enforcing compile-time requirements.

## Architecture

### 1. Concepts Define Requirements

```cpp
// From include/parse_node.h
template<typename T>
concept AstParamType = std::default_initializable<T> && std::copyable<T>;

template<typename T>
concept ParseNodeType = requires(const T& node) {
    // Must have a get_shape() method
    { node.get_shape() } -> std::convertible_to<std::string>;
    
    // Must have an astParams() method that returns an AstParamType
    { node.astParams() } -> AstParamType;
};
```

**What this means:**
- Any type can be used as AST parameters, as long as it's default-constructible and copyable
- Any parse node must have both `get_shape()` and `astParams()` methods
- The compiler checks these requirements at compile-time

### 2. Node-Specific Parameter Types

Each node type defines its own parameter struct:

```cpp
// src/ast_params/foo_ast_params.h
struct FooAstParams {
    std::string nodeType = "foo";
    std::string debugInfo;
    int fooSpecificData = 0;  // Foo-specific field!
};
```

**Benefits:**
- Type-safe: Can't accidentally pass `BarAstParams` to create a `FooAstNode`
- Extensible: Each node can have its own fields
- Clean: No need for union types or variant wrappers

### 3. CRTP for Type Safety (Optional)

The `TypedNode` template uses the Curiously Recurring Template Pattern:

```cpp
template<typename Derived, typename ParamType>
    requires AstParamType<ParamType>
struct TypedNode : Node {
    using AstParams = ParamType;
    virtual ParamType astParams() const = 0;
};
```

```cpp
// FooNode uses CRTP to declare its param type
struct FooNode : public parse_node::TypedNode<FooNode, FooAstParams> {
    FooAstParams astParams() const override {
        return FooAstParams{/* ... */};
    }
};
```

**Benefits:**
- Compile-time type checking
- No runtime overhead
- Self-documenting (you can see what param type a node uses)

### 4. Template Specialization for Creation

Instead of a runtime factory map, we use template specialization:

```cpp
// In src/ast_nodes/foo_ast_node.h
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<FooAstParams>(const FooAstParams& params) {
        return std::make_unique<FooAstNode>(params);
    }
}
```

**Benefits:**
- **No `alwayslink` needed**: Templates are instantiated when used
- **No static registration overhead**: No startup cost
- **Type-safe**: Can't pass wrong param type
- **Optimizable**: Compiler can inline everything

## Complete Example: Adding a New Node Type

Let's add a "bar" node type with its own parameters.

### Step 1: Create Bar-Specific Params

```cpp
// src/ast_params/bar_ast_params.h
#pragma once
#include <string>

struct BarAstParams {
    std::string nodeType = "bar";
    std::string debugInfo;
    bool barSpecificFlag = false;  // Bar-specific field!
    double barCost = 0.0;          // Another bar-specific field!
};
```

### Step 2: Create Bar Parse Node

```cpp
// src/parse_nodes/bar_node.h
#pragma once
#include "parse_node.h"
#include "bar_ast_params.h"
#include <string>

struct BarNode : public parse_node::TypedNode<BarNode, BarAstParams> {
    std::string data;
    
    BarNode(const std::string& inputData = "") : data(inputData) {}
    
    std::string get_shape() const override {
        return "bar_shape";
    }
    
    BarAstParams astParams() const override {
        BarAstParams params;
        params.nodeType = "bar";
        params.debugInfo = "BarNode: " + data;
        params.barSpecificFlag = true;
        params.barCost = 150.0;
        return params;
    }
};

// Compiler verifies BarNode satisfies the concept!
static_assert(parse_node::ParseNodeType<BarNode>);
```

### Step 3: Create Bar AST Node

```cpp
// src/ast_nodes/bar_ast_node.h
#pragma once
#include "ast_node.h"
#include "bar_ast_params.h"
#include <string>

struct BarAstNode : public ast_node::Node {
    std::string nodeType;
    std::string debugInfo;
    bool flag;
    double cost;
    
    BarAstNode(const BarAstParams& params)
        : nodeType(params.nodeType)
        , debugInfo(params.debugInfo)
        , flag(params.barSpecificFlag)
        , cost(params.barCost) {}
    
    std::string debugName() const override {
        return "BarAstNode[" + nodeType + "]: " + debugInfo + 
               " (flag=" + (flag ? "true" : "false") + ", cost=" + std::to_string(cost) + ")";
    }
    
    LogicalParams logicalParams() const override {
        return LogicalParams{
            .nodeType = "bar",
            .optimizationHint = "expensive_operation"
        };
    }
};

// Specialize create for BarAstParams
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<BarAstParams>(const BarAstParams& params) {
        return std::make_unique<BarAstNode>(params);
    }
}
```

### Step 4: Update BUILD.bazel

```python
# Add bar params library
cc_library(
    name = "bar_ast_params",
    hdrs = ["src/ast_params/bar_ast_params.h"],
    strip_include_prefix = "src/ast_params",
    visibility = ["//visibility:public"],
)

# Add bar parse node
cc_library(
    name = "bar_parse_node",
    srcs = ["src/parse_nodes/bar_node.cpp"],
    hdrs = ["src/parse_nodes/bar_node.h"],
    includes = ["include"],
    deps = [
        ":parse_node",
        ":bar_ast_params",
    ],
    visibility = ["//visibility:public"],
    alwayslink = 1,  # Only for runtime registration
)

# Add bar AST node
cc_library(
    name = "bar_ast_node",
    srcs = ["src/ast_nodes/bar_ast_node.cpp"],
    hdrs = ["src/ast_nodes/bar_ast_node.h"],
    includes = ["include"],
    deps = [
        ":ast_node",
        ":bar_ast_params",
    ],
    visibility = ["//visibility:public"],
)
```

### Step 5: Use It

```cpp
#include "src/parse_nodes/bar_node.h"
#include "src/ast_nodes/bar_ast_node.h"

int main() {
    // Create bar parse node
    BarNode barNode("my_bar_data");
    
    // Get bar-specific params
    auto barParams = barNode.astParams();
    std::cout << "Bar cost: " << barParams.barCost << std::endl;
    
    // Create bar AST node with type safety
    auto barAstNode = ast_node::create<BarAstParams>(barParams);
    std::cout << barAstNode->debugName() << std::endl;
}
```

## Key Benefits of This Design

### ✅ Type Safety
```cpp
FooNode fooNode;
auto fooParams = fooNode.astParams();  // Returns FooAstParams

// This works:
auto fooAst = ast_node::create<FooAstParams>(fooParams);  ✅

// This won't compile:
auto wrongAst = ast_node::create<BarAstParams>(fooParams);  ❌
// Error: no matching function for call to 'create'
```

### ✅ No Runtime Overhead
- Template specialization means no vtable lookups
- No factory map lookups
- Compiler can inline everything
- Dead code elimination removes unused specializations

### ✅ No `alwayslink` Required (for AST nodes)
- Templates are instantiated on-demand
- No static initialization
- Smaller binaries
- Faster startup

### ✅ Better Error Messages
```cpp
struct BadNode {};

// Compiler error: static assertion failed
static_assert(parse_node::ParseNodeType<BadNode>);
// Error: constraints not satisfied:
//   - missing get_shape() method
//   - missing astParams() method
```

### ✅ Self-Documenting
```cpp
// You can see what param type FooNode uses:
using FooParamType = FooNode::AstParams;  // FooAstParams

// Concepts document requirements:
parse_node::ParseNodeType auto myNode = FooNode{};
```

## Comparing Approaches

### Old Approach: Single AstParams

```cpp
struct AstParams {
    std::string nodeType;
    std::string debugInfo;
    // All nodes must fit their data into these fields
};
```

**Problems:**
- Can't add node-specific fields
- Type erasure loses information
- Runtime string matching on nodeType
- Harder to extend

### New Approach: Node-Specific Params

```cpp
struct FooAstParams {
    std::string nodeType;
    std::string debugInfo;
    int fooSpecificData;  // Foo-specific!
};

struct BarAstParams {
    std::string nodeType;
    std::string debugInfo;
    double barCost;       // Bar-specific!
    bool barFlag;         // Bar-specific!
};
```

**Benefits:**
- Each node has exactly the fields it needs
- Type safety prevents mixups
- Compile-time checking
- Better performance

## Advanced: Concept-Driven Generic Algorithms

With concepts, you can write generic algorithms:

```cpp
// Generic function that works with any parse node
template<parse_node::ParseNodeType T>
void processParseNode(const T& node) {
    std::cout << "Shape: " << node.get_shape() << std::endl;
    auto params = node.astParams();
    // params has the type-specific fields!
}

// Use it with any node type:
FooNode foo;
BarNode bar;
processParseNode(foo);  // Works!
processParseNode(bar);  // Works!
```

## C++20 Requirements

This design requires:
- **C++20 compiler** (concepts support)
- **Bazel**: `.bazelrc` must have `--cxxopt=-std=c++20`

### Compatibility

| Compiler | Version | Support |
|----------|---------|---------|
| GCC | 10+ | ✅ |
| Clang | 10+ | ✅ |
| Apple Clang | 13+ | ✅ |
| MSVC | VS 2019 16.10+ | ✅ |

## Migration Guide

### From Old System to Concepts

1. **Keep runtime registration for polymorphism** (if you need `createFromInput`)
2. **Add template specialization for type-safe creation**
3. **Use concepts to document requirements**
4. **Create node-specific param types as needed**

Both systems can coexist:
- Runtime factory (`createFromInput`) for string-based creation
- Template specialization (`create<ParamType>`) for type-safe creation

## Summary

The concept-based design provides:
- 🎯 **Type Safety**: Compile-time checking of node requirements
- ⚡ **Performance**: No runtime overhead, full optimization
- 📦 **Modularity**: Each node type is truly independent
- 📝 **Documentation**: Concepts serve as machine-checked documentation
- 🔧 **Extensibility**: Easy to add new node types with custom parameters
- 🛡️ **Safety**: Impossible to mix incompatible param types

This is a **modern C++ architecture** that leverages the latest language features for maximum safety and performance! 🚀

