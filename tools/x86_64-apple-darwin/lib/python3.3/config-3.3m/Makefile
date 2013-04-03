# Generated automatically from Makefile.pre by makesetup.
# Top-level Makefile for Python
#
# As distributed, this file is called Makefile.pre.in; it is processed
# into the real Makefile by running the script ./configure, which
# replaces things like @spam@ with values appropriate for your system.
# This means that if you edit Makefile, your changes get lost the next
# time you run the configure script.  Ideally, you can do:
#
#	./configure
#	make
#	make test
#	make install
#
# If you have a previous version of Python installed that you don't
# want to overwrite, you can use "make altinstall" instead of "make
# install".  Refer to the "Installing" section in the README file for
# additional details.
#
# See also the section "Build instructions" in the README file.

# === Variables set by makesetup ===

MODOBJS=          Modules/_threadmodule.o  Modules/signalmodule.o  Modules/posixmodule.o  Modules/errnomodule.o  Modules/pwdmodule.o  Modules/_sre.o  Modules/_functoolsmodule.o  Modules/operator.o  Modules/_collectionsmodule.o  Modules/itertoolsmodule.o  Modules/_iomodule.o Modules/iobase.o Modules/fileio.o Modules/bytesio.o Modules/bufferedio.o Modules/textio.o Modules/stringio.o  Modules/_codecsmodule.o  Modules/multibytecodec.o  Modules/_codecs_cn.o  Modules/_codecs_hk.o  Modules/_codecs_iso2022.o  Modules/_codecs_jp.o  Modules/_codecs_kr.o  Modules/_codecs_tw.o  Modules/_localemodule.o  Modules/zipimport.o  Modules/faulthandler.o  Modules/symtablemodule.o  Modules/arraymodule.o  Modules/cmathmodule.o Modules/_math.o  Modules/mathmodule.o  Modules/_struct.o  Modules/timemodule.o  Modules/_weakref.o  Modules/_randommodule.o  Modules/atexitmodule.o  Modules/_elementtree.o  Modules/xmlparse.o Modules/xmlrole.o Modules/xmltok.o Modules/pyexpat.o  Modules/_pickle.o  Modules/_datetimemodule.o  Modules/_bisectmodule.o  Modules/_heapqmodule.o  Modules/unicodedata.o  Modules/fcntlmodule.o  Modules/grpmodule.o  Modules/selectmodule.o  Modules/mmapmodule.o  Modules/_csv.o  Modules/socketmodule.o  Modules/_cryptmodule.o  Modules/termios.o  Modules/resource.o  Modules/_posixsubprocess.o  Modules/md5module.o  Modules/sha1module.o  Modules/sha256module.o  Modules/sha512module.o  Modules/syslogmodule.o  Modules/binascii.o  Modules/parsermodule.o  Modules/zlibmodule.o  Modules/_json.o  Modules/_lsprof.o Modules/rotatingtree.o  Modules/audioop.o  Modules/multiprocessing.o Modules/semaphore.o  Modules/_decimal.o Modules/basearith.o Modules/constants.o Modules/basearith.o Modules/constants.o Modules/context.o Modules/convolute.o Modules/crt.o Modules/difradix2.o Modules/fnt.o Modules/fourstep.o Modules/io.o Modules/memory.o Modules/mpdecimal.o Modules/numbertheory.o Modules/sixstep.o Modules/transpose.o  Modules/darwin64.o Modules/malloc_closure.o Modules/dlfcn_simple.o Modules/_ctypes.o Modules/callbacks.o Modules/callproc.o Modules/stgdict.o Modules/cfield.o Modules/ffi.o Modules/x86-ffi64.o
MODLIBS=        $(LOCALMODLIBS) $(BASEMODLIBS)

# === Variables set by configure
VERSION=	3.3
srcdir=		../../source/python
VPATH=		../../source/python
abs_srcdir=	/Volumes/BigDisk/Users/benno/brkawy/products/xyz/build/python-x86_64-apple-darwin/../../source/python
abs_builddir=	/Volumes/BigDisk/Users/benno/brkawy/products/xyz/build/python-x86_64-apple-darwin


CC=		gcc
CXX=		g++
MAINCC=		$(CC)
LINKCC=		$(PURIFY) $(MAINCC)
AR=		ar
RANLIB=		ranlib
READELF=	:
SOABI=		cpython-33m
LDVERSION=	$(VERSION)$(ABIFLAGS)
HGVERSION=	
HGTAG=		
HGBRANCH=	

GNULD=		no

# Shell used by make (some versions default to the login shell, which is bad)
SHELL=		/bin/sh

# Use this to make a link between python$(VERSION) and python in $(BINDIR)
LN=		ln

# Portable install script (configure doesn't always guess right)
INSTALL=	/usr/bin/install -c
INSTALL_PROGRAM=${INSTALL}
INSTALL_SCRIPT= ${INSTALL}
INSTALL_DATA=	${INSTALL} -m 644
# Shared libraries must be installed with executable mode on some systems;
# rather than figuring out exactly which, we always give them executable mode.
# Also, making them read-only seems to be a good idea...
INSTALL_SHARED= ${INSTALL} -m 555

MKDIR_P=	../../source/python/install-sh -c -d

MAKESETUP=      $(srcdir)/Modules/makesetup

# Compiler options
OPT=		-DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
BASECFLAGS=	 -fno-strict-aliasing
BASECPPFLAGS=	
CONFIGURE_CFLAGS=	
CONFIGURE_CPPFLAGS=	-I/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/include -I/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/x86_64-apple-darwin/include
CONFIGURE_LDFLAGS=	-Wl,-Z -Wl,-search_paths_first -L/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/x86_64-apple-darwin/lib -F/Library/Frameworks -F/System/Library/Frameworks
# Avoid assigning CFLAGS, LDFLAGS, etc. so users can use them on the
# command line to append to these values without stomping the pre-set
# values.
PY_CFLAGS=	$(BASECFLAGS) $(OPT) $(CONFIGURE_CFLAGS) $(CFLAGS) $(EXTRA_CFLAGS)
# Both CPPFLAGS and LDFLAGS need to contain the shell's value for setup.py to
# be able to build extension modules using the directories specified in the
# environment variables
PY_CPPFLAGS=	$(BASECPPFLAGS) -I. -IInclude -I$(srcdir)/Include $(CONFIGURE_CPPFLAGS) $(CPPFLAGS)
PY_LDFLAGS=	$(CONFIGURE_LDFLAGS) $(LDFLAGS)
NO_AS_NEEDED=	
LDLAST=		
SGI_ABI=	
CCSHARED=	
LINKFORSHARED=	 -framework CoreFoundation
ARFLAGS=	rc
# Extra C flags added for building the interpreter object files.
CFLAGSFORSHARED=
# C flags used for building the interpreter object files
PY_CORE_CFLAGS=	$(PY_CFLAGS) $(PY_CPPFLAGS) $(CFLAGSFORSHARED) -DPy_BUILD_CORE


# Machine-dependent subdirectories
MACHDEP=	darwin

# Install prefix for architecture-independent files
prefix=		/noprefix

# Install prefix for architecture-dependent files
exec_prefix=	/noprefix/x86_64-apple-darwin

# Install prefix for data files
datarootdir=    ${prefix}/share

# Expanded directories
BINDIR=		${exec_prefix}/bin
LIBDIR=		${exec_prefix}/lib
MANDIR=		${datarootdir}/man
INCLUDEDIR=	${prefix}/include
CONFINCLUDEDIR=	$(exec_prefix)/include
SCRIPTDIR=	$(prefix)/lib
ABIFLAGS=	m

# Detailed destination directories
BINLIBDEST=	$(LIBDIR)/python$(VERSION)
LIBDEST=	$(SCRIPTDIR)/python$(VERSION)
INCLUDEPY=	$(INCLUDEDIR)/python$(LDVERSION)
CONFINCLUDEPY=	$(CONFINCLUDEDIR)/python$(LDVERSION)

# Symbols used for using shared libraries
SO=		.so
LDSHARED=	$(CC) -bundle -undefined dynamic_lookup $(PY_LDFLAGS)
BLDSHARED=	$(CC) -bundle -undefined dynamic_lookup $(PY_LDFLAGS)
LDCXXSHARED=	$(CXX) -bundle -undefined dynamic_lookup
DESTSHARED=	$(BINLIBDEST)/lib-dynload

# Executable suffix (.exe on Windows and Mac OS X)
EXE=		
BUILDEXE=	

# Short name and location for Mac OS X Python framework
UNIVERSALSDK=
PYTHONFRAMEWORK=	
PYTHONFRAMEWORKDIR=	no-framework
PYTHONFRAMEWORKPREFIX=	
PYTHONFRAMEWORKINSTALLDIR= 
# Deployment target selected during configure, to be checked
# by distutils. The export statement is needed to ensure that the
# deployment target is active during build.
MACOSX_DEPLOYMENT_TARGET=10.6
export MACOSX_DEPLOYMENT_TARGET

# Options to enable prebinding (for fast startup prior to Mac OS X 10.3)
OTHER_LIBTOOL_OPT=

# Environment to run shared python without installed libraries
RUNSHARED=       

# Modes for directories, executables and data files created by the
# install process.  Default to user-only-writable for all file types.
DIRMODE=	755
EXEMODE=	755
FILEMODE=	644

# configure script arguments
CONFIG_ARGS=	 '--prefix=/noprefix' '--exec-prefix=/noprefix/x86_64-apple-darwin' '--host=x86_64-apple-darwin' '--build=x86_64-apple-darwin' '--disable-shared' 'build_alias=x86_64-apple-darwin' 'host_alias=x86_64-apple-darwin' 'LDFLAGS=-Wl,-Z -Wl,-search_paths_first -L/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/x86_64-apple-darwin/lib -F/Library/Frameworks -F/System/Library/Frameworks' 'CPPFLAGS=-I/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/include -I/Volumes/BigDisk/Users/benno/brkawy/products/xyz/devtree/python-x86_64-apple-darwin/x86_64-apple-darwin/include'


# Subdirectories with code
SRCDIRS= 	Parser Grammar Objects Python Modules Mac

# Other subdirectories
SUBDIRSTOO=	Include Lib Misc

# Files and directories to be distributed
CONFIGFILES=	configure configure.ac acconfig.h pyconfig.h.in Makefile.pre.in
DISTFILES=	README ChangeLog $(CONFIGFILES)
DISTDIRS=	$(SUBDIRS) $(SUBDIRSTOO) Ext-dummy
DIST=		$(DISTFILES) $(DISTDIRS)


LIBRARY=	libpython$(VERSION)$(ABIFLAGS).a
LDLIBRARY=      libpython$(VERSION)$(ABIFLAGS).a
BLDLIBRARY=     $(LDLIBRARY)
PY3LIBRARY=     
DLLLIBRARY=	
LDLIBRARYDIR=   
INSTSONAME=	$(LDLIBRARY)


LIBS=		-ldl  -framework CoreFoundation
LIBM=		
LIBC=		
SYSLIBS=	$(LIBM) $(LIBC)
SHLIBS=		$(LIBS)

THREADOBJ=	Python/thread.o
DLINCLDIR=	.
DYNLOADFILE=	dynload_shlib.o
MACHDEP_OBJS=	
LIBOBJDIR=	Python/
LIBOBJS=	

PYTHON=		python$(EXE)
BUILDPYTHON=	python$(BUILDEXE)

PYTHON_FOR_BUILD=PYTHONHASHSEED=0 ./$(BUILDPYTHON) -E
_PYTHON_HOST_PLATFORM=
HOST_GNU_TYPE=  x86_64-apple-darwin

# The task to run while instrument when building the profile-opt target
PROFILE_TASK=	$(srcdir)/Tools/pybench/pybench.py -n 2 --with-gc --with-syscheck
#PROFILE_TASK=	$(srcdir)/Lib/test/regrtest.py

# === Definitions added by makesetup ===

LOCALMODLIBS=                                                         -lz      
BASEMODLIBS=
PYTHONPATH=$(COREPYTHONPATH)
COREPYTHONPATH=$(DESTPATH)$(SITEPATH)$(TESTPATH)$(MACHDEPPATH)$(EXTRAMACHDEPPATH)
EXTRAMACHDEPPATH=
MACHDEPPATH=:plat-$(MACHDEP)
TESTPATH=
SITEPATH=
DESTPATH=
MACHDESTLIB=$(BINLIBDEST)
DESTLIB=$(LIBDEST)


