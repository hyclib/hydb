//
// Copyright(c) 1999-2010 hyrj. All rights reserved.
// author:       lipengxiang
// description:  编码工具; Unicode转换格式(UTF, UCS Transformation Format)
//

#ifndef __BP_ENCODINGUTILS_H_20101109131547__
#define __BP_ENCODINGUTILS_H_20101109131547__

namespace hydb
{
	//----------------------------------------------------------------------------------------------------
	//! Define UTF type
	//----------------------------------------------------------------------------------------------------
    typedef unsigned char      utf8;   //! utf-8
    typedef unsigned short     utf16;  //! utf-16
    typedef unsigned int       utf32;  //! utf-32


    //----------------------------------------------------------------------------------------------------
    //!	convert between utf-8, utf-16 and utf-32
    //----------------------------------------------------------------------------------------------------
    class DB_EncodingUtils
    {
    public:
        //! 转换标记
        typedef enum {
            CF_strict = 0,  //! 严格转换
            CF_lenient      //! 宽松转换
        } ConversionFlag;

    public:

        /*---------------------------
          utf编码
        ---------------------------*/

        //! 判断是否是合法的utf-8
        static bool IsLegalUTF8( const utf8* pStream, unsigned int length );

        //! string 与 UTF8 间的转换; 返回转换的字符长度.
        static unsigned int  UTF8ToStr( const utf8* pSrcStream, unsigned int srcLength,  char* pDestStream, unsigned int destLength );
        static unsigned int  UTF8ToStr( const utf8* pSrcStream, unsigned int srcLength,  wchar_t* pDestStream, unsigned int destLength );

        //! unicode间转换; 返回转换的字符长度.
        static unsigned int UTF8toUTF16( const utf8* pSrcStream, unsigned int srcLength,  utf16* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
        static unsigned int UTF8toUTF32( const utf8* pSrcStream, unsigned int srcLength,  utf32* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
        static unsigned int UTF16toUTF8( const utf16* pSrcStream, unsigned int srcLength,  utf8* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
        static unsigned int UTF16toUTF32( const utf16* pSrcStream, unsigned int srcLength,  utf32* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
        static unsigned int UTF32toUTF8( const utf32* pSrcStream, unsigned int srcLength,  utf8* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
        static unsigned int UTF32toUTF16( const utf32* pSrcStream, unsigned int srcLength,  utf16* pDestStream, unsigned int destLength, ConversionFlag flags = CF_strict );
    };

} // end namespace hydb

#endif // end of define __BP_ENCODINGUTILS_H_20101109131547__
