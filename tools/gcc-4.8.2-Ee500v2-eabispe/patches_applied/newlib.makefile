--- newlib-1.17.0/newlib/libc/machine/powerpc/Makefile.in-orig	2009-04-22 14:48:06.000000000 -0500
+++ newlib-1.17.0/newlib/libc/machine/powerpc/Makefile.in	2009-04-22 14:52:08.000000000 -0500
@@ -403,10 +403,10 @@
 	uninstall-am uninstall-info-am
 
 
-$(lpfx)vec_reallocr.o: vec_mallocr.c
+vec_reallocr.o: vec_mallocr.c
 	$(VEC_MALLOC_COMPILE) -DDEFINE_VECREALLOC -c $(srcdir)/vec_mallocr.c -o $@
 
-$(lpfx)vec_callocr.o: vec_mallocr.c
+vec_callocr.o: vec_mallocr.c
 	$(VEC_MALLOC_COMPILE) -DDEFINE_VECCALLOC -c $(srcdir)/vec_mallocr.c -o $@
 # Tell versions [3.59,3.63) of GNU make to not export all variables.
 # Otherwise a system limit (for SysV at least) may be exceeded.