##########################################################################
# Modules
MODULE_OBJS=	\
		Modules/config.o \
		Modules/getpath.o \
		Modules/main.o \
		Modules/gcmodule.o

# Used of signalmodule.o is not available
SIGNAL_OBJS=	

IO_H=		Modules/_io/_iomodule.h

IO_OBJS=	\
		Modules/_io/_iomodule.o \
		Modules/_io/iobase.o \
		Modules/_io/fileio.o \
		Modules/_io/bufferedio.o \
		Modules/_io/textio.o \
		Modules/_io/bytesio.o \
		Modules/_io/stringio.o

##########################################################################
# Grammar
GRAMMAR_H=	Include/graminit.h
GRAMMAR_C=	Python/graminit.c
GRAMMAR_INPUT=	$(srcdir)/Grammar/Grammar


LIBFFI_INCLUDEDIR=	

##########################################################################
# Parser
PGEN=		Parser/pgen$(EXE)

PSRCS=		\
		Parser/acceler.c \
		Parser/grammar1.c \
		Parser/listnode.c \
		Parser/node.c \
		Parser/parser.c \
		Parser/bitset.c \
		Parser/metagrammar.c \
		Parser/firstsets.c \
		Parser/grammar.c \
		Parser/pgen.c

POBJS=		\
		Parser/acceler.o \
		Parser/grammar1.o \
		Parser/listnode.o \
		Parser/node.o \
		Parser/parser.o \
		Parser/bitset.o \
		Parser/metagrammar.o \
		Parser/firstsets.o \
		Parser/grammar.o \
		Parser/pgen.o

PARSER_OBJS=	$(POBJS) Parser/myreadline.o Parser/parsetok.o Parser/tokenizer.o

PGSRCS=		\
		Objects/obmalloc.c \
		Python/dynamic_annotations.c \
		Python/mysnprintf.c \
		Python/pyctype.c \
		Parser/tokenizer_pgen.c \
		Parser/printgrammar.c \
		Parser/parsetok_pgen.c \
		Parser/pgenmain.c

PGOBJS=		\
		Objects/obmalloc.o \
		Python/dynamic_annotations.o \
		Python/mysnprintf.o \
		Python/pyctype.o \
		Parser/tokenizer_pgen.o \
		Parser/printgrammar.o \
		Parser/parsetok_pgen.o \
		Parser/pgenmain.o

PARSER_HEADERS= \
		$(srcdir)/Parser/parser.h \
		$(srcdir)/Include/parsetok.h \
		$(srcdir)/Parser/tokenizer.h

PGENSRCS=	$(PSRCS) $(PGSRCS)
PGENOBJS=	$(POBJS) $(PGOBJS)

##########################################################################
# AST
AST_H_DIR=	Include
AST_H=		$(AST_H_DIR)/Python-ast.h
AST_C_DIR=	Python
AST_C=		$(AST_C_DIR)/Python-ast.c
AST_ASDL=	$(srcdir)/Parser/Python.asdl

ASDLGEN_FILES=	$(srcdir)/Parser/asdl.py $(srcdir)/Parser/asdl_c.py
# XXX Note that a build now requires Python exist before the build starts
ASDLGEN=	python $(srcdir)/Parser/asdl_c.py

##########################################################################
# Python

OPCODETARGETS_H= \
		Python/opcode_targets.h

OPCODETARGETGEN= \
		$(srcdir)/Python/makeopcodetargets.py

OPCODETARGETGEN_FILES= \
		$(OPCODETARGETGEN) $(srcdir)/Lib/opcode.py

PYTHON_OBJS=	\
		Python/_warnings.o \
		Python/Python-ast.o \
		Python/asdl.o \
		Python/ast.o \
		Python/bltinmodule.o \
		Python/ceval.o \
		Python/compile.o \
		Python/codecs.o \
		Python/dynamic_annotations.o \
		Python/errors.o \
		Python/frozenmain.o \
		Python/future.o \
		Python/getargs.o \
		Python/getcompiler.o \
		Python/getcopyright.o \
		Python/getplatform.o \
		Python/getversion.o \
		Python/graminit.o \
		Python/import.o \
		Python/importdl.o \
		Python/marshal.o \
		Python/modsupport.o \
		Python/mystrtoul.o \
		Python/mysnprintf.o \
		Python/peephole.o \
		Python/pyarena.o \
		Python/pyctype.o \
		Python/pyfpe.o \
		Python/pymath.o \
		Python/pystate.o \
		Python/pythonrun.o \
		Python/pytime.o \
		Python/random.o \
		Python/structmember.o \
		Python/symtable.o \
		Python/sysmodule.o \
		Python/traceback.o \
		Python/getopt.o \
		Python/pystrcmp.o \
		Python/pystrtod.o \
		Python/dtoa.o \
		Python/formatter_unicode.o \
		Python/fileutils.o \
		Python/$(DYNLOADFILE) \
		$(LIBOBJS) \
		$(MACHDEP_OBJS) \
		$(THREADOBJ)


##########################################################################
# Objects
OBJECT_OBJS=	\
		Objects/abstract.o \
		Objects/accu.o \
		Objects/boolobject.o \
		Objects/bytes_methods.o \
		Objects/bytearrayobject.o \
		Objects/bytesobject.o \
		Objects/cellobject.o \
		Objects/classobject.o \
		Objects/codeobject.o \
		Objects/complexobject.o \
		Objects/descrobject.o \
		Objects/enumobject.o \
		Objects/exceptions.o \
		Objects/genobject.o \
		Objects/fileobject.o \
		Objects/floatobject.o \
		Objects/frameobject.o \
		Objects/funcobject.o \
		Objects/iterobject.o \
		Objects/listobject.o \
		Objects/longobject.o \
		Objects/dictobject.o \
		Objects/memoryobject.o \
		Objects/methodobject.o \
		Objects/moduleobject.o \
		Objects/namespaceobject.o \
		Objects/object.o \
		Objects/obmalloc.o \
		Objects/capsule.o \
		Objects/rangeobject.o \
		Objects/setobject.o \
		Objects/sliceobject.o \
		Objects/structseq.o \
		Objects/tupleobject.o \
		Objects/typeobject.o \
		Objects/unicodeobject.o \
		Objects/unicodectype.o \
		Objects/weakrefobject.o

##########################################################################
# objects that get linked into the Python library
LIBRARY_OBJS_OMIT_FROZEN=	\
		Modules/getbuildinfo.o \
		$(PARSER_OBJS) \
		$(OBJECT_OBJS) \
		$(PYTHON_OBJS) \
		$(MODULE_OBJS) \
		$(SIGNAL_OBJS) \
		$(MODOBJS)

LIBRARY_OBJS=	\
		$(LIBRARY_OBJS_OMIT_FROZEN) \
		Python/frozen.o

#########################################################################
# Rules

# Default target
all:		build_all
build_all:	$(BUILDPYTHON) oldsharedmods sharedmods gdbhooks Modules/_testembed

# Compile a binary with gcc profile guided optimization.
profile-opt:
	@echo "Building with support for profile generation:"
	$(MAKE) clean
	$(MAKE) build_all_generate_profile
	@echo "Running benchmark to generate profile data:"
	$(MAKE) profile-removal
	$(MAKE) run_profile_task
	@echo "Rebuilding with profile guided optimizations:"
	$(MAKE) clean
	$(MAKE) build_all_use_profile

build_all_generate_profile:
	$(MAKE) all CFLAGS="$(CFLAGS) -fprofile-generate" LIBS="$(LIBS) -lgcov"

run_profile_task:
	: # FIXME: can't run for a cross build
	$(RUNSHARED) ./$(BUILDPYTHON) $(PROFILE_TASK)

build_all_use_profile:
	$(MAKE) all CFLAGS="$(CFLAGS) -fprofile-use -fprofile-correction"

coverage:
	@echo "Building with support for coverage checking:"
	$(MAKE) clean
	$(MAKE) all CFLAGS="$(CFLAGS) -O0 -pg -fprofile-arcs -ftest-coverage" LIBS="$(LIBS) -lgcov"


# Build the interpreter
$(BUILDPYTHON):	Modules/python.o $(LIBRARY) $(LDLIBRARY) $(PY3LIBRARY)
	$(LINKCC) $(PY_LDFLAGS) $(LINKFORSHARED) -o $@ Modules/python.o $(BLDLIBRARY) $(LIBS) $(MODLIBS) $(SYSLIBS) $(LDLAST)

platform: $(BUILDPYTHON) pybuilddir.txt
	$(RUNSHARED) $(PYTHON_FOR_BUILD) -c 'import sys ; from sysconfig import get_platform ; print(get_platform()+"-"+sys.version[0:3])' >platform

# Create build directory and generate the sysconfig build-time data there.
# pybuilddir.txt contains the name of the build dir and is used for
# sys.path fixup -- see Modules/getpath.c.
pybuilddir.txt: $(BUILDPYTHON)
	$(RUNSHARED) $(PYTHON_FOR_BUILD) -S -m sysconfig --generate-posix-vars

# Build the shared modules
# Under GNU make, MAKEFLAGS are sorted and normalized; the 's' for
# -s, --silent or --quiet is always the first char.
# Under BSD make, MAKEFLAGS might be " -s -v x=y".
sharedmods: $(BUILDPYTHON) pybuilddir.txt
	@case "$$MAKEFLAGS" in \
	    *\ -s*|s*) quiet="-q";; \
	    *) quiet="";; \
	esac; \
	$(RUNSHARED) CC='$(CC)' LDSHARED='$(BLDSHARED)' OPT='$(OPT)' \
		$(PYTHON_FOR_BUILD) $(srcdir)/setup.py $$quiet build

# Build static library
# avoid long command lines, same as LIBRARY_OBJS
$(LIBRARY): $(LIBRARY_OBJS)
	-rm -f $@
	$(AR) $(ARFLAGS) $@ Modules/getbuildinfo.o
	$(AR) $(ARFLAGS) $@ $(PARSER_OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJECT_OBJS)
	$(AR) $(ARFLAGS) $@ $(PYTHON_OBJS) Python/frozen.o
	$(AR) $(ARFLAGS) $@ $(MODULE_OBJS) $(SIGNAL_OBJS)
	$(AR) $(ARFLAGS) $@ $(MODOBJS)
	$(RANLIB) $@

libpython$(LDVERSION).so: $(LIBRARY_OBJS)
	if test $(INSTSONAME) != $(LDLIBRARY); then \
		$(BLDSHARED) -Wl,-h$(INSTSONAME) -o $(INSTSONAME) $(LIBRARY_OBJS) $(MODLIBS) $(SHLIBS) $(LIBC) $(LIBM) $(LDLAST); \
		$(LN) -f $(INSTSONAME) $@; \
	else \
		$(BLDSHARED) -o $@ $(LIBRARY_OBJS) $(MODLIBS) $(SHLIBS) $(LIBC) $(LIBM) $(LDLAST); \
	fi

libpython3.so:	libpython$(LDVERSION).so
	$(BLDSHARED) $(NO_AS_NEEDED) -o $@ -Wl,-h$@ $^

libpython$(LDVERSION).dylib: $(LIBRARY_OBJS)
	 $(CC) -dynamiclib -Wl,-single_module $(PY_LDFLAGS) -undefined dynamic_lookup -Wl,-install_name,$(prefix)/lib/libpython$(LDVERSION).dylib -Wl,-compatibility_version,$(VERSION) -Wl,-current_version,$(VERSION) -o $@ $(LIBRARY_OBJS) $(SHLIBS) $(LIBC) $(LIBM) $(LDLAST); \


libpython$(VERSION).sl: $(LIBRARY_OBJS)
	$(LDSHARED) -o $@ $(LIBRARY_OBJS) $(MODLIBS) $(SHLIBS) $(LIBC) $(LIBM) $(LDLAST)

# Copy up the gdb python hooks into a position where they can be automatically
# loaded by gdb during Lib/test/test_gdb.py
#
# Distributors are likely to want to install this somewhere else e.g. relative
# to the stripped DWARF data for the shared library.
gdbhooks: $(BUILDPYTHON)-gdb.py

