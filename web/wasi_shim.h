// Force-included only for the WebAssembly build (see web/build.sh).
// WASI has no shell, so system("clear") becomes an ANSI clear-screen
// sequence that the browser terminal understands.
#pragma once
#include <stdlib.h>
#include <stdio.h>
#ifdef __cplusplus
#include <cstdlib>
#endif
static inline int wasi_system_stub(const char *cmd) {
    (void)cmd;
    fputs("\x1b[2J\x1b[H", stdout);
    fflush(stdout);
    return 0;
}
#define system(cmd) wasi_system_stub(cmd)
