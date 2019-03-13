//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy yun network
// author:       lipengxiang
// description:  
//

#ifndef __BP_DB_LOG_HPP_20141110111027__
#define __BP_DB_LOG_HPP_20141110111027__

#include "db_stringUtils.hpp"

#define DB_LOG_PATH "./db_err.log"

namespace hydb
{
	inline void db_log( const char* msg )
    {
        FILE *pFile = fopen(DB_LOG_PATH, "ab+");
        if(pFile)
        {
            const char* rt = "\n";
            fseek(pFile, 0, SEEK_END);
            fwrite(msg, sizeof(u8), strlen(msg), pFile);
            fwrite(rt, sizeof(u8), 1, pFile);
            fclose(pFile);
        }
    }
    inline void db_log( const wchar_t* msgs )
    {
        char msg[STRING_BUFFER_LENGTH] = {0};
        uinteger l = STRING_BUFFER_LENGTH -1;
        utils_string_Wide2narrow(msgs, wcslen(msgs), msg, l);

        FILE *pFile = fopen(DB_LOG_PATH, "ab+");
        if(pFile)
        {
            const char* rt = "\n";
            fseek(pFile, 0, SEEK_END);
            fwrite(msg, sizeof(u8), strlen(msg), pFile);
            fwrite(rt, sizeof(u8), 1, pFile);
            fclose(pFile);
        }
    }
} // end namespace hydb

#endif // end of define __BP_DB_LOG_HPP_20141110111027__
