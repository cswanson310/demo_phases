#pragma once
#include <string>
#include <functional>
#include <memory>
#include <concepts>

// Forward declarations
struct AstNode;

// Base interface for parse nodes (can be used polymorphically)
struct ParseNode {
    virtual ~ParseNode() = default;
    virtual std::string get_shape() const = 0;
    
    // Virtual method to create the corresponding AST node
    // Each concrete parse node implements this using its specific param type
    virtual std::unique_ptr<AstNode> createAstNode() const = 0;
};

// CRTP base class for parse nodes with type-safe params
template<typename Derived, typename ParamType>
struct TypedParseNode : ParseNode {
    using AstParams = ParamType;
    
    // Derived must implement this
    virtual ParamType astParams() const = 0;
    
    // createAstNode() is inherited from ParseNode base class
    // Derived classes should override it
};

// Factory registration for polymorphic usage
using ParseNodeFactory = std::function<std::unique_ptr<ParseNode>(std::string)>;
void registerParseNode(std::string name, ParseNodeFactory factory);
std::unique_ptr<ParseNode> createParseNodeFromInput(std::string name, std::string argString);

// Helper class for automatic registration
class ParseNodeRegistrar {
public:
    ParseNodeRegistrar(std::string name, ParseNodeFactory factory) {
        registerParseNode(name, factory);
    }
};

#define REGISTER_PARSE_NODE(name, factory) \
    static ParseNodeRegistrar _parse_registrar_##name(#name, factory)
