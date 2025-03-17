/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2012-2024.
==============================================================================*/
#include <kernel.h>

#define COMMON_PLATFORM_SUPPORTS_FOPEN

#define FMOD_Main() main(int, char**)
#define Common_TTY(format, ...) fprintf(stderr, format, __VA_ARGS__)
