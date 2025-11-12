# Pipeline System - File Structure

## Core Infrastructure Files

### Parse Node Layer
- **`include/parse_node.h`** - Base parse node interface with `AstParams` struct and registration system
- **`src/parse_node.cpp`** - Parse node factory registry implementation

### AST Node Layer
- **`include/ast_node.h`** - Base AST node interface with `LogicalParams` struct and registration system
- **`src/ast_node.cpp`** - AST node factory registry implementation

### Logical Node Layer
- **`include/logical_node.h`** - Base logical node interface and registration system
- **`src/logical_node.cpp`** - Logical node factory registry implementation

## Example Node Implementations

### Foo Node (Complete Pipeline Example)

#### Parse Layer
- **`src/parse_nodes/foo_node.h`** - FooNode class definition
- **`src/parse_nodes/foo_node.cpp`** - FooNode registration

#### AST Layer
- **`src/ast_nodes/foo_ast_node.h`** - FooAstNode class definition
- **`src/ast_nodes/foo_ast_node.cpp`** - FooAstNode registration

#### Logical Layer
- **`src/logical_nodes/foo_logical_node.h`** - FooLogicalNode class definition
- **`src/logical_nodes/foo_logical_node.cpp`** - FooLogicalNode registration

## Build Configuration

- **`BUILD.bazel`** - Bazel build targets for all libraries and binaries
  - `:parse_node` - Core parse node library
  - `:parse_nodes_impl` - Parse node implementations (alwayslink)
  - `:ast_node` - Core AST node library
  - `:ast_nodes_impl` - AST node implementations (alwayslink)
  - `:logical_node` - Core logical node library
  - `:logical_nodes_impl` - Logical node implementations (alwayslink)
  - `:toy_app` - Main application binary

## Application

- **`src/main.cpp`** - Demonstrates the complete pipeline

## Documentation

- **`NODE_PIPELINE_GUIDE.md`** - Comprehensive guide for extending the system
- **`REGISTRATION_GUIDE.md`** - Guide for static registration system
- **`PIPELINE_FILES.md`** - This file

## Directory Structure

```
toy_cpp/
├── include/
│   ├── parse_node.h       # Parse layer base + registration
│   ├── ast_node.h         # AST layer base + registration
│   └── logical_node.h     # Logical layer base + registration
│
├── src/
│   ├── parse_node.cpp     # Parse registry implementation
│   ├── ast_node.cpp       # AST registry implementation
│   ├── logical_node.cpp   # Logical registry implementation
│   │
│   ├── parse_nodes/
│   │   ├── foo_node.h
│   │   └── foo_node.cpp
│   │
│   ├── ast_nodes/
│   │   ├── foo_ast_node.h
│   │   └── foo_ast_node.cpp
│   │
│   ├── logical_nodes/
│   │   ├── foo_logical_node.h
│   │   └── foo_logical_node.cpp
│   │
│   └── main.cpp
│
├── BUILD.bazel
├── NODE_PIPELINE_GUIDE.md
├── REGISTRATION_GUIDE.md
└── PIPELINE_FILES.md
```

## Key Design Patterns

### Static Registration Pattern
Each node implementation uses a macro to create a static object that registers itself:
```cpp
REGISTER_NODE(foo, factory)           // Parse nodes
REGISTER_AST_NODE(foo, factory)       // AST nodes
REGISTER_LOGICAL_NODE(foo, factory)   // Logical nodes
```

### Factory Pattern
Each layer has a factory function type:
```cpp
parse_node::Parser    = function(string) -> unique_ptr<Node>
ast_node::Factory     = function(nodeType, debugInfo) -> unique_ptr<Node>
logical_node::Factory = function(nodeType, optimizationHint) -> unique_ptr<Node>
```

### Parameter Passing Pattern
Layers communicate via parameter structs:
```cpp
parse_node::AstParams      // Parse → AST
ast_node::LogicalParams    // AST → Logical
```

## Adding a New Node Type

To add a new node type "xyz", create these 6 files:
1. `src/parse_nodes/xyz_node.h`
2. `src/parse_nodes/xyz_node.cpp`
3. `src/ast_nodes/xyz_ast_node.h`
4. `src/ast_nodes/xyz_ast_node.cpp`
5. `src/logical_nodes/xyz_logical_node.h`
6. `src/logical_nodes/xyz_logical_node.cpp`

Then add them to the appropriate `*_nodes_impl` libraries in `BUILD.bazel`.

See `NODE_PIPELINE_GUIDE.md` for detailed instructions.

