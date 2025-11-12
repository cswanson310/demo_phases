# Static Registration System Guide

This project uses a static initialization system that allows library code to register nodes before `main()` executes.

## How It Works

The registration system uses C++ static initialization. When you create a static object at global scope, its constructor runs before `main()` begins. The `NodeRegistrar` class leverages this to automatically register node types.

## Adding a New Node Type

### Step 1: Create your node header file

```cpp
// src/parse_nodes/bar_node.h
#pragma once
#include "parse_node.h"
#include <string>

struct BarNode : public parse_node::Node {
    std::string get_shape() const override {
        return "bar_shape";
    }
};
```

### Step 2: Create your node implementation file with registration

```cpp
// src/parse_nodes/bar_node.cpp
#include "bar_node.h"
#include "parse_node.h"
#include <memory>

// This macro expands to create a static NodeRegistrar object
// Its constructor runs before main(), registering the "bar" node type
REGISTER_NODE(bar, [](std::string argString) {
    return std::make_unique<BarNode>();
});
```

### Step 3: Add to BUILD.bazel

Add your new file to the `parse_nodes_impl` library:

```python
cc_library(
    name = "parse_nodes_impl",
    srcs = [
        "src/parse_nodes/foo_node.cpp",
        "src/parse_nodes/bar_node.cpp",  # Add your new file
    ],
    hdrs = [
        "src/parse_nodes/foo_node.h",
        "src/parse_nodes/bar_node.h",     # Add your new header
    ],
    includes = ["include"],
    deps = [":parse_node"],
    visibility = ["//visibility:public"],
    alwayslink = 1,  # IMPORTANT: This ensures registration code runs
)
```

### Step 4: Use it in main

```cpp
int main() {
    auto node = parse_node::createFromInput("bar", "some_arg");
    std::cout << node->get_shape() << std::endl;  // prints: bar_shape
    return 0;
}
```

## Key Points

1. **`alwayslink = 1`**: This is crucial! Without it, the linker may optimize out your registration code if no symbols are directly referenced.

2. **`REGISTER_NODE` macro**: Use this macro at global scope in your .cpp file. It creates a static object that registers your node during static initialization.

3. **Static initialization order**: Registration happens before `main()`, but the order between different .cpp files is undefined. Design your registration to not depend on initialization order.

4. **Thread safety**: If you need to support multithreading, you may need to add locks to the registration system.

## Example Output

When you run the program, you'll see registration messages before main() runs:

```
[Startup] Registering node type: foo
[Startup] Registering node type: bar
2 + 2 = 4
shape of one foo_shape
```

## Alternative: Manual Registration

If you prefer explicit registration over static initialization, you can create an initialization function:

```cpp
void initializeAllNodes() {
    parse_node::registerNode("foo", [](std::string s) { 
        return std::make_unique<FooNode>(); 
    });
    parse_node::registerNode("bar", [](std::string s) { 
        return std::make_unique<BarNode>(); 
    });
}

int main() {
    initializeAllNodes();  // Call explicitly
    // ... rest of main
}
```

However, static registration is more scalable as each node type can be added independently without modifying a central initialization function.

