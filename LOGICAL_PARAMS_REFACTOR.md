# AST→Logical Parameter Refactoring

## Summary

Refactored the AST→Logical transformation to use **type-specific logical parameter structs** for each node type, mirroring the Parse→AST pattern. This ensures complete flexibility and type safety throughout the entire pipeline.

## What Changed

### Before: Generic LogicalParams

Previously, all AST nodes used the same generic `LogicalParams` struct:

```cpp
// include/logical_params.h - Generic for all nodes
struct LogicalParams {
    std::string nodeType;
    std::string optimizationHint;
};

// All AST nodes returned the same type
struct FooAstNode : public ast_node::Node {
    LogicalParams logicalParams() const override;  // Generic
};

struct BarAstNode : public ast_node::Node {
    LogicalParams logicalParams() const override;  // Generic
};
```

### After: Type-Specific LogicalParams

Now each node type has its own logical parameter struct:

```cpp
// src/logical_params/foo_logical_params.h - Foo-specific
struct FooLogicalParams {
    std::string nodeType;
    std::string optimizationHint;
    int costMultiplier;  // Foo-specific field
};

// src/logical_params/bar_logical_params.h - Bar-specific
struct BarLogicalParams {
    std::string nodeType;
    std::string optimizationHint;
    bool canUseIndex;       // Bar-specific fields
    int estimatedRows;
    double selectivity;
};

// Each AST node returns its own type
struct FooAstNode : public ast_node::Node {
    FooLogicalParams logicalParams() const;  // Type-specific (not virtual)
};

struct BarAstNode : public ast_node::Node {
    BarLogicalParams logicalParams() const;  // Type-specific (not virtual)
};
```

## Files Created

### New Parameter Libraries

1. **src/logical_params/foo_logical_params.h**
   - Defines `FooLogicalParams` with `costMultiplier`
   
2. **src/logical_params/bar_logical_params.h**
   - Defines `BarLogicalParams` with `canUseIndex`, `estimatedRows`, `selectivity`

## Files Modified

### 1. include/logical_node.h
- Removed generic `create(const LogicalParams&)` function
- Added template-based `create<ParamType>(const ParamType&)` function
- Updated factory/registration types to be template-based
- Added `LogicalParamType` concept for parameter validation

### 2. src/logical_node.cpp
- Removed old registration system
- Template specializations are now in individual node headers

### 3. include/ast_node.h
- Removed `virtual LogicalParams logicalParams() const` from base `Node` interface
- Kept `virtual createLogicalNode()` for polymorphic transformation
- Each concrete AST node now has its own non-virtual `logicalParams()` method

### 4. src/ast_nodes/foo_ast_node.h
- Added `#include "foo_logical_params.h"`
- Changed `logicalParams()` to return `FooLogicalParams` (not virtual)
- Computes `costMultiplier` based on `fooData`

### 5. src/ast_nodes/foo_ast_node.cpp
- Updated `createLogicalNode()` to use `logical_node::create<FooLogicalParams>`
- Includes `src/logical_nodes/foo_logical_node.h` for template specialization

### 6. src/ast_nodes/bar_ast_node.h
- Added `#include "bar_logical_params.h"`
- Changed `logicalParams()` to return `BarLogicalParams` (not virtual)
- Computes detailed parameters: `canUseIndex`, `estimatedRows`, `selectivity`

### 7. src/ast_nodes/bar_ast_node.cpp
- Updated `createLogicalNode()` to use `logical_node::create<BarLogicalParams>`
- Includes `src/logical_nodes/bar_logical_node.h` for template specialization

### 8. src/logical_nodes/foo_logical_node.h
- Added `#include "foo_logical_params.h"`
- Updated constructor to take `FooLogicalParams`
- Added `costMultiplier` member
- Updated `explain()` to show cost multiplier and computed cost
- Added template specialization: `logical_node::create<FooLogicalParams>`

### 9. src/logical_nodes/foo_logical_node.cpp
- Removed old registration system

### 10. src/logical_nodes/bar_logical_node.h
- Added `#include "bar_logical_params.h"`
- Updated constructor to take `BarLogicalParams`
- Added `canUseIndex`, `estimatedRows`, `selectivity` members
- Updated `explain()` to show all parameters
- Added template specialization: `logical_node::create<BarLogicalParams>`

