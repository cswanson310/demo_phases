# Pipeline Architecture - Clean Module Boundaries

## Overview

The system has been refactored to have clean, minimal dependencies between layers. Each layer is completely independent and only depends on lightweight parameter structs.

## Module Dependency Graph

```
┌─────────────────┐
│  ast_params     │  (Pure header, no deps)
│  AstParams      │
└─────────────────┘
        ↑
        │
┌───────┴─────────┐
│  parse_node     │  (Only depends on ast_params)
│  - Node base    │
│  - Factory reg  │
└─────────────────┘
        ↑
        │
┌───────┴──────────┐
│ parse_nodes_impl │  (Concrete implementations)
│ - FooNode        │
│ - BarNode etc.   │
└──────────────────┘


┌─────────────────┐
│ logical_params  │  (Pure header, no deps)
│ LogicalParams   │
└─────────────────┘
        ↑
        │
┌───────┴─────────┐      ┌─────────────────┐
│  ast_node       │──────┤  ast_params     │
│  - Node base    │      │  AstParams      │
│  - Factory reg  │      └─────────────────┘
└─────────────────┘
        ↑
        │
┌───────┴──────────┐
│ ast_nodes_impl   │  (Concrete implementations)
│ - FooAstNode     │
│ - BarAstNode etc.│
└──────────────────┘


┌─────────────────┐      ┌─────────────────┐
│  logical_node   │──────┤ logical_params  │
│  - Node base    │      │ LogicalParams   │
│  - Factory reg  │      └─────────────────┘
└─────────────────┘
        ↑
        │
┌───────┴──────────────┐
│ logical_nodes_impl   │  (Concrete implementations)
│ - FooLogicalNode     │
│ - BarLogicalNode etc.│
└──────────────────────┘
```

## Key Design Principles

### 1. **Parameter Structs as Interface Boundaries**

Each parameter struct is a pure data structure with **zero dependencies**:

- **`ast_params.h`** - Defines `AstParams` struct
  - Used by parse nodes to describe what AST node to create
  - No includes except `<string>`

- **`logical_params.h`** - Defines `LogicalParams` struct
  - Used by AST nodes to describe what logical node to create
  - No includes except `<string>`

### 2. **Layer Independence**

Each layer is completely independent:

| Layer | Dependencies | What it knows about |
|-------|-------------|-------------------|
| `parse_node` | `ast_params` only | How to create parse nodes, what params AST needs |
| `ast_node` | `ast_params`, `logical_params` only | How to create AST nodes, what params logical needs |
| `logical_node` | `logical_params` only | How to create logical nodes |

**Important:** No layer depends on the previous or next layer's full implementation!

### 3. **Implementation Isolation**

Concrete node implementations (the `*_impl` libraries) are completely separate:
- Can be compiled independently
- Can be added/removed without affecting other layers
- Only linked together at the binary level via `alwayslink`

## Benefits of This Architecture

### ✅ **Clean Compilation Boundaries**
- Changing a parse node doesn't require recompiling AST layer
- Changing an AST node doesn't require recompiling logical layer
- Only the parameter structs are shared contracts

### ✅ **Easy Testing**
- Each layer can be unit tested independently
- Mock implementations can be created without full pipeline

### ✅ **Plugin Architecture Ready**
- Node implementations can be dynamically loaded
- New node types can be added as separate modules
- No central registration file to modify

### ✅ **Parallel Development**
- Different teams can work on different layers
- Only need to agree on parameter struct format
- No cross-layer coupling

### ✅ **Minimal Rebuild Times**
- Changes to node implementations don't cascade
- Parameter structs rarely change
- Base layers are stable

## Data Flow

```
Input String
    ↓
parse_node::createFromInput("foo", "data")
    ↓
FooNode
    ↓
FooNode::astParams() → returns AstParams{nodeType="foo", debugInfo="..."}
    ↓
ast_node::create(astParams)
    ↓
FooAstNode
    ↓
FooAstNode::logicalParams() → returns LogicalParams{nodeType="foo", optimizationHint="..."}
    ↓
logical_node::create(logicalParams)
    ↓
FooLogicalNode
    ↓
Execution / Explain Plan
```

## Adding New Layers

To add a new layer (e.g., Physical layer), follow this pattern:

1. **Create a params header** (`physical_params.h`)
   ```cpp
   struct PhysicalParams {
       std::string nodeType;
       std::string executionStrategy;
   };
   ```

2. **Update the previous layer** (logical_node) to return `PhysicalParams`
   ```cpp
   virtual PhysicalParams physicalParams() const = 0;
   ```

3. **Create the new layer** (`physical_node.h/cpp`)
   - Depend only on `physical_params`
   - Implement factory registration
   - Implement `create(const PhysicalParams& params)`

4. **Create implementations** (`physical_nodes_impl`)
   - Implement concrete physical node types
   - Use `REGISTER_PHYSICAL_NODE` macro
   - Set `alwayslink = 1`

## BUILD.bazel Structure

```python
# Pure parameter libraries (header-only, no dependencies)
cc_library(name = "ast_params", hdrs = ["include/ast_params.h"])
cc_library(name = "logical_params", hdrs = ["include/logical_params.h"])

# Base layer libraries (depend only on params)
cc_library(name = "parse_node", deps = [":ast_params"])
cc_library(name = "ast_node", deps = [":ast_params", ":logical_params"])
cc_library(name = "logical_node", deps = [":logical_params"])

# Implementation libraries (depend on base + params)
cc_library(name = "parse_nodes_impl", deps = [":parse_node"], alwayslink = 1)
cc_library(name = "ast_nodes_impl", deps = [":ast_node"], alwayslink = 1)
cc_library(name = "logical_nodes_impl", deps = [":logical_node"], alwayslink = 1)
```

## Comparison: Before vs After

### Before Refactoring
```
parse_node.h contains AstParams
    ↓
ast_node.h depends on parse_node.h (circular risk!)
ast_node.h contains LogicalParams
    ↓
logical_node.h depends on ast_node.h (transitive deps!)
```

**Problems:**
- Circular dependency risk
- Tight coupling between layers
- Changes cascade through all layers
- Can't compile layers independently

### After Refactoring
```
ast_params.h (standalone)
    ↑
parse_node.h depends on ast_params.h

logical_params.h (standalone)
    ↑
ast_node.h depends on ast_params.h and logical_params.h

logical_node.h depends on logical_params.h
```

**Benefits:**
- No circular dependencies possible
- Minimal coupling (only through params)
- Changes don't cascade
- Each layer compiles independently
- Parameter structs are the only contracts

## Summary

This architecture demonstrates **Interface Segregation Principle** and **Dependency Inversion Principle**:

- ✅ Each layer depends on abstractions (params), not implementations
- ✅ High-level modules don't depend on low-level modules
- ✅ Clear, minimal interfaces between components
- ✅ Easy to extend, hard to break

The parameter structs act as **pure interfaces** with no implementation dependencies, allowing complete modularity while maintaining type safety and compile-time checking.

