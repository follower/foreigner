#include "crossplatform.h"

#ifdef IS_WINDOWS
#include <string>
std::string error_number_as_string;
#endif

HANDLE open_library(const char *path) {
#ifdef IS_UNIX
    //
    // Note: Experience has shown that multiple platforms/libraries
    //       have issues with handling dynamically loaded shared
    //       libraries and Thread Local Variables/Thread Local Storage
    //       and/or at exit terminators properly.
    //
    //       This can result in multiple thread crashes when the
    //       program exits because the library is no longer loaded
    //       but pointers to the library clean-up functions still exist.
    //
    //       The best workaround found so far is to...just not unload
    //       a library when it is closed. :/
    //
    //       The workaround is implemented via the addition of an
    //       additional `dlopen()` call which is (by design) never
    //       matched with a `dlclose()` call. This results in all
    //       at exit/thread local storage cleanup happening when the
    //       main thread is exiting.
    //
    //       The disadvantage of this workaround is that it means
    //       it's not possible to "reload" a shared library but
    //       we can cross that bridge when someone gets to it. :)
    //
    //       Additional note: In theory `RTLD_NODELETE` should have
    //       also worked as a workaround but it led to crashes.
    //       (Also tried `RTLD_LOCAL` or `RTLD_FIRST` approaches.)
    //
    //
    //       TODO: Document TLV on Mac issue with Library X.
    //       TODO: Document at exit terminator issue on Linux with Library Y.
    //
    dlopen(path, RTLD_LAZY|RTLD_GLOBAL); // Workaround to bump ref-count so library never unloaded.
    return dlopen(path, RTLD_LAZY|RTLD_GLOBAL|RTLD_NOLOAD);  // TODO: Is RTLD_LAZY the best?
#else
    return LoadLibrary(path);
#endif
}

const char *open_library_error() {
#ifdef IS_UNIX
    return dlerror();
#else
    // TODO: Use standard approach to convert GetLastError() to message.
    error_number_as_string = std::to_string(GetLastError());
    return error_number_as_string.c_str();
#endif
}

int close_library(HANDLE handle) {
#ifdef IS_UNIX
    // Note: See workaround note in `open_library()` above as to why
    //       the following line will no longer unload library.
    return dlclose(handle);
#else
    return FreeLibrary((HMODULE) handle);
#endif
}

SYMBOL get_symbol(HANDLE handle, const char *symbol) {
#ifdef IS_UNIX
    return dlsym(handle, symbol);
#else
    return (void *)GetProcAddress((HMODULE) handle, symbol);
#endif
}

