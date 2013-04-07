# -----------------------------------------------------------------------------
# cpp.py
#
# Author:  David Beazley (http://www.dabeaz.com)
# Copyright (C) 2007
# All rights reserved
#
# This module implements an ANSI-C style lexical preprocessor for PLY.
# -----------------------------------------------------------------------------

"""Usage Notes: This module is used for simple parsing and extraction of C files features
for use within the overall RTOS build and deployment system.

This module is unlikely to reproduce the exact same pre-processing as
the compiler's pre-processor, so should only be used in cases where
this is not relevant to the outcome.

"""


import re
import copy
import time
import os.path
import ply.lex as lex
from ply.lex import token

literals = "+-*/%|&~^<>=!?()[]{}.,;:\\\'\""

# The freestanding headers are provided as builtins:
#
# <float.h> <iso646.h> <limits.h> <stdarg.h> <stdbool.h> <stddef.h> <stdint.h>.

builtins = {
    'float.h': """#ifndef _FLOAT_H
#define _FLOAT_H

#define FLT_ROUNDS -1
#define FLT_EVAL_METHOD -1

/* A minimal implementation */
#define FLT_RADIX 16
#define FLT_MANT_DIG 6
#define FLT_EPSILON 9.53674316E-07F
#define FLT_DIG 6
#define FLT_MIN_EXP -31
#define FLT_MIN 2.93873588E-39F
#define FLT_MIN_10_EXP -38
#define FLT_MAX_EXP +32
#define FLT_MAX 3.40282347E+38F
#define FLT_MAX_10_EXP +38

#endif /* _FLOAT_H */
""",
    'iso646.h': """#ifndef _ISO646_H

#define and &&
#define and_eq &=
#define bitand &
#define bitor |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^
#define xor_eq ^=

#endif
""",
    'limits.h': """#ifndef _LIMITS_H

#define CHAR_BIT 8
#define SCHAR_MIN -127
#define SCHAR_MAX +127
#define UCHAR_MAX 255

#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define MB_LEN_MAX 1

#define SHRT_MIN -32767
#define SHRT_MAX +32767
#define USHRT_MAX 65535

#define INT_MIN -32767
#define INT_MAX +32767
#define UINT_MAX 65535

#define LONG_MIN -2147483647
#define LONG_MAX +2147483647
#define ULONG_MAX 4294967295

#define LLONG_MIN -9223372036854775807
#define LLONG_MAX +9223372036854775807
#define ULLONG_MAX 18446744073709551615

#endif
""",
    'stdint.h': """#ifndef _STDINT_H

typedef int8_t char
typedef int16_t short
typedef int32_t int
typedef int64_t long

typedef uint8_t unsigned char
typedef uint16_t unsigned short
typedef uint32_t unsigned int
typedef uint64_t unsigned long

typedef int_least8_t char
typedef int_least16_t short
typedef int_least32_t int
typedef int_least64_t long

typedef uint_least8_t unsigned char
typedef uint_least16_t unsigned short
typedef uint_least32_t unsigned int
typedef uint_least64_t unsigned long

typedef int_fast8_t char
typedef int_fast16_t short
typedef int_fast32_t int
typedef int_fast64_t long

typedef uint_fast8_t unsigned char
typedef uint_fast16_t unsigned short
typedef uint_fast32_t unsigned int
typedef uint_fast64_t unsigned long

typedef intptr_t long
typedef uintptr_t unsigned long

typedef intmax_t long long
typedef uintmax_t unsigned long long

#define INT8_MIN -128
#define INT8_MAX 127
#define UINT8_MAX 256

#define INT_LEAST8_MIN -128
#define INT_LEAST8_MAX 127
#define UINT_LEAST8_MAX 256

#define INT_FAST8_MIN -128
#define INT_FAST8_MAX 127
#define UINT_FAST8_MAX 256

#define INT16_MIN -32768
#define INT16_MAX 32767
#define UINT16_MAX 65535

#define INT_LEAST16_MIN -32768
#define INT_LEAST16_MAX 32767
#define UINT_LEAST16_MAX 65535

#define INT_FAST16_MIN -32768
#define INT_FAST16_MAX 32767
#define UINT_FAST16_MAX 65535

#define INT32_MIN -2147483648
#define INT32_MAX 2147483647
#define UINT32_MAX 4294967295

#define INT_LEAST32_MIN -2147483648
#define INT_LEAST32_MAX 2147483647
#define UINT_LEAST32_MAX 4294967295

#define INT_FAST32_MIN -2147483648
#define INT_FAST32_MAX 2147483647
#define UINT_FAST32_MAX 4294967295

#define INT64_MIN -9223372036854775808LL
#define INT64_MAX 9223372036854775807LL
#define UINT64_MAX 18446744073709551615LL

#define INTPTR_MIN INT16_MIN
#define INTPTR_MAX INT16_MAX
#define UINTPTR_MAX UINT16_MAX

#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#define PTRDIFF_MIN −65535
#define PTRDIFF_MAX +65535

#define SIZE_MAX 65535

#define WCHAR_MIN 0
#define WCHAR_MAX 255

#define WINT_MIN 0
#define WINT_MAX UINT16_MAX

#define INTMAX_C(value) value
#define UINTMAX_C(value) value

#endif
""",
    'stdarg.h': """#ifndef _STDARG_H

typedef va_list unsigned int
#define va_arg(ap, type) (type) 0
#define va_copy(dest, src) dest = src
#define va_end(ap) ap = 0
#define va_start(ap, parmN) ap = parmN

#endif
""",
    'stdbool.h': """#ifndef _STDBOOL_H

#define bool _Bool
#define true 1
#define false 0
#define _bool_true_false_are_defined 1

#endif
""",
    'stddef.h': """#ifndef _STDDEF_H

#define NULL 0
#define offsetof(t, m)  ((size_t)(&((t *)0)->m))

typedef ptrdiff_t long;
typedef size_t unsigned;
typedef wchar_t unsigned;

#endif
""",

}

