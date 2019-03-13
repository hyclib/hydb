//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  UTF 转换器实现; reference to ConvertUTF.h
//



//-----------------------------------------------
// the content of ConvertUTF
//-----------------------------------------------

/*
* Copyright 2001-2004 Unicode, Inc.
* 
* Disclaimer
* 
* This source code is provided as is by Unicode, Inc. No claims are
* made as to fitness for any particular purpose. No warranties of any
* kind are expressed or implied. The recipient agrees to determine
* applicability of information provided. If this file has been
* purchased on magnetic or optical media from Unicode, Inc., the
* sole remedy for any claim will be exchange of defective media
* within 90 days of receipt.
* 
* Limitations on Rights to Redistribute This Code
* 
* Unicode, Inc. hereby grants the right to freely use the information
* supplied in this file in the creation of products supporting the
* Unicode Standard, and to make copies of this file in any form
* for internal or external distribution as long as this notice
* remains attached.
*/

/* ---------------------------------------------------------------------

Conversions between UTF32, UTF-16, and UTF-8.  Header file.

Several funtions are included here, forming a complete set of
conversions between the three formats.  UTF-7 is not included
here, but is handled in a separate source file.

Each of these routines takes pointers to input buffers and output
buffers.  The input buffers are const.

Each routine converts the text between *sourceStart and sourceEnd,
putting the result into the buffer between *targetStart and
targetEnd. Note: the end pointers are *after* the last item: e.g. 
*(sourceEnd - 1) is the last item.

The return result indicates whether the conversion was successful,
and if not, whether the problem was in the source or target buffers.
(Only the first encountered problem is indicated.)

After the conversion, *sourceStart and *targetStart are both
updated to point to the end of last text successfully converted in
the respective buffers.

Input parameters:
sourceStart - pointer to a pointer to the source buffer.
The contents of this are modified on return so that
it points at the next thing to be converted.
targetStart - similarly, pointer to pointer to the target buffer.
sourceEnd, targetEnd - respectively pointers to the ends of the
two buffers, for overflow checking only.

These conversion functions take a DB_ConversionFlags argument. When this
flag is set to strict, both irregular sequences and isolated surrogates
will cause an error.  When the flag is set to lenient, both irregular
sequences and isolated surrogates are converted.

Whether the flag is strict or lenient, all illegal sequences will cause
an error return. This includes sequences such as: <F4 90 80 80>, <C0 80>,
or <A0> in UTF-8, and values above 0x10FFFF in UTF-32. Conformant code
must check for illegal sequences.

When the flag is set to lenient, characters over 0x10FFFF are converted
to the replacement character; otherwise (when the flag is set to strict)
they constitute an error.

Output parameters:
The value "sourceIllegal" is returned from some routines if the input
sequence is malformed.  When "sourceIllegal" is returned, the source
value will point to the illegal value that caused the problem. E.g.,
in UTF-8 when a sequence is malformed, it points to the start of the
malformed sequence.  

Author: Mark E. Davis, 1994.
Rev History: Rick McGowan, fixes & updates May 2001.
Fixes & updates, Sept 2001.

------------------------------------------------------------------------ */

/* ---------------------------------------------------------------------
The following 4 definitions are compiler-specific.
The C standard does not guarantee that wchar_t has at least
16 bits, so wchar_t is no less portable than unsigned short!
All should be unsigned values to avoid sign extension during
bit mask & shift operations.
------------------------------------------------------------------------ */

typedef unsigned int	UTF32;	/* at least 32 bits */
typedef unsigned short	UTF16;	/* at least 16 bits */
typedef unsigned char	UTF8;	/* typically 8 bits */
typedef unsigned char	Boolean; /* 0 or 1 */

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

typedef enum {
    conversionOK, 		/* conversion successful */
    sourceExhausted,	/* partial character in source, but hit end */
    targetExhausted,	/* insuff. room in target for conversion */
    sourceIllegal		/* source sequence is illegal/malformed */
} DB_ConversionResult;

typedef enum {
    strictConversion = 0,
    lenientConversion
} DB_ConversionFlags;

