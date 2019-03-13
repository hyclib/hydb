
#define DB_ELIMINATE

#include "../../include/hydb.h"

#if CN_PLATFORM == CN_PLATFORM_IOS
#else
#ifdef DB_SUPPORT_SQLITE
#include "../../ext/sqlite/shell.c"
#endif
#endif