class CppError(Exception):
    pass

class CppLexer:
    tokens = (
        'CPP_ID',
        'CPP_INTEGER',
        'CPP_FLOAT',
        'CPP_STRING',
        'CPP_CHAR',
        'CPP_WS',
        'CPP_COMMENT1',
        'CPP_COMMENT2',
        'CPP_POUND',
        'CPP_DPOUND'
    )

    @token(r'\s+')
    def t_CPP_WS(self, t):
        # Whitespace
        t.lexer.lineno += t.value.count("\n")
        return t

    t_CPP_POUND = r'\#'
    t_CPP_DPOUND = r'\#\#'
    t_CPP_ID = r'[A-Za-z_][\w_]*'  # Identifier

    @token(r'(((((0x)|(0X))[0-9a-fA-F]+)|(\d+))([uU]|[lL]|[uU][lL]|[lL][uU])?)')
    def CPP_INTEGER(self, t):
        # Integer literal
        return t

    t_CPP_INTEGER = CPP_INTEGER

    # Floating literal
    t_CPP_FLOAT = r'((\d+)(\.\d+)(e(\+|-)?(\d+))? | (\d+)e(\+|-)?(\d+))([lL]|[fF])?'

    @token(r'\"([^\\\n]|(\\(.|\n)))*?\"')
    def t_CPP_STRING(self, t):
        # String literal
        t.lexer.lineno += t.value.count("\n")
        return t

    @token(r'(L)?\'([^\\\n]|(\\(.|\n)))*?\'')
    def t_CPP_CHAR(self, t):
        # Character constant 'c' or L'c'
        t.lexer.lineno += t.value.count("\n")
        return t

    @token(r'(/\*(.|\n)*?\*/)')
    def t_CPP_COMMENT1(self, t):
        # Comment
        ncr = t.value.count("\n")
        t.lexer.lineno += ncr
        # replace with one space or a number of '\n'
        t.type = 'CPP_WS'; t.value = '\n' * ncr if ncr else ' '
        return t

    @token(r'(//.*?(\n|$))')
    def t_CPP_COMMENT2(self, t):
        # Line comment
        # replace with '/n'
        t.type = 'CPP_WS'; t.value = '\n'

    def t_error(self, t):
        t.type = t.value[0]
        t.value = t.value[0]
        t.lexer.skip(1)
        return t

