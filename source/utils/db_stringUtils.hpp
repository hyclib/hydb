//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// hyc libaray
// author:       lipengxiang
// description:  the utility of string
//

#ifndef __BP_DB_STRINGUTILS_H_20101206134234__
#define __BP_DB_STRINGUTILS_H_20101206134234__


namespace hydb
{
    /*---------------------------
    !  convert multibyte characters to a corresponding sequence of wide characters
    !  \param cMbs: The number of multibyte characters to convert
    !  \param cVcs: The length of the corresponding sequence of wide characters
    ---------------------------*/
    bool utils_string_Narrow2wide( const char* mbstr, uinteger cSizeMbs, OUT wchar_t* vcstr, OUT uinteger& cSizeVcs );

    /*---------------------------
    !  convert a corresponding sequence of wide characters to multibyte characters
    !  \param cVcs: The number of the corresponding sequence of wide characters to convert
    !  \param cMbs: The length of multibyte characters
    ---------------------------*/
    bool utils_string_Wide2narrow( const wchar_t* vcstr, uinteger cSizeVcs, OUT char* mbstr, OUT uinteger& cSizeMbs );


#if CN_PLATFORM == CN_PLATFORM_WINDOWS
    //-----------------------------------------------
    inline bool utils_string_Narrow2wide( const char* mbstr, uinteger cSizeMbs, OUT wchar_t* vcstr, OUT uinteger& cSizeVcs )
    {
        // check parameters
        if ( NULL == mbstr || NULL == vcstr || !cSizeMbs || !cSizeVcs )
            return false;

        // convert (in windows Platform)
        integer c = ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, mbstr, cSizeMbs, vcstr, cSizeVcs );
        cSizeVcs = c;

        return ( c != 0 );
    }

    //-----------------------------------------------
    inline bool utils_string_Wide2narrow( const wchar_t* vcstr, uinteger cSizeVcs, OUT char* mbstr, OUT uinteger& cSizeMbs )
    {
        // check parameters
        if ( NULL == mbstr || NULL == vcstr || !cSizeMbs || !cSizeVcs )
        {
            return false;
        }

        // convert (in windows Platform)
        BOOL r;
        integer c = WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, vcstr, cSizeVcs, mbstr, cSizeMbs, "?", &r );
        if ( r || c == 0 )
            return false;
        cSizeMbs = c;
        return true;
    }
#else
    //-----------------------------------------------
    inline bool utils_string_Narrow2wide( const char* mbstr, uinteger cSizeMbs, OUT wchar_t* vcstr, OUT uinteger& cSizeVcs )
    {
        // check parameters
        if ( NULL == mbstr || NULL == vcstr || !cSizeMbs || !cSizeVcs )
            return false;

        // convert
        uinteger r = mbstowcs( vcstr, mbstr, cSizeVcs );
        cSizeVcs = r;
        return true;
    }

    //-----------------------------------------------
    inline bool utils_string_Wide2narrow( const wchar_t* vcstr, uinteger cSizeVcs, OUT char* mbstr, OUT uinteger& cSizeMbs )
    {
        // check parameters
        if ( NULL == mbstr || NULL == vcstr || !cSizeMbs || !cSizeVcs )
        {
            return false;
        }

        // convert
        uinteger r = wcstombs( mbstr, vcstr, cSizeMbs );
        cSizeMbs = r;
        return true;

    }
#endif


    class DB_StringUtils
    {        
    public:
        /*---------------------------
        !  convert string to number
        ---------------------------*/
        
        static integer  StrToInteger( const char* pszStr );
        static uinteger StrToUInteger( const char* pszStr );
        static integer  StrToInteger( const wchar_t* pszStr );
        static uinteger StrToUInteger( const wchar_t* pszStr );
        
        static s32  StrToS32( const char* pszStr );
        static s64  StrToS64( const char* pszStr );
        static u32  StrToU32( const char* pszStr );
        static u64  StrToU64( const char* pszStr );
        static f32  StrToF32( const char* pszStr );
        static f64  StrToF64( const char* pszStr );
        static s32  StrToS32( const wchar_t* pszStr );
        static s64  StrToS64( const wchar_t* pszStr );
        static u32  StrToU32( const wchar_t* pszStr );
        static u64  StrToU64( const wchar_t* pszStr );
        static f32  StrToF32( const wchar_t* pszStr );
        static f64  StrToF64( const wchar_t* pszStr );
    };

} // end namespace hydb

#endif // end of define __BP_DB_STRINGUTILS_H_20101206134234__