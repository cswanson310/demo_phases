# Concept-Based Architecture Summary

## What We Built

You now have a **type-safe, concept-driven node pipeline** where each node type can define its own parameter type while still satisfying compile-time requirements.

## The Problem We Solved

**Your Original Question:**
> "I would like each ParseNode type to be able to create a param type of its own and be able to create an AstNode from this param type."

**The Challenge:**
- How do we allow each node type to have its own parameter type?
- How do we maintain type safety across the pipeline?
- How do we avoid runtime overhead?

## The Solution: C++20 Concepts

### 1. Node-Specific Parameters

Each node type gets its own parameter struct:

```cpp
// Foo has its own params
struct FooAstParams {
    std::string nodeType = "foo";
    std::string debugInfo;
    int fooSpecificData = 0;  // Foo-specific!
};

// Bar would have different params
struct BarAstParams {
    std::string nodeType = "bar";
    std::string debugInfo;
    double barCost = 0.0;     // Bar-specific!
    bool barFlag = false;      // Bar-specific!
};
```

### 2. Concepts Define Requirements

Instead of inheritance from a fixed interface, we use concepts:

```cpp
template<typename T>
concept AstParamType = std::default_initializable<T> && std::copyable<T>;

template<typename T>
concept ParseNodeType = requires(const T& node) {
    { node.get_shape() } -> std::convertible_to<std::string>;
    { node.astParams() } -> AstParamType;
};
```

**This means:** "Any type that has `get_shape()` and `astParams()` is a valid parse node."

### 3. Type-Safe Creation via Template Specialization

```cpp
// In foo_ast_node.h
namespace ast_node {
    template<>
    inline std::unique_ptr<Node> create<FooAstParams>(const FooAstParams& params) {
        return std::make_unique<FooAstNode>(params);
    }
}

// Usage:
FooNode parseNode;
auto params = parseNode.astParams();  // Returns FooAstParams
auto astNode = ast_node::create<FooAstParams>(params);  // Type-safe!
```

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│ Concept-Based Type Requirements (Compile-Time)             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ParseNodeType<T>        AstParamType<T>                   │
│       ↓                        ↓                            │
│  ┌─────────┐             ┌──────────┐                      │
│  │ FooNode │────────────→│FooAstParams│                     │
│  └─────────┘             └──────────┘                      │
│       │                        │                            │
│       │ astParams()            │ template<>                 │
│       │ returns                │ create<FooAstParams>()     │
│       ↓                        ↓                            │
│                          ┌──────────┐                       │
│                          │FooAstNode│                       │
│                          └──────────┘                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘

Key Insight: Each node type uses its own parameter type,
             but all satisfy the same concept requirements!
```

## File Structure

```
toy_cpp/
├── include/
│   ├── parse_node.h         # Defines ParseNodeType concept
│   └── ast_node.h           # Defines AstParamType concept + template create
│
├── src/
│   ├── ast_params/
│   │   ├── foo_ast_params.h # Foo-specific parameters
│   │   └── bar_ast_params.h # Bar-specific parameters
│   │
│   ├── parse_nodes/
│   │   ├── foo_node.h       # FooNode : TypedNode<FooNode, FooAstParams>
│   │   └── bar_node.h       # BarNode : TypedNode<BarNode, BarAstParams>
│   │
│   └── ast_nodes/
│       ├── foo_ast_node.h   # FooAstNode + create<FooAstParams> specialization
│       └── bar_ast_node.h   # BarAstNode + create<BarAstParams> specialization
```

## Key Features

### ✅ Type Safety
```cpp
FooNode foo;
auto params = foo.astParams();  // Type: FooAstParams

// This compiles:
auto ast = ast_node::create<FooAstParams>(params);  ✅

// This doesn't compile:
auto wrong = ast_node::create<BarAstParams>(params);  ❌
// Compiler error: no matching function
```

### ✅ Compile-Time Verification
```cpp
struct FooNode : parse_node::TypedNode<FooNode, FooAstParams> {
    // ...
};

// Compiler verifies FooNode satisfies the concept:
static_assert(parse_node::ParseNodeType<FooNode>);
```

### ✅ No Runtime Overhead
- Template specialization → zero runtime cost
- Compiler inlines everything
- No virtual dispatch for param creation
- No factory map lookups

### ✅ Clean Module Boundaries
```
foo_ast_params.h  →  No dependencies (pure data)
     ↓
foo_node.h       →  Depends only on parse_node.h + foo_ast_params.h
     ↓
