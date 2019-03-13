//
// Copyright(c) 1999-2011 hyrj. All rights reserved.
// vines
// author:       lipengxiang
// description:  
//

#ifndef __BP_PREDEFINE_H_20111028110505__
#define __BP_PREDEFINE_H_20111028110505__

#include "../include/hydb.h"

// mysql
#ifdef DB_SUPPORT_MYSQL
#   ifdef _WIN32
#       pragma comment( lib, "libmysql.lib")
#   endif
//#include "../ext/mysql/include/mysql.h"
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <map>
#include <vector>
#endif

#define STRING_BUFFER_LENGTH 1024
#define KEY_BUFFER_LENGTH    64
#define GET_STR_VALUE_TEMP_BUF_LENGTH 10240

#include "utils/db_stringUtils.hpp"
#include "utils/db_thread.hpp"
#include "utils/db_encodingUtils.hpp"
#include "utils/db_log.hpp"

extern "C" s32 MYSQL_PING_INTERVAL;


#endif // end of define __BP_PREDEFINE_H_20111028110505__

