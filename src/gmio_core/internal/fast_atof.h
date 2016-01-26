/* Copyright (C) 2002-2012 Nikolaus Gebhardt
 * This file is part of the "Irrlicht Engine" and the "irrXML" project.
 * For conditions of distribution and use, see copyright notice in irrlicht.h
 * and irrXML.h
 */

/* Adapted to ISO-C90 */

#ifndef GMIO_INTERNAL_FAST_ATOF_H
#define GMIO_INTERNAL_FAST_ATOF_H

#include "../global.h"
#include "string_ascii_utils.h"

#include <float.h>
#include <limits.h>
#include <math.h>

GMIO_INLINE bool is_local_decimal_point(char in)
{
    /*! Selection of characters which count as decimal point in fast_atof
     * TODO: This should probably also be used in irr::core::string, but
     * the float-to-string code used there has to be rewritten first.
     */
    return in == '.';
}

/* #define ASSIMP_FAST_ATOF */
#define IRRLICH_FAST_ATOF

#if defined(ASSIMP_FAST_ATOF)

static const double fast_atof_table[16] =  {  // we write [16] here instead of [] to work around a swig bug
    0.0,
    0.1,
    0.01,
    0.001,
    0.0001,
    0.00001,
    0.000001,
    0.0000001,
    0.00000001,
    0.000000001,
    0.0000000001,
    0.00000000001,
    0.000000000001,
    0.0000000000001,
    0.00000000000001,
    0.000000000000001
};

/* ------------------------------------------------------------------------------------
 * Special version of the function, providing higher accuracy and safety
 * It is mainly used by fast_atof to prevent ugly and unwanted integer overflows.
 * ------------------------------------------------------------------------------------ */
GMIO_INLINE uint64_t strtoul10_64( const char* in, const char** out, unsigned int* max_inout)
{
    unsigned int cur = 0;
    uint64_t value = 0;
    const bool running = true;

    if ( !gmio_ascii_isdigit(*in) )
        return value;
        /* throw std::invalid_argument(std::string("The string \"") + in + "\" cannot be converted into a value."); */

    while ( running )
    {
        const uint64_t new_value = ( value * 10 ) + ( *in - '0' );

        if ( !gmio_ascii_isdigit(*in) )
            break;

        if (new_value < value) /* numeric overflow, we rely on you */
            return value;
            /* throw std::overflow_error(std::string("Converting the string \"") + in + "\" into a value resulted in overflow."); */

        value = new_value;

        ++in;
        ++cur;

        if (max_inout && *max_inout == cur) {

            if (out) { /* skip to end */
                while (gmio_ascii_isdigit(*in))
                    ++in;
                *out = in;
            }

            return value;
        }
    }
    if (out)
        *out = in;

    if (max_inout)
        *max_inout = cur;

    return value;
}

/* ------------------------------------------------------------------------------------
 * signed variant of strtoul10_64
 * ------------------------------------------------------------------------------------*/
GMIO_INLINE int64_t strtol10_64(const char* in, const char** out, unsigned int* max_inout)
{
    const bool inv = (*in == '-');
    int64_t value;
    if (inv || *in == '+')
        ++in;

    value = strtoul10_64(in, out, max_inout);
    if (inv) {
        value = -value;
    }
    return value;
}


/* Number of relevant decimals for floating-point parsing. */
#define AI_FAST_ATOF_RELAVANT_DECIMALS 15

/* ------------------------------------------------------------------------------------
 * Provides a fast function for converting a string into a float,
 * about 6 times faster than atof in win32.
 * If you find any bugs, please send them to me, niko (at) irrlicht3d.org.
 * ------------------------------------------------------------------------------------*/
