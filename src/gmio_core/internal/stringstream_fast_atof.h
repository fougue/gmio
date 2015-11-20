/* Copyright (C) 2002-2012 Nikolaus Gebhardt
 * This file is part of the "Irrlicht Engine" and the "irrXML" project.
 * For conditions of distribution and use, see copyright notice in irrlicht.h
 * and irrXML.h
 */

/* Adapted to ISO-C90 */

#ifndef GMIO_INTERNAL_STRINGSTREAM_FAST_ATOF_H
#define GMIO_INTERNAL_STRINGSTREAM_FAST_ATOF_H

#include "fast_atof.h"
#include "stringstream.h"

GMIO_INLINE uint32_t gmio_stringstream_strtoul10(gmio_stringstream_t* it)
{
    unsigned int value = 0;
    const char* in = gmio_stringstream_current_char(it);
    for (;
         in != NULL && gmio_ascii_isdigit(*in);
         in = gmio_stringstream_next_char(it))
    {
        value = (value * 10) + (*in - '0');
    }
    return value;
}

GMIO_INLINE int32_t gmio_stringstream_strtol10(gmio_stringstream_t* it)
{
    const char* in = gmio_stringstream_current_char(it);
    const gmio_bool_t inv = (*in == '-');
    int value = 0;
    if (inv || *in == '+')
        in = gmio_stringstream_next_char(it);

    value = gmio_stringstream_strtoul10(it);
    if (inv)
        value = -value;
    return value;
}

typedef struct
{
    float val;
    unsigned char_diff;
} gmio_stringstream_strtof10_result_t;

GMIO_INLINE gmio_stringstream_strtof10_result_t gmio_stringstream_strtof10(
        gmio_stringstream_t* it)
{
    const char* in = gmio_stringstream_current_char(it);
    const uint32_t MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
    uint32_t int_val = 0;
    float float_val = 0.f;
    unsigned char_diff = 0;
    gmio_stringstream_strtof10_result_t result;

    /* Use integer arithmetic for as long as possible, for speed and
     * precision */
    for (;
         in != NULL && gmio_ascii_isdigit(*in) && int_val < MAX_SAFE_U32_VALUE;
         in = gmio_stringstream_next_char(it))
    {
        int_val = (int_val * 10) + (*in - '0');
        ++char_diff;
    }
    float_val = (float)int_val;
    /* If there are any digits left to parse, then we need to use floating point
     * arithmetic from here */
    for (;
         in != NULL && gmio_ascii_isdigit(*in) && float_val <= FLT_MAX;
         in = gmio_stringstream_next_char(it))
    {
        float_val = (float_val * 10) + (*in - '0');
        ++char_diff;
    }
    result.val = float_val;
    result.char_diff = char_diff;
    return result;
}

GMIO_INLINE float gmio_stringstream_fast_atof(gmio_stringstream_t* it)
{
    const char* in = gmio_stringstream_current_char(it);
    const gmio_bool_t negative = ('-' == *in);
    float value = 0.f;

    /* Please run the regression test when making any modifications to this
     * function. */
    if (negative || ('+' == *in))
        in = gmio_stringstream_next_char(it);
    value = gmio_stringstream_strtof10(it).val;
    in = gmio_stringstream_current_char(it);
    if (is_local_decimal_point(*in)) {
        const gmio_stringstream_strtof10_result_t decimal =
                gmio_stringstream_strtof10(
                    gmio_stringstream_move_next_char(it));
        value += decimal.val * fast_atof_table[decimal.char_diff];
        in = gmio_stringstream_current_char(it);
    }
    if (in != NULL && ('e' == *in || 'E' == *in)) {
        in = gmio_stringstream_next_char(it);
        /* Assume that the exponent is a whole number.
         * strtol10() will deal with both + and - signs,
         * but calculate as float to prevent overflow at FLT_MAX */
        value *=
#ifdef GMIO_HAVE_POWF_FUNC
                powf(10.f, (float)gmio_stringstream_strtol10(it));
#else
                (float)pow(10., (double)gmio_stringstream_strtol10(it));
#endif
    }
    return negative ? -value : value;
}

#endif /* GMIO_INTERNAL_STRINGSTREAM_FAST_ATOF_H */
