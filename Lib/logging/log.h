#pragma once

#include "logging_stack.h"

#define LogTime "time"

#define Log_info(X) LogInfo((X))
#define Log_warn(X) LogWarn((X))
#define Log_error(X) LogError((X))
#define Log_debug(X) LogDebug((X))
#define Log_always(X) LogAlways((X))