# -----------------------------------------------------------------------------
# trigraph()
#
# Given an input string, this function replaces all trigraph sequences.
# The following mapping is used:
#
#     ??=    #
#     ??/    \
#     ??'    ^
#     ??(    [
#     ??)    ]
#     ??!    |
#     ??<    {
#     ??>    }
#     ??-    ~
# -----------------------------------------------------------------------------

_trigraph_pat = re.compile(r'''\?\?[=/\'\(\)\!<>\-]''')
_trigraph_rep = {
    '=': '#',
    '/': '\\',
    "'": '^',
    '(': '[',
    ')': ']',
    '!': '|',
    '<': '{',
    '>': '}',
    '-': '~'
}


def trigraph(input):
    return _trigraph_pat.sub(lambda g: _trigraph_rep[g.group()[-1]],input)


class Macro(object):
    # ------------------------------------------------------------------
    # Macro object
    #
    # This object holds information about preprocessor macros
    #
    #    .name      - Macro name (string)
    #    .value     - Macro value (a list of tokens)
    #    .arglist   - List of argument names
    #    .variadic  - Boolean indicating whether or not variadic macro
    #    .vararg    - Name of the variadic parameter
    #
    # When a macro is created, the macro replacement token sequence is
    # pre-scanned and used to create patch lists that are later used
    # during macro expansion
    # ------------------------------------------------------------------
    def __init__(self, name, value, arglist=None, variadic=False):
        self.name = name
        self.value = value
        self.arglist = arglist
        self.variadic = variadic
        if variadic:
            self.vararg = arglist[-1]
        self.source = None