/* This is for C++ and does no harm in C */
#ifdef __cplusplus
extern "C" {
#endif

    DB_ConversionResult DB_ConvertUTF8toUTF16 (
        const UTF8** sourceStart, const UTF8* sourceEnd, 
        UTF16** targetStart, UTF16* targetEnd, DB_ConversionFlags flags);

    DB_ConversionResult DB_ConvertUTF16toUTF8 (
        const UTF16** sourceStart, const UTF16* sourceEnd, 
        UTF8** targetStart, UTF8* targetEnd, DB_ConversionFlags flags);

    DB_ConversionResult DB_ConvertUTF8toUTF32 (
        const UTF8** sourceStart, const UTF8* sourceEnd, 
        UTF32** targetStart, UTF32* targetEnd, DB_ConversionFlags flags);

    DB_ConversionResult DB_ConvertUTF32toUTF8 (
        const UTF32** sourceStart, const UTF32* sourceEnd, 
        UTF8** targetStart, UTF8* targetEnd, DB_ConversionFlags flags);

    DB_ConversionResult DB_ConvertUTF16toUTF32 (
        const UTF16** sourceStart, const UTF16* sourceEnd, 
        UTF32** targetStart, UTF32* targetEnd, DB_ConversionFlags flags);

    DB_ConversionResult DB_ConvertUTF32toUTF16 (
        const UTF32** sourceStart, const UTF32* sourceEnd, 
        UTF16** targetStart, UTF16* targetEnd, DB_ConversionFlags flags);

    Boolean DB_isLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd);

#ifdef __cplusplus
}
#endif

/* --------------------------------------------------------------------- */
#ifdef CVTUTF_DEBUG
#include <stdio.h>
#endif

static const int DB_halfShift  = 10; /* used for shifting by 10 bits */

static const UTF32 DB_halfBase = 0x0010000UL;
static const UTF32 DB_halfMask = 0x3FFUL;

#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF
#define false	   0
#define true	    1

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF32toUTF16 (
                                      const UTF32** sourceStart, const UTF32* sourceEnd, 
                                      UTF16** targetStart, UTF16* targetEnd, DB_ConversionFlags flags) 
{
    DB_ConversionResult result = conversionOK;
    const UTF32* source = *sourceStart;
    UTF16* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch;
        if (target >= targetEnd) {
            result = targetExhausted; break;
        }
        ch = *source++;
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = (UTF16)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_LEGAL_UTF32) {
            if (flags == strictConversion) {
                result = sourceIllegal;
            } else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                --source; /* Back up source pointer! */
                result = targetExhausted; break;
            }
            ch -= DB_halfBase;
            *target++ = (UTF16)((ch >> DB_halfShift) + UNI_SUR_HIGH_START);
            *target++ = (UTF16)((ch & DB_halfMask) + UNI_SUR_LOW_START);
        }
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF16toUTF32 (
                                      const UTF16** sourceStart, const UTF16* sourceEnd, 
                                      UTF32** targetStart, UTF32* targetEnd, DB_ConversionFlags flags) 
{
    DB_ConversionResult result = conversionOK;
    const UTF16* source = *sourceStart;
    UTF32* target = *targetStart;
    UTF32 ch, ch2;
    while (source < sourceEnd) {
        const UTF16* oldSource = source; /*  In case we have to back up because of target overflow. */
        ch = *source++;
        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << DB_halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + DB_halfBase;
                    ++source;
                } else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                }
            } else { /* We don't have the 16 bits following the high surrogate. */
                --source; /* return to the high surrogate */
                result = sourceExhausted;
                break;
            }
        } else if (flags == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        if (target >= targetEnd) {
            source = oldSource; /* Back up source pointer! */
            result = targetExhausted; break;
        }
        *target++ = ch;
    }
    *sourceStart = source;
    *targetStart = target;
#ifdef CVTUTF_DEBUG
    if (result == sourceIllegal) {
        fprintf(stderr, "DB_ConvertUTF16toUTF32 illegal seq 0x%04x,%04x\n", ch, ch2);
        fflush(stderr);
    }
#endif
    return result;
}

/* --------------------------------------------------------------------- */

