Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

It is great to get some posix support! Some minor comments below:

Location: pylib/release.py:449
Comment: I remember this coming up before and that we are purposefully not using
         os.path.join to build the path. Would it be worth having a variable for
         the separator and use this instead of the explicit '/'? This way we
         would have the rationale for it once, and the usages clear throughout
         the file. This would avoid both having duplicate comments and possible
         issues if one were to change an explicit separator to os.path.join.

         Feel free to ignore this if it is out of scope.


Location:  pm/tasks/enable_posix_releases:30
Comment: 'prj/prj.sh build posix.acamar' should be 'bin/prj.sh build posix.acamar'


Location:  pm/tasks/enable_posix_releases:30
Comment: I'm getting a bunch of build errors on OSX. I know it is not officially
         a supported platform so I'm leaving it as an obversation.

$ gcc --version
Configured with: --prefix=/Library/Developer/CommandLineTools/usr --with-gxx-include-dir=/usr/include/c++/4.2.1
Apple LLVM version 7.3.0 (clang-703.0.31)
Target: x86_64-apple-darwin15.6.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin

Error output:
In file included from out/posix/acamar/posix/rtos-acamar.c:29:
/usr/include/ucontext.h:43:2: error: The deprecated ucontext routines require _XOPEN_SOURCE to be defined
#error The deprecated ucontext routines require _XOPEN_SOURCE to be defined
 ^
out/posix/acamar/posix/rtos-acamar.c:82:9: error: unknown type name 'ucontext_t'
typedef ucontext_t context_t;
        ^
out/posix/acamar/posix/rtos-acamar.c:135:5: error: implicit declaration of function 'getcontext' [-Werror,-Wimplicit-function-declaration]
    getcontext(ctx);
    ^
out/posix/acamar/posix/rtos-acamar.c:135:5: error: declaration of built-in function 'getcontext' requires inclusion of the header <ucontext.h> [-Werror,-Wbuiltin-requires-header]
out/posix/acamar/posix/rtos-acamar.c:136:8: error: member reference base type 'context_t' (aka 'int') is not a structure or union
    ctx->uc_stack.ss_sp = stack_base;
    ~~~^ ~~~~~~~~
out/posix/acamar/posix/rtos-acamar.c:137:8: error: member reference base type 'context_t' (aka 'int') is not a structure or union
    ctx->uc_stack.ss_size = stack_size;
    ~~~^ ~~~~~~~~
out/posix/acamar/posix/rtos-acamar.c:138:8: error: member reference base type 'context_t' (aka 'int') is not a structure or union
    ctx->uc_link = NULL;
    ~~~^ ~~~~~~~
out/posix/acamar/posix/rtos-acamar.c:139:5: error: implicit declaration of function 'makecontext' [-Werror,-Wimplicit-function-declaration]
    makecontext(ctx, fn, 0);
    ^
out/posix/acamar/posix/rtos-acamar.c:148:5: error: implicit declaration of function 'swapcontext' [-Werror,-Wimplicit-function-declaration]
    context_switch(get_task_context(from), get_task_context(to));
    ^
out/posix/acamar/posix/rtos-acamar.c:118:34: note: expanded from macro 'context_switch'
#define context_switch(from, to) swapcontext(from, to)
                                 ^
out/posix/acamar/posix/rtos-acamar.c:157:5: error: implicit declaration of function 'setcontext' [-Werror,-Wimplicit-function-declaration]
    context_switch_first(get_task_context(RTOS_TASK_ID_ZERO));
    ^
out/posix/acamar/posix/rtos-acamar.c:119:34: note: expanded from macro 'context_switch_first'
#define context_switch_first(to) setcontext(to)
                                 ^
out/posix/acamar/posix/rtos-acamar.c:157:5: note: did you mean 'getcontext'?
out/posix/acamar/posix/rtos-acamar.c:119:34: note: expanded from macro 'context_switch_first'
#define context_switch_first(to) setcontext(to)
                                 ^
out/posix/acamar/posix/rtos-acamar.c:135:5: note: 'getcontext' declared here
    getcontext(ctx);
    ^
10 errors generated.
ERROR:prj:Command gcc -o out/posix/acamar/system -Wall -Werror -std=c90 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE -Iout/posix/acamar -Iout/posix/acamar/posix -Iout/posix/acamar/generic out/posix/acamar/posix/rtos-acamar.c share/packages/rtos-example/acamar-test.c out/posix/acamar/generic/debug.c share/packages/posix/debug.c returned non-zero error code: 1