foo_ast_node.h   →  Depends only on ast_node.h + foo_ast_params.h
```

Each node type is a completely independent module!

## Usage Example

```cpp
#include "src/parse_nodes/foo_node.h"
#include "src/ast_nodes/foo_ast_node.h"

int main() {
    // Step 1: Create parse node
    FooNode parseNode("input_data");
    std::cout << parseNode.get_shape() << std::endl;  // "foo_shape"
    
    // Step 2: Get type-specific params
    FooAstParams params = parseNode.astParams();
    std::cout << "Foo data: " << params.fooSpecificData << std::endl;
    
    // Step 3: Create AST node (type-safe!)
    auto astNode = ast_node::create<FooAstParams>(params);
    std::cout << astNode->debugName() << std::endl;
    
    // Step 4: Continue pipeline...
    auto logicalParams = astNode->logicalParams();
    auto logicalNode = logical_node::create(logicalParams);
}
```

## Benefits Over Previous Design

| Feature | Old (Single AstParams) | New (Concept-Based) |
|---------|----------------------|---------------------|
| Type Safety | ⚠️ Runtime checks | ✅ Compile-time |
| Node-Specific Fields | ❌ No | ✅ Yes |
| Performance | ⚠️ Virtual calls | ✅ Zero overhead |
| Extensibility | ⚠️ Limited | ✅ Unlimited |
| Error Messages | ⚠️ Runtime errors | ✅ Compile-time errors |
| `alwayslink` Needed | ✅ Yes | ❌ No (for templates) |

## Verified with Bazel

```bash
$ bazel build //...
✅ Build completed successfully

$ bazel run //:toy_app
=== Concept-Based Node Pipeline Demo ===
...
AST params: nodeType=foo, fooData=42
AST debug name: FooAstNode[foo]: ...data: my_input_data (data=42)
...
=== Pipeline Complete ===
```

## Technical Details

### C++20 Features Used

1. **Concepts** - Define compile-time requirements
2. **`requires` clauses** - Constrain templates
3. **Template specialization** - Type-safe creation functions
4. **CRTP** (Curiously Recurring Template Pattern) - Type information at compile time

### Build Configuration

```python
# .bazelrc
build --cxxopt=-std=c++20  # Enable C++20
build --host_cxxopt=-std=c++20
```

## Adding More Node Types

To add a new node type "bar":

1. Create `src/ast_params/bar_ast_params.h` with bar-specific fields
2. Create `src/parse_nodes/bar_node.h` implementing ParseNodeType concept
3. Create `src/ast_nodes/bar_ast_node.h` with `create<BarAstParams>` specialization
4. Add to BUILD.bazel

**No changes needed to infrastructure code!**

## Comparison to Other Patterns

### vs. Variant/Union Types
```cpp
// Variant approach (NOT used):
std::variant<FooAstParams, BarAstParams> params;  ❌
// Problems: Runtime overhead, visitor boilerplate, error-prone

// Template approach (USED):
template<typename ParamType>
std::unique_ptr<Node> create(const ParamType& params);  ✅
// Benefits: Zero overhead, type-safe, clean
```

### vs. Inheritance Hierarchy
```cpp
// Inheritance approach (NOT used):
struct AstParams {
    virtual ~AstParams() = default;
};
struct FooAstParams : AstParams { ... };  ❌
// Problems: Virtual dispatch, heap allocation, type erasure

// Template approach (USED):
struct FooAstParams { ... };  // Plain struct  ✅
// Benefits: No virtual calls, stack-allocated, full type info
```

## Documentation Files

- 📘 **`CONCEPTS_GUIDE.md`** - Complete guide to the concept-based system
- 📘 **`CONCEPTS_SUMMARY.md`** - This file (executive summary)
- 📘 **`ARCHITECTURE.md`** - Clean module boundaries (from previous refactoring)
- 📘 **`NODE_PIPELINE_GUIDE.md`** - Original pipeline guide

## Success Metrics

✅ **Type Safety**: Compile-time verification via concepts
✅ **Modularity**: Each node type is independent
✅ **Performance**: Zero runtime overhead from type system
✅ **Extensibility**: Easy to add new node types
✅ **Modern C++**: Leverages C++20 features
✅ **Production Ready**: Builds and runs successfully

## Conclusion

You now have a **cutting-edge C++ architecture** that:
- Uses C++20 concepts for compile-time type safety
- Allows each node type to have its own parameter type
- Has zero runtime overhead from the type system
- Maintains clean module boundaries
- Is easy to extend with new node types

This is **production-quality modern C++** demonstrating advanced language features in a practical, maintainable way! 🎉

