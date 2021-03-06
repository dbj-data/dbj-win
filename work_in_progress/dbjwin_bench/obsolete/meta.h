#pragma once
/*
no includes in here please

CAUTION! DBJ_APP_NAME is used as folder name at one point
, do not use chars that must not be used in file/folder names

*/
#define DBJ_APP_NAME "DBJ+BENCH"
#define DBJ_APP_MAJOR 3
#define DBJ_APP_MINOR 0
#define DBJ_APP_PATCH 0
#define DBJ_APP_VERSION "3.0.0"
#define DBJ_APP_OWNER "dbj@dbj.org"
#define DBJ_APP_LICENSE "LICENSE_DBJ"
#define DBJ_APP_COPYRIGHT "(c) 2020 by dbj@dbj.org"

#pragma comment( user, DBJ_APP_COPYRIGHT ", " DBJ_APP_NAME "[" DBJ_APP_VERSION "] compiled by " DBJ_APP_OWNER  " on " __DATE__ " at " __TIME__ )

// command line args understood by DBJ+FWK
// '+' is used to differentiate from "normal" switches
#define DBJ_CL_ARG_SHOW_BUILD_ENV  "+build"
#define DBJ_CL_ARG_LOG_TEST  "+logtest"
#define DBJ_CL_ARG_IGNORE_UBENCH  "+nobench"
#define DBJ_CL_ARG_IGNORE_UTEST  "+notest"
#define DBJ_CL_ARG_HELP  "+help"

// this means direct EASTL header(s) includes
#define DBJ_FWK_EASTL_DIRECT_DEPENDANCY 1

// EOF