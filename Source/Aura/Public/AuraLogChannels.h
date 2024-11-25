// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

/**
 * A macro to declare a logging category as a C++ "extern",
 * usually declared in the header and paired with DEFINE_LOG_CATEGORY in the source.
 * Accessible by all files that include the header.
 * 形参: \n
 * CategoryName – to declare \n
 * DefaultVerbosity – run time verbosity \n
 * CompileTimeVerbosity – verbosity to compile into the code
 */
DECLARE_LOG_CATEGORY_EXTERN(LogAura, Log, All);
