# Node Transformer Library

## Overview

The `node_transformer` library provides a bridge between the polymorphic `parse_node` layer and the type-safe `ast_node` layer. It allows you to convert any `parse_node::Node` to its corresponding `ast_node::Node` without knowing the concrete type at compile time.

## Problem It Solves

**Challenge:** You have a polymorphic parse node (from `parse_node::createFromInput`) but need to create the corresponding AST node:

```cpp
auto parseNode = parse_node::createFromInput("foo", "data");  // Returns unique_ptr<Node>
// How do we create the AST node without knowing the concrete type?
```

**Solution:** The transformer library uses virtual dispatch to call the appropriate type-specific creation function:

```cpp
auto astNode = node_transformer::parseToAst(parseNode);  // Magic! ✨
```

## Architecture

### Component Diagram

```
┌────────────────────────────────────────────────────────────┐
│                   node_transformer                         │
│  ┌──────────────────────────────────────────────────────┐ │
│  │  parseToAst(const parse_node::Node& node)            │ │
│  │     │                                                 │ │
│  │     ↓                                                 │ │
│  │  calls node.createAstNode() [virtual dispatch]       │ │
│  └──────────────────────────────────────────────────────┘ │
│          │                                                 │
│          │ Depends on                                      │
│          ↓                                                 │
│  ┌─────────────┐         ┌─────────────┐                 │
│  │ parse_node  │         │  ast_node   │                 │
│  │   (base)    │         │   (base)    │                 │
│  └─────────────┘         └─────────────┘                 │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│             Concrete Node Implementations                   │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ FooNode::createAstNode()                             │ │
│  │    return ast_node::create<FooAstParams>(...)        │ │
│  └──────────────────────────────────────────────────────┘ │
│          │                                                 │
│          ↓  Uses template specialization                  │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ ast_node::create<FooAstParams>() specialization      │ │
│  │    return std::make_unique<FooAstNode>(params)       │ │
│  └──────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────┘
```

## How It Works

### Step 1: Virtual Method in Base Class

`parse_node::Node` has a pure virtual method:

```cpp
struct Node {
    virtual std::unique_ptr<ast_node::Node> createAstNode() const = 0;
};
```

### Step 2: Each Concrete Node Implements It

Each parse node type implements the virtual method using its specific param type:

```cpp
struct FooNode : public parse_node::TypedNode<FooNode, FooAstParams> {
    std::unique_ptr<ast_node::Node> createAstNode() const override {
        return ast_node::create<FooAstParams>(astParams());
    }
};
```

### Step 3: Transformer Calls Virtual Method

The transformer library simply calls the virtual method:

```cpp
namespace node_transformer {
    std::unique_ptr<ast_node::Node> parseToAst(const parse_node::Node& parseNode) {
        return parseNode.createAstNode();  // Virtual dispatch!
    }
}
```

## Usage

### Basic Usage

```cpp
#include "node_transformer.h"

// Create a parse node polymorphically
auto parseNode = parse_node::createFromInput("foo", "input_data");

// Transform to AST node
auto astNode = node_transformer::parseToAst(parseNode);

// Use the AST node
std::cout << astNode->debugName() << std::endl;
```

### With Unique Pointer

```cpp
std::unique_ptr<parse_node::Node> parseNode = /* ... */;

// Convenience overload accepts unique_ptr
auto astNode = node_transformer::parseToAst(parseNode);
```

### In a Pipeline

```cpp
auto parseNode = parse_node::createFromInput("foo", "data");
auto astNode = node_transformer::parseToAst(parseNode);
auto logicalParams = astNode->logicalParams();
auto logicalNode = logical_node::create(logicalParams);
```

## Adding Support for New Node Types

When you add a new node type, you must implement `createAstNode()`:

### Step 1: Create Parameter Type

```cpp
// src/ast_params/bar_ast_params.h
struct BarAstParams {
    std::string nodeType = "bar";
    std::string debugInfo;
    double barSpecificValue = 0.0;
};
```

### Step 2: Create AST Node with Template Specialization

```cpp
// src/ast_nodes/bar_ast_node.h
struct BarAstNode : public ast_node::Node {
    BarAstNode(const BarAstParams& params) { /* ... */ }
    // ...
};

namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<BarAstParams>(const BarAstParams& params) {
        return std::make_unique<BarAstNode>(params);
    }
}
```

### Step 3: Implement createAstNode in Parse Node

**Header:**
```cpp
// src/parse_nodes/bar_node.h
struct BarNode : public parse_node::TypedNode<BarNode, BarAstParams> {
    std::string get_shape() const override;
    BarAstParams astParams() const override;
    std::unique_ptr<ast_node::Node> createAstNode() const override;
};
```

**Implementation:**
```cpp
// src/parse_nodes/bar_node.cpp
#include "bar_node.h"
#include "src/ast_nodes/bar_ast_node.h"

std::unique_ptr<ast_node::Node> BarNode::createAstNode() const {
    return ast_node::create<BarAstParams>(astParams());
}
```

**Important:** Implement `createAstNode()` in the .cpp file (not inline in the header) to avoid template instantiation order issues.

