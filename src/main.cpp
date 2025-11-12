#include <iostream>
#include "lib.h"
#include "parse_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "node_transformer.h"
#include "src/ast_nodes/foo_ast_node.h"
#include "src/ast_nodes/bar_ast_node.h"
#include "src/parse_nodes/foo_node.h"
#include "src/parse_nodes/bar_node.h"

void processNode(const std::string& nodeType, const std::string& inputData) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Processing \"" << nodeType << "\" node" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Step 1: Create parse node polymorphically (from string input)
    std::cout << "\n[1] Creating parse node from input: \"" << inputData << "\"" << std::endl;
    auto parseNode = parse_node::createFromInput(nodeType, inputData);
    std::cout << "    Shape: " << parseNode->get_shape() << std::endl;
    
    // Step 2: Transform to AST node using the transformer
    std::cout << "\n[2] Transforming to AST node (via node_transformer)" << std::endl;
    auto astNode = node_transformer::parseToAst(parseNode);
    std::cout << "    " << astNode->debugName() << std::endl;
    
    // Step 3: Create logical node
    std::cout << "\n[3] Creating logical node" << std::endl;
    auto logicalParams = astNode->logicalParams();
    auto logicalNode = logical_node::create(logicalParams);
    std::cout << "    " << logicalNode->debugName() << std::endl;
    
    // Step 4: Show execution plan
    std::cout << "\n[4] Execution Plan:" << std::endl;
    std::cout << logicalNode->explain() << std::endl;
}

int main() {
    std::cout << "╔════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Multi-Type Node Pipeline Demonstration   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n2 + 2 = " << add(2, 2) << std::endl;
    
    // Demonstrate Foo node (has int fooSpecificData)
    processNode("foo", "my_input_data");
    
    // Demonstrate Bar node (has vector<string>, bool, double)
    processNode("bar", "bar_input_data");
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n✅ Both node types processed successfully!" << std::endl;
    std::cout << "\nKey observations:" << std::endl;
    std::cout << "  • FooNode uses FooAstParams with int fooSpecificData" << std::endl;
    std::cout << "  • BarNode uses BarAstParams with vector, bool, and double" << std::endl;
    std::cout << "  • Each node type has completely different fields" << std::endl;
    std::cout << "  • node_transformer works with both types polymorphically" << std::endl;
    std::cout << "  • Type-specific logic is preserved throughout the pipeline" << std::endl;
    std::cout << "  • No runtime type checks or casts needed!" << std::endl;
    
    return 0;
}