/*
* Index into the table below with the first Byte of a UTF-8 sequence to
* get the number of trailing bytes that are supposed to follow it.
* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
* left as-is for anyone who may want to do such conversion, which was
* allowed in earlier algorithms.
*/
static const char DB_trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
* Magic values subtracted from a buffer value during UTF8 conversion.
* This table contains as many values as there might be trailing bytes
* in a UTF-8 sequence.
*/
static const UTF32 DB_offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
* Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
* into the first Byte, depending on how many bytes follow.  There are
* as many entries in this table as there are UTF-8 sequence types.
* (I.e., one Byte sequence, two Byte... etc.). Remember that sequencs
* for *legal* UTF-8 will be 4 or fewer bytes total.
*/
static const UTF8 DB_firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/* --------------------------------------------------------------------- */

/* The interface converts a whole buffer to avoid function-call overhead.
* Constants have been gathered. Loops & conditionals have been removed as
* much as possible for efficiency, in favor of drop-through switches.
* (See "Note A" at the bottom of the file for equivalent code.)
* If your compiler supports it, the "DB_isLegalUTF8" call can be turned
* into an inline function.
*/

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF16toUTF8 (
                                     const UTF16** sourceStart, const UTF16* sourceEnd, 
                                     UTF8** targetStart, UTF8* targetEnd, DB_ConversionFlags flags) 
{
    DB_ConversionResult result = conversionOK;
    const UTF16* source = *sourceStart;
    UTF8* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch;
        unsigned short bytesToWrite = 0;
        const UTF32 byteMask = 0xBF;
        const UTF32 byteMark = 0x80; 
        const UTF16* oldSource = source; /* In case we have to back up because of target overflow. */
        ch = *source++;
        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                UTF32 ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << DB_halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + DB_halfBase;
                    ++source;
                } else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                }
            } else { /* We don't have the 16 bits following the high surrogate. */
                --source; /* return to the high surrogate */
                result = sourceExhausted;
                break;
            }
        } else if (flags == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /* Figure out how many bytes the result will require */
        if (ch < (UTF32)0x80) {	     bytesToWrite = 1;
        } else if (ch < (UTF32)0x800) {     bytesToWrite = 2;
        } else if (ch < (UTF32)0x10000) {   bytesToWrite = 3;
        } else if (ch < (UTF32)0x110000) {  bytesToWrite = 4;
        } else {			    bytesToWrite = 3;
        ch = UNI_REPLACEMENT_CHAR;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            source = oldSource; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
        case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 1: *--target =  (UTF8)(ch | DB_firstByteMark[bytesToWrite]);
        }
        target += bytesToWrite;
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}



/* --------------------------------------------------------------------- */

/*
* Utility routine to tell whether a sequence of bytes is legal UTF-8.
* This must be called with the length pre-determined by the first Byte.
* If not calling this from ConvertUTF8to*, then the length can be set by:
*  length = DB_trailingBytesForUTF8[*source]+1;
* and the sequence is illegal right away if there aren't that many bytes
* available.
* If presented with a length > 4, this returns false.  The Unicode
* definition of UTF-8 goes up to 4-Byte sequences.
*/

static Boolean DB_isLegalUTF8(const UTF8 *source, int length) {
    UTF8 a;
    const UTF8 *srcptr = source+length;
    switch (length) {
    default: return false;
        /* Everything else falls through when "true"... */
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 2: if ((a = (*--srcptr)) > 0xBF) return false;

        switch (*source) {
            /* no fall-through in this inner switch */
    case 0xE0: if (a < 0xA0) return false; break;
    case 0xED: if (a > 0x9F) return false; break;
    case 0xF0: if (a < 0x90) return false; break;
    case 0xF4: if (a > 0x8F) return false; break;
    default:   if (a < 0x80) return false;
        }

    case 1: if (*source >= 0x80 && *source < 0xC2) return false;
    }
    if (*source > 0xF4) return false;
    return true;
}

/* --------------------------------------------------------------------- */

