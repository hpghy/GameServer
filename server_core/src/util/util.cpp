#include <iostream>
#include <execinfo.h>
#include "util/util.h"


void backTrace()
{
    const uint32_t size = 50;
    void* array[size];
    int stack_num = backtrace(array, size);

    // TODO...append time format
    std::cerr << THREAD_ID << " create core.backtrace\n";
    int fd = open("core.backtrace", O_CREAT | O_WRONLY, 00777);
    backtrace_symbols_fd(array, stack_num, fd);
    close(fd);

    // create coredump file
    std::cerr << "raise SIGABRT\n";
    raise(SIGABRT);

    exit(0);
}