SRC_GDB_HOOKS=$(srcdir)/Tools/gdb/libpython.py
$(BUILDPYTHON)-gdb.py: $(SRC_GDB_HOOKS)
	$(INSTALL_DATA) $(SRC_GDB_HOOKS) $(BUILDPYTHON)-gdb.py

# This rule is here for OPENSTEP/Rhapsody/MacOSX. It builds a temporary
# minimal framework (not including the Lib directory and such) in the current
# directory.
RESSRCDIR=Mac/Resources/framework
$(PYTHONFRAMEWORKDIR)/Versions/$(VERSION)/$(PYTHONFRAMEWORK): \
		$(LIBRARY) \
		$(RESSRCDIR)/Info.plist
	$(INSTALL) -d -m $(DIRMODE) $(PYTHONFRAMEWORKDIR)/Versions/$(VERSION)
	$(CC) -o $(LDLIBRARY) $(PY_LDFLAGS) -dynamiclib \
		-all_load $(LIBRARY) -Wl,-single_module \
		-install_name $(DESTDIR)$(PYTHONFRAMEWORKINSTALLDIR)/Versions/$(VERSION)/$(PYTHONFRAMEWORK) \
		-compatibility_version $(VERSION) \
		-current_version $(VERSION) \
		-framework CoreFoundation $(LIBS);
	$(INSTALL) -d -m $(DIRMODE)  \
		$(PYTHONFRAMEWORKDIR)/Versions/$(VERSION)/Resources/English.lproj
	$(INSTALL_DATA) $(RESSRCDIR)/Info.plist \
		$(PYTHONFRAMEWORKDIR)/Versions/$(VERSION)/Resources/Info.plist
	$(LN) -fsn $(VERSION) $(PYTHONFRAMEWORKDIR)/Versions/Current
	$(LN) -fsn Versions/Current/$(PYTHONFRAMEWORK) $(PYTHONFRAMEWORKDIR)/$(PYTHONFRAMEWORK)
	$(LN) -fsn Versions/Current/Resources $(PYTHONFRAMEWORKDIR)/Resources

# This rule builds the Cygwin Python DLL and import library if configured
# for a shared core library; otherwise, this rule is a noop.
$(DLLLIBRARY) libpython$(VERSION).dll.a: $(LIBRARY_OBJS)
	if test -n "$(DLLLIBRARY)"; then \
		$(LDSHARED) -Wl,--out-implib=$@ -o $(DLLLIBRARY) $^ \
			$(LIBS) $(MODLIBS) $(SYSLIBS) $(LDLAST); \
	else true; \
	fi


oldsharedmods: $(SHAREDMODS)


Makefile Modules/config.c: Makefile.pre \
				$(srcdir)/Modules/config.c.in \
				$(MAKESETUP) \
				Modules/Setup.config \
				Modules/Setup \
				Modules/Setup.local
	$(SHELL) $(MAKESETUP) -c $(srcdir)/Modules/config.c.in \
				-s Modules \
				Modules/Setup.config \
				Modules/Setup.local \
				Modules/Setup
	@mv config.c Modules
	@echo "The Makefile was updated, you may need to re-run make."


Modules/Setup: $(srcdir)/Modules/Setup.dist
	@if test -f Modules/Setup; then \
		echo "-----------------------------------------------"; \
		echo "Modules/Setup.dist is newer than Modules/Setup;"; \
		echo "check to make sure you have all the updates you"; \
		echo "need in your Modules/Setup file."; \
		echo "Usually, copying Modules/Setup.dist to Modules/Setup will work."; \
		echo "-----------------------------------------------"; \
	fi

Modules/_testembed: Modules/_testembed.o $(LIBRARY) $(LDLIBRARY) $(PY3LIBRARY)
	$(LINKCC) $(PY_LDFLAGS) $(LINKFORSHARED) -o $@ Modules/_testembed.o $(BLDLIBRARY) $(LIBS) $(MODLIBS) $(SYSLIBS) $(LDLAST)

############################################################################
# Importlib

Modules/_freeze_importlib: Modules/_freeze_importlib.o $(LIBRARY_OBJS_OMIT_FROZEN)
	$(LINKCC) $(PY_LDFLAGS) -o $@ Modules/_freeze_importlib.o $(LIBRARY_OBJS_OMIT_FROZEN) $(LIBS) $(MODLIBS) $(SYSLIBS) $(LDLAST)

Python/importlib.h: $(srcdir)/Lib/importlib/_bootstrap.py Modules/_freeze_importlib.c
	$(MAKE) Modules/_freeze_importlib
	./Modules/_freeze_importlib \
		$(srcdir)/Lib/importlib/_bootstrap.py Python/importlib.h


############################################################################
# Special rules for object files

Modules/getbuildinfo.o: $(PARSER_OBJS) \
		$(OBJECT_OBJS) \
		$(PYTHON_OBJS) \
		$(MODULE_OBJS) \
		$(SIGNAL_OBJS) \
		$(MODOBJS) \
		$(srcdir)/Modules/getbuildinfo.c
	$(CC) -c $(PY_CORE_CFLAGS) \
	      -DHGVERSION="\"`LC_ALL=C $(HGVERSION)`\"" \
	      -DHGTAG="\"`LC_ALL=C $(HGTAG)`\"" \
	      -DHGBRANCH="\"`LC_ALL=C $(HGBRANCH)`\"" \
	      -o $@ $(srcdir)/Modules/getbuildinfo.c

Modules/getpath.o: $(srcdir)/Modules/getpath.c Makefile
	$(CC) -c $(PY_CORE_CFLAGS) -DPYTHONPATH='"$(PYTHONPATH)"' \
		-DPREFIX='"$(prefix)"' \
		-DEXEC_PREFIX='"$(exec_prefix)"' \
		-DVERSION='"$(VERSION)"' \
		-DVPATH='"$(VPATH)"' \
		-o $@ $(srcdir)/Modules/getpath.c

Modules/python.o: $(srcdir)/Modules/python.c
	$(MAINCC) -c $(PY_CORE_CFLAGS) -o $@ $(srcdir)/Modules/python.c

Modules/_testembed.o: $(srcdir)/Modules/_testembed.c
	$(MAINCC) -c $(PY_CORE_CFLAGS) -o $@ $(srcdir)/Modules/_testembed.c

Python/dynload_shlib.o: $(srcdir)/Python/dynload_shlib.c Makefile
	$(CC) -c $(PY_CORE_CFLAGS) \
		-DSOABI='"$(SOABI)"' \
		-o $@ $(srcdir)/Python/dynload_shlib.c

Python/sysmodule.o: $(srcdir)/Python/sysmodule.c Makefile
	$(CC) -c $(PY_CORE_CFLAGS) \
		-DABIFLAGS='"$(ABIFLAGS)"' \
		-o $@ $(srcdir)/Python/sysmodule.c

$(IO_OBJS): $(IO_H)

$(GRAMMAR_H): $(GRAMMAR_INPUT) $(PGENSRCS)
		@$(MKDIR_P) Include
		$(MAKE) $(PGEN)
		$(PGEN) $(GRAMMAR_INPUT) $(GRAMMAR_H) $(GRAMMAR_C)
$(GRAMMAR_C): $(GRAMMAR_H) $(GRAMMAR_INPUT) $(PGENSRCS)
		$(MAKE) $(GRAMMAR_H)
		touch $(GRAMMAR_C)

$(PGEN):	$(PGENOBJS)
		$(CC) $(OPT) $(PY_LDFLAGS) $(PGENOBJS) $(LIBS) -o $(PGEN)

Parser/grammar.o:	$(srcdir)/Parser/grammar.c \
				$(srcdir)/Include/token.h \
				$(srcdir)/Include/grammar.h
Parser/metagrammar.o:	$(srcdir)/Parser/metagrammar.c

Parser/tokenizer_pgen.o:	$(srcdir)/Parser/tokenizer.c
Parser/parsetok_pgen.o:	$(srcdir)/Parser/parsetok.c
Parser/printgrammar.o: $(srcdir)/Parser/printgrammar.c

Parser/pgenmain.o:	$(srcdir)/Include/parsetok.h

$(AST_H): $(AST_ASDL) $(ASDLGEN_FILES)
	$(MKDIR_P) $(AST_H_DIR)
	$(ASDLGEN) -h $(AST_H_DIR) $(AST_ASDL)

$(AST_C): $(AST_H) $(AST_ASDL) $(ASDLGEN_FILES)
	$(MKDIR_P) $(AST_C_DIR)
	$(ASDLGEN) -c $(AST_C_DIR) $(AST_ASDL)

Python/compile.o Python/symtable.o Python/ast.o: $(GRAMMAR_H) $(AST_H)

Python/getplatform.o: $(srcdir)/Python/getplatform.c
		$(CC) -c $(PY_CORE_CFLAGS) -DPLATFORM='"$(MACHDEP)"' -o $@ $(srcdir)/Python/getplatform.c

Python/importdl.o: $(srcdir)/Python/importdl.c
		$(CC) -c $(PY_CORE_CFLAGS) -I$(DLINCLDIR) -o $@ $(srcdir)/Python/importdl.c

Objects/unicodectype.o:	$(srcdir)/Objects/unicodectype.c \
				$(srcdir)/Objects/unicodetype_db.h

BYTESTR_DEPS = \
		$(srcdir)/Include/bytes_methods.h \
		$(srcdir)/Objects/stringlib/count.h \
		$(srcdir)/Objects/stringlib/ctype.h \
		$(srcdir)/Objects/stringlib/eq.h \
		$(srcdir)/Objects/stringlib/fastsearch.h \
		$(srcdir)/Objects/stringlib/find.h \
		$(srcdir)/Objects/stringlib/find_max_char.h \
		$(srcdir)/Objects/stringlib/partition.h \
		$(srcdir)/Objects/stringlib/split.h \
		$(srcdir)/Objects/stringlib/stringdefs.h \
		$(srcdir)/Objects/stringlib/transmogrify.h \
		$(srcdir)/Objects/stringlib/unicodedefs.h \
		$(srcdir)/Objects/stringlib/localeutil.h \
		$(srcdir)/Objects/stringlib/undef.h

UNICODE_DEPS = $(BYTESTR_DEPS) \
		$(srcdir)/Objects/stringlib/asciilib.h \
		$(srcdir)/Objects/stringlib/codecs.h \
		$(srcdir)/Objects/stringlib/ucs1lib.h \
		$(srcdir)/Objects/stringlib/ucs2lib.h \
		$(srcdir)/Objects/stringlib/ucs4lib.h \
		$(srcdir)/Objects/stringlib/unicode_format.h \
		$(srcdir)/Objects/stringlib/unicodedefs.h

Objects/bytesobject.o: $(srcdir)/Objects/bytesobject.c $(BYTESTR_DEPS)

Objects/bytearrayobject.o: $(srcdir)/Objects/bytearrayobject.c $(BYTESTR_DEPS)

Objects/unicodeobject.o: $(srcdir)/Objects/unicodeobject.c $(UNICODE_DEPS)

Objects/dictobject.o: $(srcdir)/Objects/stringlib/eq.h
Objects/setobject.o: $(srcdir)/Objects/stringlib/eq.h

$(OPCODETARGETS_H): $(OPCODETARGETGEN_FILES)
	$(OPCODETARGETGEN) $(OPCODETARGETS_H)

Python/ceval.o: $(OPCODETARGETS_H) $(srcdir)/Python/ceval_gil.h

Python/formatter_unicode.o: $(srcdir)/Python/formatter_unicode.c \
				$(BYTESTR_DEPS)

Python/frozen.o: Python/importlib.h

Objects/typeobject.o: Objects/typeslots.inc
Objects/typeslots.inc: $(srcdir)/Include/typeslots.h $(srcdir)/Objects/typeslots.py
	$(PYTHON) $(srcdir)/Objects/typeslots.py < $(srcdir)/Include/typeslots.h > Objects/typeslots.inc

############################################################################
# Header files