/*
* Exported function to return whether a UTF-8 sequence is legal or not.
* This is not used here; it's just exported.
*/
Boolean DB_isLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd) {
    int length = DB_trailingBytesForUTF8[*source]+1;
    if (source+length > sourceEnd) {
        return false;
    }
    return DB_isLegalUTF8(source, length);
}

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF8toUTF16 (
                                     const UTF8** sourceStart, const UTF8* sourceEnd, 
                                     UTF16** targetStart, UTF16* targetEnd, DB_ConversionFlags flags) 
{
    DB_ConversionResult result = conversionOK;
    const UTF8* source = *sourceStart;
    UTF16* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch = 0;
        unsigned short extraBytesToRead = DB_trailingBytesForUTF8[*source];
        if (source + extraBytesToRead >= sourceEnd) {
            result = sourceExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (! DB_isLegalUTF8(source, extraBytesToRead+1)) {
            result = sourceIllegal;
            break;
        }
        /*
        * The cases all fall through. See "Note A" below.
        */
        switch (extraBytesToRead) {
        case 5: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 4: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 3: ch += *source++; ch <<= 6;
        case 2: ch += *source++; ch <<= 6;
        case 1: ch += *source++; ch <<= 6;
        case 0: ch += *source++;
        }
        ch -= DB_offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            source -= (extraBytesToRead+1); /* Back up source pointer! */
            result = targetExhausted; break;
        }
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = (UTF16)ch; /* normal case */
            }
        } else if (ch > UNI_MAX_UTF16) {
            if (flags == strictConversion) {
                result = sourceIllegal;
                source -= (extraBytesToRead+1); /* return to the start */
                break; /* Bail out; shouldn't continue */
            } else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } else {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                source -= (extraBytesToRead+1); /* Back up source pointer! */
                result = targetExhausted; break;
            }
            ch -= DB_halfBase;
            *target++ = (UTF16)((ch >> DB_halfShift) + UNI_SUR_HIGH_START);
            *target++ = (UTF16)((ch & DB_halfMask) + UNI_SUR_LOW_START);
        }
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF32toUTF8 (
                                     const UTF32** sourceStart, const UTF32* sourceEnd, 
                                     UTF8** targetStart, UTF8* targetEnd, DB_ConversionFlags flags) 
{
    DB_ConversionResult result = conversionOK;
    const UTF32* source = *sourceStart;
    UTF8* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch;
        unsigned short bytesToWrite = 0;
        const UTF32 byteMask = 0xBF;
        const UTF32 byteMark = 0x80; 
        ch = *source++;
        if (flags == strictConversion ) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /*
        * Figure out how many bytes the result will require. Turn any
        * illegally large UTF32 things (> Plane 17) into replacement chars.
        */
        if (ch < (UTF32)0x80) {	     bytesToWrite = 1;
        } else if (ch < (UTF32)0x800) {     bytesToWrite = 2;
        } else if (ch < (UTF32)0x10000) {   bytesToWrite = 3;
        } else if (ch <= UNI_MAX_LEGAL_UTF32) {  bytesToWrite = 4;
        } else {			    bytesToWrite = 3;
        ch = UNI_REPLACEMENT_CHAR;
        result = sourceIllegal;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            --source; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
        case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
        case 1: *--target = (UTF8) (ch | DB_firstByteMark[bytesToWrite]);
        }
        target += bytesToWrite;
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}

/* --------------------------------------------------------------------- */

DB_ConversionResult DB_ConvertUTF8toUTF32 (
                                     const UTF8** sourceStart, const UTF8* sourceEnd, 
                                     UTF32** targetStart, UTF32* targetEnd, DB_ConversionFlags flags)
{
    DB_ConversionResult result = conversionOK;
    const UTF8* source = *sourceStart;
    UTF32* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch = 0;
        unsigned short extraBytesToRead = DB_trailingBytesForUTF8[*source];
        if (source + extraBytesToRead >= sourceEnd) {
            result = sourceExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (! DB_isLegalUTF8(source, extraBytesToRead+1)) {
            result = sourceIllegal;
            break;
        }
        /*
        * The cases all fall through. See "Note A" below.
        */
        switch (extraBytesToRead) {
        case 5: ch += *source++; ch <<= 6;
        case 4: ch += *source++; ch <<= 6;
        case 3: ch += *source++; ch <<= 6;
        case 2: ch += *source++; ch <<= 6;
        case 1: ch += *source++; ch <<= 6;
        case 0: ch += *source++;
        }
        ch -= DB_offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            source -= (extraBytesToRead+1); /* Back up the source pointer! */
            result = targetExhausted; break;
        }
        if (ch <= UNI_MAX_LEGAL_UTF32) {
            /*
            * UTF-16 surrogate values are illegal in UTF-32, and anything
            * over Plane 17 (> 0x10FFFF) is illegal.
            */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = ch;
            }
        } else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
            result = sourceIllegal;
            *target++ = UNI_REPLACEMENT_CHAR;
        }
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}

