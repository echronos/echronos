/* This file is part of the CodeSourcery C Library (CSLIBC).

   Copyright (c) 2010 CodeSourcery, Inc.

   THIS FILE CONTAINS PROPRIETARY, CONFIDENTIAL, AND TRADE SECRET
   INFORMATION OF CODESOURCERY AND/OR ITS LICENSORS.

   You may not use, modify or distribute this file without the express
   written permission of CodeSourcery or its authorized
   distributor. Please consult your license agreement for the
   applicable terms and conditions.  */

#ifndef _POWERPC_ASM_H_
#define _POWERPC_ASM_H_

#include <ppc-asm.h>

#define L(name) .L##name

#define ENTRY(name) FUNC_START(name)

#define EALIGN(name, alignp2, words)            \
  .align alignp2;                               \
  FUNC_START(name)

#define END(name) FUNC_END(name)

/* ppc-asm.h uses mnemonic names for these.  */
#define r1 1
#define r2 2

#ifdef __ASSEMBLER__
#define C_SYMBOL_NAME(name) name
#define ASM_LINE_SEP ;
#define weak_alias(original,alias) .weak C_SYMBOL_NAME (alias) ASM_LINE_SEP C_SYMBOL_NAME (alias) = C_SYMBOL_NAME (original)
#else
#define weak_alias(name,aliasname) _weak_alias (name, aliasname)
#define _weak_alias(name,aliasname) extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));
#endif

#endif
