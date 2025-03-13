#ifndef BASE_LOG_H
#define BASE_LOG_H

typedef enum LogMsgKind
{
	LogMsgKind_Info,
	LogMsgKind_Warning,
	LogMsgKind_UserError,
	LogMsgKind_RecoverableError,
	LogMsgKind_FatalError,
	LogMsgKind_COUNT,
}
LogMsgKind;

typedef struct LogScope LogScope;
struct LogScope
{
	LogScope *next;
	S64 log_arena_pos;
	String8List strings[LogMsgKind_COUNT];
};

typedef struct LogScopeResult LogScopeResult;
struct LogScopeResult
{
	String8 strings[LogMsgKind_COUNT];
};

typedef struct Log Log;
struct Log
{
	Arena *arena;
	LogScope *top_scope; // SLL stack
};

internal Log *log_alloc(void);
internal void log_release(Log *log);
internal void log_select(Log *log);

internal void log_msg(LogMsgKind kind, String8 string);
internal void log_msgf(LogMsgKind kind, char *fmt, ...);
#define log_info(s)                      log_msg(LogMsgKind_Info, (s))
#define log_infof(fmt, ...)              log_msgf(LogMsgKind_Info, (fmt), __VA_ARGS__)
#define log_warning(s)                   log_msg(LogMsgKind_Warning, (s))
#define log_warningf(fmt, ...)           log_msgf(LogMsgKind_Warning, (fmt), __VA_ARGS__)
#define log_user_error(s)                log_msg(LogMsgKind_UserError, (s))
#define log_user_errorf(fmt, ...)        log_msgf(LogMsgKind_UserError, (fmt), __VA_ARGS__)
#define log_recoverable_error(s)         log_msg(LogMsgKind_RecoverableError, (s))
#define log_recoverable_errorf(fmt, ...) log_msgf(LogMsgKind_RecoverableError, (fmt), __VA_ARGS__)
#define log_fatal_error(s)               log_msg(LogMsgKind_FatalError, (s))
#define log_fatal_errorf(fmt, ...)       log_msgf(LogMsgKind_FatalError, (fmt), __VA_ARGS__)

#define LogInfoNamedBlock(s) DeferLoop(log_infof("%S:\n{\n", (s)), log_infof("}\n"))
#define LogInfoNamedBlockF(fmt, ...) DeferLoop((log_infof(fmt, __VA_ARGS__), log_infof(":\n{\n")), log_infof("}\n"))

internal void log_scope_begin(void);
internal LogScopeResult log_scope_end(Arena *arena);

#endif // BASE_LOG_H