GMIO_INLINE const char* fast_atoreal_move(const char* c, double* out, bool check_comma)
{
    double f = 0;
    const bool inv = (*c == '-');

    if (inv || *c == '+') {
        ++c;
    }

    if ((c[0] == 'N' || c[0] == 'n') && gmio_ascii_strincmp(c, "nan", 3) == 0)
    {
        /* TODO: write NAN */
        /* out = std::numeric_limits<Real>::quiet_NaN(); */
        c += 3;
        return c;
    }

    if ((c[0] == 'I' || c[0] == 'i') && gmio_ascii_strincmp(c, "inf", 3) == 0)
    {
        /* TODO: write INF */
        /* out = std::numeric_limits<Real>::infinity(); */
        if (inv) {
            *out = -(*out);
        }
        c += 3;
        if ((c[0] == 'I' || c[0] == 'i') && gmio_ascii_strincmp(c, "inity", 5) == 0)
        {
            c += 5;
        }
        return c;
    }

    if (!gmio_ascii_isdigit(c[0]) &&
        !((c[0] == '.' || (check_comma && c[0] == ',')) && gmio_ascii_isdigit(c[1])))
    {
        return c;
        /*throw std::invalid_argument("Cannot parse string "
                                    "as real number: does not start with digit "
                                    "or decimal point followed by digit.");*/
    }

    if (*c != '.' && (!check_comma || c[0] != ','))
    {
        f = (double)strtoul10_64(c, &c, NULL);
    }

    if ((*c == '.' || (check_comma && c[0] == ',')) && gmio_ascii_isdigit(c[1]))
    {
        unsigned int diff = AI_FAST_ATOF_RELAVANT_DECIMALS;
        double pl;
        ++c;

        /* NOTE: The original implementation is highly inaccurate here. The precision of a single
         * IEEE 754 float is not high enough, everything behind the 6th digit tends to be more
         * inaccurate than it would need to be. Casting to double seems to solve the problem.
         * strtol_64 is used to prevent integer overflow.
         *
         * Another fix: this tends to become 0 for long numbers if we don't limit the maximum
         * number of digits to be read. AI_FAST_ATOF_RELAVANT_DECIMALS can be a value between
         * 1 and 15. */
        pl = (double)strtoul10_64(c, &c, &diff);

        pl *= fast_atof_table[diff];
        f += pl;
    }
    /* For backwards compatibility: eat trailing dots, but not trailing commas. */
    else if (*c == '.') {
        ++c;
    }

    /* A major 'E' must be allowed. Necessary for proper reading of some DXF files.
     * Thanks to Zhao Lei to point out that this if() must be outside the if (*c == '.' ..) */
    if (*c == 'e' || *c == 'E') {
        const bool einv = (*(c+1)=='-');
        double exp;

        ++c;
        if (einv || *c=='+') {
            ++c;
        }

        /* The reason float constants are used here is that we've seen cases where compilers
         * would perform such casts on compile-time constants at runtime, which would be
         * bad considering how frequently fast_atoreal_move<float> is called in Assimp. */
        exp = (double)strtoul10_64(c, &c, NULL);
        if (einv) {
            exp = -exp;
        }
        f *= pow(10.0, exp);
    }

    if (inv) {
        f = -f;
    }
    *out = f;
    return c;
}

GMIO_INLINE float fast_atof(const char* c)
{
    double ret;
    fast_atoreal_move(c, &ret, true);
    return (float)ret;
}

GMIO_INLINE double fast_atod(const char* c)
{
    double ret;
    fast_atoreal_move(c, &ret, true);
    return ret;
}

GMIO_INLINE float fast_strtof(const char* str, const char** out)
{
    double ret;
    if (out)
        *out = fast_atoreal_move(str, &ret, true);
    else
        fast_atoreal_move(str, &ret, true);
    return (float)ret;
}


#elif defined(IRRLICH_FAST_ATOF)

/* we write [17] here instead of [] to work around a swig bug */
static const float fast_atof_table[17] = {
    0.f,
    0.1f,
    0.01f,
    0.001f,
    0.0001f,
    0.00001f,
    0.000001f,
    0.0000001f,
    0.00000001f,
    0.000000001f,
    0.0000000001f,
    0.00000000001f,
    0.000000000001f,
    0.0000000000001f,
    0.00000000000001f,
    0.000000000000001f,
    0.0000000000000001f
};

/*! Convert a simple string of base 10 digits into an unsigned 32 bit integer.
 *
 * \param[in] in: The string of digits to convert. No leading chars are
 * allowed, only digits 0 to 9. Parsing stops at the first non-digit.
 * \param[out] out: (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The unsigned integer value of the digits. If the string specifies
 * too many digits to encode in an uint32_t then INT_MAX will be returned.
 */
