
#define DB_ELIMINATE

#include "../../include/hydb.h"
#ifdef DB_SUPPORT_SQLITE

#if CN_PLATFORM == CN_PLATFORM_IOS
#else
#include "../../ext/sqlite/sqlite3.c"
#endif

#include "hy_sqlite3.h"

SQLITE_API void wait_for_end( sqlite3 *db )
{
#if CN_PLATFORM == CN_PLATFORM_IOS
#else
    sqlite3_mutex_enter(db->mutex);
    sqlite3_mutex_leave(db->mutex);
#endif
}

#endif
