//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// author:       lipengxiang
// description:  
//

#ifndef __BP_HY_SQLITE3_H_20140801110035__
#define __BP_HY_SQLITE3_H_20140801110035__

#if CN_PLATFORM == CN_PLATFORM_IOS
#include <sqlite3.h>
#else
#include "../../ext/sqlite/sqlite3.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

     void wait_for_end( sqlite3 *db );

#ifdef __cplusplus
};
#endif

#endif // end of define __BP_HY_SQLITE3_H_20140801110035__
