#pragma once

#include "../../gmio_core/span.h"
#include "../../gmio_core/vecgeom.h"
#include "../stl_triangle.h"
#include "../../gmio_core/internal/text_istream.h"

#include <string>

namespace gmio {

class STL_AsciiParser {
public:
    enum Token {
        Token_Null = 0,
        Token_EndFacet,
        Token_EndLoop,
        Token_EndSolid,
        Token_Facet,
        Token_Loop,
        Token_Normal,
        Token_Outer,
        Token_Solid,
        Token_Vertex,
        Token_Id,
        Token_Float = Token_Id,
        Token_Empty,
        Token_Unknown
    };

    STL_AsciiParser(TextIStream* istream);

    // Parses "solid <name>"
    bool parseBeginSolid(std::string* solidname);

    // Parses an STL facet, ie. facet ... endfacet
    bool parseFacet(STL_Triangle* facet);

    // Parses "endsolid <name>"
    bool parseEndSolid();

    const std::string& currentError() const;

    Token currentToken() const;

private:
    static Token findToken(const char* word, size_t word_len);
    static Token findToken(const std::string& word);
    static const char* tokenToString(Token token);
    static bool tokenMatchCandidate(Token token, Span<const Token> candidates);
    bool currentTokenMatchCandidate(Span<const Token> candidates) const;

    void error(const char* msg);
    void errorTokenExpected(Token token);

    bool eatNextToken();
    bool eatNextToken(Token expected_token);
    bool eatNextTokenInplace(Token expected_token);

    static inline bool isFloatChar(const char* str);

    bool parseSolidNameBegin(std::string* solidname);
    bool parseSolidNameEnd();
    bool parseXyzCoords(Vec3f* coords);

    Token m_current_token = Token_Unknown;
    std::string m_current_strtoken;
    std::string m_current_error;
    TextIStream* m_stream;
};

} // namespace gmio
