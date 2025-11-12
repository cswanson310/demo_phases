#include <iostream>
#include "lib.h"
#include "parse_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "node_transformer.h"
#include "ast_to_logical_transformer.h"
#include "src/ast_nodes/limit_ast_node.h"
#include "src/ast_nodes/bar_ast_node.h"
#include "src/parse_nodes/limit_node.h"
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
    
    // Step 3: Transform to logical node using the AST→Logical transformer
    std::cout << "\n[3] Transforming to logical node (via ast_to_logical_transformer)" << std::endl;
    auto logicalNode = ast_to_logical_transformer::astToLogical(astNode);
    std::cout << "    " << logicalNode->debugName() << std::endl;
    
    // Step 4: Show execution plan
    std::cout << "\n[4] Execution Plan:" << std::endl;
    std::cout << logicalNode->explain() << std::endl;
}

int main() {
    std::cout << "╔════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Multi-Type Node Pipeline Demonstration   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════╝" << std::endl;
    
    // Demonstrate Limit node (has single int limitValue)
    processNode("limit", "1000");
    
    // Demonstrate Bar node (has vector<string>, bool, double)
    processNode("bar", "bar_input_data");
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n✅ Both node types processed successfully!" << std::endl;
    std::cout << "\nKey observations:" << std::endl;
    std::cout << "  • LimitNode: Simple int parameter wrapped in type-safe structs" << std::endl;
    std::cout << "    - LimitAstParams { limitValue: int }" << std::endl;
    std::cout << "    - LimitLogicalParams { rowLimit: int }" << std::endl;
    std::cout << "  • BarNode: Complex parameters with multiple types" << std::endl;
    std::cout << "    - BarAstParams { items: vector<string>, flag: bool, cost: double }" << std::endl;
    std::cout << "    - BarLogicalParams { canUseIndex: bool, rows: int, selectivity: double }" << std::endl;
    std::cout << "  • Type-specific params at every phase: Parse → AST → Logical" << std::endl;
    std::cout << "  • Transformers handle all node types polymorphically" << std::endl;
    std::cout << "  • No runtime type checks or casts needed!" << std::endl;
    
    return 0;
}