PYTHON_HEADERS= \
		$(srcdir)/Include/Python.h \
		$(srcdir)/Include/abstract.h \
		$(srcdir)/Include/accu.h \
		$(srcdir)/Include/asdl.h \
		$(srcdir)/Include/ast.h \
		$(srcdir)/Include/bltinmodule.h \
		$(srcdir)/Include/bitset.h \
		$(srcdir)/Include/boolobject.h \
		$(srcdir)/Include/bytes_methods.h \
		$(srcdir)/Include/bytearrayobject.h \
		$(srcdir)/Include/bytesobject.h \
		$(srcdir)/Include/cellobject.h \
		$(srcdir)/Include/ceval.h \
		$(srcdir)/Include/classobject.h \
		$(srcdir)/Include/code.h \
		$(srcdir)/Include/codecs.h \
		$(srcdir)/Include/compile.h \
		$(srcdir)/Include/complexobject.h \
		$(srcdir)/Include/descrobject.h \
		$(srcdir)/Include/dictobject.h \
		$(srcdir)/Include/dtoa.h \
		$(srcdir)/Include/dynamic_annotations.h \
		$(srcdir)/Include/enumobject.h \
		$(srcdir)/Include/errcode.h \
		$(srcdir)/Include/eval.h \
		$(srcdir)/Include/fileobject.h \
		$(srcdir)/Include/fileutils.h \
		$(srcdir)/Include/floatobject.h \
		$(srcdir)/Include/frameobject.h \
		$(srcdir)/Include/funcobject.h \
		$(srcdir)/Include/genobject.h \
		$(srcdir)/Include/import.h \
		$(srcdir)/Include/intrcheck.h \
		$(srcdir)/Include/iterobject.h \
		$(srcdir)/Include/listobject.h \
		$(srcdir)/Include/longintrepr.h \
		$(srcdir)/Include/longobject.h \
		$(srcdir)/Include/marshal.h \
		$(srcdir)/Include/memoryobject.h \
		$(srcdir)/Include/metagrammar.h \
		$(srcdir)/Include/methodobject.h \
		$(srcdir)/Include/modsupport.h \
		$(srcdir)/Include/moduleobject.h \
		$(srcdir)/Include/namespaceobject.h \
		$(srcdir)/Include/node.h \
		$(srcdir)/Include/object.h \
		$(srcdir)/Include/objimpl.h \
		$(srcdir)/Include/opcode.h \
		$(srcdir)/Include/osdefs.h \
		$(srcdir)/Include/patchlevel.h \
		$(srcdir)/Include/pgen.h \
		$(srcdir)/Include/pgenheaders.h \
		$(srcdir)/Include/pyarena.h \
		$(srcdir)/Include/pyatomic.h \
		$(srcdir)/Include/pycapsule.h \
		$(srcdir)/Include/pyctype.h \
		$(srcdir)/Include/pydebug.h \
		$(srcdir)/Include/pyerrors.h \
		$(srcdir)/Include/pyfpe.h \
		$(srcdir)/Include/pymath.h \
		$(srcdir)/Include/pygetopt.h \
		$(srcdir)/Include/pymacro.h \
		$(srcdir)/Include/pymem.h \
		$(srcdir)/Include/pyport.h \
		$(srcdir)/Include/pystate.h \
		$(srcdir)/Include/pystrcmp.h \
		$(srcdir)/Include/pystrtod.h \
		$(srcdir)/Include/pythonrun.h \
		$(srcdir)/Include/pythread.h \
		$(srcdir)/Include/pytime.h \
		$(srcdir)/Include/rangeobject.h \
		$(srcdir)/Include/setobject.h \
		$(srcdir)/Include/sliceobject.h \
		$(srcdir)/Include/structmember.h \
		$(srcdir)/Include/structseq.h \
		$(srcdir)/Include/symtable.h \
		$(srcdir)/Include/sysmodule.h \
		$(srcdir)/Include/traceback.h \
		$(srcdir)/Include/tupleobject.h \
		$(srcdir)/Include/ucnhash.h \
		$(srcdir)/Include/unicodeobject.h \
		$(srcdir)/Include/warnings.h \
		$(srcdir)/Include/weakrefobject.h \
		pyconfig.h \
		$(PARSER_HEADERS)

$(LIBRARY_OBJS) $(MODOBJS) Modules/python.o: $(PYTHON_HEADERS)


######################################################################

TESTOPTS=	$(EXTRATESTOPTS)
TESTPYTHON=	$(RUNSHARED) ./$(BUILDPYTHON) $(TESTPYTHONOPTS)
TESTRUNNER=	$(TESTPYTHON) $(srcdir)/Tools/scripts/run_tests.py
TESTTIMEOUT=	3600

# Run a basic set of regression tests.
# This excludes some tests that are particularly resource-intensive.
test:		all platform
		$(TESTRUNNER) $(TESTOPTS)

# Run the full test suite twice - once without .pyc files, and once with.
# In the past, we've had problems where bugs in the marshalling or
# elsewhere caused bytecode read from .pyc files to behave differently
# than bytecode generated directly from a .py source file.  Sometimes
# the bytecode read from a .pyc file had the bug, sometimes the directly
# generated bytecode.  This is sometimes a very shy bug needing a lot of
# sample data.
testall:	all platform
		-find $(srcdir)/Lib -name '*.py[co]' -print | xargs rm -f
		$(TESTPYTHON) -E $(srcdir)/Lib/compileall.py
		-find $(srcdir)/Lib -name '*.py[co]' -print | xargs rm -f
		-$(TESTRUNNER) -u all $(TESTOPTS)
		$(TESTRUNNER) -u all $(TESTOPTS)

# Run the test suite for both architectures in a Universal build on OSX.
# Must be run on an Intel box.
testuniversal:	all platform
		if [ `arch` != 'i386' ];then \
			echo "This can only be used on OSX/i386" ;\
			exit 1 ;\
		fi
		$(TESTRUNNER) -u all $(TESTOPTS)
		$(RUNSHARED) /usr/libexec/oah/translate \
			./$(BUILDPYTHON) -E -m test -j 0 -u all $(TESTOPTS)

# Like testall, but with only one pass and without multiple processes.
# Run an optional script to include information about the build environment.
buildbottest:	all platform
		-@if which pybuildbot.identify >/dev/null 2>&1; then \
			pybuildbot.identify "CC='$(CC)'" "CXX='$(CXX)'"; \
		fi
		$(TESTRUNNER) -j 1 -u all -W --timeout=$(TESTTIMEOUT) $(TESTOPTS)

QUICKTESTOPTS=	$(TESTOPTS) -x test_subprocess test_io test_lib2to3 \
		test_multibytecodec test_urllib2_localnet test_itertools \
		test_multiprocessing test_mailbox test_socket test_poll \
		test_select test_zipfile test_concurrent_futures
quicktest:	all platform
		$(TESTRUNNER) $(QUICKTESTOPTS)


install: altinstall bininstall

altinstall:	 altbininstall libinstall inclinstall libainstall \
                sharedinstall oldsharedinstall maninstall 

# Install shared libraries enabled by Setup
DESTDIRS=	$(exec_prefix) $(LIBDIR) $(BINLIBDEST) $(DESTSHARED)

oldsharedinstall: $(DESTSHARED) $(SHAREDMODS)
		@for i in X $(SHAREDMODS); do \
		  if test $$i != X; then \
		    echo $(INSTALL_SHARED) $$i $(DESTSHARED)/`basename $$i`; \
		    $(INSTALL_SHARED) $$i $(DESTDIR)$(DESTSHARED)/`basename $$i`; \
		  fi; \
		done

$(DESTSHARED):
		@for i in $(DESTDIRS); \
		do \
			if test ! -d $(DESTDIR)$$i; then \
				echo "Creating directory $$i"; \
				$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
			else    true; \
			fi; \
		done

# Install the interpreter with $(VERSION) affixed
# This goes into $(exec_prefix)
altbininstall: $(BUILDPYTHON)
	@for i in $(BINDIR) $(LIBDIR); \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	$(INSTALL_PROGRAM) $(BUILDPYTHON) $(DESTDIR)$(BINDIR)/python$(LDVERSION)$(EXE)
	-if test "$(VERSION)" != "$(LDVERSION)"; then \
		if test -f $(DESTDIR)$(BINDIR)/python$(VERSION)$(EXE) -o -h $(DESTDIR)$(BINDIR)/python$(VERSION)$(EXE); \
		then rm -f $(DESTDIR)$(BINDIR)/python$(VERSION)$(EXE); \
		fi; \
		(cd $(DESTDIR)$(BINDIR); $(LN) python$(LDVERSION)$(EXE) python$(VERSION)$(EXE)); \
	fi
	if test -f $(LDLIBRARY); then \
		if test -n "$(DLLLIBRARY)" ; then \
			$(INSTALL_SHARED) $(DLLLIBRARY) $(DESTDIR)$(BINDIR); \
		else \
			$(INSTALL_SHARED) $(LDLIBRARY) $(DESTDIR)$(LIBDIR)/$(INSTSONAME); \
			if test $(LDLIBRARY) != $(INSTSONAME); then \
				(cd $(DESTDIR)$(LIBDIR); $(LN) -sf $(INSTSONAME) $(LDLIBRARY)) \
			fi \
		fi; \
		if test -n "$(PY3LIBRARY)"; then \
			$(INSTALL_SHARED) $(PY3LIBRARY) $(DESTDIR)$(LIBDIR)/$(PY3LIBRARY); \
		fi; \
	else	true; \
	fi

bininstall: altbininstall
	@for i in $(LIBPC); \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	-if test -f $(DESTDIR)$(BINDIR)/python3$(EXE) -o -h $(DESTDIR)$(BINDIR)/python3$(EXE); \
	then rm -f $(DESTDIR)$(BINDIR)/python3$(EXE); \
	else true; \
	fi
	(cd $(DESTDIR)$(BINDIR); $(LN) -s python$(VERSION)$(EXE) python3$(EXE))
	-if test "$(VERSION)" != "$(LDVERSION)"; then \
		rm -f $(DESTDIR)$(BINDIR)/python$(VERSION)-config; \
		(cd $(DESTDIR)$(BINDIR); $(LN) -s python$(LDVERSION)-config python$(VERSION)-config); \
		rm -f $(DESTDIR)$(LIBPC)/python-$(LDVERSION).pc; \
		(cd $(DESTDIR)$(LIBPC); $(LN) -s python-$(VERSION).pc python-$(LDVERSION).pc); \
	fi
	-rm -f $(DESTDIR)$(BINDIR)/python3-config
	(cd $(DESTDIR)$(BINDIR); $(LN) -s python$(VERSION)-config python3-config)
	-rm -f $(DESTDIR)$(LIBPC)/python3.pc
	(cd $(DESTDIR)$(LIBPC); $(LN) -s python-$(VERSION).pc python3.pc)
	-rm -f $(DESTDIR)$(BINDIR)/idle3
	(cd $(DESTDIR)$(BINDIR); $(LN) -s idle$(VERSION) idle3)
	-rm -f $(DESTDIR)$(BINDIR)/pydoc3
	(cd $(DESTDIR)$(BINDIR); $(LN) -s pydoc$(VERSION) pydoc3)
	-rm -f $(DESTDIR)$(BINDIR)/2to3
	(cd $(DESTDIR)$(BINDIR); $(LN) -s 2to3-$(VERSION) 2to3)
	-rm -f $(DESTDIR)$(BINDIR)/pyvenv
	(cd $(DESTDIR)$(BINDIR); $(LN) -s pyvenv-$(VERSION) pyvenv)

# Install the manual page
maninstall:
	@for i in $(MANDIR) $(MANDIR)/man1; \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	$(INSTALL_DATA) $(srcdir)/Misc/python.man \
		$(DESTDIR)$(MANDIR)/man1/python$(VERSION).1