class Preprocessor(object):
    def __init__(self, *, include_paths=None, macro_callback=None, token_callback=None, ignore=None):
        self.lexer = lex.lex(CppLexer())
        self._lexprobe()

        self.path = [] if include_paths is None else include_paths
        self.macro_callback = macro_callback
        self.token_callback = token_callback
        self.ignore = {} if ignore is None else ignore
        self.strict = True

    def parse(self, data, source=None):
        """Parse input text."""
        self.temp_path = []
        self.macros = {}
        tm = time.localtime()
        self._define('__DATE__ "{}"'.format(time.strftime("%b %d %Y",tm)), 0)
        self._define('__TIME__ "{}"'.format(time.strftime("%H:%M:%S",tm)), 0)

        parser = self._parsegen(data, source)
        for tok in parser:
            if self.token_callback and tok.type not in self.ignore:
                self.token_callback(tok)

    def _parsegen(self, data, source=None):
        """Parse an input string"""
        if source is None:
            source = ""

        self.source = source
        self._define('__FILE__ "{}"'.format(source), 0)
        chunk = []
        enable = True
        iftrigger = False
        ifstack = []

        for x in self._group_lines(trigraph(data)):
            for i, tok in enumerate(x):
                if tok.type not in self.t_WS:
                    break

            if tok.value == '#':
                # Preprocessor directive

                # insert necessary whitespace instead of eaten tokens
                for tok in x:
                    if tok.type in self.t_WS and '\n' in tok.value:
                        chunk.append(tok)

                dirtokens = self._tokenstrip(x[i + 1:])
                if len(dirtokens) == 0:
                    continue

                lineno, name, args = dirtokens[0].lineno, dirtokens[0].value, self._tokenstrip(dirtokens[1:])

                if name == 'define':
                    if enable:
                        yield from self._expand_macros(chunk)
                        chunk = []
                        self._define(args, lineno)

                elif name == 'include':
                    if enable:
                        yield from self._expand_macros(chunk)
                        chunk = []

                        oldfile = self.macros['__FILE__']
                        yield from self._include(args, lineno)
                        self.macros['__FILE__'] = oldfile
                        self.source = source

                elif name == 'include_next':
                    if enable:
                        self._error(lineno, "#include_next directive not supported")

                elif name == 'undef':
                    if enable:
                        yield from self._expand_macros(chunk)
                        chunk = []
                        self._undef(args, lineno)

                elif name == 'ifdef':
                    ifstack.append((enable, iftrigger))
                    if enable:
                        self._check_single_arg(name, args, lineno)
                        if not args[0].value in self.macros:
                            enable = False
                            iftrigger = False
                        else:
                            iftrigger = True

                elif name == 'ifndef':
                    ifstack.append((enable, iftrigger))
                    if enable:
                        self._check_single_arg(name, args, lineno)
                        if args[0].value in self.macros:
                            enable = False
                            iftrigger = False
                        else:
                            iftrigger = True

                elif name == 'if':
                    ifstack.append((enable, iftrigger))
                    if enable:
                        result = self._eval_expr(args, lineno)
                        if not result:
                            enable = False
                            iftrigger = False
                        else:
                            iftrigger = True

                elif name == 'elif':
                    if ifstack:
                        if ifstack[-1][0]: # We only pay attention if outer "if" allows this
                            if enable:  # If already true, we flip enable False
                                enable = False
                            elif not iftrigger:  # If False, but not triggered yet, we'll check expression
                                result = self._eval_expr(args, lineno)
                                if result:
                                    enable  = True
                                    iftrigger = True
                    else:
                        self._error(lineno, "Misplaced #elif")

                elif name == 'else':
                    if ifstack:
                        if ifstack[-1][0]:
                            if enable:
                                enable = False
                            elif not iftrigger:
                                enable = True
                                iftrigger = True
                    else:
                        self._error(lineno, "Misplaced #else")

                elif name == 'endif':
                    if ifstack:
                        enable, iftrigger = ifstack.pop()
                    else:
                        self._error(lineno, "Misplaced #endif")

                elif name == 'error':
                    if enable:
                        self._error(lineno, "Error directive")

                else:
                    # Unknown preprocessor directive
                    self._error(lineno, "Unknown directive #{}".format(name))

            else:
                # Normal text
                if enable:
                    chunk.extend(x)

        yield from self._expand_macros(chunk)


    def _tokenize(self, text):
        """Utility function. Given a string of text, tokenize into a list of tokens."""
        tokens = []
        self.lexer.input(text)
        while True:
            tok = self.lexer.token()
            if not tok:
                break
            tokens.append(tok)
        return tokens

    def _error(self, line, msg):
        # Report a preprocessor error/warning of some kind
        raise CppError("{}:{} error: {}".format(self.source, line, msg))

    def _warning(self, line, msg):
        # Report a preprocessor error/warning of some kind
        if self.strict:
            self._error(line, msg)
        else:
            print("{}:{} warning: {}".format(self.source, line, msg))

    def _lexprobe(self):
        """This method probes the preprocessor lexer object to discover
        the token types of symbols that are important to the preprocessor.

        If this works right, the preprocessor will simply "work"
        with any suitable lexer regardless of how tokens have been named.
        """
        # Determine the token type for identifiers
        self.lexer.input("identifier")
        tok = self.lexer.token()
        if not tok or tok.value != "identifier":
            raise Exception("Couldn't determine identifier type")
        else:
            self.t_ID = tok.type

        # Determine the token type for integers
        self.lexer.input("12345")
        tok = self.lexer.token()
        if not tok or int(tok.value) != 12345:
            raise Exception("Couldn't determine integer type")
        else:
            self.t_INTEGER = tok.type
            self.t_INTEGER_TYPE = type(tok.value)

        # Determine the token type for strings enclosed in double quotes
        self.lexer.input("\"filename\"")
        tok = self.lexer.token()
        if not tok or tok.value != "\"filename\"":
            raise Exception("Couldn't determine string type")
        else:
            self.t_STRING = tok.type

        # Determine the token type for whitespace--if any
        self.lexer.input("  ")
        tok = self.lexer.token()
        if not tok or tok.value != "  ":
            self.t_SPACE = None
        else:
            self.t_SPACE = tok.type

        # Determine the token type for newlines
        self.lexer.input("\n")
        tok = self.lexer.token()
        if not tok or tok.value != "\n":
            self.t_NEWLINE = None
            raise Exception("Couldn't determine token for newlines")
        else:
            self.t_NEWLINE = tok.type

        self.t_WS = (self.t_SPACE, self.t_NEWLINE)

        # Check for other characters used by the preprocessor
        chars = [ '<','>','#','##','\\','(',')',',','.']
        for c in chars:
            self.lexer.input(c)
            tok = self.lexer.token()
            if not tok or tok.value != c:
                raise Exception("Unable to lex '{}' required for preprocessor".format(c))

    def _group_lines(self, data):
        """Given an input string, this function splits it into lines.  Trailing whitespace
        is removed.   Any line ending with \ is grouped with the next line.  This
        function forms the lowest level of the preprocessor---grouping into text into
        a line-by-line format.

        """
        lex = self.lexer.clone()
        lines = [x.rstrip() for x in data.splitlines()]
        for i in range(len(lines)):
            j = i + 1
            while lines[i].endswith('\\') and (j < len(lines)):
                lines[i] = lines[i][:-1]+lines[j]
                lines[j] = ""
                j += 1

        data = "\n".join(lines)
        lex.input(data)
        lex.lineno = 1

        current_line = []
        while True:
            tok = lex.token()
            if not tok:
                break
            current_line.append(tok)
            if tok.type in self.t_WS and '\n' in tok.value:
                yield current_line
                current_line = []

        if current_line:
            yield current_line

    def _tokenstrip(self, tokens):
        """Remove leading/trailing whitespace tokens from a token list."""
        # Remove leading whitespace
        i = 0
        while i < len(tokens) and tokens[i].type in self.t_WS:
            i += 1
        del tokens[:i]
        # Remove trailing whitespace
        i = len(tokens) - 1
        while i >= 0 and tokens[i].type in self.t_WS:
            i -= 1
        del tokens[i + 1:]

        return tokens

    def _collect_args(self, tokens):
        """Collects comma separated arguments from a list of tokens.

        The arguments must be enclosed in parenthesis.

        Returns a tuple (tokencount, args, positions) where
          tokencount is the number of tokens consumed,
          args is a list of arguments,
          and positions is a list of integers containing the starting index of each argument.
        Each argument is represented by a list of tokens.

        When collecting arguments, leading and trailing whitespace is removed from each argument.

        This function properly handles nested parenthesis and commas---these do not define new arguments.

        """
        args = []
        positions = []
        current_arg = []
        nesting = 1
        tokenlen = len(tokens)

        # Search for the opening '('.
        i = 0
        while (i < tokenlen) and (tokens[i].type in self.t_WS):
            i += 1

        if (i < tokenlen) and (tokens[i].value == '('):
            positions.append(i+1)
        else:
            self._error(tokens[0].lineno, "Missing '(' in macro arguments")
            return 0, [], []

        i += 1

        while i < tokenlen:
            t = tokens[i]
            if t.value == '(':
                current_arg.append(t)
                nesting += 1
            elif t.value == ')':
                nesting -= 1
                if nesting == 0:
                    if current_arg:
                        args.append(self._tokenstrip(current_arg))
                        positions.append(i)
                    return i + 1, args, positions
                current_arg.append(t)
            elif t.value == ',' and nesting == 1:
                args.append(self._tokenstrip(current_arg))
                positions.append(i+1)
                current_arg = []
            else:
                current_arg.append(t)
            i += 1

        # Missing end argument
        self._error(tokens[-1].lineno, "Missing ')' in macro arguments")
        return 0, [], []

    def _macro_prescan(self, macro):
        """Examine the macro value (token sequence) and identify patch points.

        This is used to speed up macro expansion later on---we'll know
        right away where to apply patches to the value to form the expansion

        """
        macro.patch     = []             # Standard macro arguments
        macro.str_patch = []             # String conversion expansion
        macro.var_comma_patch = []       # Variadic macro comma patch

        i = 0
        while i < len(macro.value):
            if macro.value[i].type == self.t_ID and macro.value[i].value in macro.arglist:
                argnum = macro.arglist.index(macro.value[i].value)
                # Conversion of argument to a string
                if i > 0 and macro.value[i-1].value == '#':
                    macro.value[i] = copy.copy(macro.value[i])
                    macro.value[i].type = self.t_STRING
                    del macro.value[i-1]
                    macro.str_patch.append((argnum, i - 1))
                    continue
                # Concatenation
                elif (i > 0 and macro.value[i-1].value == '##'):
                    macro.patch.append(('c', argnum, i - 1))
                    del macro.value[i-1]
                    continue
                elif ((i+1) < len(macro.value) and macro.value[i+1].value == '##'):
                    macro.patch.append(('c', argnum, i))
                    i += 1
                    continue
                # Standard expansion
                else:
                    macro.patch.append(('e', argnum, i))
            elif macro.value[i].value == '##':
                if macro.variadic and (i > 0) and (macro.value[i - 1].value == ',') and \
                        ((i + 1) < len(macro.value)) and (macro.value[i + 1].type == self.t_ID) and \
                        (macro.value[i + 1].value == macro.vararg):
                    macro.var_comma_patch.append(i - 1)
            i += 1
        macro.patch.sort(key=lambda x: x[2], reverse=True)

    def _macro_expand_args(self,macro,args):
        """Given a Macro and list of arguments (each a token list).

        This method returns an expanded version of a macro.
        The return value is a token sequence representing the replacement macro tokens

        """
        # Make a copy of the macro token sequence
        rep = [copy.copy(_x) for _x in macro.value]

        # Make string expansion patches.  These do not alter the length of the replacement sequence
        str_expansion = {}
        for argnum, i in macro.str_patch:
            if argnum not in str_expansion:
                str_expansion[argnum] = '"{}"'.format("".join([x.value for x in args[argnum]])).replace("\\","\\\\")
            rep[i] = copy.copy(rep[i])
            rep[i].value = str_expansion[argnum]

        # Make the variadic macro comma patch.  If the variadic macro argument is empty, we get rid
        comma_patch = False
        if macro.variadic and not args[-1]:
            for i in macro.var_comma_patch:
                rep[i] = None
                comma_patch = True

        # Make all other patches.   The order of these matters.  It is assumed that the patch list
        # has been sorted in reverse order of patch location since replacements will cause the
        # size of the replacement sequence to expand from the patch point.

        expanded = {}
        for ptype, argnum, i in macro.patch:
            # Concatenation. Argument is left unexpanded
            if ptype == 'c':
                rep[i:i+1] = args[argnum]
            # Normal expansion. Argument is macro expanded first
            elif ptype == 'e':
                if argnum not in expanded:
                    expanded[argnum] = self._expand_macros(args[argnum])
                rep[i:i+1] = expanded[argnum]

        # Get rid of removed comma if necessary
        if comma_patch:
            rep = [_i for _i in rep if _i]

        if self.macro_callback:
            self.macro_callback(macro.name, [expanded[i] for i in range(len(expanded))])
        return rep

    def _expand_macros(self, tokens, expanded=None):
        """Given a list of tokens, this function performs macro expansion.
        The expanded argument is a dictionary that contains macros already
        expanded.  This is used to prevent infinite recursion.

        """
        if expanded is None:
            expanded = {}
        i = 0
        while i < len(tokens):
            t = tokens[i]
            if t.type == self.t_ID:
                if t.value in self.macros and t.value not in expanded:
                    # Yes, we found a macro match
                    expanded[t.value] = True

                    m = self.macros[t.value]
                    if not m.arglist:
                        # A simple macro
                        ex = self._expand_macros([copy.copy(_x) for _x in m.value], expanded)
                        for e in ex:
                            e.lineno = t.lineno
                        tokens[i:i+1] = ex
                        i += len(ex)
                    else:
                        # A macro with arguments
                        j = i + 1
                        while j < len(tokens) and tokens[j].type in self.t_WS:
                            j += 1
                        if tokens[j].value == '(':
                            tokcount,args,positions = self._collect_args(tokens[j:])
                            if not m.variadic and len(args) !=  len(m.arglist):
                                self._error(t.lineno, "Macro {} requires {} arguments".format(t.value, len(m.arglist)))
                                i = j + tokcount
                            elif m.variadic and len(args) < len(m.arglist)-1:
                                if len(m.arglist) > 2:
                                    self._error(t.lineno, "Macro {} must have at least {} arguments".format(t.value, len(m.arglist)-1))
                                else:
                                    self._error(t.lineno, "Macro {} must have at least {} argument".format(t.value, len(m.arglist)-1))
                                i = j + tokcount
                            else:
                                if m.variadic:
                                    if len(args) == len(m.arglist)-1:
                                        args.append([])
                                    else:
                                        args[len(m.arglist)-1] = tokens[j+positions[len(m.arglist)-1]:j+tokcount-1]
                                        del args[len(m.arglist):]

                                # Get macro replacement text
                                rep = self._macro_expand_args(m, args)
                                rep = self._expand_macros(rep, expanded)
                                for r in rep:
                                    r.lineno = t.lineno
                                tokens[i:j+tokcount] = rep
                                i += len(rep)
                    del expanded[t.value]
                    continue
                elif t.value == '__LINE__':
                    t.type = self.t_INTEGER
                    t.value = self.t_INTEGER_TYPE(t.lineno)

            i += 1
        return tokens

    def _eval_expr(self, args, lineno):
        """Evaluate an expression token sequence for the purposes of evaluating integral expressions."""
        # Search for defined macros
        i = 0
        while i < len(args):
            if args[i].type == self.t_ID and args[i].value == 'defined':
                j = i + 1
                needparen = False
                result = "0L"
                while j < len(args):
                    if args[j].type in self.t_WS:
                        j += 1
                        continue
                    elif args[j].type == self.t_ID:
                        if args[j].value in self.macros:
                            result = "1L"
                        else:
                            result = "0L"
                        if not needparen: break
                    elif args[j].value == '(':
                        needparen = True
                    elif args[j].value == ')':
                        break
                    else:
                        self._error(lineno, "Malformed defined()")
                    j += 1
                args[i].type = self.t_INTEGER
                args[i].value = self.t_INTEGER_TYPE(result)
                del args[i+1:j+1]
            i += 1
        args = self._expand_macros(args)
        for i, t in enumerate(args):
            if t.type == self.t_ID:
                args[i] = copy.copy(t)
                args[i].type = self.t_INTEGER
                args[i].value = self.t_INTEGER_TYPE("0L")
            elif t.type == self.t_INTEGER:
                args[i] = copy.copy(t)
                # Strip off any trailing suffixes
                args[i].value = str(args[i].value)
                while args[i].value[-1] not in "0123456789abcdefABCDEF":
                    args[i].value = args[i].value[:-1]

        expr = "".join([str(x.value) for x in args])
        c_expr = expr
        expr = expr.replace("&&", " and ")
        expr = expr.replace("||", " or ")
        expr = expr.replace("!", " not ")
        expr = expr.replace("L", "")
        expr = expr.replace("U", "")
        py_expr = expr
        try:
            result = eval(py_expr, {}, {})
        except Exception:
            self._error(lineno, "Couldn't evaluate expression: '{}'".format(c_expr))
            result = 0
        return result

    def _include(self, args, lineno):
        """Implementation of file-inclusion"""
        # Try to extract the filename and then process an include file
        if len(args) == 0:
            self._error(lineno, '#include expects "FILENAME" or <FILENAME>')

        if args[0].value != '<' and args[0].type != self.t_STRING:
            args = self._expand_macros(args)

        if args[0].value == '<':
            # Include <...>
            i = 1
            while i < len(args):
                if args[i].value == '>':
                    break
                i += 1
            else:
                self._error(args[0].lineno, "Malformed #include <...>")
                return
            filename = "".join([x.value for x in args[1:i]])
            path = self.path + [""] + self.temp_path
        elif args[0].type == self.t_STRING:
            filename = args[0].value[1:-1]
            path = self.temp_path + [""] + self.path
        else:
            self._error(lineno, "Malformed #include statement")
            return

        if filename in builtins:
            data = builtins[filename]
            yield from self._parsegen(data, filename)
        else:
            for p in path:
                iname = os.path.join(p,filename)
                try:
                    data = open(iname, "r").read()
                    dname = os.path.dirname(iname)
                    if dname:
                        self.temp_path.insert(0,dname)
                    yield from self._parsegen(data, filename)
                    if dname:
                        del self.temp_path[0]
                    break
                except IOError:
                    pass
            else:
                self._error(lineno, "Couldn't find '{}'".format(filename))

    def _define(self, args, lineno):
        """Define a new macro"""
        if isinstance(args, str):
            args = self._tokenize(args)

        try:
            name = args[0]
            mtype = args[1] if len(args) > 1 else None

            if not mtype:
                m = Macro(name.value,[])
                self.macros[name.value] = m

            elif mtype.type in self.t_WS:
                # A normal macro
                m = Macro(name.value,self._tokenstrip(args[2:]))
                self.macros[name.value] = m

            elif mtype.value == '(':
                # A macro with arguments
                tokcount, macro_args, positions = self._collect_args(args[1:])
                variadic = False
                for a in macro_args:
                    if variadic:
                        self._error(lineno, "No more arguments may follow a variadic argument")
                        break
                    astr = "".join([str(_i.value) for _i in a])
                    if astr == "...":
                        variadic = True
                        a[0].type = self.t_ID
                        a[0].value = '__VA_ARGS__'
                        variadic = True
                        del a[1:]
                        continue
                    elif astr[-3:] == "..." and a[0].type == self.t_ID:
                        variadic = True
                        del a[1:]
                        # If, for some reason, "." is part of the identifier, strip off the name for the purposes
                        # of macro expansion
                        if a[0].value[-3:] == '...':
                            a[0].value = a[0].value[:-3]
                        continue
                    if len(a) > 1 or a[0].type != self.t_ID:
                        self._error(lineno, "Invalid macro argument")
                        break
                else:
                    mvalue = self._tokenstrip(args[1+tokcount:])
                    i = 0
                    while i < len(mvalue):
                        if i+1 < len(mvalue):
                            if mvalue[i].type in self.t_WS and mvalue[i+1].value == '##':
                                del mvalue[i]
                                continue
                            elif mvalue[i].value == '##' and mvalue[i+1].type in self.t_WS:
                                del mvalue[i+1]
                        i += 1
                    m = Macro(name.value, mvalue, [x[0].value for x in macro_args], variadic)
                    self._macro_prescan(m)
                    self.macros[name.value] = m
            else:
                self._error(lineno, "Bad macro definition")
        except LookupError:
            self._error(lineno, "Bad macro definition")

    def _undef(self, args, lineno):
        """Undefine a macro.

        If the macro doesn't exist this is a no-op.
        If there are no args, then an error is raised.
        If there is more than one arg, a warning is raised.

        """
        self._check_single_arg('undef', args, lineno)
        try:
            del self.macros[args[0].value]
        except LookupError:
            pass

    def _check_single_arg(self, name, args, lineno):
        if len(args) == 0:
            self._error(lineno, "no macro name given in #{} directive".format(name))
        if len(args) > 1:
            self._warning(lineno, "extra tokens at end of #{} directive".format(name))