### Step 4: Update BUILD.bazel

Make sure parse_nodes_impl depends on ast_nodes_impl:

```python
cc_library(
    name = "bar_parse_node",
    srcs = ["src/parse_nodes/bar_node.cpp"],
    hdrs = ["src/parse_nodes/bar_node.h"],
    deps = [
        ":parse_node",
        ":ast_node",
        ":ast_nodes_impl",  # Important!
        ":bar_ast_params",
    ],
    alwayslink = 1,
)
```

## Dependencies

The transformer library depends on both parse_node and ast_node base libraries:

```python
cc_library(
    name = "node_transformer",
    srcs = ["src/node_transformer.cpp"],
    hdrs = ["include/node_transformer.h"],
    deps = [
        ":parse_node",
        ":ast_node",
    ],
)
```

**Note:** It does NOT depend on specific node implementations. The concrete implementations depend on the transformer library indirectly through the base classes.

## Dependency Graph

```
                  ┌──────────────┐
                  │ toy_app      │
                  └──────┬───────┘
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        ↓                ↓                ↓
┌─────────────┐  ┌──────────────┐  ┌────────────────┐
│parse_nodes  │  │node_transformer│ │ast_nodes_impl  │
│   _impl     │  │                 │ │                │
└──────┬──────┘  └───────┬────────┘ └───────┬────────┘
       │                 │                  │
       │         ┌───────┴───────┐          │
       │         │               │          │
       ↓         ↓               ↓          ↓
┌─────────────┐ ┌──────────┐ ┌──────────┐
│parse_node   │ │ast_node  │ │ast_node  │
│   (base)    │ │  (base)  │ │  (base)  │
└─────────────┘ └──────────┘ └──────────┘
```

## Benefits

### ✅ Type Erasure with Type Safety
- Accepts any `parse_node::Node` (polymorphic)
- Returns correctly-typed `ast_node::Node`
- Type-specific logic handled per node type

### ✅ Clean Separation
- Parse node implementations know how to create their AST nodes
- Transformer library just provides a convenient interface
- No central dispatch table needed

### ✅ Extensible
- Add new node types without modifying transformer
- Each node type is self-contained
- No registration needed in transformer library

### ✅ Performance
- Single virtual call (no multiple lookups)
- Inline-able template specializations
- No runtime type information needed

## Comparison to Alternatives

### Alternative 1: Type-Erased Parameters

```cpp
// NOT used
std::any params = parseNode->getParams();  ❌
// Problems:
// - Loses type safety
// - Runtime overhead
// - Requires type checking
```

### Alternative 2: Visitor Pattern

```cpp
// NOT used
struct AstCreator : public ParseNodeVisitor {  ❌
    void visit(FooNode& node) override { /* ... */ }
    void visit(BarNode& node) override { /* ... */ }
};
// Problems:
// - Need to modify visitor for each new node type
// - More boilerplate
// - Less flexible
```

### Alternative 3: Runtime Type Map

```cpp
// NOT used
std::map<std::string, std::function<...>> creators;  ❌
// Problems:
// - Runtime lookup overhead
// - String-based dispatch
// - Separate registration needed
```

### Our Approach: Virtual Method

```cpp
// USED ✅
parseNode->createAstNode();  // Single virtual call
// Benefits:
// - Type-safe
// - Fast (single virtual dispatch)
// - Self-contained in each node
// - No central registry
```

## Technical Notes

### Template Instantiation Order

**Problem:** Template specializations must be defined before they're implicitly instantiated.

**Solution:** Implement `createAstNode()` in .cpp files (not inline in headers).

**Bad:**
```cpp
// foo_node.h
struct FooNode {
    std::unique_ptr<ast_node::Node> createAstNode() const override {
        return ast_node::create<FooAstParams>(...);  // ❌ Instantiates template
    }
};
// Later: explicit specialization of create<FooAstParams> → ERROR!
```

**Good:**
```cpp
// foo_node.h
struct FooNode {
    std::unique_ptr<ast_node::Node> createAstNode() const override;  // ✅ Declaration only
};

// foo_node.cpp
#include "foo_ast_node.h"  // Has the specialization
std::unique_ptr<ast_node::Node> FooNode::createAstNode() const {
    return ast_node::create<FooAstParams>(...);  // ✅ Specialization already defined
}
```

### Why Not Use CRTP for createAstNode?

CRTP (Curiously Recurring Template Pattern) could provide static polymorphism, but we need runtime polymorphism here because:

1. `parse_node::createFromInput` returns `unique_ptr<Node>` (base class)
2. We don't know the concrete type at compile time
3. The whole point is runtime type erasure with type-safe recovery

## Summary

The `node_transformer` library provides:

- 🎯 **Simple API**: `parseToAst()` just works
- 🔒 **Type Safe**: Each node uses its own param type internally
- ⚡ **Fast**: Single virtual call, no lookups
- 📦 **Modular**: No dependencies on concrete implementations
- 🔧 **Extensible**: Add nodes without modifying transformer

It's the perfect bridge between polymorphic input (string-based creation) and type-safe output (template-based AST creation)! 🌉

