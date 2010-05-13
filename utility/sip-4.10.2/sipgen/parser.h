
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_API = 258,
     TK_DEFENCODING = 259,
     TK_PLUGIN = 260,
     TK_DOCSTRING = 261,
     TK_DOC = 262,
     TK_EXPORTEDDOC = 263,
     TK_MAKEFILE = 264,
     TK_ACCESSCODE = 265,
     TK_GETCODE = 266,
     TK_SETCODE = 267,
     TK_PREINITCODE = 268,
     TK_INITCODE = 269,
     TK_POSTINITCODE = 270,
     TK_UNITCODE = 271,
     TK_MODCODE = 272,
     TK_TYPECODE = 273,
     TK_PREPYCODE = 274,
     TK_COPYING = 275,
     TK_MAPPEDTYPE = 276,
     TK_CODELINE = 277,
     TK_IF = 278,
     TK_END = 279,
     TK_NAME = 280,
     TK_PATHNAME = 281,
     TK_STRING = 282,
     TK_VIRTUALCATCHERCODE = 283,
     TK_TRAVERSECODE = 284,
     TK_CLEARCODE = 285,
     TK_GETBUFFERCODE = 286,
     TK_RELEASEBUFFERCODE = 287,
     TK_READBUFFERCODE = 288,
     TK_WRITEBUFFERCODE = 289,
     TK_SEGCOUNTCODE = 290,
     TK_CHARBUFFERCODE = 291,
     TK_PICKLECODE = 292,
     TK_METHODCODE = 293,
     TK_FROMTYPE = 294,
     TK_TOTYPE = 295,
     TK_TOSUBCLASS = 296,
     TK_INCLUDE = 297,
     TK_OPTINCLUDE = 298,
     TK_IMPORT = 299,
     TK_EXPHEADERCODE = 300,
     TK_MODHEADERCODE = 301,
     TK_TYPEHEADERCODE = 302,
     TK_MODULE = 303,
     TK_CMODULE = 304,
     TK_CONSMODULE = 305,
     TK_COMPOMODULE = 306,
     TK_CLASS = 307,
     TK_STRUCT = 308,
     TK_PUBLIC = 309,
     TK_PROTECTED = 310,
     TK_PRIVATE = 311,
     TK_SIGNALS = 312,
     TK_SLOTS = 313,
     TK_BOOL = 314,
     TK_SHORT = 315,
     TK_INT = 316,
     TK_LONG = 317,
     TK_FLOAT = 318,
     TK_DOUBLE = 319,
     TK_CHAR = 320,
     TK_WCHAR_T = 321,
     TK_VOID = 322,
     TK_PYOBJECT = 323,
     TK_PYTUPLE = 324,
     TK_PYLIST = 325,
     TK_PYDICT = 326,
     TK_PYCALLABLE = 327,
     TK_PYSLICE = 328,
     TK_PYTYPE = 329,
     TK_VIRTUAL = 330,
     TK_ENUM = 331,
     TK_SIGNED = 332,
     TK_UNSIGNED = 333,
     TK_SCOPE = 334,
     TK_LOGICAL_OR = 335,
     TK_CONST = 336,
     TK_STATIC = 337,
     TK_SIPSIGNAL = 338,
     TK_SIPSLOT = 339,
     TK_SIPANYSLOT = 340,
     TK_SIPRXCON = 341,
     TK_SIPRXDIS = 342,
     TK_SIPSLOTCON = 343,
     TK_SIPSLOTDIS = 344,
     TK_NUMBER = 345,
     TK_REAL = 346,
     TK_TYPEDEF = 347,
     TK_NAMESPACE = 348,
     TK_TIMELINE = 349,
     TK_PLATFORMS = 350,
     TK_FEATURE = 351,
     TK_LICENSE = 352,
     TK_QCHAR = 353,
     TK_TRUE = 354,
     TK_FALSE = 355,
     TK_NULL = 356,
     TK_OPERATOR = 357,
     TK_THROW = 358,
     TK_QOBJECT = 359,
     TK_EXCEPTION = 360,
     TK_RAISECODE = 361,
     TK_EXPLICIT = 362,
     TK_TEMPLATE = 363,
     TK_ELLIPSIS = 364,
     TK_DEFMETATYPE = 365,
     TK_DEFSUPERTYPE = 366
   };