# Install the library
PLATDIR=	plat-$(MACHDEP)
EXTRAPLATDIR= @EXTRAPLATDIR@
MACHDEPS=	$(PLATDIR) $(EXTRAPLATDIR)
XMLLIBSUBDIRS=  xml xml/dom xml/etree xml/parsers xml/sax
LIBSUBDIRS=	tkinter tkinter/test tkinter/test/test_tkinter \
		tkinter/test/test_ttk site-packages test \
		test/capath test/data \
		test/cjkencodings test/decimaltestdata test/xmltestdata \
		test/subprocessdata test/sndhdrdata \
		test/tracedmodules test/encoded_modules \
		test/namespace_pkgs \
		test/namespace_pkgs/both_portions \
		test/namespace_pkgs/both_portions/foo \
		test/namespace_pkgs/not_a_namespace_pkg \
		test/namespace_pkgs/not_a_namespace_pkg/foo \
		test/namespace_pkgs/portion1 \
		test/namespace_pkgs/portion1/foo \
		test/namespace_pkgs/portion2 \
		test/namespace_pkgs/portion2/foo \
		test/namespace_pkgs/project1 \
		test/namespace_pkgs/project1/parent \
		test/namespace_pkgs/project1/parent/child \
		test/namespace_pkgs/project2 \
		test/namespace_pkgs/project2/parent \
		test/namespace_pkgs/project2/parent/child \
		test/namespace_pkgs/project3 \
		test/namespace_pkgs/project3/parent \
		test/namespace_pkgs/project3/parent/child \
                test/namespace_pkgs/module_and_namespace_package \
                test/namespace_pkgs/module_and_namespace_package/a_test \
		collections concurrent concurrent/futures encodings \
		email email/mime test/test_email test/test_email/data \
		html json test/json_tests http dbm xmlrpc \
		sqlite3 sqlite3/test \
		logging csv wsgiref urllib \
		lib2to3 lib2to3/fixes lib2to3/pgen2 lib2to3/tests \
		lib2to3/tests/data lib2to3/tests/data/fixers \
		lib2to3/tests/data/fixers/myfixes \
		ctypes ctypes/test ctypes/macholib idlelib idlelib/Icons \
		distutils distutils/command distutils/tests $(XMLLIBSUBDIRS) \
		importlib test/test_importlib test/test_importlib/builtin \
		test/test_importlib/extension test/test_importlib/frozen \
		test/test_importlib/import_ test/test_importlib/source \
		turtledemo \
		multiprocessing multiprocessing/dummy \
		unittest unittest/test unittest/test/testmock \
		venv venv/scripts venv/scripts/posix \
		curses pydoc_data $(MACHDEPS)
libinstall:	build_all $(srcdir)/Lib/$(PLATDIR) $(srcdir)/Modules/xxmodule.c
	@for i in $(SCRIPTDIR) $(LIBDEST) $(DESTSHARED); \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	@for d in $(LIBSUBDIRS); \
	do \
		a=$(srcdir)/Lib/$$d; \
		if test ! -d $$a; then continue; else true; fi; \
		b=$(LIBDEST)/$$d; \
		if test ! -d $(DESTDIR)$$b; then \
			echo "Creating directory $$b"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$b; \
		else	true; \
		fi; \
	done
	@for i in $(srcdir)/Lib/*.py; \
	do \
		if test -x $$i; then \
			$(INSTALL_SCRIPT) $$i $(DESTDIR)$(LIBDEST); \
			echo $(INSTALL_SCRIPT) $$i $(LIBDEST); \
		else \
			$(INSTALL_DATA) $$i $(DESTDIR)$(LIBDEST); \
			echo $(INSTALL_DATA) $$i $(LIBDEST); \
		fi; \
	done
	@for i in `cat pybuilddir.txt`/_sysconfigdata.py; \
	do \
		if test -x $$i; then \
			$(INSTALL_SCRIPT) $$i $(DESTDIR)$(DESTSHARED); \
			echo $(INSTALL_SCRIPT) $$i $(DESTSHARED); \
		else \
			$(INSTALL_DATA) $$i $(DESTDIR)$(DESTSHARED); \
			echo $(INSTALL_DATA) $$i $(DESTSHARED); \
		fi; \
	done
	@for d in $(LIBSUBDIRS); \
	do \
		a=$(srcdir)/Lib/$$d; \
		if test ! -d $$a; then continue; else true; fi; \
		if test `ls $$a | wc -l` -lt 1; then continue; fi; \
		b=$(LIBDEST)/$$d; \
		for i in $$a/*; \
		do \
			case $$i in \
			*CVS) ;; \
			*.py[co]) ;; \
			*.orig) ;; \
			*~) ;; \
			*) \
				if test -d $$i; then continue; fi; \
				if test -x $$i; then \
				    echo $(INSTALL_SCRIPT) $$i $$b; \
				    $(INSTALL_SCRIPT) $$i $(DESTDIR)$$b; \
				else \
				    echo $(INSTALL_DATA) $$i $$b; \
				    $(INSTALL_DATA) $$i $(DESTDIR)$$b; \
				fi;; \
			esac; \
		done; \
	done
	$(INSTALL_DATA) $(srcdir)/LICENSE $(DESTDIR)$(LIBDEST)/LICENSE.txt
	if test -d $(DESTDIR)$(LIBDEST)/distutils/tests; then \
		$(INSTALL_DATA) $(srcdir)/Modules/xxmodule.c \
			$(DESTDIR)$(LIBDEST)/distutils/tests ; \
	fi
	-PYTHONPATH=$(DESTDIR)$(LIBDEST)  $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -Wi $(DESTDIR)$(LIBDEST)/compileall.py \
		-d $(LIBDEST) -f \
		-x 'bad_coding|badsyntax|site-packages|lib2to3/tests/data' \
		$(DESTDIR)$(LIBDEST)
	-PYTHONPATH=$(DESTDIR)$(LIBDEST) $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -Wi -O $(DESTDIR)$(LIBDEST)/compileall.py \
		-d $(LIBDEST) -f \
		-x 'bad_coding|badsyntax|site-packages|lib2to3/tests/data' \
		$(DESTDIR)$(LIBDEST)
	-PYTHONPATH=$(DESTDIR)$(LIBDEST)  $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -Wi $(DESTDIR)$(LIBDEST)/compileall.py \
		-d $(LIBDEST)/site-packages -f \
		-x badsyntax $(DESTDIR)$(LIBDEST)/site-packages
	-PYTHONPATH=$(DESTDIR)$(LIBDEST) $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -Wi -O $(DESTDIR)$(LIBDEST)/compileall.py \
		-d $(LIBDEST)/site-packages -f \
		-x badsyntax $(DESTDIR)$(LIBDEST)/site-packages
	-PYTHONPATH=$(DESTDIR)$(LIBDEST) $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -m lib2to3.pgen2.driver $(DESTDIR)$(LIBDEST)/lib2to3/Grammar.txt
	-PYTHONPATH=$(DESTDIR)$(LIBDEST) $(RUNSHARED) \
		$(PYTHON_FOR_BUILD) -m lib2to3.pgen2.driver $(DESTDIR)$(LIBDEST)/lib2to3/PatternGrammar.txt

# Create the PLATDIR source directory, if one wasn't distributed..
$(srcdir)/Lib/$(PLATDIR):
	mkdir $(srcdir)/Lib/$(PLATDIR)
	cp $(srcdir)/Lib/plat-generic/regen $(srcdir)/Lib/$(PLATDIR)/regen
	export PATH; PATH="`pwd`:$$PATH"; \
	export PYTHONPATH; PYTHONPATH="`pwd`/Lib"; \
	export DYLD_FRAMEWORK_PATH; DYLD_FRAMEWORK_PATH="`pwd`"; \
	export EXE; EXE="$(BUILDEXE)"; \
	cd $(srcdir)/Lib/$(PLATDIR); $(RUNSHARED) ./regen

python-config: $(srcdir)/Misc/python-config.in
	# Substitution happens here, as the completely-expanded BINDIR
	# is not available in configure
	sed -e "s,@EXENAME@,$(BINDIR)/python$(LDVERSION)$(EXE)," < $(srcdir)/Misc/python-config.in >python-config

# Install the include files
INCLDIRSTOMAKE=$(INCLUDEDIR) $(CONFINCLUDEDIR) $(INCLUDEPY) $(CONFINCLUDEPY)
inclinstall:
	@for i in $(INCLDIRSTOMAKE); \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	@for i in $(srcdir)/Include/*.h; \
	do \
		echo $(INSTALL_DATA) $$i $(INCLUDEPY); \
		$(INSTALL_DATA) $$i $(DESTDIR)$(INCLUDEPY); \
	done
	$(INSTALL_DATA) pyconfig.h $(DESTDIR)$(CONFINCLUDEPY)/pyconfig.h

# Install the library and miscellaneous stuff needed for extending/embedding
# This goes into $(exec_prefix)
LIBPL=		$(BINLIBDEST)/config-$(LDVERSION)

# pkgconfig directory
LIBPC=		$(LIBDIR)/pkgconfig

libainstall:	all python-config
	@for i in $(LIBDIR) $(LIBPL) $(LIBPC) $(BINDIR); \
	do \
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	@if test -d $(LIBRARY); then :; else \
		if test "$(PYTHONFRAMEWORKDIR)" = no-framework; then \
			if test "$(SO)" = .dll; then \
				$(INSTALL_DATA) $(LDLIBRARY) $(DESTDIR)$(LIBPL) ; \
			else \
				$(INSTALL_DATA) $(LIBRARY) $(DESTDIR)$(LIBPL)/$(LIBRARY) ; \
				$(RANLIB) $(DESTDIR)$(LIBPL)/$(LIBRARY) ; \
			fi; \
		else \
			echo Skip install of $(LIBRARY) - use make frameworkinstall; \
		fi; \
	fi
	$(INSTALL_DATA) Modules/config.c $(DESTDIR)$(LIBPL)/config.c
	$(INSTALL_DATA) Modules/python.o $(DESTDIR)$(LIBPL)/python.o
	$(INSTALL_DATA) $(srcdir)/Modules/config.c.in $(DESTDIR)$(LIBPL)/config.c.in
	$(INSTALL_DATA) Makefile $(DESTDIR)$(LIBPL)/Makefile
	$(INSTALL_DATA) Modules/Setup $(DESTDIR)$(LIBPL)/Setup
	$(INSTALL_DATA) Modules/Setup.local $(DESTDIR)$(LIBPL)/Setup.local
	$(INSTALL_DATA) Modules/Setup.config $(DESTDIR)$(LIBPL)/Setup.config
	$(INSTALL_DATA) Misc/python.pc $(DESTDIR)$(LIBPC)/python-$(VERSION).pc
	$(INSTALL_SCRIPT) $(srcdir)/Modules/makesetup $(DESTDIR)$(LIBPL)/makesetup
	$(INSTALL_SCRIPT) $(srcdir)/install-sh $(DESTDIR)$(LIBPL)/install-sh
	$(INSTALL_SCRIPT) python-config $(DESTDIR)$(BINDIR)/python$(LDVERSION)-config
	rm python-config
	@if [ -s Modules/python.exp -a \
		"`echo $(MACHDEP) | sed 's/^\(...\).*/\1/'`" = "aix" ]; then \
		echo; echo "Installing support files for building shared extension modules on AIX:"; \
		$(INSTALL_DATA) Modules/python.exp		\
				$(DESTDIR)$(LIBPL)/python.exp;		\
		echo; echo "$(LIBPL)/python.exp";		\
		$(INSTALL_SCRIPT) $(srcdir)/Modules/makexp_aix	\
				$(DESTDIR)$(LIBPL)/makexp_aix;		\
		echo "$(LIBPL)/makexp_aix";			\
		$(INSTALL_SCRIPT) $(srcdir)/Modules/ld_so_aix	\
				$(DESTDIR)$(LIBPL)/ld_so_aix;		\
		echo "$(LIBPL)/ld_so_aix";			\
		echo; echo "See Misc/AIX-NOTES for details.";	\
	else true; \
	fi

# Install the dynamically loadable modules
# This goes into $(exec_prefix)
sharedinstall: sharedmods
	$(RUNSHARED) $(PYTHON_FOR_BUILD) $(srcdir)/setup.py install \
	   	--prefix=$(prefix) \
		--install-scripts=$(BINDIR) \
		--install-platlib=$(DESTSHARED) \
		--root=$(DESTDIR)/
	-rm $(DESTDIR)$(DESTSHARED)/_sysconfigdata.py
	-rm -r $(DESTDIR)$(DESTSHARED)/__pycache__

# Here are a couple of targets for MacOSX again, to install a full
# framework-based Python. frameworkinstall installs everything, the
# subtargets install specific parts. Much of the actual work is offloaded to
# the Makefile in Mac
#
#
# This target is here for backward compatiblity, previous versions of Python
# hadn't integrated framework installation in the normal install process.
frameworkinstall: install

# On install, we re-make the framework
# structure in the install location, /Library/Frameworks/ or the argument to
# --enable-framework. If --enable-framework has been specified then we have
# automatically set prefix to the location deep down in the framework, so we
# only have to cater for the structural bits of the framework.

frameworkinstallframework: frameworkinstallstructure install frameworkinstallmaclib

