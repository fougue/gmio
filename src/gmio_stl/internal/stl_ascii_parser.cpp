#include "stl_ascii_parser.h"

#include "../../gmio_core/internal/string_ascii_utils.h"

#include <algorithm>
#include <sstream>

namespace gmio {

//
// STL ASCII grammar:
//
//  CONTENTS  -> SOLID
//  CONTENTS  -> SOLID  SOLID
//
//  SOLID     -> BEG_SOLID  FACETS  END_SOLID
//  BEG_SOLID -> solid  SOLID_NAME
//  END_SOLID -> endsolid SOLID_NAME
//
//  SOLID_NAME ->
//  SOLID_NAME -> [id]  (Note: [id] == facet is forbidden)
//
//  FACETS ->
//  FACETS -> F
//  FACETS -> FF
//  F      -> facet N outer loop V V V endloop endfacet
//
//  V   -> vertex XYZ
//  N   -> normal XYZ
//  XYZ -> [float] [float] [float]
//
//
//  Nullable, FIRST and FOLLOW:
//
//              | Nullable |     FIRST    |           FOLLOW
//    ----------+----------+--------------+-----------------------------
//     CONTENTS |    N           solid
//        SOLID |    N           solid                 solid
//    BEG_SOLID |    N           solid                 facet
//    END_SOLID |    N          endsolid               solid
//   SOLID_NAME |    Y           [id]           facet, endsolid, solid
//       FACETS |    Y           facet
//            F |    N           facet             facet, endsolid
//            N |    N           normal                 outer
//            V |    N           vertex             vertex, endloop
//          XYZ |    N           [float]        outer, vertex, endloop
//

STL_AsciiParser::STL_AsciiParser(TextIStream* istream)
    : m_stream(istream)
{
}

bool STL_AsciiParser::parseBeginSolid(std::string *solidname)
{
    if (this->eatNextToken(Token_Solid))
        return this->parseSolidNameBegin(solidname);
    return false;
}

bool STL_AsciiParser::parseEndSolid()
{
    if (m_current_token == Token_EndSolid
            || this->eatNextToken(Token_EndSolid))
    {
        this->parseSolidNameEnd();
        return true;
    }
    return false;
}

const std::string &STL_AsciiParser::currentError() const
{
    return m_current_error;
}

STL_AsciiParser::Token STL_AsciiParser::currentToken() const
{
    return m_current_token;
}

// Qualifies input string as a token
STL_AsciiParser::Token STL_AsciiParser::findToken(const char *word, size_t word_len)
{
    // Try to find non "endXxx" token
    if (word_len >= 4) {
        switch (word[0]) {
        case 'f':
        case 'F':
            if (ascii_stricmp(word + 1, "acet") == 0)
                return Token_Facet;
            break;
        case 'l':
        case 'L':
            if (ascii_stricmp(word + 1, "oop") == 0)
                return Token_Loop;
            break;
        case 'n':
        case 'N':
            if (ascii_stricmp(word + 1, "ormal") == 0)
                return Token_Normal;
            break;
        case 'o':
        case 'O':
            if (ascii_stricmp(word + 1, "uter") == 0)
                return Token_Outer;
            break;
        case 's':
        case 'S':
            if (ascii_stricmp(word + 1, "olid") == 0)
                return Token_Solid;
            break;
        case 'v':
        case 'V':
            if (ascii_stricmp(word + 1, "ertex") == 0)
                return Token_Vertex;
            break;
        default:
            break;
        }
    }

    // Might be "end..." token
    if (word_len >= 7 && ascii_istartsWith(word, "end")) {
        switch (word[3]) {
        case 'f':
        case 'F':
            if (ascii_stricmp(word + 4, "acet") == 0)
                return Token_EndFacet;
            break;
        case 'l':
        case 'L':
            if (ascii_stricmp(word + 4, "oop") == 0)
                return Token_EndLoop;
            break;
        case 's':
        case 'S':
            if (ascii_stricmp(word + 4, "olid") == 0)
                return Token_EndSolid;
            break;
        default:
            break;
        }
    }

    // Get rid of ill-formed token
    if (word_len == 0)
        return Token_Empty;

    return Token_Id;
}

STL_AsciiParser::Token STL_AsciiParser::findToken(const std::string &word)
{
    return STL_AsciiParser::findToken(word.data(), word.size());
}

// Returns the string corresponding to token
const char *STL_AsciiParser::tokenToString(Token token)
{
#if 0
    static const Span<const char> array_strtoken[] = {
        {}, // null_token
        makeSpan("endfacet"),
        makeSpan("endloop"),
        makeSpan("endsolid"),
        makeSpan("facet"),
        makeSpan("loop"),
        makeSpan("normal"),
        makeSpan("outer"),
        makeSpan("solid"),
        makeSpan("vertex"),
        makeSpan("ID"), // ID_token
        {},             // empty_token
        makeSpan("?")   // unknown_token
    };
    return array_strtoken[token].data();
#else
    switch (token) {
    case Token_Null: return "";
    case Token_EndFacet: return "endfacet";
    case Token_EndLoop: return "endloop";
    case Token_EndSolid: return "endsolid";
    case Token_Facet: return "facet";
    case Token_Loop: return "loop";
    case Token_Normal: return "normal";
    case Token_Outer: return "outer";
    case Token_Solid: return "solid";
    case Token_Vertex: return "vertex";
    case Token_Id: return "ID";
    case Token_Empty: return "";
    case Token_Unknown: return "?";
    }
#endif
    return "?";
}

// Returns true if 'token' matches one of the candidate tokens
bool STL_AsciiParser::tokenMatchCandidate(Token token, Span<const Token> candidates)
{
    return std::find(candidates.cbegin(), candidates.cend(), token)
           != candidates.cend();
}

bool STL_AsciiParser::currentTokenMatchCandidate(Span<const Token> candidates) const
{
    return STL_AsciiParser::tokenMatchCandidate(m_current_token, candidates);
}

void STL_AsciiParser::error(const char *msg)
{
    std::stringstream sstr;
    sstr << "STL_AsciiParser error: " << msg << '\n'
         << "    current token: \"" << m_current_strtoken << '"';
    m_current_error = std::move(sstr.str());
}

void STL_AsciiParser::errorTokenExpected(Token token)
{
    std::stringstream sstr;
    sstr << "expected <" << STL_AsciiParser::tokenToString(token)
         << ">, got <" << STL_AsciiParser::tokenToString(m_current_token) << '>';
    this->error(sstr.str().c_str());
}

bool STL_AsciiParser::eatNextToken()
{
    m_current_strtoken.resize(0);
    const bool eat_ok = m_stream->eatWord(&m_current_strtoken);
    m_current_token =
            eat_ok ?
                STL_AsciiParser::findToken(m_current_strtoken) :
                Token_Unknown;
    return eat_ok;
}

// Eats next token string and checks it against an expected token
bool STL_AsciiParser::eatNextToken(Token expected_token)
{
    m_current_strtoken.resize(0);
    if (m_stream->eatWord(&m_current_strtoken)) {
        const char* expected_strtoken =
                STL_AsciiParser::tokenToString(expected_token);
        if (ascii_stricmp(m_current_strtoken.data(), expected_strtoken) == 0) {
            m_current_token = expected_token;
            return true;
        }
        m_current_token = Token_Unknown;
        if (expected_token == Token_Unknown)
            return true;
        this->errorTokenExpected(expected_token);
    }
    else {
        this->error("Failure to get next token");
    }
    return false;
}

// Eats next token string and checks it against an expected token.
// This procedure does the same thing as STL_AsciiParser::eatNextToken() but is
// faster as it does not copy the token string(it just "reads" it).
// It performs "in-place" case insensitive string comparison of the current
// token string against expected
bool STL_AsciiParser::eatNextTokenInplace(Token expected_token)
{
    const char* expected_token_str = STL_AsciiParser::tokenToString(expected_token);
    const char* curr_char = m_stream->skipAsciiSpaces();
    while (true) {
        if (curr_char == nullptr || ascii_isSpace(*curr_char)) {
            if (*expected_token_str == '\0') {
                m_current_token = expected_token;
                return true;
            }
            break;
        }
        else if (!ascii_iequals(*curr_char, *expected_token_str)
                 || *expected_token_str == '\0')
        {
            break;
        }
        else {
            curr_char = m_stream->nextChar();
            ++expected_token_str;
        }
    }

    // Error, copy the wrong token
    // -- Copy the matching part of the expected token
    m_current_strtoken.resize(0);
    const char* it = STL_AsciiParser::tokenToString(expected_token);
    while (it != expected_token_str)
        m_current_strtoken.push_back(*(it++));
    // -- Copy the non matching part
    while (curr_char != nullptr && !ascii_isSpace(*curr_char)) {
        m_current_strtoken.push_back(*curr_char);
        curr_char = m_stream->nextChar();
    }
    m_current_token = STL_AsciiParser::findToken(m_current_strtoken);

    // Notify error
    this->errorTokenExpected(expected_token);
    return false;
}

// Returns true if first char of input string is valid for float numbers
bool STL_AsciiParser::isFloatChar(const char *str)
{
    const char c = str != nullptr ? *str : '\0';
    return ascii_isDigit(c)
            || c == '-'
            || c == '.'
            || c == 'e'
            || c == 'E'
            || c == '+';
}

// Parses the (optional) solid name that appears after token "solid"
bool STL_AsciiParser::parseSolidNameBegin(std::string* solidname)
{
    if (this->eatNextToken()) {
        m_current_token = STL_AsciiParser::findToken(m_current_strtoken);
        if (m_current_token == Token_Facet
                || m_current_token == Token_EndSolid)
        {
            m_current_strtoken.clear();
            return true;
        }
        else {
            // Solid name can be made of multiple words
            static const Token end_tokens[] = { Token_Facet, Token_EndSolid };
            bool eat_ok = true;
            while (eat_ok && !this->currentTokenMatchCandidate(end_tokens)) {
                solidname->append(m_current_strtoken);
                m_stream->eatAsciiSpaces(solidname);
                eat_ok = this->eatNextToken();
            }
            ascii_trimEnd(solidname);
            return eat_ok;
        }
    }
    return false;
}

// Parses the (optional) solid name that appears after token "endsolid"
bool STL_AsciiParser::parseSolidNameEnd()
{
    // Eat whole line after "endsolid"
    const char* c = m_stream->currentChar();
    while (c != nullptr && *c != '\n' && *c != '\r')
        c = m_stream->nextChar();
    return true;
}

// Parses STL (x,y,z) coords, each coord being separated by whitespaces
bool STL_AsciiParser::parseXyzCoords(Vec3f *coords)
{
    const char* strbuff = nullptr;

    strbuff = m_stream->skipAsciiSpaces();
    const bool x_ok = STL_AsciiParser::isFloatChar(strbuff);
    coords->x = m_stream->eatFloat();

    strbuff = m_stream->skipAsciiSpaces();
    const bool y_ok = STL_AsciiParser::isFloatChar(strbuff);
    coords->y = m_stream->eatFloat();

    strbuff = m_stream->skipAsciiSpaces();
    const bool z_ok = STL_AsciiParser::isFloatChar(strbuff);
    coords->z = m_stream->eatFloat();

#if 0
    m_current_strtoken.resize(0);
    m_current_token = Token_Unknown;
#endif

    return x_ok && y_ok && z_ok;
}

bool STL_AsciiParser::parseFacet(STL_Triangle *facet)
{
    bool ok = true;
    if (m_current_token != Token_Facet)
        ok = ok && this->eatNextTokenInplace(Token_Facet);

    ok = ok && this->eatNextTokenInplace(Token_Normal);
    ok = ok && this->parseXyzCoords(&facet->n);

    ok = ok && this->eatNextTokenInplace(Token_Outer);
    ok = ok && this->eatNextTokenInplace(Token_Loop);

    ok = ok && this->eatNextTokenInplace(Token_Vertex);
    ok = ok && this->parseXyzCoords(&facet->v1);
    ok = ok && this->eatNextTokenInplace(Token_Vertex);
    ok = ok && this->parseXyzCoords(&facet->v2);
    ok = ok && this->eatNextTokenInplace(Token_Vertex);
    ok = ok && this->parseXyzCoords(&facet->v3);

    ok = ok && this->eatNextTokenInplace(Token_EndLoop);
    ok = ok && this->eatNextTokenInplace(Token_EndFacet);

    // Eat next unknown token
    if (ok && !this->eatNextToken())
        ok = false;

    return ok;
}

} // namespace gmio