### 11. src/logical_nodes/bar_logical_node.cpp
- Removed old registration system

### 12. BUILD.bazel
- Added `cc_library` targets for:
  - `:foo_logical_params`
  - `:bar_logical_params`
- Updated dependencies:
  - `foo_ast_nodes` now depends on `:foo_logical_params` and `:foo_logical_nodes`
  - `bar_ast_nodes` now depends on `:bar_logical_params` and `:bar_logical_nodes`
  - `foo_logical_nodes` now depends on `:foo_logical_params`
  - `bar_logical_nodes` now depends on `:bar_logical_params`

### 13. src/main.cpp
- Updated summary text to reflect type-specific logical parameters
- Changed from "Both use standard LogicalParams (pass-through pattern)"
- To "Type-specific params at every phase: Parse → AST → Logical"

## Key Architectural Changes

### Template Specialization Pattern

Logical node creation now uses template specialization, just like AST nodes:

```cpp
// In foo_logical_node.h
namespace logical_node {
    template<>
    inline std::unique_ptr<Node> create<FooLogicalParams>(const FooLogicalParams& params) {
        return std::make_unique<FooLogicalNode>(params);
    }
}
```

### Non-Virtual Parameter Methods

The `logicalParams()` method is now **non-virtual** and type-specific:

```cpp
struct FooAstNode : public ast_node::Node {
    // Type-specific, not virtual
    FooLogicalParams logicalParams() const { ... }
    
    // Still virtual for polymorphic transformation
    std::unique_ptr<logical_node::Node> createLogicalNode() const override { ... }
};
```

### Dependency Flow

```
foo_logical_params (header-only, no deps)
         ↓
foo_logical_nodes (depends on foo_logical_params)
         ↑
foo_ast_nodes (depends on both foo_logical_params and foo_logical_nodes)
```

## Benefits

### 1. Consistent Pattern
- Parse→AST uses type-specific params (FooAstParams, BarAstParams)
- AST→Logical now also uses type-specific params (FooLogicalParams, BarLogicalParams)
- Same pattern throughout the entire pipeline

### 2. Maximum Flexibility
- Each node type can define exactly what logical information it needs
- Foo has simple `costMultiplier`
- Bar has complex `canUseIndex`, `estimatedRows`, `selectivity`

### 3. Compile-Time Safety
- Template specialization ensures correct param types
- Compiler catches mismatches between param types and node types
- No runtime type checking needed

### 4. Clean Module Boundaries
- Each param library is independent
- No circular dependencies
- Clear separation of concerns

### 5. Polymorphic Transformation Still Works
- Despite type-specific params, transformations remain polymorphic
- `ast_to_logical_transformer::astToLogical(astNode)` works for any AST node
- Virtual `createLogicalNode()` method handles the polymorphism

## Testing

The application successfully demonstrates:

1. **Foo Pipeline:**
   ```
   FooNode → FooAstParams{fooSpecificData=42}
          → FooAstNode → FooLogicalParams{costMultiplier=2}
          → FooLogicalNode (cost: 200 units)
   ```

2. **Bar Pipeline:**
   ```
   BarNode → BarAstParams{items, flag, costEstimate}
          → BarAstNode → BarLogicalParams{canUseIndex=true, estimatedRows=200, selectivity=0.1}
          → BarLogicalNode (cost: 50 units, can use index)
   ```

## Migration Notes

### For Existing Code
If you have existing AST nodes using `LogicalParams`:

1. Create a new `XxxLogicalParams` struct in `src/logical_params/`
2. Update your AST node to return that type from `logicalParams()`
3. Update your logical node to accept that type in its constructor
4. Add template specialization in your logical node header
5. Update BUILD.bazel dependencies

### For New Node Types
Follow the pattern established by Foo and Bar:
- Always create type-specific parameter structs at each phase
- Use template specialization for `create` functions
- Keep the virtual `createXxxNode()` methods for polymorphism

## Related Documentation

- [TYPE_SPECIFIC_PARAMS.md](TYPE_SPECIFIC_PARAMS.md) - Detailed explanation of the pattern
- [NODE_PIPELINE_GUIDE.md](NODE_PIPELINE_GUIDE.md) - How to add new nodes
- [TRANSFORMER_PATTERN.md](TRANSFORMER_PATTERN.md) - How transformers work

