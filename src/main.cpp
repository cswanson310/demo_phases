#include <iostream>
#include "lib.h"
#include "parse_node.h"
#include "ast_node.h"
#include "logical_node.h"
#include "node_transformer.h"
#include "src/ast_nodes/foo_ast_node.h"  // Include AST node BEFORE parse node
#include "src/parse_nodes/foo_node.h"

int main() {
    std::cout << "=== Concept-Based Node Pipeline Demo ===" << std::endl << std::endl;
    
    std::cout << "2 + 2 = " << add(2, 2) << std::endl << std::endl;
    
    // Step 1: Create a parse node polymorphically (from string input)
    std::cout << "Step 1: Creating parse node from input" << std::endl;
    auto myParseNode = parse_node::createFromInput("foo", "my_input_data");
    std::cout << "Parse shape: " << myParseNode->get_shape() << std::endl;
    
    // Step 2: Transform parse node to AST node using the transformer library
    std::cout << "\nStep 2: Transforming parse node to AST node (via transformer)" << std::endl;
    auto myAstNode = node_transformer::parseToAst(myParseNode);
    std::cout << "AST debug name: " << myAstNode->debugName() << std::endl;
    
    // Step 3: Create logical node
    std::cout << "\nStep 3: Creating logical node" << std::endl;
    auto logicalParams = static_cast<FooAstNode*>(myAstNode.get())->logicalParams();
    auto myLogicalNode = logical_node::create(logicalParams);
    std::cout << "Logical debug name: " << myLogicalNode->debugName() << std::endl;
    std::cout << "\n" << myLogicalNode->explain() << std::endl;
    
    std::cout << "\n=== Pipeline Complete ===" << std::endl;
    std::cout << "\nDemonstration:" << std::endl;
    std::cout << "- Parse node was created from string input \"foo\"" << std::endl;
    std::cout << "- node_transformer::parseToAst() handled the type-specific conversion" << std::endl;
    std::cout << "- Each node type manages its own parameter types internally" << std::endl;
    
    return 0;
}
