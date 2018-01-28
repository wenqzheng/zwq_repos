#ifdef __cplusplus
extern "C" {
#endif

// header file for postgresql server
#include "/usr/local/pgsql/include/server/postgres.h"
#include "/usr/local/pgsql/include/server/fmgr.h"
#include "/usr/local/pgsql/include/server/utils/geo_decls.h"
#include "/usr/local/pgsql/include/server/utils/builtins.h"
#include "/usr/local/pgsql/include/server/executor/spi.h"

// magiv for module
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

// C like function declaration
PG_FUNCTION_INFO_V1(add_one);
PG_FUNCTION_INFO_V1(add_one_float8);
PG_FUNCTION_INFO_V1(makepoint);
PG_FUNCTION_INFO_V1(copytext);
PG_FUNCTION_INFO_V1(concat_text);
PG_FUNCTION_INFO_V1(execq);

Datum add_one(PG_FUNCTION_ARGS);
Datum add_one_float8(PG_FUNCTION_ARGS);
Datum makepoint(PG_FUNCTION_ARGS);
Datum copytext(PG_FUNCTION_ARGS);
Datum concat_text(PG_FUNCTION_ARGS);
Datum execq(PG_FUNCTION_ARGS);

#ifdef __cplusplus
}
#endif 