/* ---------------------------------------------------------------------

Note A.
The fall-through switches in UTF-8 reading code save a
temp variable, some decrements & conditionals.  The switches
are equivalent to the following loop:
{
int tmpBytesToRead = extraBytesToRead+1;
do {
ch += *source++;
--tmpBytesToRead;
if (tmpBytesToRead) ch <<= 6;
} while (tmpBytesToRead > 0);
}
In UTF-8 writing code, the switches on "bytesToWrite" are
similarly unrolled loops.

--------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------
// the content of ConvertUTF
//----------------------------------------------------------------------------------------------------

#include "hyc/_platform.h"

#include "db_encodingUtils.hpp"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#include <Windows.h>
#include <wchar.h>
#endif

#ifndef MIN
#define MIN(A,B) ((A)<(B)?(A):(B))
#endif

namespace hydb
{

    static bool DB_CN_IS_UTF8_CSTRING() {
        static bool unchecked = true;
        static bool isUtf8 = false;
        if (unchecked)
        {
            unchecked = false;
            if (0 == strcmp("严", (char*)"\xe4\xb8\xa5"))
            {
                isUtf8 = true;
            }
        }

        return isUtf8;
    }

    //----------------------------------------------------------------------------------------------------
    //*	DB_EncodingUtils
    //----------------------------------------------------------------------------------------------------
     

    //-----------------------------------------------
    bool DB_EncodingUtils::IsLegalUTF8( const utf8* pCode, unsigned int length )
    {
        return 0 != DB_isLegalUTF8(pCode, length);
    }

    
    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF8toUTF16( const utf8* pSrcStream, unsigned int srcLength,  utf16* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf16* pDestStreamSrc = pDestStream;

        const utf8** ppSource = &pSrcStream;
        utf16** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF8toUTF16(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
        ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }

    }

    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF8toUTF32( const utf8* pSrcStream, unsigned int srcLength,  utf32* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf32* pDestStreamSrc = pDestStream;

        const utf8** ppSource = &pSrcStream;
        utf32** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF8toUTF32(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
            ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }
    }

    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF16toUTF8( const utf16* pSrcStream, unsigned int srcLength,  utf8* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf8* pDestStreamSrc = pDestStream;

        const utf16** ppSource = &pSrcStream;
        utf8** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF16toUTF8(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
            ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }
    }

    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF16toUTF32( const utf16* pSrcStream, unsigned int srcLength,  utf32* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf32* pDestStreamSrc = pDestStream;

        const utf16** ppSource = &pSrcStream;
        utf32** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF16toUTF32(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
            ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }
    }

    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF32toUTF8( const utf32* pSrcStream, unsigned int srcLength,  utf8* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf8* pDestStreamSrc = pDestStream;

        const utf32** ppSource = &pSrcStream;
        utf8** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF32toUTF8(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
            ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }
    }

    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF32toUTF16( const utf32* pSrcStream, unsigned int srcLength,  utf16* pDestStream, unsigned int destLength, ConversionFlag flags /*= CF_strict */ )
    {
        utf16* pDestStreamSrc = pDestStream;

        const utf32** ppSource = &pSrcStream;
        utf16** ppDest = &pDestStream;

        if (conversionOK == DB_ConvertUTF32toUTF16(
            ppSource, pSrcStream+srcLength, 
            ppDest, pDestStream+destLength,
            ((CF_strict==flags) ? strictConversion : lenientConversion)
            ))
        {
            return (unsigned int)(pDestStream-pDestStreamSrc);
        }
        else
        {
            return 0;
        }
    }

    //-----------------------------------------------
    //  UTF转换表; 只支持含最多3个字节表示的字符; 不支持少数的4字节字符
//     static const u16 g_UTF8TABLE[] = { 0x007F, 0x07FF, 0xFFFF, 0x10FFFF };

    // 获得字节的指定位