frameworkinstallstructure:	$(LDLIBRARY)
	@if test "$(PYTHONFRAMEWORKDIR)" = no-framework; then \
		echo Not configured with --enable-framework; \
		exit 1; \
	else true; \
	fi
	@for i in $(prefix)/Resources/English.lproj $(prefix)/lib; do\
		if test ! -d $(DESTDIR)$$i; then \
			echo "Creating directory $(DESTDIR)$$i"; \
			$(INSTALL) -d -m $(DIRMODE) $(DESTDIR)$$i; \
		else	true; \
		fi; \
	done
	$(LN) -fsn include/python$(LDVERSION) $(DESTDIR)$(prefix)/Headers
	sed 's/%VERSION%/'"`$(RUNSHARED) ./$(BUILDPYTHON) -c 'import platform; print(platform.python_version())'`"'/g' < $(RESSRCDIR)/Info.plist > $(DESTDIR)$(prefix)/Resources/Info.plist
	$(LN) -fsn $(VERSION) $(DESTDIR)$(PYTHONFRAMEWORKINSTALLDIR)/Versions/Current
	$(LN) -fsn Versions/Current/$(PYTHONFRAMEWORK) $(DESTDIR)$(PYTHONFRAMEWORKINSTALLDIR)/$(PYTHONFRAMEWORK)
	$(LN) -fsn Versions/Current/Headers $(DESTDIR)$(PYTHONFRAMEWORKINSTALLDIR)/Headers
	$(LN) -fsn Versions/Current/Resources $(DESTDIR)$(PYTHONFRAMEWORKINSTALLDIR)/Resources
	$(INSTALL_SHARED) $(LDLIBRARY) $(DESTDIR)$(PYTHONFRAMEWORKPREFIX)/$(LDLIBRARY)

# This installs Mac/Lib into the framework
# Install a number of symlinks to keep software that expects a normal unix
# install (which includes python-config) happy.
frameworkinstallmaclib:
	ln -fs "../../../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/python$(VERSION)/config-$(LDVERSION)/libpython$(LDVERSION).a"
	ln -fs "../../../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/python$(VERSION)/config-$(LDVERSION)/libpython$(LDVERSION).dylib"
	ln -fs "../../../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/python$(VERSION)/config-$(LDVERSION)/libpython$(VERSION).a"
	ln -fs "../../../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/python$(VERSION)/config-$(LDVERSION)/libpython$(VERSION).dylib"
	ln -fs "../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/libpython$(LDVERSION).dylib"
	ln -fs "../$(PYTHONFRAMEWORK)" "$(DESTDIR)$(prefix)/lib/libpython$(VERSION).dylib"

# This installs the IDE, the Launcher and other apps into /Applications
frameworkinstallapps:
	cd Mac && $(MAKE) installapps DESTDIR="$(DESTDIR)"

# This install the unix python and pythonw tools in /usr/local/bin
frameworkinstallunixtools:
	cd Mac && $(MAKE) installunixtools DESTDIR="$(DESTDIR)"

frameworkaltinstallunixtools:
	cd Mac && $(MAKE) altinstallunixtools DESTDIR="$(DESTDIR)"

# This installs the Tools into the applications directory.
# It is not part of a normal frameworkinstall
frameworkinstallextras:
	cd Mac && $(MAKE) installextras DESTDIR="$(DESTDIR)"

# This installs a few of the useful scripts in Tools/scripts
scriptsinstall:
	SRCDIR=$(srcdir) $(RUNSHARED) \
	$(PYTHON_FOR_BUILD) $(srcdir)/Tools/scripts/setup.py install \
	--prefix=$(prefix) \
	--install-scripts=$(BINDIR) \
	--root=$(DESTDIR)/

# Build the toplevel Makefile
Makefile.pre: $(srcdir)/Makefile.pre.in config.status
	CONFIG_FILES=Makefile.pre CONFIG_HEADERS= $(SHELL) config.status
	$(MAKE) -f Makefile.pre Makefile

# Run the configure script.
config.status:	$(srcdir)/configure
	$(SHELL) $(srcdir)/configure $(CONFIG_ARGS)

.PRECIOUS: config.status $(BUILDPYTHON) Makefile Makefile.pre

# Some make's put the object file in the current directory
.c.o:
	$(CC) -c $(PY_CORE_CFLAGS) -o $@ $<

# Run reindent on the library
reindent:
	./$(BUILDPYTHON) $(srcdir)/Tools/scripts/reindent.py -r $(srcdir)/Lib

# Rerun configure with the same options as it was run last time,
# provided the config.status script exists
recheck:
	$(SHELL) config.status --recheck
	$(SHELL) config.status

# Rebuild the configure script from configure.ac; also rebuild pyconfig.h.in
autoconf:
	(cd $(srcdir); autoconf -Wall)
	(cd $(srcdir); autoheader -Wall)