#endif
/* Tokens.  */
#define TK_API 258
#define TK_DEFENCODING 259
#define TK_PLUGIN 260
#define TK_DOCSTRING 261
#define TK_DOC 262
#define TK_EXPORTEDDOC 263
#define TK_MAKEFILE 264
#define TK_ACCESSCODE 265
#define TK_GETCODE 266
#define TK_SETCODE 267
#define TK_PREINITCODE 268
#define TK_INITCODE 269
#define TK_POSTINITCODE 270
#define TK_UNITCODE 271
#define TK_MODCODE 272
#define TK_TYPECODE 273
#define TK_PREPYCODE 274
#define TK_COPYING 275
#define TK_MAPPEDTYPE 276
#define TK_CODELINE 277
#define TK_IF 278
#define TK_END 279
#define TK_NAME 280
#define TK_PATHNAME 281
#define TK_STRING 282
#define TK_VIRTUALCATCHERCODE 283
#define TK_TRAVERSECODE 284
#define TK_CLEARCODE 285
#define TK_GETBUFFERCODE 286
#define TK_RELEASEBUFFERCODE 287
#define TK_READBUFFERCODE 288
#define TK_WRITEBUFFERCODE 289
#define TK_SEGCOUNTCODE 290
#define TK_CHARBUFFERCODE 291
#define TK_PICKLECODE 292
#define TK_METHODCODE 293
#define TK_FROMTYPE 294
#define TK_TOTYPE 295
#define TK_TOSUBCLASS 296
#define TK_INCLUDE 297
#define TK_OPTINCLUDE 298
#define TK_IMPORT 299
#define TK_EXPHEADERCODE 300
#define TK_MODHEADERCODE 301
#define TK_TYPEHEADERCODE 302
#define TK_MODULE 303
#define TK_CMODULE 304
#define TK_CONSMODULE 305
#define TK_COMPOMODULE 306
#define TK_CLASS 307
#define TK_STRUCT 308
#define TK_PUBLIC 309
#define TK_PROTECTED 310
#define TK_PRIVATE 311
#define TK_SIGNALS 312
#define TK_SLOTS 313
#define TK_BOOL 314
#define TK_SHORT 315
#define TK_INT 316
#define TK_LONG 317
#define TK_FLOAT 318
#define TK_DOUBLE 319
#define TK_CHAR 320
#define TK_WCHAR_T 321
#define TK_VOID 322
#define TK_PYOBJECT 323
#define TK_PYTUPLE 324
#define TK_PYLIST 325
#define TK_PYDICT 326
#define TK_PYCALLABLE 327
#define TK_PYSLICE 328
#define TK_PYTYPE 329
#define TK_VIRTUAL 330
#define TK_ENUM 331
#define TK_SIGNED 332
#define TK_UNSIGNED 333
#define TK_SCOPE 334
#define TK_LOGICAL_OR 335
#define TK_CONST 336
#define TK_STATIC 337
#define TK_SIPSIGNAL 338
#define TK_SIPSLOT 339
#define TK_SIPANYSLOT 340
#define TK_SIPRXCON 341
#define TK_SIPRXDIS 342
#define TK_SIPSLOTCON 343
#define TK_SIPSLOTDIS 344
#define TK_NUMBER 345
#define TK_REAL 346
#define TK_TYPEDEF 347
#define TK_NAMESPACE 348
#define TK_TIMELINE 349
#define TK_PLATFORMS 350
#define TK_FEATURE 351
#define TK_LICENSE 352
#define TK_QCHAR 353
#define TK_TRUE 354
#define TK_FALSE 355
#define TK_NULL 356
#define TK_OPERATOR 357
#define TK_THROW 358
#define TK_QOBJECT 359
#define TK_EXCEPTION 360
#define TK_RAISECODE 361
#define TK_EXPLICIT 362
#define TK_TEMPLATE 363
#define TK_ELLIPSIS 364
#define TK_DEFMETATYPE 365
#define TK_DEFSUPERTYPE 366




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 145 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"

    char            qchar;
    char            *text;
    long            number;
    double          real;
    argDef          memArg;
    signatureDef    signature;
    signatureDef    *optsignature;
    throwArgs       *throwlist;
    codeBlock       *codeb;
    valueDef        value;
    valueDef        *valp;
    optFlags        optflags;
    optFlag         flag;
    scopedNameDef   *scpvalp;
    fcallDef        fcall;
    int             boolean;
    exceptionDef    exceptionbase;
    classDef        *klass;



/* Line 1676 of yacc.c  */
#line 297 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