#define BIT_GET_WORD( v, index ) (((v)>>(index))&0x1)
#define BIT_GET_BYTE( v, index ) (((v)>>(index))&0x1)


    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF8ToStr( const utf8* pSrcStream, unsigned int srcLength,  char* pDestStream, unsigned int destLength )
    {
        if (!pDestStream || !pSrcStream)
        {
            return 0;
        }

        if (DB_CN_IS_UTF8_CSTRING())
        {
            unsigned int l = MIN(destLength, srcLength);
            memcpy(pDestStream, pSrcStream, l);
            return l;
        }
        else
        {
            unsigned int ll = destLength+1;
            wchar_t* pBuf = (wchar_t*)new wchar_t[ll];
            unsigned int pos = UTF8ToStr(pSrcStream, srcLength, pBuf, ll);
            if (pos == 0)
            {
                delete[] pBuf;
                return 0;
            }

            pBuf[pos] = L'\0';

            // to multibyte
            unsigned int dl = pos;

            // convert
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
            int c;
            int r = WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, pBuf, dl, pDestStream, destLength, "?", &c );
            if ( c || r == 0 )
#else
            unsigned int r = wcstombs( pDestStream, pBuf, dl );
            if (-1 == r)
#endif
            {
                delete[] pBuf;
                return 0;
            }

            delete[] pBuf;
            return r;
        }
    }
    
    //-----------------------------------------------
    unsigned int DB_EncodingUtils::UTF8ToStr( const utf8* pSrcStream, unsigned int srcLength,  wchar_t* pDestStream, unsigned int destLength )
    {
        if (!pDestStream || !pSrcStream)
        {
            return 0;
        }

        // unicode16.
        if (sizeof(wchar_t) == 2)
        {
            return UTF8toUTF16((utf8*)pSrcStream, srcLength, (utf16*)pDestStream, destLength);
        }
        // unicode32.
        else
        {
            return UTF8toUTF32((utf8*)pSrcStream, srcLength, (utf32*)pDestStream, destLength);
        }

//         if (!pDestStream || !pSrcStream)
//         {
//             return 0;
//         }
// 
//         wchar_t* pBuf = pDestStream;
//         unsigned int ll = destLength;
// 
// 
//         // 转换
//         unsigned int pos = 0;
//         unsigned int i = 0;
//         if (pSrcStream[0] == 0xEF
//             && pSrcStream[1] == 0xBB
//             && pSrcStream[2] == 0xBF)
//         {
//             i = 3;
//         }
// 
//         unsigned int maxlen = Math::Min(srcLength, destLength);
// 
//         for ( ; i < maxlen; ++i )
//         {
//             utf8 t = pSrcStream[i];
// 
//             if ( 0 == (t&0x80) ) // 单字节
//             {
//                 if ( pos == ll )    break;
// 
//                 pBuf[pos++] = (wchar_t)t;
//             }
//             else if ( 1 == BIT_GET_BYTE(t,7) && 1 == BIT_GET_BYTE(t,6) && 0 == BIT_GET_BYTE(t,5) )  // 双字节
//             {
//                 if ( pos == ll || i == srcLength-1 )    break;
// 
//                 wchar_t& t16 = pBuf[pos++];
//                 utf8 t1 = pSrcStream[i+1];
// 
//                 t16 = (wchar_t)t;
//                 t16 <<= 8+3;  t16 >>= 5;
// 
//                 t1 <<= 2;   t1 >>= 2;
//                 t16 |= (wchar_t)t1;
// 
//                 ++i;
//             }
//             else if ( 1 == BIT_GET_BYTE(t,7) && 1 == BIT_GET_BYTE(t,6) && 1 == BIT_GET_BYTE(t,5) && 0 == BIT_GET_BYTE(t,4) )  // 三字节
//             {
//                 if ( pos == ll || i == srcLength-2 )    break;
// 
//                 wchar_t& t16 = pBuf[pos++];
//                 utf16 t1 = (utf16)pSrcStream[i+1];
//                 utf8 t2 = pSrcStream[i+2];
// 
//                 t16 = (wchar_t)t;
//                 t16 <<= 8+4;
// 
//                 t1 <<= 8+2;   t1 >>= 4;
//                 t16 |= t1;
// 
//                 t2 <<= 2;   t2 >>= 2;
//                 t16 |= (wchar_t)t2;
// 
//                 i += 2;
//             } // if..else
//         } // for 
// 
//         return pos;
    }

    
} // end namespace hydb
