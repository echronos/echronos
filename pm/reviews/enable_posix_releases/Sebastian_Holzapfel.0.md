Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Location: packages/posix/build.py:56
Comment 1: Rework
Changes look good in general, however I have a minor build nit with recent glibc/gcc versions:
The `_BSD_SOURCE` flag was deprecated in glibc 2.20, so if I try to build a posix system on my machine with these gcc & glibc versions:

    $ gcc -v
    ...
    gcc version 6.3.1 20170306 (GCC)
    $ ldd --version
    ldd (GNU libc) 2.25
    ...

Then there are a bunch of deprecation warnings that result in the build failing:

    $ prj/app/prj.py build posix.acamar
    In file included from /usr/include/bits/libc-header-start.h:33:0,
                     from /usr/include/stdint.h:26,
                     from /usr/lib/gcc/x86_64-pc-linux-gnu/6.3.1/include/stdint.h:9,
                     from out/posix/acamar/posix/rtos-acamar.h:7,
                     from out/posix/acamar/posix/rtos-acamar.c:2:
    /usr/include/features.h:180:3: error: #warning "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE" [-Werror=cpp]
     # warning "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE"
       ^~~~~~~
    cc1: all warnings being treated as errors
    ....