GMIO_INLINE uint32_t strtoul10(const char* in, const char** out)
{
    unsigned int value = 0;
    for (; gmio_ascii_isdigit(*in); ++in)
        value = (value * 10) + (*in - '0');
    if (out)
        *out = in;
    return value;
}

/*! Convert a simple string of base 10 digits into a signed 32 bit integer.
 *
 * \param[in] in: The string of digits to convert. Only a leading - or +
 * followed by digits 0 to 9 will be considered. Parsing stops at the first
 * non-digit.
 * \param[out] out: (optional) If provided, it will be set to point at the
 * first character not used in the calculation.
 * \return The signed integer value of the digits. If the string specifies
 * too many digits to encode in an int32_t then +INT_MAX or -INT_MAX will be
 * returned.
 */
GMIO_INLINE int32_t strtol10(const char* in, const char** out)
{
    const bool inv = (*in == '-');
    int value = 0;
    if (inv || *in == '+')
        ++in;

    value = strtoul10(in, out);
    if (inv)
        value = -value;
    return value;
}

/*! Converts a sequence of digits into a whole positive floating point value.
 *
 * Only digits 0 to 9 are parsed. Parsing stops at any other character,
 * including sign characters or a decimal point.
 * \param in: the sequence of digits to convert.
 * \param out: (optional) will be set to point at the first non-converted
 * character.
 * \return The whole positive floating point representation of the digit
 * sequence.
 */
GMIO_INLINE float strtof10(const char* in, const char** out)
{
    const uint32_t MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
    uint32_t int_val = 0;
    float float_val = 0.f;

    /* Use integer arithmetic for as long as possible, for speed and
     * precision */
    for (; gmio_ascii_isdigit(*in) && int_val < MAX_SAFE_U32_VALUE; ++in)
        int_val = (int_val * 10) + (*in - '0');
    float_val = (float)int_val;
    /* If there are any digits left to parse, then we need to use floating point
     * arithmetic from here */
    for (; gmio_ascii_isdigit(*in) && float_val <= FLT_MAX; ++in)
        float_val = (float_val * 10) + (*in - '0');
    if (out)
        *out = in;
    return float_val;
}

/*! Provides a fast function for converting a string into a float.
 *
 * This is not guaranteed to be as accurate as atof(), but is
 * approximately 6 to 8 times as fast.
 * \param[in] in The string to convert.
 * \param[out] result The resultant float will be written here.
 * \return Pointer to the first character in the string that wasn't used
 * to create the float value.
 */
GMIO_INLINE const char* fast_atof_move(const char* in, float* result)
{
    const bool negative = ('-' == *in);
    float value = 0.f;

    /* Please run the regression test when making any modifications to this
     * function. */
    *result = 0.f;
    if (negative || ('+' == *in))
        ++in;
    value = strtof10(in, &in);
    if (is_local_decimal_point(*in)) {
        const char* after_decimal = ++in;
        const float decimal = strtof10(in, &after_decimal);
        value += decimal * fast_atof_table[after_decimal - in];
        in = after_decimal;
    }
    if ('e' == *in || 'E' == *in) {
        ++in;
        /* Assume that the exponent is a whole number.
         * strtol10() will deal with both + and - signs,
         * but calculate as float to prevent overflow at FLT_MAX */
        value *=
#ifdef GMIO_HAVE_POWF_FUNC
                powf(10.f, (float)strtol10(in, &in));
#else
                (float)pow(10., (double)strtol10(in, &in));
#endif
    }
    *result = negative ? -value : value;
    return in;
}

/*! Convert a string to a floating point number
 *
 * \param str The string to convert.
 * \param out Optional pointer to the first character in the string that
 * wasn't used to create the float value.
 * \result Float value parsed from the input string
 */
GMIO_INLINE float fast_strtof(const char* str, const char** out)
{
    float ret;
    if (out)
        *out = fast_atof_move(str, &ret);
    else
        fast_atof_move(str, &ret);
    return ret;
}

GMIO_INLINE float fast_atof(const char* str)
{
    float ret;
    fast_atof_move(str, &ret);
    return ret;
}
#endif

#endif /* GMIO_INTERNAL_FAST_ATOF_H */