# Create a tags file for vi
tags::
	cd $(srcdir); \
	ctags -w -t Include/*.h; \
	for i in $(SRCDIRS); do ctags -w -t -a $$i/*.[ch]; \
	done; \
	sort -o tags tags

# Create a tags file for GNU Emacs
TAGS::
	cd $(srcdir); \
	etags Include/*.h; \
	for i in $(SRCDIRS); do etags -a $$i/*.[ch]; done

# Touch generated files
touch:
	hg --config extensions.touch=Tools/hg/hgtouch.py touch -v

# Sanitation targets -- clean leaves libraries, executables and tags
# files, which clobber removes as well
pycremoval:
	-find $(srcdir) -depth -name '__pycache__' -exec rm -rf {} ';'
	-find $(srcdir) -name '*.py[co]' -exec rm -f {} ';'

rmtestturds:
	-rm -f *BAD *GOOD *SKIPPED
	-rm -rf OUT
	-rm -f *.TXT
	-rm -f *.txt
	-rm -f gb-18030-2000.xml

docclean:
	-rm -rf Doc/build
	-rm -rf Doc/tools/sphinx Doc/tools/pygments Doc/tools/docutils

clean: pycremoval
	find . -name '*.[oa]' -exec rm -f {} ';'
	find . -name '*.s[ol]' -exec rm -f {} ';'
	find . -name '*.so.[0-9]*.[0-9]*' -exec rm -f {} ';'
	find build -name 'fficonfig.h' -exec rm -f {} ';' || true
	find build -name '*.py' -exec rm -f {} ';' || true
	find build -name '*.py[co]' -exec rm -f {} ';' || true
	-rm -f pybuilddir.txt
	-rm -f Lib/lib2to3/*Grammar*.pickle
	-rm -f Modules/_testembed Modules/_freeze_importlib

profile-removal:
	find . -name '*.gc??' -exec rm -f {} ';'

clobber: clean profile-removal
	-rm -f $(BUILDPYTHON) $(PGEN) $(LIBRARY) $(LDLIBRARY) $(DLLLIBRARY) \
		tags TAGS \
		config.cache config.log pyconfig.h Modules/config.c
	-rm -rf build platform
	-rm -rf $(PYTHONFRAMEWORKDIR)

# Make things extra clean, before making a distribution:
# remove all generated files, even Makefile[.pre]
# Keep configure and Python-ast.[ch], it's possible they can't be generated
distclean: clobber
	for file in Lib/test/data/* ; do \
	    if test "$$file" != "Lib/test/data/README"; then rm "$$file"; fi; \
	done
	-rm -f core Makefile Makefile.pre config.status \
		Modules/Setup Modules/Setup.local Modules/Setup.config \
		Modules/ld_so_aix Modules/python.exp Misc/python.pc
	-rm -f python*-gdb.py
	find $(srcdir) '(' -name '*.fdc' -o -name '*~' \
			   -o -name '[@,#]*' -o -name '*.old' \
			   -o -name '*.orig' -o -name '*.rej' \
			   -o -name '*.bak' ')' \
			   -exec rm -f {} ';'

# Check for smelly exported symbols (not starting with Py/_Py)
smelly: all
	nm -p $(LIBRARY) | \
		sed -n "/ [TDB] /s/.* //p" | grep -v "^_*Py" | sort -u; \

# Find files with funny names
funny:
	find $(SUBDIRS) $(SUBDIRSTOO) \
		-type d \
		-o -name '*.[chs]' \
		-o -name '*.py' \
		-o -name '*.pyw' \
		-o -name '*.dat' \
		-o -name '*.el' \
		-o -name '*.fd' \
		-o -name '*.in' \
		-o -name '*.gif' \
		-o -name '*.txt' \
		-o -name '*.xml' \
		-o -name '*.xbm' \
		-o -name '*.xpm' \
		-o -name '*.uue' \
		-o -name '*.decTest' \
		-o -name '*.tmCommand' \
		-o -name '*.tmSnippet' \
		-o -name 'Setup' \
		-o -name 'Setup.*' \
		-o -name regen \
		-o -name README \
		-o -name NEWS \
		-o -name HISTORY \
		-o -name Makefile \
		-o -name ChangeLog \
		-o -name .hgignore \
		-o -name .bzrignore \
		-o -name MANIFEST \
		-o -print

# Perform some verification checks on any modified files.
patchcheck:
	$(RUNSHARED) ./$(BUILDPYTHON) $(srcdir)/Tools/scripts/patchcheck.py

# Dependencies

Python/thread.o:  $(srcdir)/Python/thread_foobar.h $(srcdir)/Python/thread_nt.h $(srcdir)/Python/thread_os2.h $(srcdir)/Python/thread_pth.h $(srcdir)/Python/thread_pthread.h

# Declare targets that aren't real files
.PHONY: all build_all sharedmods oldsharedmods test quicktest
.PHONY: install altinstall oldsharedinstall bininstall altbininstall
.PHONY: maninstall libinstall inclinstall libainstall sharedinstall
.PHONY: frameworkinstall frameworkinstallframework frameworkinstallstructure
.PHONY: frameworkinstallmaclib frameworkinstallapps frameworkinstallunixtools
.PHONY: frameworkaltinstallunixtools recheck autoconf clean clobber distclean
.PHONY: smelly funny patchcheck touch
.PHONY: gdbhooks

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
# Local Variables:
# mode: makefile
# End:

# Rules appended by makedepend

Modules/_threadmodule.o: $(srcdir)/Modules/_threadmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_threadmodule.c -o Modules/_threadmodule.o
Modules/_threadmodule$(SO):  Modules/_threadmodule.o; $(BLDSHARED)  Modules/_threadmodule.o   -o Modules/_threadmodule$(SO)
Modules/signalmodule.o: $(srcdir)/Modules/signalmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/signalmodule.c -o Modules/signalmodule.o
Modules/signalmodule$(SO):  Modules/signalmodule.o; $(BLDSHARED)  Modules/signalmodule.o   -o Modules/signalmodule$(SO)
Modules/posixmodule.o: $(srcdir)/Modules/posixmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/posixmodule.c -o Modules/posixmodule.o
Modules/posixmodule$(SO):  Modules/posixmodule.o; $(BLDSHARED)  Modules/posixmodule.o   -o Modules/posixmodule$(SO)
Modules/errnomodule.o: $(srcdir)/Modules/errnomodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/errnomodule.c -o Modules/errnomodule.o
Modules/errnomodule$(SO):  Modules/errnomodule.o; $(BLDSHARED)  Modules/errnomodule.o   -o Modules/errnomodule$(SO)
Modules/pwdmodule.o: $(srcdir)/Modules/pwdmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/pwdmodule.c -o Modules/pwdmodule.o
Modules/pwdmodule$(SO):  Modules/pwdmodule.o; $(BLDSHARED)  Modules/pwdmodule.o   -o Modules/pwdmodule$(SO)
Modules/_sre.o: $(srcdir)/Modules/_sre.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_sre.c -o Modules/_sre.o
Modules/_sre$(SO):  Modules/_sre.o; $(BLDSHARED)  Modules/_sre.o   -o Modules/_sre$(SO)
Modules/_functoolsmodule.o: $(srcdir)/Modules/_functoolsmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_functoolsmodule.c -o Modules/_functoolsmodule.o
Modules/_functoolsmodule$(SO):  Modules/_functoolsmodule.o; $(BLDSHARED)  Modules/_functoolsmodule.o   -o Modules/_functoolsmodule$(SO)
Modules/operator.o: $(srcdir)/Modules/operator.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/operator.c -o Modules/operator.o
Modules/operator$(SO):  Modules/operator.o; $(BLDSHARED)  Modules/operator.o   -o Modules/operator$(SO)
Modules/_collectionsmodule.o: $(srcdir)/Modules/_collectionsmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_collectionsmodule.c -o Modules/_collectionsmodule.o
Modules/_collectionsmodule$(SO):  Modules/_collectionsmodule.o; $(BLDSHARED)  Modules/_collectionsmodule.o   -o Modules/_collectionsmodule$(SO)
Modules/itertoolsmodule.o: $(srcdir)/Modules/itertoolsmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/itertoolsmodule.c -o Modules/itertoolsmodule.o
Modules/itertoolsmodule$(SO):  Modules/itertoolsmodule.o; $(BLDSHARED)  Modules/itertoolsmodule.o   -o Modules/itertoolsmodule$(SO)
Modules/_iomodule.o: $(srcdir)/Modules/_io/_iomodule.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/_iomodule.c -o Modules/_iomodule.o
Modules/iobase.o: $(srcdir)/Modules/_io/iobase.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/iobase.c -o Modules/iobase.o
Modules/fileio.o: $(srcdir)/Modules/_io/fileio.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/fileio.c -o Modules/fileio.o
Modules/bytesio.o: $(srcdir)/Modules/_io/bytesio.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/bytesio.c -o Modules/bytesio.o
Modules/bufferedio.o: $(srcdir)/Modules/_io/bufferedio.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/bufferedio.c -o Modules/bufferedio.o
Modules/textio.o: $(srcdir)/Modules/_io/textio.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/textio.c -o Modules/textio.o
Modules/stringio.o: $(srcdir)/Modules/_io/stringio.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_io -c $(srcdir)/Modules/_io/stringio.c -o Modules/stringio.o
Modules/_iomodule$(SO):  Modules/_iomodule.o Modules/iobase.o Modules/fileio.o Modules/bytesio.o Modules/bufferedio.o Modules/textio.o Modules/stringio.o; $(BLDSHARED)  Modules/_iomodule.o Modules/iobase.o Modules/fileio.o Modules/bytesio.o Modules/bufferedio.o Modules/textio.o Modules/stringio.o   -o Modules/_iomodule$(SO)
Modules/_codecsmodule.o: $(srcdir)/Modules/_codecsmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_codecsmodule.c -o Modules/_codecsmodule.o
Modules/_codecsmodule$(SO):  Modules/_codecsmodule.o; $(BLDSHARED)  Modules/_codecsmodule.o   -o Modules/_codecsmodule$(SO)
Modules/multibytecodec.o: $(srcdir)/Modules/cjkcodecs/multibytecodec.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/multibytecodec.c -o Modules/multibytecodec.o
Modules/_multibytecodecmodule$(SO):  Modules/multibytecodec.o; $(BLDSHARED)  Modules/multibytecodec.o   -o Modules/_multibytecodecmodule$(SO)
Modules/_codecs_cn.o: $(srcdir)/Modules/cjkcodecs/_codecs_cn.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_cn.c -o Modules/_codecs_cn.o
Modules/_codecs_cn$(SO):  Modules/_codecs_cn.o; $(BLDSHARED)  Modules/_codecs_cn.o   -o Modules/_codecs_cn$(SO)
Modules/_codecs_hk.o: $(srcdir)/Modules/cjkcodecs/_codecs_hk.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_hk.c -o Modules/_codecs_hk.o
Modules/_codecs_hk$(SO):  Modules/_codecs_hk.o; $(BLDSHARED)  Modules/_codecs_hk.o   -o Modules/_codecs_hk$(SO)
Modules/_codecs_iso2022.o: $(srcdir)/Modules/cjkcodecs/_codecs_iso2022.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_iso2022.c -o Modules/_codecs_iso2022.o
Modules/_codecs_iso2022$(SO):  Modules/_codecs_iso2022.o; $(BLDSHARED)  Modules/_codecs_iso2022.o   -o Modules/_codecs_iso2022$(SO)
Modules/_codecs_jp.o: $(srcdir)/Modules/cjkcodecs/_codecs_jp.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_jp.c -o Modules/_codecs_jp.o
Modules/_codecs_jp$(SO):  Modules/_codecs_jp.o; $(BLDSHARED)  Modules/_codecs_jp.o   -o Modules/_codecs_jp$(SO)
Modules/_codecs_kr.o: $(srcdir)/Modules/cjkcodecs/_codecs_kr.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_kr.c -o Modules/_codecs_kr.o
Modules/_codecs_kr$(SO):  Modules/_codecs_kr.o; $(BLDSHARED)  Modules/_codecs_kr.o   -o Modules/_codecs_kr$(SO)
Modules/_codecs_tw.o: $(srcdir)/Modules/cjkcodecs/_codecs_tw.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cjkcodecs/_codecs_tw.c -o Modules/_codecs_tw.o
Modules/_codecs_tw$(SO):  Modules/_codecs_tw.o; $(BLDSHARED)  Modules/_codecs_tw.o   -o Modules/_codecs_tw$(SO)
Modules/_localemodule.o: $(srcdir)/Modules/_localemodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_localemodule.c -o Modules/_localemodule.o
Modules/_localemodule$(SO):  Modules/_localemodule.o; $(BLDSHARED)  Modules/_localemodule.o   -o Modules/_localemodule$(SO)
Modules/zipimport.o: $(srcdir)/Modules/zipimport.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/zipimport.c -o Modules/zipimport.o
Modules/zipimport$(SO):  Modules/zipimport.o; $(BLDSHARED)  Modules/zipimport.o   -o Modules/zipimport$(SO)
Modules/faulthandler.o: $(srcdir)/Modules/faulthandler.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/faulthandler.c -o Modules/faulthandler.o
Modules/faulthandler$(SO):  Modules/faulthandler.o; $(BLDSHARED)  Modules/faulthandler.o   -o Modules/faulthandler$(SO)
Modules/symtablemodule.o: $(srcdir)/Modules/symtablemodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/symtablemodule.c -o Modules/symtablemodule.o
Modules/_symtablemodule$(SO):  Modules/symtablemodule.o; $(BLDSHARED)  Modules/symtablemodule.o   -o Modules/_symtablemodule$(SO)
Modules/arraymodule.o: $(srcdir)/Modules/arraymodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/arraymodule.c -o Modules/arraymodule.o
Modules/arraymodule$(SO):  Modules/arraymodule.o; $(BLDSHARED)  Modules/arraymodule.o   -o Modules/arraymodule$(SO)
Modules/cmathmodule.o: $(srcdir)/Modules/cmathmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/cmathmodule.c -o Modules/cmathmodule.o
Modules/_math.o: $(srcdir)/Modules/_math.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_math.c -o Modules/_math.o
Modules/cmathmodule$(SO):  Modules/cmathmodule.o Modules/_math.o; $(BLDSHARED)  Modules/cmathmodule.o Modules/_math.o   -o Modules/cmathmodule$(SO)
Modules/mathmodule.o: $(srcdir)/Modules/mathmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/mathmodule.c -o Modules/mathmodule.o
Modules/mathmodule$(SO):  Modules/mathmodule.o; $(BLDSHARED)  Modules/mathmodule.o   -o Modules/mathmodule$(SO)
Modules/_struct.o: $(srcdir)/Modules/_struct.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_struct.c -o Modules/_struct.o
Modules/_struct$(SO):  Modules/_struct.o; $(BLDSHARED)  Modules/_struct.o   -o Modules/_struct$(SO)
Modules/timemodule.o: $(srcdir)/Modules/timemodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/timemodule.c -o Modules/timemodule.o
Modules/timemodule$(SO):  Modules/timemodule.o; $(BLDSHARED)  Modules/timemodule.o   -o Modules/timemodule$(SO)
Modules/_weakref.o: $(srcdir)/Modules/_weakref.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_weakref.c -o Modules/_weakref.o
Modules/_weakref$(SO):  Modules/_weakref.o; $(BLDSHARED)  Modules/_weakref.o   -o Modules/_weakref$(SO)
Modules/_randommodule.o: $(srcdir)/Modules/_randommodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_randommodule.c -o Modules/_randommodule.o
Modules/_randommodule$(SO):  Modules/_randommodule.o; $(BLDSHARED)  Modules/_randommodule.o   -o Modules/_randommodule$(SO)
Modules/atexitmodule.o: $(srcdir)/Modules/atexitmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/atexitmodule.c -o Modules/atexitmodule.o
Modules/atexitmodule$(SO):  Modules/atexitmodule.o; $(BLDSHARED)  Modules/atexitmodule.o   -o Modules/atexitmodule$(SO)
Modules/_elementtree.o: $(srcdir)/Modules/_elementtree.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/expat -DHAVE_EXPAT_CONFIG_H -DUSE_PYEXPAT_CAPI -c $(srcdir)/Modules/_elementtree.c -o Modules/_elementtree.o
Modules/_elementtree$(SO):  Modules/_elementtree.o; $(BLDSHARED)  Modules/_elementtree.o   -o Modules/_elementtree$(SO)
Modules/xmlparse.o: $(srcdir)/Modules/expat/xmlparse.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/expat -DHAVE_EXPAT_CONFIG_H -DUSE_PYEXPAT_CAPI -c $(srcdir)/Modules/expat/xmlparse.c -o Modules/xmlparse.o
Modules/xmlrole.o: $(srcdir)/Modules/expat/xmlrole.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/expat -DHAVE_EXPAT_CONFIG_H -DUSE_PYEXPAT_CAPI -c $(srcdir)/Modules/expat/xmlrole.c -o Modules/xmlrole.o
Modules/xmltok.o: $(srcdir)/Modules/expat/xmltok.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/expat -DHAVE_EXPAT_CONFIG_H -DUSE_PYEXPAT_CAPI -c $(srcdir)/Modules/expat/xmltok.c -o Modules/xmltok.o
Modules/pyexpat.o: $(srcdir)/Modules/pyexpat.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/expat -DHAVE_EXPAT_CONFIG_H -DUSE_PYEXPAT_CAPI -c $(srcdir)/Modules/pyexpat.c -o Modules/pyexpat.o
Modules/pyexpat$(SO):  Modules/xmlparse.o Modules/xmlrole.o Modules/xmltok.o Modules/pyexpat.o; $(BLDSHARED)  Modules/xmlparse.o Modules/xmlrole.o Modules/xmltok.o Modules/pyexpat.o   -o Modules/pyexpat$(SO)
Modules/_pickle.o: $(srcdir)/Modules/_pickle.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_pickle.c -o Modules/_pickle.o
Modules/_pickle$(SO):  Modules/_pickle.o; $(BLDSHARED)  Modules/_pickle.o   -o Modules/_pickle$(SO)
Modules/_datetimemodule.o: $(srcdir)/Modules/_datetimemodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_datetimemodule.c -o Modules/_datetimemodule.o
Modules/_datetimemodule$(SO):  Modules/_datetimemodule.o; $(BLDSHARED)  Modules/_datetimemodule.o   -o Modules/_datetimemodule$(SO)
Modules/_bisectmodule.o: $(srcdir)/Modules/_bisectmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_bisectmodule.c -o Modules/_bisectmodule.o
Modules/_bisectmodule$(SO):  Modules/_bisectmodule.o; $(BLDSHARED)  Modules/_bisectmodule.o   -o Modules/_bisectmodule$(SO)
Modules/_heapqmodule.o: $(srcdir)/Modules/_heapqmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_heapqmodule.c -o Modules/_heapqmodule.o
Modules/_heapqmodule$(SO):  Modules/_heapqmodule.o; $(BLDSHARED)  Modules/_heapqmodule.o   -o Modules/_heapqmodule$(SO)
Modules/unicodedata.o: $(srcdir)/Modules/unicodedata.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/unicodedata.c -o Modules/unicodedata.o
Modules/unicodedata$(SO):  Modules/unicodedata.o; $(BLDSHARED)  Modules/unicodedata.o   -o Modules/unicodedata$(SO)
Modules/fcntlmodule.o: $(srcdir)/Modules/fcntlmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/fcntlmodule.c -o Modules/fcntlmodule.o
Modules/fcntlmodule$(SO):  Modules/fcntlmodule.o; $(BLDSHARED)  Modules/fcntlmodule.o   -o Modules/fcntlmodule$(SO)
Modules/grpmodule.o: $(srcdir)/Modules/grpmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/grpmodule.c -o Modules/grpmodule.o
Modules/grpmodule$(SO):  Modules/grpmodule.o; $(BLDSHARED)  Modules/grpmodule.o   -o Modules/grpmodule$(SO)
Modules/selectmodule.o: $(srcdir)/Modules/selectmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/selectmodule.c -o Modules/selectmodule.o
Modules/selectmodule$(SO):  Modules/selectmodule.o; $(BLDSHARED)  Modules/selectmodule.o   -o Modules/selectmodule$(SO)
Modules/mmapmodule.o: $(srcdir)/Modules/mmapmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/mmapmodule.c -o Modules/mmapmodule.o
Modules/mmapmodule$(SO):  Modules/mmapmodule.o; $(BLDSHARED)  Modules/mmapmodule.o   -o Modules/mmapmodule$(SO)
Modules/_csv.o: $(srcdir)/Modules/_csv.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_csv.c -o Modules/_csv.o
Modules/_csv$(SO):  Modules/_csv.o; $(BLDSHARED)  Modules/_csv.o   -o Modules/_csv$(SO)
Modules/socketmodule.o: $(srcdir)/Modules/socketmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/socketmodule.c -o Modules/socketmodule.o
Modules/_socketmodule$(SO):  Modules/socketmodule.o; $(BLDSHARED)  Modules/socketmodule.o   -o Modules/_socketmodule$(SO)
Modules/_cryptmodule.o: $(srcdir)/Modules/_cryptmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_cryptmodule.c -o Modules/_cryptmodule.o
Modules/_cryptmodule$(SO):  Modules/_cryptmodule.o; $(BLDSHARED)  Modules/_cryptmodule.o   -o Modules/_cryptmodule$(SO)
Modules/termios.o: $(srcdir)/Modules/termios.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/termios.c -o Modules/termios.o
Modules/termios$(SO):  Modules/termios.o; $(BLDSHARED)  Modules/termios.o   -o Modules/termios$(SO)
Modules/resource.o: $(srcdir)/Modules/resource.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/resource.c -o Modules/resource.o
Modules/resource$(SO):  Modules/resource.o; $(BLDSHARED)  Modules/resource.o   -o Modules/resource$(SO)
Modules/_posixsubprocess.o: $(srcdir)/Modules/_posixsubprocess.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_posixsubprocess.c -o Modules/_posixsubprocess.o
Modules/_posixsubprocess$(SO):  Modules/_posixsubprocess.o; $(BLDSHARED)  Modules/_posixsubprocess.o   -o Modules/_posixsubprocess$(SO)
Modules/md5module.o: $(srcdir)/Modules/md5module.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/md5module.c -o Modules/md5module.o
Modules/_md5module$(SO):  Modules/md5module.o; $(BLDSHARED)  Modules/md5module.o   -o Modules/_md5module$(SO)
Modules/sha1module.o: $(srcdir)/Modules/sha1module.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/sha1module.c -o Modules/sha1module.o
Modules/_sha1module$(SO):  Modules/sha1module.o; $(BLDSHARED)  Modules/sha1module.o   -o Modules/_sha1module$(SO)
Modules/sha256module.o: $(srcdir)/Modules/sha256module.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/sha256module.c -o Modules/sha256module.o
Modules/_sha256module$(SO):  Modules/sha256module.o; $(BLDSHARED)  Modules/sha256module.o   -o Modules/_sha256module$(SO)
Modules/sha512module.o: $(srcdir)/Modules/sha512module.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/sha512module.c -o Modules/sha512module.o
Modules/_sha512module$(SO):  Modules/sha512module.o; $(BLDSHARED)  Modules/sha512module.o   -o Modules/_sha512module$(SO)
Modules/syslogmodule.o: $(srcdir)/Modules/syslogmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/syslogmodule.c -o Modules/syslogmodule.o
Modules/syslogmodule$(SO):  Modules/syslogmodule.o; $(BLDSHARED)  Modules/syslogmodule.o   -o Modules/syslogmodule$(SO)
Modules/binascii.o: $(srcdir)/Modules/binascii.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/binascii.c -o Modules/binascii.o
Modules/binascii$(SO):  Modules/binascii.o; $(BLDSHARED)  Modules/binascii.o   -o Modules/binascii$(SO)
Modules/parsermodule.o: $(srcdir)/Modules/parsermodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/parsermodule.c -o Modules/parsermodule.o
Modules/parsermodule$(SO):  Modules/parsermodule.o; $(BLDSHARED)  Modules/parsermodule.o   -o Modules/parsermodule$(SO)
Modules/zlibmodule.o: $(srcdir)/Modules/zlibmodule.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/zlibmodule.c -o Modules/zlibmodule.o
Modules/zlibmodule$(SO):  Modules/zlibmodule.o; $(BLDSHARED)  Modules/zlibmodule.o  -lz  -o Modules/zlibmodule$(SO)
Modules/_json.o: $(srcdir)/Modules/_json.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_json.c -o Modules/_json.o
Modules/_json$(SO):  Modules/_json.o; $(BLDSHARED)  Modules/_json.o   -o Modules/_json$(SO)
Modules/_lsprof.o: $(srcdir)/Modules/_lsprof.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/_lsprof.c -o Modules/_lsprof.o
Modules/rotatingtree.o: $(srcdir)/Modules/rotatingtree.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/rotatingtree.c -o Modules/rotatingtree.o
Modules/_lsprof$(SO):  Modules/_lsprof.o Modules/rotatingtree.o; $(BLDSHARED)  Modules/_lsprof.o Modules/rotatingtree.o   -o Modules/_lsprof$(SO)
Modules/audioop.o: $(srcdir)/Modules/audioop.c; $(CC) $(PY_CORE_CFLAGS)  -c $(srcdir)/Modules/audioop.c -o Modules/audioop.o
Modules/audioop$(SO):  Modules/audioop.o; $(BLDSHARED)  Modules/audioop.o   -o Modules/audioop$(SO)
Modules/multiprocessing.o: $(srcdir)/Modules/_multiprocessing/multiprocessing.c; $(CC) $(PY_CORE_CFLAGS)  -IModules/_multiprocessing -c $(srcdir)/Modules/_multiprocessing/multiprocessing.c -o Modules/multiprocessing.o
Modules/semaphore.o: $(srcdir)/Modules/_multiprocessing/semaphore.c; $(CC) $(PY_CORE_CFLAGS)  -IModules/_multiprocessing -c $(srcdir)/Modules/_multiprocessing/semaphore.c -o Modules/semaphore.o
Modules/_multiprocessingmodule$(SO):  Modules/multiprocessing.o Modules/semaphore.o; $(BLDSHARED)  Modules/multiprocessing.o Modules/semaphore.o   -o Modules/_multiprocessingmodule$(SO)
Modules/_decimal.o: $(srcdir)/Modules/_decimal/_decimal.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/_decimal.c -o Modules/_decimal.o
Modules/basearith.o: $(srcdir)/Modules/_decimal/libmpdec/basearith.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/basearith.c -o Modules/basearith.o
Modules/constants.o: $(srcdir)/Modules/_decimal/libmpdec/constants.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/constants.c -o Modules/constants.o
Modules/basearith.o: $(srcdir)/Modules/_decimal/libmpdec/basearith.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/basearith.c -o Modules/basearith.o
Modules/constants.o: $(srcdir)/Modules/_decimal/libmpdec/constants.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/constants.c -o Modules/constants.o
Modules/context.o: $(srcdir)/Modules/_decimal/libmpdec/context.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/context.c -o Modules/context.o
Modules/convolute.o: $(srcdir)/Modules/_decimal/libmpdec/convolute.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/convolute.c -o Modules/convolute.o
Modules/crt.o: $(srcdir)/Modules/_decimal/libmpdec/crt.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/crt.c -o Modules/crt.o
Modules/difradix2.o: $(srcdir)/Modules/_decimal/libmpdec/difradix2.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/difradix2.c -o Modules/difradix2.o
Modules/fnt.o: $(srcdir)/Modules/_decimal/libmpdec/fnt.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/fnt.c -o Modules/fnt.o
Modules/fourstep.o: $(srcdir)/Modules/_decimal/libmpdec/fourstep.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/fourstep.c -o Modules/fourstep.o
Modules/io.o: $(srcdir)/Modules/_decimal/libmpdec/io.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/io.c -o Modules/io.o
Modules/memory.o: $(srcdir)/Modules/_decimal/libmpdec/memory.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/memory.c -o Modules/memory.o
Modules/mpdecimal.o: $(srcdir)/Modules/_decimal/libmpdec/mpdecimal.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/mpdecimal.c -o Modules/mpdecimal.o
Modules/numbertheory.o: $(srcdir)/Modules/_decimal/libmpdec/numbertheory.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/numbertheory.c -o Modules/numbertheory.o
Modules/sixstep.o: $(srcdir)/Modules/_decimal/libmpdec/sixstep.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/sixstep.c -o Modules/sixstep.o
Modules/transpose.o: $(srcdir)/Modules/_decimal/libmpdec/transpose.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_decimal/libmpdec -DCONFIG_64 -DASM -c $(srcdir)/Modules/_decimal/libmpdec/transpose.c -o Modules/transpose.o
Modules/_decimal$(SO):  Modules/_decimal.o Modules/basearith.o Modules/constants.o Modules/basearith.o Modules/constants.o Modules/context.o Modules/convolute.o Modules/crt.o Modules/difradix2.o Modules/fnt.o Modules/fourstep.o Modules/io.o Modules/memory.o Modules/mpdecimal.o Modules/numbertheory.o Modules/sixstep.o Modules/transpose.o; $(BLDSHARED)  Modules/_decimal.o Modules/basearith.o Modules/constants.o Modules/basearith.o Modules/constants.o Modules/context.o Modules/convolute.o Modules/crt.o Modules/difradix2.o Modules/fnt.o Modules/fourstep.o Modules/io.o Modules/memory.o Modules/mpdecimal.o Modules/numbertheory.o Modules/sixstep.o Modules/transpose.o   -o Modules/_decimal$(SO)
Modules/darwin64.o: $(srcdir)/Modules/_ctypes/libffi_osx/x86/darwin64.S; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/libffi_osx/x86/darwin64.S -o Modules/darwin64.o
Modules/malloc_closure.o: $(srcdir)/Modules/_ctypes/malloc_closure.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/malloc_closure.c -o Modules/malloc_closure.o
Modules/dlfcn_simple.o: $(srcdir)/Modules/_ctypes/darwin/dlfcn_simple.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/darwin/dlfcn_simple.c -o Modules/dlfcn_simple.o
Modules/_ctypes.o: $(srcdir)/Modules/_ctypes/_ctypes.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/_ctypes.c -o Modules/_ctypes.o
Modules/callbacks.o: $(srcdir)/Modules/_ctypes/callbacks.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/callbacks.c -o Modules/callbacks.o
Modules/callproc.o: $(srcdir)/Modules/_ctypes/callproc.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/callproc.c -o Modules/callproc.o
Modules/stgdict.o: $(srcdir)/Modules/_ctypes/stgdict.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/stgdict.c -o Modules/stgdict.o
Modules/cfield.o: $(srcdir)/Modules/_ctypes/cfield.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/cfield.c -o Modules/cfield.o
Modules/ffi.o: $(srcdir)/Modules/_ctypes/libffi_osx/ffi.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/libffi_osx/ffi.c -o Modules/ffi.o
Modules/x86-ffi64.o: $(srcdir)/Modules/_ctypes/libffi_osx/x86/x86-ffi64.c; $(CC) $(PY_CORE_CFLAGS)  -I$(srcdir)/Modules/_ctypes/libffi_osx/include -DMACOSX -c $(srcdir)/Modules/_ctypes/libffi_osx/x86/x86-ffi64.c -o Modules/x86-ffi64.o
Modules/_ctypes$(SO):  Modules/darwin64.o Modules/malloc_closure.o Modules/dlfcn_simple.o Modules/_ctypes.o Modules/callbacks.o Modules/callproc.o Modules/stgdict.o Modules/cfield.o Modules/ffi.o Modules/x86-ffi64.o; $(BLDSHARED)  Modules/darwin64.o Modules/malloc_closure.o Modules/dlfcn_simple.o Modules/_ctypes.o Modules/callbacks.o Modules/callproc.o Modules/stgdict.o Modules/cfield.o Modules/ffi.o Modules/x86-ffi64.o   -o Modules/_ctypes$(SO)
