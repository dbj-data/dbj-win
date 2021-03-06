#pragma once

#include "../dbjwin_windows_includer.h"
// #include "dbj_start_stop.h"

#pragma warning( push )
#pragma warning( disable : 26451 )
#pragma warning( disable : 28182 )

DBJWIN_EXTERN_C_BEGIN 

#define DBJWIN_CLI_MAXLEN 1024 

	/*
	*  http://alter.org.ua/docs/win/args/
	*
	* for some reason it is not on MS DOCS any more
	* and not in Win32 any more
	*/

	// http://alter.org.ua/docs/win/args/
	inline PCHAR*
		CommandLineToArgvA(
			PCHAR CmdLine,
			int* _argc
		)
	{
		PCHAR* argv = NULL;
		PCHAR  _argv = NULL;
		ULONG   len = 0UL;
		ULONG   argc = 0UL;
		CHAR   a = 0;
		ULONG   i = 0UL, j = 0UL;

		BOOLEAN  in_QM = FALSE;
		BOOLEAN  in_TEXT = FALSE;
		BOOLEAN  in_SPACE = FALSE;

		
		// DBJ 2022JAN09 -- returns 0 on error
		len = (ULONG)dbjwin_strnlena(CmdLine, DBJWIN_CLI_MAXLEN );
		DBJWIN_TERROR( len > 0 );
		// len = (ULONG)strlen(CmdLine);

		i = ((len + 2UL) / 2) * sizeof(PVOID) + sizeof(PVOID);

		argv = (PCHAR*)LocalAlloc(GMEM_FIXED,
			i + (len + 2UL) * sizeof(CHAR));

		_argv = (PCHAR)(((PUCHAR)argv) + i);

		argc = 0;
		argv[argc] = _argv;
		in_QM = FALSE;
		in_TEXT = FALSE;
		in_SPACE = TRUE;
		i = 0;
		j = 0;

		while ((a = CmdLine[i])) {
			if (in_QM) {
				if (a == '\"') {
					in_QM = FALSE;
				}
				else {
					_argv[j] = a;
					j++;
				}
			}
			else {
				switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
				}
			}
			i++;
		}
		_argv[j] = '\0';
		argv[argc] = NULL;

		(*_argc) = argc;
		return argv;
	}
#pragma warning( pop )

DBJWIN_EXTERN_C_END

	//---------------------------------------------------------------------
	// TODO: refactor to C
	//---------------------------------------------------------------------
namespace dbj {

	//---------------------------------------------------------------------
	namespace {
	namespace detail {
		struct app_args_ final {
			int argc;
			char** argv;
		};
	}

    // initial value means "not harvested yet"
	inline detail::app_args_ app_cli_args = { 0,0 };


DBJWIN_CTOR_FUNCTION
	DBJWIN_INLINE void* harvest_app_args_(void*)
	{
		if (app_cli_args.argc < 1) {
			app_cli_args.argv =
				CommandLineToArgvA(
					GetCommandLineA(), &app_cli_args.argc
				);
		}
		DBJWIN_TERROR(app_cli_args.argc > 0);
		return 0;
	}

DBJWIN_DTOR_FUNCTION
	DBJWIN_INLINE void* free_app_args_(void*)
	{
		if (app_cli_args.argc > 0) {
			LocalFree(app_cli_args.argv);
			app_cli_args.argc = 0;
		}
		return 0;
	}
	}

	// arguments parsing/handling signals

	enum class app_args_result { proceed, stop };

	DBJWIN_INLINE app_args_result app_args_stop(const char*) { return app_args_result::stop; }
	DBJWIN_INLINE app_args_result app_args_proceed(const char*) { return app_args_result::proceed; }

	/*
	if arg not found return proceed
	otherwise return callback result

	note: this model does check only for arguments , not for arguments and their values

	"+help" will match
	"+help=whatever" will NOT match

	*/
	DBJWIN_INLINE app_args_result app_args_callback_(const char arg_name[], app_args_result(*callb_)(const char*))
	{
		// DBJ TODO: conditional MT lock here
		// must have more than 0 args
		DBJWIN_TERROR(app_cli_args.argc > 0);
		if (app_cli_args.argc < 2)
			return app_args_result::proceed;

		size_t arg_name_len = strnlen_s(arg_name, 0xFF);
		DBJWIN_TERROR(arg_name_len);

		// start from 1
		for (int index = 1; index < app_cli_args.argc; ++index)
		{
			if (0 == strncmp(app_cli_args.argv[index], arg_name, arg_name_len)) {
				// argument is found
				DBJWIN_TERROR((void*)callb_);
				return callb_(arg_name);
			}
		}
		return app_args_result::proceed;
	}
#if 0
	namespace win {

		//---------------------------------------------------------------------
		// this is simpler but each instance will make a copy of cli args
		struct cli_args final
		{
			int argc{};
			char** argv{};

			~cli_args() noexcept {
				if (argv != 0) {
					::LocalFree(argv);
					argc = 0;
					argv = nullptr;
					if (app_cli_args.argc < 1) {
						app_cli_args.argc = 0;
						app_cli_args.argv = nullptr;
					}
				}
			}

			// NOTE: rezult of GetCommandLineA() must NOT be freed
			// NOTE: rezult of CommandLineToArgvA() must be freed
			cli_args() noexcept : argv(
				CommandLineToArgvA(
					::GetCommandLineA(), &argc
				)
			)
			{
				if (app_cli_args.argc < 1) {
					// populate the C struct above 
					app_cli_args.argc = this->argc;
					app_cli_args.argv = this->argv;
				}
			}
			// no copy
			cli_args(cli_args const&) = delete;
			cli_args& operator = (cli_args const&) = delete;
			// no move
			cli_args(cli_args&&) = delete;
			cli_args& operator = (cli_args&&) = delete;

		}; // cli_args

		inline cli_args command_line_{};
	}
} // dbj::win
#endif // 0

} // dbj

