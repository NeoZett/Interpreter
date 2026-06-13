#include "LexerFactory.hpp"

namespace LexerFactory
{
    template<typename TokenType>
    Lexer<TokenType> create(const Source& source)
    {
        std::vector<typename Lexer<TokenType>::RulePtr> rules;

        rules.push_back(std::make_unique<IdentifierRule<TokenType>>());
        rules.push_back(std::make_unique<NumberRule<TokenType>>());
        rules.push_back(std::make_unique<StringRule<TokenType>>());

        rules.push_back(std::make_unique<PlusRule<TokenType>>());
        rules.push_back(std::make_unique<MinusRule<TokenType>>());
        rules.push_back(std::make_unique<EqualRule<TokenType>>());

        return Lexer<TokenType>(source, std::move(rules));
    }
}