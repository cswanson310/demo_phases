#include "foo_node.h"
#include "include/parse_node.h"
#include <memory>

using namespace std::string_literals;

// This static object's constructor runs before main()
// registering the "foo" node type automatically
REGISTER_NODE(foo, [](std::string argString) {
    return std::make_unique<FooNode>();
});