
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 19 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sip.h"


#define MAX_NESTED_IF       10
#define MAX_NESTED_SCOPE    10

#define inMainModule()      (currentSpec->module == currentModule || currentModule->container != NULL)


static sipSpec *currentSpec;            /* The current spec being parsed. */
static stringList *neededQualifiers;    /* The list of required qualifiers. */
static stringList *excludedQualifiers;  /* The list of excluded qualifiers. */
static moduleDef *currentModule;        /* The current module being parsed. */
static mappedTypeDef *currentMappedType;    /* The current mapped type. */
static enumDef *currentEnum;            /* The current enum being parsed. */
static int sectionFlags;                /* The current section flags. */
static int currentOverIsVirt;           /* Set if the overload is virtual. */
static int currentCtorIsExplicit;       /* Set if the ctor is explicit. */
static int currentIsStatic;             /* Set if the current is static. */
static int currentIsTemplate;           /* Set if the current is a template. */
static char *previousFile;              /* The file just parsed. */
static parserContext currentContext;    /* The current context. */
static int skipStackPtr;                /* The skip stack pointer. */
static int skipStack[MAX_NESTED_IF];    /* Stack of skip flags. */
static classDef *scopeStack[MAX_NESTED_SCOPE];  /* The scope stack. */
static int sectFlagsStack[MAX_NESTED_SCOPE];    /* The section flags stack. */
static int currentScopeIdx;             /* The scope stack index. */
static int currentTimelineOrder;        /* The current timeline order. */
static classList *currentSupers;        /* The current super-class list. */
static int defaultKwdArgs;              /* Support keyword arguments by default. */
static int makeProtPublic;              /* Treat protected items as public. */


static const char *getPythonName(optFlags *optflgs, const char *cname);
static classDef *findClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname);
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff);
static classDef *newClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *snd);
static void finishClass(sipSpec *, moduleDef *, classDef *, optFlags *);
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new);
static mappedTypeDef *newMappedType(sipSpec *,argDef *, optFlags *);
static enumDef *newEnum(sipSpec *pt, moduleDef *mod, mappedTypeDef *mt_scope,
        char *name, optFlags *of, int flags);
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod, classDef *scope, scopedNameDef *fqname, classTmplDef *tcd, templateDef *td);
static void newTypedef(sipSpec *, moduleDef *, char *, argDef *, optFlags *);
static void newVar(sipSpec *, moduleDef *, char *, int, argDef *, optFlags *,
        codeBlock *, codeBlock *, codeBlock *);
static void newCtor(char *, int, signatureDef *, optFlags *, codeBlock *,
        throwArgs *, signatureDef *, int, codeBlock *);
static void newFunction(sipSpec *, moduleDef *, classDef *, mappedTypeDef *,
        int, int, int, char *, signatureDef *, int, int, optFlags *,
        codeBlock *, codeBlock *, throwArgs *, signatureDef *, codeBlock *);
static optFlag *findOptFlag(optFlags *,char *,flagType);
static memberDef *findFunction(sipSpec *, moduleDef *, classDef *,
        mappedTypeDef *, const char *, int, int, int);
static void checkAttributes(sipSpec *, moduleDef *, classDef *,
        mappedTypeDef *, const char *, int);
static void newModule(FILE *fp, char *filename);
static moduleDef *allocModule();
static void parseFile(FILE *fp, char *name, moduleDef *prevmod, int optional);
static void handleEOF(void);
static void handleEOM(void);
static qualDef *findQualifier(const char *name);
static scopedNameDef *text2scopedName(ifaceFileDef *scope, char *text);
static scopedNameDef *scopeScopedName(ifaceFileDef *scope,
        scopedNameDef *name);
static void pushScope(classDef *);
static void popScope(void);
static classDef *currentScope(void);
static void newQualifier(moduleDef *,int,int,char *,qualType);
static void newImport(char *filename);
static int timePeriod(char *,char *);
static int platOrFeature(char *,int);
static int isNeeded(qualDef *);
static int notSkipping(void);
static void getHooks(optFlags *,char **,char **);
static int getTransfer(optFlags *optflgs);
static int getReleaseGIL(optFlags *optflgs);
static int getHoldGIL(optFlags *optflgs);
static int getDeprecated(optFlags *optflgs);
static int getAllowNone(optFlags *optflgs);
static const char *getDocType(optFlags *optflgs);
static const char *getDocValue(optFlags *optflgs);
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd, templateDef *td, classDef *ncd);
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td, classDef *ncd);
static int search_back(const char *end, const char *start, const char *target);
static char *type2string(argDef *ad);
static char *scopedNameToString(scopedNameDef *name);
static void addUsedFromCode(sipSpec *pt, ifaceFileList **used, const char *sname);
static int sameName(scopedNameDef *snd, const char *sname);
static int stringFind(stringList *sl, const char *s);
static void setModuleName(sipSpec *pt, moduleDef *mod, const char *fullname);
static int foundInScope(scopedNameDef *fq_name, scopedNameDef *rel_name);
static void defineClass(scopedNameDef *snd, classList *supers, optFlags *of);
static classDef *completeClass(scopedNameDef *snd, optFlags *of, int has_def);
static memberDef *instantiateTemplateMethods(memberDef *tmd, moduleDef *mod);
static void instantiateTemplateEnums(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static void instantiateTemplateVars(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static overDef *instantiateTemplateOverloads(sipSpec *pt, overDef *tod,
        memberDef *tmethods, memberDef *methods, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values);
static void resolveAnyTypedef(sipSpec *pt, argDef *ad);
static void addVariable(sipSpec *pt, varDef *vd);
static void applyTypeFlags(moduleDef *mod, argDef *ad, optFlags *flags);
static argType convertEncoding(const char *encoding);
static apiVersionRangeDef *getAPIRange(optFlags *optflgs);
static apiVersionRangeDef *convertAPIRange(moduleDef *mod, nameDef *name,
        int from, int to);
static char *convertFeaturedString(char *fs);
static scopedNameDef *text2scopePart(char *text);
static int usesKeywordArgs(optFlags *optflgs, signatureDef *sd);
static char *strip(char *s);
static int isEnabledFeature(const char *name);


/* Line 189 of yacc.c  */
#line 200 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 481 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 493 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1154

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  148
/* YYNRULES -- Number of rules.  */
#define YYNRULES  367
/* YYNRULES -- Number of states.  */
#define YYNSTATES  630

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   366

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   117,     2,     2,     2,   132,   124,     2,
     115,   116,   122,   121,   119,   118,     2,   123,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   129,   114,
     127,   120,   128,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   130,     2,   131,   133,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   112,   125,   113,   126,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    84,    86,    88,    90,    92,    94,    96,    99,
     102,   106,   116,   117,   121,   124,   125,   131,   132,   139,
     144,   146,   149,   151,   154,   157,   159,   161,   175,   176,
     184,   186,   189,   190,   196,   198,   201,   203,   206,   207,
     213,   215,   218,   220,   225,   227,   230,   234,   239,   241,
     245,   247,   250,   253,   256,   259,   262,   266,   268,   270,
     272,   274,   275,   277,   280,   283,   286,   287,   290,   291,
     294,   295,   298,   301,   304,   307,   310,   311,   313,   316,
     319,   322,   325,   328,   331,   334,   337,   340,   343,   346,
     349,   352,   355,   358,   361,   364,   367,   372,   375,   377,
     380,   381,   390,   391,   393,   394,   396,   397,   399,   401,
     404,   406,   408,   413,   414,   416,   417,   420,   421,   424,
     426,   430,   432,   434,   436,   438,   440,   442,   443,   445,
     447,   449,   451,   454,   456,   460,   462,   464,   469,   471,
     473,   475,   477,   479,   481,   483,   484,   486,   490,   496,
     508,   509,   510,   519,   520,   524,   529,   530,   531,   540,
     541,   544,   546,   550,   552,   553,   557,   559,   562,   564,
     566,   568,   570,   572,   574,   576,   578,   580,   582,   584,
     586,   588,   590,   592,   594,   596,   598,   600,   602,   604,
     606,   608,   611,   614,   618,   622,   626,   629,   630,   632,
     644,   645,   649,   651,   662,   663,   669,   670,   677,   678,
     680,   695,   703,   718,   732,   734,   736,   738,   740,   742,
     744,   746,   748,   751,   754,   757,   760,   763,   766,   769,
     772,   775,   778,   782,   786,   788,   791,   794,   796,   799,
     802,   805,   807,   810,   811,   813,   814,   817,   818,   822,
     824,   828,   830,   834,   836,   842,   844,   846,   849,   850,
     852,   853,   856,   857,   860,   862,   863,   865,   869,   874,
     879,   884,   888,   892,   899,   906,   910,   913,   914,   918,
     920,   922,   924,   925,   929,   931,   939,   944,   948,   952,
     953,   955,   956,   959,   961,   966,   969,   972,   974,   976,
     979,   981,   983,   986,   989,   993,   995,   997,   999,  1002,
    1005,  1007,  1009,  1011,  1013,  1015,  1017,  1019,  1021,  1023,
    1025,  1027,  1029,  1033,  1034,  1039,  1040,  1042
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     135,     0,    -1,   136,    -1,   135,   136,    -1,    -1,   137,
     138,    -1,   175,    -1,   173,    -1,   174,    -1,   141,    -1,
     185,    -1,   179,    -1,   180,    -1,   181,    -1,   142,    -1,
     162,    -1,   157,    -1,   161,    -1,   170,    -1,   140,    -1,
     171,    -1,   172,    -1,   186,    -1,   187,    -1,   199,    -1,
     201,    -1,   202,    -1,   203,    -1,   204,    -1,   205,    -1,
     206,    -1,   207,    -1,   208,    -1,   146,    -1,   148,    -1,
     139,    -1,   166,    -1,   169,    -1,   154,    -1,   230,    -1,
     236,    -1,   233,    -1,   143,    -1,   229,    -1,   211,    -1,
     253,    -1,   273,    -1,   188,    -1,     4,    27,    -1,     5,
      25,    -1,     3,    25,    90,    -1,   105,   225,   144,   257,
     112,   189,   145,   113,   114,    -1,    -1,   115,   225,   116,
      -1,   106,   209,    -1,    -1,    21,   278,   257,   147,   150,
      -1,    -1,   235,    21,   278,   257,   149,   150,    -1,   112,
     151,   113,   114,    -1,   152,    -1,   151,   152,    -1,   188,
      -1,    39,   209,    -1,    40,   209,    -1,   211,    -1,   153,
      -1,    82,   274,    25,   115,   265,   116,   255,   280,   257,
     251,   114,   262,   263,    -1,    -1,    93,    25,   155,   112,
     156,   113,   114,    -1,   139,    -1,   156,   139,    -1,    -1,
      95,   158,   112,   159,   113,    -1,   160,    -1,   159,   160,
      -1,    25,    -1,    96,    25,    -1,    -1,    94,   163,   112,
     164,   113,    -1,   165,    -1,   164,   165,    -1,    25,    -1,
      23,   115,   168,   116,    -1,    25,    -1,   117,    25,    -1,
     167,    80,    25,    -1,   167,    80,   117,    25,    -1,   167,
      -1,   214,   118,   214,    -1,    24,    -1,    97,   257,    -1,
     110,   177,    -1,   111,   177,    -1,    50,   177,    -1,    51,
     177,    -1,   176,   177,   178,    -1,    48,    -1,    49,    -1,
      25,    -1,    26,    -1,    -1,    90,    -1,    42,    26,    -1,
      43,    26,    -1,    44,    26,    -1,    -1,    10,   209,    -1,
      -1,    11,   209,    -1,    -1,    12,   209,    -1,    20,   209,
      -1,    45,   209,    -1,    46,   209,    -1,    47,   209,    -1,
      -1,   188,    -1,    29,   209,    -1,    30,   209,    -1,    31,
     209,    -1,    32,   209,    -1,    33,   209,    -1,    34,   209,
      -1,    35,   209,    -1,    36,   209,    -1,    37,   209,    -1,
      17,   209,    -1,    18,   209,    -1,    13,   209,    -1,    14,
     209,    -1,    15,   209,    -1,    16,   209,    -1,    19,   209,
      -1,     7,   209,    -1,     8,   209,    -1,     9,    26,   213,
     209,    -1,   210,    24,    -1,    22,    -1,   210,    22,    -1,
      -1,    76,   214,   257,   212,   112,   215,   113,   114,    -1,
      -1,    26,    -1,    -1,    25,    -1,    -1,   216,    -1,   217,
      -1,   216,   217,    -1,   166,    -1,   169,    -1,    25,   219,
     257,   218,    -1,    -1,   119,    -1,    -1,   120,   224,    -1,
      -1,   120,   221,    -1,   224,    -1,   221,   222,   224,    -1,
     118,    -1,   121,    -1,   122,    -1,   123,    -1,   124,    -1,
     125,    -1,    -1,   117,    -1,   126,    -1,   118,    -1,   121,
      -1,   223,   227,    -1,   226,    -1,   225,    79,   226,    -1,
      25,    -1,   225,    -1,   278,   115,   228,   116,    -1,    91,
      -1,    90,    -1,    99,    -1,   100,    -1,   101,    -1,    27,
      -1,    98,    -1,    -1,   221,    -1,   228,   119,   221,    -1,
      92,   274,    25,   257,   114,    -1,    92,   274,   115,   277,
      25,   116,   115,   279,   116,   257,   114,    -1,    -1,    -1,
      53,   225,   231,   239,   257,   232,   242,   114,    -1,    -1,
     235,   234,   236,    -1,   108,   127,   279,   128,    -1,    -1,
      -1,    52,   225,   237,   239,   257,   238,   242,   114,    -1,
      -1,   129,   240,    -1,   241,    -1,   240,   119,   241,    -1,
     225,    -1,    -1,   112,   243,   113,    -1,   244,    -1,   243,
     244,    -1,   166,    -1,   169,    -1,   154,    -1,   230,    -1,
     236,    -1,   143,    -1,   229,    -1,   211,    -1,   261,    -1,
     200,    -1,   188,    -1,   190,    -1,   191,    -1,   192,    -1,
     193,    -1,   194,    -1,   195,    -1,   196,    -1,   197,    -1,
     198,    -1,   247,    -1,   246,    -1,   268,    -1,    41,   209,
      -1,    40,   209,    -1,    54,   245,   129,    -1,    55,   245,
     129,    -1,    56,   245,   129,    -1,    57,   129,    -1,    -1,
      58,    -1,   252,   126,    25,   115,   116,   280,   256,   257,
     114,   263,   264,    -1,    -1,   107,   248,   249,    -1,   249,
      -1,    25,   115,   265,   116,   280,   257,   250,   114,   262,
     263,    -1,    -1,   130,   115,   265,   116,   131,    -1,    -1,
     130,   274,   115,   265,   116,   131,    -1,    -1,    75,    -1,
     274,    25,   115,   265,   116,   255,   280,   256,   257,   251,
     114,   262,   263,   264,    -1,   274,   102,   120,   115,   274,
     116,   114,    -1,   274,   102,   254,   115,   265,   116,   255,
     280,   256,   257,   251,   114,   263,   264,    -1,   102,   274,
     115,   265,   116,   255,   280,   256,   257,   251,   114,   263,
     264,    -1,   121,    -1,   118,    -1,   122,    -1,   123,    -1,
     132,    -1,   124,    -1,   125,    -1,   133,    -1,   127,   127,
      -1,   128,   128,    -1,   121,   120,    -1,   118,   120,    -1,
     122,   120,    -1,   123,   120,    -1,   132,   120,    -1,   124,
     120,    -1,   125,   120,    -1,   133,   120,    -1,   127,   127,
     120,    -1,   128,   128,   120,    -1,   126,    -1,   115,   116,
      -1,   130,   131,    -1,   127,    -1,   127,   120,    -1,   120,
     120,    -1,   117,   120,    -1,   128,    -1,   128,   120,    -1,
      -1,    81,    -1,    -1,   120,    90,    -1,    -1,   123,   258,
     123,    -1,   259,    -1,   258,   119,   259,    -1,    25,    -1,
      25,   120,   260,    -1,   177,    -1,    25,   129,   178,   118,
     178,    -1,    27,    -1,    90,    -1,     6,   209,    -1,    -1,
     261,    -1,    -1,    38,   209,    -1,    -1,    28,   209,    -1,
     266,    -1,    -1,   267,    -1,   266,   119,   267,    -1,    83,
     214,   257,   220,    -1,    84,   214,   257,   220,    -1,    85,
     214,   257,   220,    -1,    86,   214,   257,    -1,    87,   214,
     257,    -1,    88,   115,   265,   116,   214,   257,    -1,    89,
     115,   265,   116,   214,   257,    -1,   104,   214,   257,    -1,
     275,   220,    -1,    -1,    82,   269,   270,    -1,   270,    -1,
     271,    -1,   273,    -1,    -1,    75,   272,   253,    -1,   253,
      -1,   274,    25,   257,   114,   182,   183,   184,    -1,    81,
     278,   277,   276,    -1,   278,   277,   276,    -1,   274,   214,
     257,    -1,    -1,   124,    -1,    -1,   277,   122,    -1,   225,
      -1,   225,   127,   279,   128,    -1,    53,   225,    -1,    78,
      60,    -1,    60,    -1,    78,    -1,    78,    61,    -1,    61,
      -1,    62,    -1,    78,    62,    -1,    62,    62,    -1,    78,
      62,    62,    -1,    63,    -1,    64,    -1,    59,    -1,    77,
      65,    -1,    78,    65,    -1,    65,    -1,    66,    -1,    67,
      -1,    68,    -1,    69,    -1,    70,    -1,    71,    -1,    72,
      -1,    73,    -1,    74,    -1,   109,    -1,   274,    -1,   279,
     119,   274,    -1,    -1,   103,   115,   281,   116,    -1,    -1,
     225,    -1,   281,   119,   225,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   346,   346,   347,   350,   350,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   389,   393,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   434,   443,
     448,   474,   513,   517,   591,   596,   596,   602,   602,   652,
     666,   667,   670,   674,   683,   692,   693,   696,   710,   710,
     747,   748,   751,   751,   778,   779,   782,   787,   792,   792,
     817,   818,   821,   826,   839,   842,   845,   848,   853,   854,
     859,   865,   892,   903,   914,   927,   940,   973,   976,   981,
     982,   998,  1001,  1004,  1009,  1014,  1019,  1022,  1027,  1030,
    1035,  1038,  1043,  1048,  1053,  1058,  1063,  1066,  1069,  1074,
    1079,  1084,  1089,  1094,  1099,  1104,  1109,  1114,  1119,  1124,
    1130,  1136,  1142,  1148,  1157,  1163,  1168,  1174,  1177,  1178,
    1189,  1189,  1201,  1204,  1209,  1212,  1217,  1218,  1221,  1222,
    1225,  1226,  1227,  1255,  1256,  1259,  1260,  1263,  1266,  1271,
    1272,  1290,  1293,  1296,  1299,  1302,  1305,  1310,  1313,  1316,
    1319,  1322,  1327,  1345,  1346,  1354,  1359,  1368,  1378,  1382,
    1386,  1390,  1394,  1398,  1402,  1408,  1413,  1419,  1437,  1444,
    1469,  1475,  1469,  1490,  1490,  1516,  1521,  1527,  1521,  1539,
    1540,  1543,  1544,  1547,  1586,  1589,  1594,  1595,  1598,  1599,
    1600,  1601,  1602,  1603,  1604,  1605,  1606,  1616,  1620,  1624,
    1635,  1646,  1657,  1668,  1679,  1690,  1701,  1712,  1723,  1724,
    1725,  1726,  1737,  1748,  1755,  1762,  1769,  1778,  1781,  1786,
    1840,  1840,  1841,  1844,  1871,  1874,  1881,  1884,  1892,  1895,
    1900,  1915,  1933,  1959,  2034,  2035,  2036,  2037,  2038,  2039,
    2040,  2041,  2042,  2043,  2044,  2045,  2046,  2047,  2048,  2049,
    2050,  2051,  2052,  2053,  2054,  2055,  2056,  2057,  2058,  2059,
    2060,  2061,  2062,  2065,  2068,  2073,  2076,  2084,  2087,  2093,
    2097,  2109,  2113,  2119,  2123,  2146,  2150,  2156,  2161,  2164,
    2167,  2170,  2175,  2178,  2183,  2231,  2236,  2242,  2269,  2278,
    2287,  2296,  2307,  2315,  2329,  2343,  2349,  2355,  2355,  2356,
    2359,  2360,  2363,  2363,  2364,  2367,  2402,  2407,  2414,  2486,
    2489,  2497,  2500,  2505,  2513,  2524,  2539,  2543,  2547,  2551,
    2555,  2559,  2563,  2567,  2571,  2575,  2579,  2583,  2587,  2591,
    2595,  2599,  2603,  2607,  2611,  2615,  2619,  2623,  2627,  2631,
    2635,  2641,  2647,  2663,  2666,  2674,  2680,  2687
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TK_API", "TK_DEFENCODING", "TK_PLUGIN",
  "TK_DOCSTRING", "TK_DOC", "TK_EXPORTEDDOC", "TK_MAKEFILE",
  "TK_ACCESSCODE", "TK_GETCODE", "TK_SETCODE", "TK_PREINITCODE",
  "TK_INITCODE", "TK_POSTINITCODE", "TK_UNITCODE", "TK_MODCODE",
  "TK_TYPECODE", "TK_PREPYCODE", "TK_COPYING", "TK_MAPPEDTYPE",
  "TK_CODELINE", "TK_IF", "TK_END", "TK_NAME", "TK_PATHNAME", "TK_STRING",
  "TK_VIRTUALCATCHERCODE", "TK_TRAVERSECODE", "TK_CLEARCODE",
  "TK_GETBUFFERCODE", "TK_RELEASEBUFFERCODE", "TK_READBUFFERCODE",
  "TK_WRITEBUFFERCODE", "TK_SEGCOUNTCODE", "TK_CHARBUFFERCODE",
  "TK_PICKLECODE", "TK_METHODCODE", "TK_FROMTYPE", "TK_TOTYPE",
  "TK_TOSUBCLASS", "TK_INCLUDE", "TK_OPTINCLUDE", "TK_IMPORT",
  "TK_EXPHEADERCODE", "TK_MODHEADERCODE", "TK_TYPEHEADERCODE", "TK_MODULE",
  "TK_CMODULE", "TK_CONSMODULE", "TK_COMPOMODULE", "TK_CLASS", "TK_STRUCT",
  "TK_PUBLIC", "TK_PROTECTED", "TK_PRIVATE", "TK_SIGNALS", "TK_SLOTS",
  "TK_BOOL", "TK_SHORT", "TK_INT", "TK_LONG", "TK_FLOAT", "TK_DOUBLE",
  "TK_CHAR", "TK_WCHAR_T", "TK_VOID", "TK_PYOBJECT", "TK_PYTUPLE",
  "TK_PYLIST", "TK_PYDICT", "TK_PYCALLABLE", "TK_PYSLICE", "TK_PYTYPE",
  "TK_VIRTUAL", "TK_ENUM", "TK_SIGNED", "TK_UNSIGNED", "TK_SCOPE",
  "TK_LOGICAL_OR", "TK_CONST", "TK_STATIC", "TK_SIPSIGNAL", "TK_SIPSLOT",
  "TK_SIPANYSLOT", "TK_SIPRXCON", "TK_SIPRXDIS", "TK_SIPSLOTCON",
  "TK_SIPSLOTDIS", "TK_NUMBER", "TK_REAL", "TK_TYPEDEF", "TK_NAMESPACE",
  "TK_TIMELINE", "TK_PLATFORMS", "TK_FEATURE", "TK_LICENSE", "TK_QCHAR",
  "TK_TRUE", "TK_FALSE", "TK_NULL", "TK_OPERATOR", "TK_THROW",
  "TK_QOBJECT", "TK_EXCEPTION", "TK_RAISECODE", "TK_EXPLICIT",
  "TK_TEMPLATE", "TK_ELLIPSIS", "TK_DEFMETATYPE", "TK_DEFSUPERTYPE", "'{'",
  "'}'", "';'", "'('", "')'", "'!'", "'-'", "','", "'='", "'+'", "'*'",
  "'/'", "'&'", "'|'", "'~'", "'<'", "'>'", "':'", "'['", "']'", "'%'",
  "'^'", "$accept", "specification", "statement", "$@1", "modstatement",
  "nsstatement", "defencoding", "plugin", "api", "exception",
  "baseexception", "raisecode", "mappedtype", "$@2", "mappedtypetmpl",
  "$@3", "mtdefinition", "mtbody", "mtline", "mtfunction", "namespace",
  "$@4", "nsbody", "platforms", "$@5", "platformlist", "platform",
  "feature", "timeline", "$@6", "qualifierlist", "qualifiername",
  "ifstart", "oredqualifiers", "qualifiers", "ifend", "license",
  "defmetatype", "defsupertype", "consmodule", "compmodule", "module",
  "modlang", "dottedname", "optnumber", "include", "optinclude", "import",
  "optaccesscode", "optgetcode", "optsetcode", "copying", "exphdrcode",
  "modhdrcode", "typehdrcode", "opttypehdrcode", "travcode", "clearcode",
  "getbufcode", "releasebufcode", "readbufcode", "writebufcode",
  "segcountcode", "charbufcode", "picklecode", "modcode", "typecode",
  "preinitcode", "initcode", "postinitcode", "unitcode", "prepycode",
  "doc", "exporteddoc", "makefile", "codeblock", "codelines", "enum",
  "$@7", "optfilename", "optname", "optenumbody", "enumbody", "enumline",
  "optcomma", "optenumassign", "optassign", "expr", "binop", "optunop",
  "value", "scopedname", "scopepart", "simplevalue", "exprlist", "typedef",
  "struct", "$@8", "$@9", "classtmpl", "$@10", "template", "class", "$@11",
  "$@12", "superclasses", "superlist", "superclass", "optclassbody",
  "classbody", "classline", "optslot", "dtor", "ctor", "$@13",
  "simplector", "optctorsig", "optsig", "optvirtual", "function",
  "operatorname", "optconst", "optabstract", "optflags", "flaglist",
  "flag", "flagvalue", "docstring", "optdocstring", "methodcode",
  "virtualcatchercode", "arglist", "rawarglist", "argvalue", "varmember",
  "$@14", "varmem", "member", "$@15", "variable", "cpptype", "argtype",
  "optref", "deref", "basetype", "cpptypelist", "optexceptions",
  "exceptionlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   123,   125,    59,    40,    41,    33,    45,    44,
      61,    43,    42,    47,    38,   124,   126,    60,    62,    58,
      91,    93,    37,    94
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   134,   135,   135,   137,   136,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   140,   141,
     142,   143,   144,   144,   145,   147,   146,   149,   148,   150,
     151,   151,   152,   152,   152,   152,   152,   153,   155,   154,
     156,   156,   158,   157,   159,   159,   160,   161,   163,   162,
     164,   164,   165,   166,   167,   167,   167,   167,   168,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   176,   177,
     177,   178,   178,   179,   180,   181,   182,   182,   183,   183,
     184,   184,   185,   186,   187,   188,   189,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   210,
     212,   211,   213,   213,   214,   214,   215,   215,   216,   216,
     217,   217,   217,   218,   218,   219,   219,   220,   220,   221,
     221,   222,   222,   222,   222,   222,   222,   223,   223,   223,
     223,   223,   224,   225,   225,   226,   227,   227,   227,   227,
     227,   227,   227,   227,   227,   228,   228,   228,   229,   229,
     231,   232,   230,   234,   233,   235,   237,   238,   236,   239,
     239,   240,   240,   241,   242,   242,   243,   243,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   245,   245,   246,
     248,   247,   247,   249,   250,   250,   251,   251,   252,   252,
     253,   253,   253,   253,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   254,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   254,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   255,   255,   256,   256,   257,   257,   258,
     258,   259,   259,   260,   260,   260,   260,   261,   262,   262,
     263,   263,   264,   264,   265,   266,   266,   266,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   269,   268,   268,
     270,   270,   272,   271,   271,   273,   274,   274,   275,   276,
     276,   277,   277,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   279,   279,   280,   280,   281,   281,   281
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       3,     9,     0,     3,     2,     0,     5,     0,     6,     4,
       1,     2,     1,     2,     2,     1,     1,    13,     0,     7,
       1,     2,     0,     5,     1,     2,     1,     2,     0,     5,
       1,     2,     1,     4,     1,     2,     3,     4,     1,     3,
       1,     2,     2,     2,     2,     2,     3,     1,     1,     1,
       1,     0,     1,     2,     2,     2,     0,     2,     0,     2,
       0,     2,     2,     2,     2,     2,     0,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     4,     2,     1,     2,
       0,     8,     0,     1,     0,     1,     0,     1,     1,     2,
       1,     1,     4,     0,     1,     0,     2,     0,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     2,     1,     3,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     3,     5,    11,
       0,     0,     8,     0,     3,     4,     0,     0,     8,     0,
       2,     1,     3,     1,     0,     3,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     3,     3,     3,     2,     0,     1,    11,
       0,     3,     1,    10,     0,     5,     0,     6,     0,     1,
      14,     7,    14,    13,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     3,     1,     2,     2,     1,     2,     2,
       2,     1,     2,     0,     1,     0,     2,     0,     3,     1,
       3,     1,     3,     1,     5,     1,     1,     2,     0,     1,
       0,     2,     0,     2,     1,     0,     1,     3,     4,     4,
       4,     3,     3,     6,     6,     3,     2,     0,     3,     1,
       1,     1,     0,     3,     1,     7,     4,     3,     3,     0,
       1,     0,     2,     1,     4,     2,     2,     1,     1,     2,
       1,     1,     2,     2,     3,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     0,     4,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     4,     2,     0,     1,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    90,   175,     0,     0,     0,     0,     0,     0,    97,
      98,     0,     0,     0,     0,   347,   337,   340,   341,   345,
     346,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   144,     0,   338,     0,     0,     0,    78,    72,     0,
     287,     0,     0,     0,   360,     0,     0,     5,    35,    19,
       9,    14,    42,    33,    34,    38,    16,    17,    15,    36,
      37,    18,    20,    21,     7,     8,     6,     0,    11,    12,
      13,    10,    22,    23,    47,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    44,   333,   173,    43,    39,    41,
     193,    40,    45,    46,     0,   331,     0,    48,    49,   138,
     134,     0,   135,   142,   129,   130,   131,   132,   127,   133,
     112,     0,   287,   144,   103,   104,   105,   113,   114,   115,
      99,   100,    94,    95,   196,   190,   343,   145,   287,   348,
     336,   339,   342,   349,   331,     0,    68,     0,     0,    77,
       0,    91,     0,    52,     0,    92,    93,   101,     0,     0,
       0,     0,   287,     0,   329,    50,   139,   137,   143,     0,
     335,    55,    84,     0,    88,     0,     0,   199,   199,   140,
     344,   329,   287,   331,     0,     0,     0,   291,     0,   289,
     305,     0,   287,   361,     0,   102,    96,   174,     0,   287,
     194,   305,     0,     0,     0,   255,     0,   254,   256,   257,
     259,   260,   274,   277,   281,     0,   258,   261,     0,   332,
     330,   327,   136,     0,    85,     0,    83,   144,     0,   287,
     287,     0,   326,     0,     0,     0,    82,     0,    80,    76,
       0,    74,     0,     0,   288,   144,   144,   144,   144,   144,
       0,     0,   144,     0,   304,   306,   144,   157,     0,     0,
       0,   195,   334,    57,     0,   106,   275,   280,   265,     0,
     279,   264,   266,   267,   269,   270,   278,   262,   282,   263,
     276,   268,   271,   305,     0,    56,    86,     0,    89,   203,
     200,   201,   197,   191,   146,   188,     0,    70,     0,   193,
      79,    81,    73,    75,    99,   295,   296,   293,   292,   290,
     287,   287,   287,   287,   287,   305,   305,   287,   283,     0,
     287,   167,   316,    53,   116,   362,     0,   283,     0,   108,
       0,   272,   273,     0,     0,     0,     0,     0,    60,    66,
      62,    65,    87,     0,   204,   204,   155,   150,   151,     0,
     147,   148,     0,     0,    71,   101,   157,   157,   157,   311,
     312,     0,     0,   315,   284,   363,   307,   328,   168,   170,
     171,   169,   158,     0,   159,   117,     0,    58,   363,   107,
       0,   110,     0,   283,    63,    64,     0,     0,    61,   202,
     248,     0,     0,   167,   287,     0,   149,     0,    69,     0,
     308,   309,   310,   144,   144,     0,   285,   161,   162,   163,
     164,   165,   166,   167,   183,   179,   178,   184,   180,   181,
     182,   176,   172,     0,     0,     0,   285,   109,     0,   325,
     251,   363,     0,    59,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   237,   237,
     237,     0,   322,   317,   240,   213,   210,   208,   209,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   217,
     215,   214,   211,   212,   248,   206,   229,   228,   242,     0,
     324,   216,   230,   319,   320,   321,   198,   192,   156,   153,
     141,     0,   101,   287,   287,   365,     0,   287,   160,   167,
      54,     0,   287,   111,   285,   305,   297,   128,   305,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   232,   231,
     238,     0,     0,     0,   236,     0,     0,     0,   205,   207,
       0,   154,   152,   287,   294,   313,   314,   366,     0,   286,
     246,   186,     0,    51,   246,   287,     0,     0,   233,   234,
     235,   323,     0,   322,   318,     0,   241,     0,     0,   364,
       0,     0,     0,   177,   167,     0,   246,   283,   363,     0,
       0,   189,   367,     0,   300,   187,   298,     0,   363,   287,
     363,   305,     0,   302,   299,   300,   300,   287,   244,   285,
       0,   301,     0,   253,   302,   302,   246,     0,     0,   287,
       0,   303,   250,   252,     0,   305,   298,     0,   247,   298,
       0,   300,   300,   300,     0,   243,   302,    67,   245,   239
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,    67,    68,    69,    70,    71,    72,
     202,   435,    73,   233,    74,   336,   295,   347,   348,   349,
      75,   194,   308,    76,   158,   250,   251,    77,    78,   157,
     247,   248,    79,   184,   185,    80,    81,    82,    83,    84,
      85,    86,    87,   142,   206,    88,    89,    90,   339,   391,
     439,    91,    92,    93,    94,   386,   470,   471,   472,   473,
     474,   475,   476,   477,   478,    95,   479,    96,    97,    98,
      99,   100,   101,   102,   103,   120,   121,   104,   241,   179,
     148,   359,   360,   361,   542,   404,   332,   382,   423,   383,
     384,   105,   106,   432,   552,   107,   108,   188,   355,   109,
     171,   309,   111,   187,   354,   239,   300,   301,   401,   484,
     485,   531,   486,   487,   537,   488,   608,   572,   489,   112,
     228,   375,   507,   161,   198,   199,   318,   594,   595,   593,
     603,   263,   264,   265,   492,   536,   493,   494,   535,   113,
     266,   267,   231,   174,   115,   204,   416,   548
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -513
static const yytype_int16 yypact[] =
{
    -513,    66,  -513,   551,  -513,  -513,    83,   118,   125,   148,
     148,   153,   148,   148,   148,   148,   148,   148,   148,   129,
      72,  -513,  -513,   182,   187,   220,   148,   148,   148,  -513,
    -513,   135,   135,   231,   231,  -513,  -513,  -513,   201,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,   239,   202,   179,   129,  1045,   240,  -513,  -513,   241,
     146,  1045,   231,   143,  -513,   135,   135,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,   135,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,   -35,  -513,  -513,  -513,  -513,
     250,  -513,  -513,  -513,    10,  -513,   184,  -513,  -513,  -513,
    -513,    82,  -513,   246,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,   231,   146,    12,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,   208,     8,  -513,  -513,   146,  -513,
    -513,  -513,   227,  -513,  -513,     7,  -513,   180,   181,  -513,
     269,  -513,   183,   -12,  1045,  -513,  -513,   207,   231,  1045,
     129,   247,   -52,   158,   131,  -513,  -513,  -513,  -513,   148,
     208,  -513,   185,   275,   221,   186,   189,   175,   175,  -513,
    -513,   131,   146,  -513,   193,   283,   284,   190,   -23,  -513,
     406,   231,   146,  -513,   -71,  -513,  -513,  -513,   -70,   146,
    -513,   406,   198,   197,   194,   196,   -43,   199,   203,   204,
     205,   210,  -513,   -29,   -31,   191,   211,   212,   206,  -513,
    -513,  -513,  -513,   215,  -513,    16,  -513,   239,   231,   146,
     146,   224,  -513,   219,     1,   857,  -513,     4,  -513,  -513,
      18,  -513,    59,   269,  -513,   239,   239,   239,   239,   239,
     222,   223,   239,   225,   226,  -513,   239,   228,   -33,   230,
    1045,  -513,  -513,  -513,   233,   307,  -513,  -513,  -513,  1045,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,   232,  -513,   234,
    -513,  -513,  -513,   406,   133,  -513,  -513,   295,  -513,   208,
     236,  -513,  -513,  -513,   200,  -513,   235,  -513,   798,  -513,
    -513,  -513,  -513,  -513,   214,  -513,  -513,  -513,  -513,  -513,
     146,   146,   146,   146,   146,   406,   406,   146,   258,   406,
     146,    38,  -513,  -513,   293,  -513,   215,   258,   148,   335,
     237,  -513,  -513,   242,   148,   148,  1045,    29,  -513,  -513,
    -513,  -513,  -513,   231,   238,   238,   243,  -513,  -513,   244,
     200,  -513,   245,   248,  -513,   207,   228,   228,   228,  -513,
    -513,   249,   251,  -513,  -513,   253,  -513,  -513,  -513,  -513,
    -513,  -513,   127,   914,  -513,  -513,   255,  -513,   253,  -513,
     148,   352,   252,   258,  -513,  -513,   322,   254,  -513,  -513,
     737,   256,   257,    38,   146,   259,  -513,  1045,  -513,   260,
    -513,  -513,  -513,   239,   239,   262,   261,  -513,  -513,  -513,
    -513,  -513,  -513,    38,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,   -25,  -513,   264,   148,   270,   261,  -513,   148,  -513,
    -513,   253,   267,  -513,   148,   148,   271,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   311,   311,
     311,   263,   268,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,   645,  -513,  -513,  -513,  -513,   273,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,   265,
    -513,    52,   207,   146,   146,   231,   282,   146,  -513,   100,
    -513,   274,   146,  -513,   261,   406,  -513,  -513,   406,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,   266,   272,   277,  -513,   991,   965,   349,  -513,  -513,
     350,  -513,  -513,   146,  -513,  -513,  -513,   208,    62,  -513,
     278,   127,   116,  -513,   278,   146,   280,   281,  -513,  -513,
    -513,  -513,    11,  -513,  -513,   271,  -513,   285,   276,  -513,
     231,  1045,   279,  -513,    38,   288,   278,   258,   253,   289,
     287,  -513,   208,   292,   347,   127,   381,   298,   253,   146,
     253,   406,   148,   361,  -513,   347,   347,   146,   286,   261,
     297,  -513,   148,  -513,   361,   361,   278,   294,   303,   146,
     290,  -513,  -513,  -513,   304,   406,   381,   306,  -513,   381,
     309,   347,   347,   347,   291,  -513,   361,  -513,  -513,  -513
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -513,  -513,   422,  -513,  -513,  -157,  -513,  -513,  -513,  -369,
    -513,  -513,  -513,  -513,  -513,  -513,    90,  -513,    80,  -513,
    -366,  -513,  -513,  -513,  -513,  -513,   213,  -513,  -513,  -513,
    -513,   209,  -280,  -513,  -513,  -279,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,    -5,  -337,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -272,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,   -10,  -513,  -273,  -513,  -513,
    -118,  -513,  -513,    69,  -513,  -513,  -108,  -431,  -513,  -513,
    -358,   -24,   296,  -513,  -513,  -350,  -349,  -513,  -513,  -513,
    -513,   427,  -158,  -513,  -513,   300,  -513,    79,    78,  -513,
     -48,  -198,  -513,  -513,  -513,   -89,  -513,  -512,  -513,  -360,
    -513,  -323,  -413,   -93,  -513,   299,  -513,  -348,  -383,  -438,
    -452,  -200,  -513,   120,  -513,  -513,   -85,  -513,  -513,  -370,
      -2,  -513,   308,   -98,    -7,  -150,  -368,  -513
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -336
static const yytype_int16 yytable[] =
{
     122,   114,   124,   125,   126,   127,   128,   129,   130,   144,
     145,   274,   132,   210,   388,   186,   137,   138,   139,   208,
     436,   351,   350,   512,   357,   358,   306,   143,   409,   246,
     495,   465,   192,  -335,   466,   172,   579,   182,   163,   181,
     490,   296,   575,   249,   168,   498,   168,   154,   270,   270,
     481,   482,   491,   155,   168,   189,   191,   271,   272,   162,
     165,   166,   385,   211,   587,   508,     4,   168,   344,   345,
     441,   160,   279,   514,   351,   350,    28,   280,   551,   212,
     357,   358,   167,   333,   314,   141,   315,   168,   307,   288,
    -333,   286,   169,   343,   614,   244,   253,   289,   287,   243,
     254,   555,   169,   201,   176,    51,   177,   180,   116,   269,
    -335,   346,   173,   173,   495,   465,   273,   310,   466,   298,
     467,   468,   193,   229,   490,   371,   372,   480,   469,   183,
    -335,   312,  -335,   297,   481,   482,   491,   320,   321,   322,
     323,   324,   397,   585,   327,   117,   302,   303,   330,   316,
     118,   364,   612,   613,    22,   378,   379,   604,   605,   380,
     140,   141,   203,   209,   381,   544,   495,   203,   543,   232,
     119,   270,   344,   345,   629,   561,   490,   268,   569,   123,
      28,   570,   131,   625,   626,   627,   609,   133,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,   467,   468,    52,    53,   134,    51,
     589,   480,   469,   135,   299,   346,  -185,   378,   379,  -185,
     597,   380,   599,    20,    21,   356,   381,   366,   367,   368,
     369,   370,   573,   621,   373,   574,   623,   377,    64,   150,
     151,   152,   483,   114,   153,   417,   136,   317,   418,   419,
     420,   421,   422,   229,   588,   230,    22,   501,   410,   411,
     412,   532,   533,   146,   147,   156,   159,   149,   335,   160,
     164,   170,   178,   213,   175,   214,   215,   340,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   168,   225,   190,
     226,   227,   195,   196,   197,   503,   504,   205,   200,    33,
     234,   235,   236,  -145,   238,   245,   114,   237,   246,   249,
     252,   499,   275,   276,   277,   556,   278,   338,   557,   281,
     352,   293,   290,   282,   283,   284,   483,   294,   389,   299,
     285,   291,   292,   305,   394,   395,   304,   325,   326,   374,
      28,   328,   334,   365,   396,   329,   390,   442,   331,   337,
     400,   362,   341,   392,   342,   353,   415,   405,   393,   431,
     407,   434,   408,   403,   438,   413,   440,   414,   443,   530,
     496,   497,   549,   500,   565,   567,   433,   505,   502,   509,
     437,   506,   515,   511,   541,   592,   518,   444,   553,   602,
     581,   600,   534,   584,  -249,   558,   577,   578,   114,   540,
     580,   559,   586,   590,   211,   203,   560,   591,   571,   615,
     545,   546,   596,   610,   550,   620,   607,   616,   619,   554,
     622,   618,   628,     5,   510,   624,   387,   398,   513,   406,
     110,    22,   399,   402,   516,   517,   539,   519,   520,   521,
     522,   523,   524,   525,   526,   527,   528,   529,   566,   376,
     568,   564,     0,     0,     0,     0,   311,     0,     0,   131,
       0,     0,   576,   313,   207,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,   547,   114,    52,    53,     0,     0,    54,   240,   255,
     256,   257,   258,   259,   260,   261,   598,     0,     0,   242,
       0,     0,     0,     0,   606,     0,     0,     0,     0,     0,
     262,     0,     0,     0,     0,    64,   617,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   562,   114,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   582,     0,     0,     0,
       0,     0,   319,     0,     6,     7,     8,     0,     9,    10,
      11,     0,     0,     0,    12,    13,    14,    15,    16,   583,
      17,    18,    19,     0,    20,    21,    22,     0,     0,     0,
       0,     0,   601,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   611,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     0,     0,     0,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,     0,    51,    52,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    56,    57,    58,    59,    60,     0,
       0,   444,     0,    61,     0,     0,    62,     0,     0,    63,
      64,    65,    66,   445,     0,     0,     0,     0,    20,    21,
     446,     0,     0,     0,   447,   448,   449,   450,   451,   452,
     453,   454,   455,     0,     0,   456,   457,     0,     0,     0,
       0,     0,    28,     0,     0,     0,     0,    33,    34,   458,
     459,   460,   461,     0,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
     462,    51,    52,    53,     0,     0,    54,   463,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,   444,     0,     0,     0,    61,     0,     0,
      62,     0,   464,     0,    64,   445,     0,     0,   538,     0,
      20,    21,   446,     0,     0,     0,   447,   448,   449,   450,
     451,   452,   453,   454,   455,     0,     0,   456,   457,     0,
       0,     0,     0,     0,    28,     0,     0,     0,     0,    33,
      34,   458,   459,   460,   461,     0,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,   462,    51,    52,    53,     0,     0,    54,   463,
       0,    20,    21,    22,     0,     0,     0,     0,     0,    55,
      56,     0,     0,     0,     0,     0,     0,     0,     0,    61,
       0,     0,    62,     0,   464,    28,    64,     0,     0,     0,
      33,    34,     0,     0,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,     0,    51,    52,    53,     0,     0,    54,
      20,    21,    22,     0,     0,     0,     0,     0,     0,     0,
      55,    56,     0,     0,     0,     0,     0,     0,     0,     0,
      61,     0,     0,    62,    28,     0,    63,    64,     0,    33,
      34,   363,     0,     0,     0,     0,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,     0,    51,    52,    53,     0,     0,    54,    22,
       0,   424,     0,     0,     0,     0,     0,     0,     0,    55,
      56,     0,     0,     0,     0,     0,     0,     0,     0,    61,
       0,     0,    62,     0,     0,    63,    64,   131,     0,     0,
       0,     0,     0,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,     0,
      22,    52,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   425,   426,     0,     0,     0,     0,
       0,     0,   427,   428,   429,   430,    22,     0,   131,     0,
       0,     0,     0,    64,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
     563,     0,    52,    53,   131,     0,    54,     0,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,     0,    61,    52,    53,
      22,     0,    54,     0,    64,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,     0,     0,     0,     0,   131,     0,
      64,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
       0,     0,    52,    53,     0,     0,    54,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    64
};

static const yytype_int16 yycheck[] =
{
      10,     3,    12,    13,    14,    15,    16,    17,    18,    33,
      34,   211,    19,   171,   337,   133,    26,    27,    28,   169,
     388,   294,   294,   436,   304,   304,    25,    32,   365,    25,
     400,   400,    25,    25,   400,    25,    25,    25,    62,   132,
     400,    25,   554,    25,    79,   403,    79,    54,   119,   119,
     400,   400,   400,    55,    79,   148,   154,   128,   128,    61,
      65,    66,   334,   115,   576,   423,     0,    79,    39,    40,
     393,   123,   115,   441,   347,   347,    47,   120,   509,   172,
     360,   360,    87,   116,    25,    26,    27,    79,   245,   120,
     115,   120,   127,   293,   606,   193,   119,   128,   127,   192,
     123,   514,   127,   115,    22,    76,    24,   131,    25,   202,
     102,    82,   102,   102,   484,   484,   209,   113,   484,   237,
     400,   400,   115,   122,   484,   325,   326,   400,   400,   117,
     122,   113,   124,   117,   484,   484,   484,   255,   256,   257,
     258,   259,   113,   574,   262,    27,   239,   240,   266,    90,
      25,   308,   604,   605,    25,   117,   118,   595,   596,   121,
      25,    26,   164,   170,   126,   502,   536,   169,   116,   179,
      22,   119,    39,    40,   626,   535,   536,   201,   116,    26,
      47,   119,    53,   621,   622,   623,   599,   115,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,   484,   484,    77,    78,    26,    76,
     578,   484,   484,    26,   238,    82,   116,   117,   118,   119,
     588,   121,   590,    23,    24,    25,   126,   320,   321,   322,
     323,   324,   116,   616,   327,   119,   619,   330,   109,    60,
      61,    62,   400,   245,    65,   118,    26,   252,   121,   122,
     123,   124,   125,   122,   577,   124,    25,   407,   366,   367,
     368,   459,   460,    62,    25,    25,    25,    65,   270,   123,
     127,    21,    26,   115,    90,   117,   118,   279,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    79,   130,    62,
     132,   133,   112,   112,    25,   413,   414,    90,   115,    52,
      25,    80,   116,   118,   129,   112,   308,   118,    25,    25,
     120,   404,   114,   116,   120,   515,   120,    10,   518,   120,
      25,   115,   131,   120,   120,   120,   484,   112,   338,   353,
     120,   120,   120,   114,   344,   345,   112,   115,   115,    81,
      47,   116,   112,   129,   346,   119,    11,    25,   120,   116,
     112,   116,   120,   116,   120,   119,   103,   113,   116,   383,
     115,   106,   114,   120,    12,   116,   114,   116,   114,    58,
     114,   114,    90,   114,    25,    25,   383,   115,   118,   115,
     390,   120,   115,   113,   119,    38,   115,     6,   114,    28,
     114,   591,   129,   114,   126,   129,   116,   116,   400,   126,
     115,   129,   114,   116,   115,   407,   129,   115,   130,   115,
     503,   504,   114,   116,   507,   615,   130,   114,   114,   512,
     114,   131,   131,     1,   434,   116,   336,   347,   438,   360,
       3,    25,   353,   355,   444,   445,   484,   447,   448,   449,
     450,   451,   452,   453,   454,   455,   456,   457,   537,   329,
     543,   536,    -1,    -1,    -1,    -1,   247,    -1,    -1,    53,
      -1,    -1,   555,   250,   168,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,   505,   484,    77,    78,    -1,    -1,    81,   188,    83,
      84,    85,    86,    87,    88,    89,   589,    -1,    -1,   191,
      -1,    -1,    -1,    -1,   597,    -1,    -1,    -1,    -1,    -1,
     104,    -1,    -1,    -1,    -1,   109,   609,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   535,   536,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   570,    -1,    -1,    -1,
      -1,    -1,   253,    -1,     3,     4,     5,    -1,     7,     8,
       9,    -1,    -1,    -1,    13,    14,    15,    16,    17,   571,
      19,    20,    21,    -1,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,   592,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   602,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    -1,    76,    77,    78,
      -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    93,    94,    95,    96,    97,    -1,
      -1,     6,    -1,   102,    -1,    -1,   105,    -1,    -1,   108,
     109,   110,   111,    18,    -1,    -1,    -1,    -1,    23,    24,
      25,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    -1,    -1,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    92,    93,    -1,
      -1,    -1,    -1,     6,    -1,    -1,    -1,   102,    -1,    -1,
     105,    -1,   107,    -1,   109,    18,    -1,    -1,   113,    -1,
      23,    24,    25,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    -1,    -1,    81,    82,
      -1,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,
      -1,    -1,   105,    -1,   107,    47,   109,    -1,    -1,    -1,
      52,    53,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    -1,    76,    77,    78,    -1,    -1,    81,
      23,    24,    25,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     102,    -1,    -1,   105,    47,    -1,   108,   109,    -1,    52,
      53,   113,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    -1,    76,    77,    78,    -1,    -1,    81,    25,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,
      -1,    -1,   105,    -1,    -1,   108,   109,    53,    -1,    -1,
      -1,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      25,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    98,    99,   100,   101,    25,    -1,    53,    -1,
      -1,    -1,    -1,   109,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    -1,    77,    78,    53,    -1,    81,    -1,    -1,    -1,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    -1,   102,    77,    78,
      25,    -1,    81,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   102,    -1,    -1,    -1,    -1,    53,    -1,
     109,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    81,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   135,   136,   137,     0,   136,     3,     4,     5,     7,
       8,     9,    13,    14,    15,    16,    17,    19,    20,    21,
      23,    24,    25,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    76,    77,    78,    81,    92,    93,    94,    95,    96,
      97,   102,   105,   108,   109,   110,   111,   138,   139,   140,
     141,   142,   143,   146,   148,   154,   157,   161,   162,   166,
     169,   170,   171,   172,   173,   174,   175,   176,   179,   180,
     181,   185,   186,   187,   188,   199,   201,   202,   203,   204,
     205,   206,   207,   208,   211,   225,   226,   229,   230,   233,
     235,   236,   253,   273,   274,   278,    25,    27,    25,    22,
     209,   210,   209,    26,   209,   209,   209,   209,   209,   209,
     209,    53,   278,   115,    26,    26,    26,   209,   209,   209,
      25,    26,   177,   177,   225,   225,    62,    25,   214,    65,
      60,    61,    62,    65,   278,   274,    25,   163,   158,    25,
     123,   257,   274,   225,   127,   177,   177,   177,    79,   127,
      21,   234,    25,   102,   277,    90,    22,    24,    26,   213,
     225,   257,    25,   117,   167,   168,   214,   237,   231,   257,
      62,   277,    25,   115,   155,   112,   112,    25,   258,   259,
     115,   115,   144,   274,   279,    90,   178,   226,   279,   278,
     236,   115,   257,   115,   117,   118,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   130,   132,   133,   254,   122,
     124,   276,   209,   147,    25,    80,   116,   118,   129,   239,
     239,   212,   276,   257,   277,   112,    25,   164,   165,    25,
     159,   160,   120,   119,   123,    83,    84,    85,    86,    87,
      88,    89,   104,   265,   266,   267,   274,   275,   225,   257,
     119,   128,   128,   257,   265,   114,   116,   120,   120,   115,
     120,   120,   120,   120,   120,   120,   120,   127,   120,   128,
     131,   120,   120,   115,   112,   150,    25,   117,   214,   225,
     240,   241,   257,   257,   112,   114,    25,   139,   156,   235,
     113,   165,   113,   160,    25,    27,    90,   177,   260,   259,
     214,   214,   214,   214,   214,   115,   115,   214,   116,   119,
     214,   120,   220,   116,   112,   274,   149,   116,    10,   182,
     274,   120,   120,   265,    39,    40,    82,   151,   152,   153,
     188,   211,    25,   119,   238,   232,    25,   166,   169,   215,
     216,   217,   116,   113,   139,   129,   257,   257,   257,   257,
     257,   265,   265,   257,    81,   255,   267,   257,   117,   118,
     121,   126,   221,   223,   224,   188,   189,   150,   255,   209,
      11,   183,   116,   116,   209,   209,   274,   113,   152,   241,
     112,   242,   242,   120,   219,   113,   217,   115,   114,   178,
     220,   220,   220,   116,   116,   103,   280,   118,   121,   122,
     123,   124,   125,   222,    27,    90,    91,    98,    99,   100,
     101,   225,   227,   278,   106,   145,   280,   209,    12,   184,
     114,   255,    25,   114,     6,    18,    25,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    40,    41,    54,    55,
      56,    57,    75,    82,   107,   143,   154,   166,   169,   188,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   200,
     211,   229,   230,   236,   243,   244,   246,   247,   249,   252,
     253,   261,   268,   270,   271,   273,   114,   114,   224,   257,
     114,   279,   118,   214,   214,   115,   120,   256,   224,   115,
     209,   113,   256,   209,   280,   115,   209,   209,   115,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
      58,   245,   245,   245,   129,   272,   269,   248,   113,   244,
     126,   119,   218,   116,   178,   257,   257,   225,   281,    90,
     257,   221,   228,   114,   257,   256,   265,   265,   129,   129,
     129,   253,   274,    75,   270,    25,   249,    25,   257,   116,
     119,   130,   251,   116,   119,   251,   257,   116,   116,    25,
     115,   114,   225,   274,   114,   221,   114,   251,   255,   280,
     116,   115,    38,   263,   261,   262,   114,   280,   257,   280,
     265,   209,    28,   264,   263,   263,   257,   130,   250,   256,
     116,   209,   264,   264,   251,   115,   114,   257,   131,   114,
     265,   262,   114,   262,   116,   263,   263,   263,   131,   264
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

/* Line 1455 of yacc.c  */
#line 350 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /*
             * We don't do these in parserEOF() because the parser is reading
             * ahead and that would be too early.
             */

            if (previousFile != NULL)
            {
                handleEOF();

                if (currentContext.prevmod != NULL)
                    handleEOM();

                free(previousFile);
                previousFile = NULL;
            }
    }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 385 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentSpec->exphdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 389 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 393 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->cppcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 421 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope == NULL)
                    yyerror("%TypeHeaderCode can only be used in a namespace, class or mapped type");

                appendCodeBlock(&scope->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
            }
        }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 434 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if ((currentModule->encoding = convertEncoding((yyvsp[(2) - (2)].text))) == no_type)
                    yyerror("The value of %DefaultEncoding must be one of \"ASCII\", \"Latin-1\", \"UTF-8\" or \"None\"");
            }
        }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 443 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            appendString(&currentSpec->plugins, (yyvsp[(2) - (2)].text));
        }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 448 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                apiVersionRangeDef *avd;

                if (findAPI(currentSpec, (yyvsp[(2) - (3)].text)) != NULL)
                    yyerror("The API name in the %API directive has already been defined");

                if ((yyvsp[(3) - (3)].number) < 1)
                    yyerror("The version number in the %API directive must be greater than or equal to 1");

                avd = sipMalloc(sizeof (apiVersionRangeDef));

                avd->api_name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));
                avd->from = (yyvsp[(3) - (3)].number);
                avd->to = -1;

                avd->next = currentModule->api_versions;
                currentModule->api_versions = avd;

                if (inMainModule())
                    setIsUsedName(avd->api_name);
            }
        }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 474 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                exceptionDef *xd;
                const char *pyname;

                if (currentSpec->genc)
                    yyerror("%Exception not allowed in a C module");

                pyname = getPythonName(&(yyvsp[(4) - (9)].optflags), scopedNameTail((yyvsp[(2) - (9)].scpvalp)));

                checkAttributes(currentSpec, currentModule, NULL, NULL,
                        pyname, FALSE);

                xd = findException(currentSpec, (yyvsp[(2) - (9)].scpvalp), TRUE);

                if (xd->cd != NULL)
                    yyerror("%Exception name has already been seen as a class name - it must be defined before being used");

                if (xd->iff->module != NULL)
                    yyerror("The %Exception has already been defined");

                /* Complete the definition. */
                xd->iff->module = currentModule;
                xd->iff->hdrcode = (yyvsp[(6) - (9)].codeb);
                xd->pyname = pyname;
                xd->bibase = (yyvsp[(3) - (9)].exceptionbase).bibase;
                xd->base = (yyvsp[(3) - (9)].exceptionbase).base;
                xd->raisecode = (yyvsp[(7) - (9)].codeb);

                if (findOptFlag(&(yyvsp[(4) - (9)].optflags), "Default", bool_flag) != NULL)
                    currentModule->defexception = xd;

                if (xd->bibase != NULL || xd->base != NULL)
                    xd->exceptionnr = currentModule->nrexceptions++;
            }
        }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 513 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.exceptionbase).bibase = NULL;
            (yyval.exceptionbase).base = NULL;
        }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 517 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            exceptionDef *xd;

            (yyval.exceptionbase).bibase = NULL;
            (yyval.exceptionbase).base = NULL;

            /* See if it is a defined exception. */
            for (xd = currentSpec->exceptions; xd != NULL; xd = xd->next)
                if (compareScopedNames(xd->iff->fqcname, (yyvsp[(2) - (3)].scpvalp)) == 0)
                {
                    (yyval.exceptionbase).base = xd;
                    break;
                }

            if (xd == NULL && (yyvsp[(2) - (3)].scpvalp)->next == NULL && strncmp((yyvsp[(2) - (3)].scpvalp)->name, "SIP_", 4) == 0)
            {
                /* See if it is a builtin exception. */

                static char *builtins[] = {
                    "Exception",
                    "StopIteration",
                    "StandardError",
                    "ArithmeticError",
                    "LookupError",
                    "AssertionError",
                    "AttributeError",
                    "EOFError",
                    "FloatingPointError",
                    "EnvironmentError",
                    "IOError",
                    "OSError",
                    "ImportError",
                    "IndexError",
                    "KeyError",
                    "KeyboardInterrupt",
                    "MemoryError",
                    "NameError",
                    "OverflowError",
                    "RuntimeError",
                    "NotImplementedError",
                    "SyntaxError",
                    "IndentationError",
                    "TabError",
                    "ReferenceError",
                    "SystemError",
                    "SystemExit",
                    "TypeError",
                    "UnboundLocalError",
                    "UnicodeError",
                    "UnicodeEncodeError",
                    "UnicodeDecodeError",
                    "UnicodeTranslateError",
                    "ValueError",
                    "ZeroDivisionError",
                    "WindowsError",
                    "VMSError",
                    NULL
                };

                char **cp;

                for (cp = builtins; *cp != NULL; ++cp)
                    if (strcmp((yyvsp[(2) - (3)].scpvalp)->name + 4, *cp) == 0)
                    {
                        (yyval.exceptionbase).bibase = *cp;
                        break;
                    }
            }

            if ((yyval.exceptionbase).bibase == NULL && (yyval.exceptionbase).base == NULL)
                yyerror("Unknown exception base type");
        }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 591 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 596 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                currentMappedType = newMappedType(currentSpec, &(yyvsp[(2) - (3)].memArg), &(yyvsp[(3) - (3)].optflags));
        }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 602 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            int a;

            if (currentSpec->genc)
                yyerror("%MappedType templates not allowed in a C module");

            /* Check the template arguments are basic types or simple names. */
            for (a = 0; a < (yyvsp[(1) - (4)].signature).nrArgs; ++a)
            {
                argDef *ad = &(yyvsp[(1) - (4)].signature).args[a];

                if (ad->atype == defined_type && ad->u.snd->next != NULL)
                    yyerror("%MappedType template arguments must be simple names");
            }

            if ((yyvsp[(3) - (4)].memArg).atype != template_type)
                yyerror("%MappedType template must map a template type");

            if (notSkipping())
            {
                mappedTypeTmplDef *mtt;
                ifaceFileDef *iff;

                /* Check a template hasn't already been provided. */
                for (mtt = currentSpec->mappedtypetemplates; mtt != NULL; mtt = mtt->next)
                    if (compareScopedNames(mtt->mt->type.u.td->fqname, (yyvsp[(3) - (4)].memArg).u.td->fqname) == 0 && sameTemplateSignature(&mtt->mt->type.u.td->types, &(yyvsp[(3) - (4)].memArg).u.td->types, TRUE))
                        yyerror("%MappedType template for this type has already been defined");

                (yyvsp[(3) - (4)].memArg).nrderefs = 0;
                (yyvsp[(3) - (4)].memArg).argflags = 0;

                mtt = sipMalloc(sizeof (mappedTypeTmplDef));

                mtt->sig = (yyvsp[(1) - (4)].signature);
                mtt->mt = allocMappedType(currentSpec, &(yyvsp[(3) - (4)].memArg));
                mtt->mt->doctype = getDocType(&(yyvsp[(4) - (4)].optflags));
                mtt->next = currentSpec->mappedtypetemplates;

                currentSpec->mappedtypetemplates = mtt;

                currentMappedType = mtt->mt;

                /* Create a dummy interface file. */
                iff = sipMalloc(sizeof (ifaceFileDef));
                iff->hdrcode = NULL;
                mtt->mt->iff = iff;
            }
        }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 652 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType->convfromcode == NULL)
                    yyerror("%MappedType must have a %ConvertFromTypeCode directive");

                if (currentMappedType->convtocode == NULL)
                    yyerror("%MappedType must have a %ConvertToTypeCode directive");

                currentMappedType = NULL;
            }
        }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 670 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentMappedType->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 674 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType -> convfromcode != NULL)
                    yyerror("%MappedType has more than one %ConvertFromTypeCode directive");

                currentMappedType -> convfromcode = (yyvsp[(2) - (2)].codeb);
            }
        }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 683 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentMappedType -> convtocode != NULL)
                    yyerror("%MappedType has more than one %ConvertToTypeCode directive");

                currentMappedType -> convtocode = (yyvsp[(2) - (2)].codeb);
            }
        }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 696 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                applyTypeFlags(currentModule, &(yyvsp[(2) - (13)].memArg), &(yyvsp[(9) - (13)].optflags));

                (yyvsp[(5) - (13)].signature).result = (yyvsp[(2) - (13)].memArg);

                newFunction(currentSpec, currentModule, NULL,
                        currentMappedType, 0, TRUE, FALSE, (yyvsp[(3) - (13)].text), &(yyvsp[(5) - (13)].signature), (yyvsp[(7) - (13)].number), FALSE,
                        &(yyvsp[(9) - (13)].optflags), (yyvsp[(13) - (13)].codeb), NULL, (yyvsp[(8) - (13)].throwlist), (yyvsp[(10) - (13)].optsignature), (yyvsp[(12) - (13)].codeb));
            }
        }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 710 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("namespace definition not allowed in a C module");

            if (notSkipping())
            {
                classDef *ns, *c_scope;
                ifaceFileDef *scope;

                if ((c_scope = currentScope()) != NULL)
                    scope = c_scope->iff;
                else
                    scope = NULL;

                ns = newClass(currentSpec, namespace_iface, NULL,
                        text2scopedName(scope, (yyvsp[(2) - (2)].text)));

                pushScope(ns);

                sectionFlags = 0;
            }
        }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 731 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (inMainModule())
                {
                    classDef *ns = currentScope();

                    setIsUsedName(ns->iff->name);
                    setIsUsedName(ns->pyname);
                }

                popScope();
            }
        }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 751 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            qualDef *qd;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == platform_qualifier)
                    yyerror("%Platforms has already been defined for this module");
        }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 758 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            qualDef *qd;
            int nrneeded;

            /*
             * Check that exactly one platform in the set was
             * requested.
             */

            nrneeded = 0;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == platform_qualifier && isNeeded(qd))
                    ++nrneeded;

            if (nrneeded > 1)
                yyerror("No more than one of these %Platforms must be specified with the -t flag");
        }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 782 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            newQualifier(currentModule,-1,-1,(yyvsp[(1) - (1)].text),platform_qualifier);
        }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 787 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            newQualifier(currentModule,-1,-1,(yyvsp[(2) - (2)].text),feature_qualifier);
        }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 792 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            currentTimelineOrder = 0;
        }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 795 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            qualDef *qd;
            int nrneeded;

            /*
             * Check that exactly one time slot in the set was
             * requested.
             */

            nrneeded = 0;

            for (qd = currentModule -> qualifiers; qd != NULL; qd = qd -> next)
                if (qd -> qtype == time_qualifier && isNeeded(qd))
                    ++nrneeded;

            if (nrneeded > 1)
                yyerror("At most one of this %Timeline must be specified with the -t flag");

            currentModule -> nrtimelines++;
        }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 821 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            newQualifier(currentModule,currentModule -> nrtimelines,currentTimelineOrder++,(yyvsp[(1) - (1)].text),time_qualifier);
        }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 826 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (skipStackPtr >= MAX_NESTED_IF)
                yyerror("Internal error: increase the value of MAX_NESTED_IF");

            /* Nested %Ifs are implicit logical ands. */

            if (skipStackPtr > 0)
                (yyvsp[(3) - (4)].boolean) = ((yyvsp[(3) - (4)].boolean) && skipStack[skipStackPtr - 1]);

            skipStack[skipStackPtr++] = (yyvsp[(3) - (4)].boolean);
        }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 839 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(1) - (1)].text),FALSE);
        }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 842 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = platOrFeature((yyvsp[(2) - (2)].text),TRUE);
        }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 845 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(3) - (3)].text),FALSE) || (yyvsp[(1) - (3)].boolean));
        }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 848 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = (platOrFeature((yyvsp[(4) - (4)].text),TRUE) || (yyvsp[(1) - (4)].boolean));
        }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 854 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = timePeriod((yyvsp[(1) - (3)].text),(yyvsp[(3) - (3)].text));
        }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 859 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (skipStackPtr-- <= 0)
                yyerror("Too many %End directives");
        }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 865 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            optFlag *of;

            if ((yyvsp[(2) - (2)].optflags).nrFlags == 0)
                yyerror("%License details not specified");

            if ((of = findOptFlag(&(yyvsp[(2) - (2)].optflags),"Type",string_flag)) == NULL)
                yyerror("%License type not specified");

            currentModule -> license = sipMalloc(sizeof (licenseDef));

            currentModule -> license -> type = of -> fvalue.sval;

            currentModule -> license -> licensee = 
                ((of = findOptFlag(&(yyvsp[(2) - (2)].optflags),"Licensee",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;

            currentModule -> license -> timestamp = 
                ((of = findOptFlag(&(yyvsp[(2) - (2)].optflags),"Timestamp",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;

            currentModule -> license -> sig = 
                ((of = findOptFlag(&(yyvsp[(2) - (2)].optflags),"Signature",string_flag)) != NULL)
                    ? of -> fvalue.sval : NULL;
        }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 892 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentModule->defmetatype != NULL)
                    yyerror("%DefaultMetatype has already been defined for this module");

                currentModule->defmetatype = cacheName(currentSpec, (yyvsp[(2) - (2)].text));
            }
        }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 903 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentModule->defsupertype != NULL)
                    yyerror("%DefaultSupertype has already been defined for this module");

                currentModule->defsupertype = cacheName(currentSpec, (yyvsp[(2) - (2)].text));
            }
        }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 914 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Make sure this is the first mention of a module. */
            if (currentSpec->module != currentModule)
                yyerror("A %ConsolidatedModule cannot be %Imported");

            if (currentModule->fullname != NULL)
                yyerror("%ConsolidatedModule must appear before any %Module or %CModule directive");

            setModuleName(currentSpec, currentModule, (yyvsp[(2) - (2)].text));
            setIsConsolidated(currentModule);
        }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 927 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Make sure this is the first mention of a module. */
            if (currentSpec->module != currentModule)
                yyerror("A %CompositeModule cannot be %Imported");

            if (currentModule->fullname != NULL)
                yyerror("%CompositeModule must appear before any %Module or %CModule directive");

            setModuleName(currentSpec, currentModule, (yyvsp[(2) - (2)].text));
            setIsComposite(currentModule);
        }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 940 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check the module hasn't already been defined. */

            moduleDef *mod;

            for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
                if (mod->fullname != NULL && strcmp(mod->fullname->text, (yyvsp[(2) - (3)].text)) == 0)
                    yyerror("Module is already defined");

            /*
             * If we are in a container module then create a component module
             * and make it current.
             */
            if (isContainer(currentModule) || currentModule->container != NULL)
            {
                mod = allocModule();

                mod->file = currentContext.filename;
                mod->container = (isContainer(currentModule) ? currentModule : currentModule->container);

                currentModule = mod;
            }

            setModuleName(currentSpec, currentModule, (yyvsp[(2) - (3)].text));
            currentModule->version = (yyvsp[(3) - (3)].number);

            if (currentSpec->genc < 0)
                currentSpec->genc = (yyvsp[(1) - (3)].boolean);
            else if (currentSpec->genc != (yyvsp[(1) - (3)].boolean))
                yyerror("Cannot mix C and C++ modules");
        }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 973 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 976 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 982 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /*
             * The grammar design is a bit broken and this is the easiest way
             * to allow periods in names.
             */

            char *cp;

            for (cp = (yyvsp[(1) - (1)].text); *cp != '\0'; ++cp)
                if (*cp != '.' && *cp != '_' && !isalnum(*cp))
                    yyerror("Invalid character in name");

            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 998 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = -1;
        }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1004 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            parseFile(NULL, (yyvsp[(2) - (2)].text), NULL, FALSE);
        }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1009 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            parseFile(NULL, (yyvsp[(2) - (2)].text), NULL, TRUE);
        }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1014 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            newImport((yyvsp[(2) - (2)].text));
        }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1019 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1022 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1027 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1030 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1035 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1038 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1043 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            appendCodeBlock(&currentModule->copying, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1048 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1053 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1058 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1063 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1069 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1074 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1079 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1084 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1089 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1094 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1099 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1104 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1109 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1114 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1119 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1124 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->preinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1130 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->initcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1136 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->postinitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1142 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentModule->unitcode, (yyvsp[(2) - (2)].codeb));
        }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1148 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /*
             * This is a no-op and is retained for compatibility
             * until the last use of it (by SIP v3) can be removed
             * from PyQt.
             */
        }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1157 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (inMainModule())
                appendCodeBlock(&currentSpec -> docs,(yyvsp[(2) - (2)].codeb));
        }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1163 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            appendCodeBlock(&currentSpec -> docs,(yyvsp[(2) - (2)].codeb));
        }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1168 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (inMainModule())
                yywarning("%Makefile is ignored, please use the -b flag instead");
        }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1178 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(1) - (2)].codeb);

            append(&(yyval.codeb)->frag, (yyvsp[(2) - (2)].codeb)->frag);

            free((yyvsp[(2) - (2)].codeb)->frag);
            free((char *)(yyvsp[(2) - (2)].codeb)->filename);
            free((yyvsp[(2) - (2)].codeb));
        }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1189 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (sectionFlags != 0 && (sectionFlags & ~(SECT_IS_PUBLIC | SECT_IS_PROT)) != 0)
                    yyerror("Class enums must be in the public or protected sections");

                currentEnum = newEnum(currentSpec, currentModule,
                        currentMappedType, (yyvsp[(2) - (3)].text), &(yyvsp[(3) - (3)].optflags), sectionFlags);
            }
        }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1201 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.text) = NULL;
        }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1204 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1209 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.text) = NULL;
        }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1212 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.text) = (yyvsp[(1) - (1)].text);
        }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1227 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                enumMemberDef *emd, **tail;

                /* Note that we don't use the assigned value. */
                emd = sipMalloc(sizeof (enumMemberDef));

                emd -> pyname = cacheName(currentSpec, getPythonName(&(yyvsp[(3) - (4)].optflags), (yyvsp[(1) - (4)].text)));
                emd -> cname = (yyvsp[(1) - (4)].text);
                emd -> ed = currentEnum;
                emd -> next = NULL;

                checkAttributes(currentSpec, currentModule, emd->ed->ecd,
                        emd->ed->emtd, emd->pyname->text, FALSE);

                /* Append to preserve the order. */
                for (tail = &currentEnum->members; *tail != NULL; tail = &(*tail)->next)
                    ;

                *tail = emd;

                if (inMainModule())
                    setIsUsedName(emd -> pyname);
            }
        }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1263 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.valp) = NULL;
        }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1266 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.valp) = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1272 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            valueDef *vd;
 
            if ((yyvsp[(1) - (3)].valp) -> vtype == string_value || (yyvsp[(3) - (3)].valp) -> vtype == string_value)
                yyerror("Invalid binary operator for string");
 
            /* Find the last value in the existing expression. */
 
            for (vd = (yyvsp[(1) - (3)].valp); vd -> next != NULL; vd = vd -> next)
                ;
 
            vd -> vbinop = (yyvsp[(2) - (3)].qchar);
            vd -> next = (yyvsp[(3) - (3)].valp);

            (yyval.valp) = (yyvsp[(1) - (3)].valp);
        }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1290 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1293 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1296 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '*';
        }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1299 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '/';
        }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1302 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '&';
        }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1305 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '|';
        }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1310 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '\0';
        }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1313 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '!';
        }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1316 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '~';
        }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1319 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '-';
        }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1322 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.qchar) = '+';
        }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1327 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if ((yyvsp[(1) - (2)].qchar) != '\0' && (yyvsp[(2) - (2)].value).vtype == string_value)
                yyerror("Invalid unary operator for string");
 
            /*
             * Convert the value to a simple expression on the
             * heap.
             */
 
            (yyval.valp) = sipMalloc(sizeof (valueDef));
 
            *(yyval.valp) = (yyvsp[(2) - (2)].value);
            (yyval.valp) -> vunop = (yyvsp[(1) - (2)].qchar);
            (yyval.valp) -> vbinop = '\0';
            (yyval.valp) -> next = NULL;
        }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1346 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("Scoped names are not allowed in a C module");

            appendScopedName(&(yyvsp[(1) - (3)].scpvalp),(yyvsp[(3) - (3)].scpvalp));
        }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1354 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.scpvalp) = text2scopePart((yyvsp[(1) - (1)].text));
        }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1359 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /*
             * We let the C++ compiler decide if the value is a valid one - no
             * point in building a full C++ parser here.
             */

            (yyval.value).vtype = scoped_value;
            (yyval.value).u.vscp = (yyvsp[(1) - (1)].scpvalp);
        }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1368 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            fcallDef *fcd;

            fcd = sipMalloc(sizeof (fcallDef));
            *fcd = (yyvsp[(3) - (4)].fcall);
            fcd -> type = (yyvsp[(1) - (4)].memArg);

            (yyval.value).vtype = fcall_value;
            (yyval.value).u.fcd = fcd;
        }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1378 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = real_value;
            (yyval.value).u.vreal = (yyvsp[(1) - (1)].real);
        }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1382 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = (yyvsp[(1) - (1)].number);
        }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1386 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = 1;
        }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1390 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = 0;
        }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1394 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = numeric_value;
            (yyval.value).u.vnum = 0;
        }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1398 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = string_value;
            (yyval.value).u.vstr = (yyvsp[(1) - (1)].text);
        }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1402 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.value).vtype = qchar_value;
            (yyval.value).u.vqchar = (yyvsp[(1) - (1)].qchar);
        }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1408 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* No values. */

            (yyval.fcall).nrArgs = 0;
        }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1413 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* The single or first expression. */

            (yyval.fcall).args[0] = (yyvsp[(1) - (1)].valp);
            (yyval.fcall).nrArgs = 1;
        }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1419 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check that it wasn't ...(,expression...). */

            if ((yyval.fcall).nrArgs == 0)
                yyerror("First argument to function call is missing");

            /* Check there is room. */

            if ((yyvsp[(1) - (3)].fcall).nrArgs == MAX_NR_ARGS)
                yyerror("Too many arguments to function call");

            (yyval.fcall) = (yyvsp[(1) - (3)].fcall);

            (yyval.fcall).args[(yyval.fcall).nrArgs] = (yyvsp[(3) - (3)].valp);
            (yyval.fcall).nrArgs++;
        }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1437 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                applyTypeFlags(currentModule, &(yyvsp[(2) - (5)].memArg), &(yyvsp[(4) - (5)].optflags));
                newTypedef(currentSpec, currentModule, (yyvsp[(3) - (5)].text), &(yyvsp[(2) - (5)].memArg), &(yyvsp[(4) - (5)].optflags));
            }
        }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1444 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                signatureDef *sig;
                argDef ftype;

                applyTypeFlags(currentModule, &(yyvsp[(2) - (11)].memArg), &(yyvsp[(10) - (11)].optflags));

                memset(&ftype, 0, sizeof (argDef));

                /* Create the full signature on the heap. */
                sig = sipMalloc(sizeof (signatureDef));
                *sig = (yyvsp[(8) - (11)].signature);
                sig->result = (yyvsp[(2) - (11)].memArg);

                /* Create the full type. */
                ftype.atype = function_type;
                ftype.nrderefs = (yyvsp[(4) - (11)].number);
                ftype.u.sa = sig;

                newTypedef(currentSpec, currentModule, (yyvsp[(5) - (11)].text), &ftype, &(yyvsp[(10) - (11)].optflags));
            }
        }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1469 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc && (yyvsp[(2) - (2)].scpvalp)->next != NULL)
                yyerror("Namespaces not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1475 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                if (currentSpec->genc && currentSupers != NULL)
                    yyerror("Super-classes not allowed in a C module struct");

                defineClass((yyvsp[(2) - (5)].scpvalp), currentSupers, &(yyvsp[(5) - (5)].optflags));
                sectionFlags = SECT_IS_PUBLIC;
            }
        }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1484 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1490 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {currentIsTemplate = TRUE;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1490 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Class templates not allowed in a C module");

            if (notSkipping())
            {
                classTmplDef *tcd;

                /*
                 * Make sure there is room for the extra class name argument.
                 */
                if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                    yyerror("Internal error - increase the value of MAX_NR_ARGS");

                tcd = sipMalloc(sizeof (classTmplDef));
                tcd->sig = (yyvsp[(1) - (3)].signature);
                tcd->cd = (yyvsp[(3) - (3)].klass);
                tcd->next = currentSpec->classtemplates;

                currentSpec->classtemplates = tcd;
            }

            currentIsTemplate = FALSE;
        }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1516 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.signature) = (yyvsp[(3) - (4)].signature);
        }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1521 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Class definition not allowed in a C module");

            if (notSkipping())
                currentSupers = NULL;
        }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1527 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                defineClass((yyvsp[(2) - (5)].scpvalp), currentSupers, &(yyvsp[(5) - (5)].optflags));
                sectionFlags = SECT_IS_PRIVATE;
            }
        }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1533 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                (yyval.klass) = completeClass((yyvsp[(2) - (8)].scpvalp), &(yyvsp[(5) - (8)].optflags), (yyvsp[(7) - (8)].boolean));
        }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1547 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                argDef ad;
                classDef *super;
                scopedNameDef *snd = (yyvsp[(1) - (1)].scpvalp);

                /*
                 * This is a hack to allow typedef'ed classes to be used before
                 * we have resolved the typedef definitions.  Unlike elsewhere,
                 * we require that the typedef is defined before being used.
                 */
                for (;;)
                {
                    ad.atype = no_type;
                    ad.argflags = 0;
                    ad.nrderefs = 0;
                    ad.original_type = NULL;

                    searchTypedefs(currentSpec, snd, &ad);

                    if (ad.atype != defined_type)
                        break;

                    if (ad.nrderefs != 0 || isConstArg(&ad) || isReference(&ad))
                        break;

                    snd = ad.u.snd;
                }

                if (ad.atype != no_type)
                    yyerror("Super-class list contains an invalid type");

                super = findClass(currentSpec, class_iface, NULL, snd);
                appendToClassList(&currentSupers, super);
            }
        }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1586 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = FALSE;
        }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1589 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.boolean) = TRUE;
        }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1606 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                /* Make sure this is before any ctor docstrings. */
                (yyvsp[(1) - (1)].codeb)->next = scope->docstring;
                scope->docstring = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1616 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->cppcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1620 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
                appendCodeBlock(&currentScope()->iff->hdrcode, (yyvsp[(1) - (1)].codeb));
        }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1624 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->travcode != NULL)
                    yyerror("%GCTraverseCode already given for class");

                scope->travcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1635 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->clearcode != NULL)
                    yyerror("%GCClearCode already given for class");

                scope->clearcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1646 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->getbufcode != NULL)
                    yyerror("%BIGetBufferCode already given for class");

                scope->getbufcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1657 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->releasebufcode != NULL)
                    yyerror("%BIReleaseBufferCode already given for class");

                scope->releasebufcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1668 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->readbufcode != NULL)
                    yyerror("%BIGetReadBufferCode already given for class");

                scope->readbufcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1679 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->writebufcode != NULL)
                    yyerror("%BIGetWriteBufferCode already given for class");

                scope->writebufcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1690 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->segcountcode != NULL)
                    yyerror("%BIGetSegCountCode already given for class");

                scope->segcountcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1701 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->charbufcode != NULL)
                    yyerror("%BIGetCharBufferCode already given for class");

                scope->charbufcode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1712 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->picklecode != NULL)
                    yyerror("%PickleCode already given for class");

                scope->picklecode = (yyvsp[(1) - (1)].codeb);
            }
        }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1726 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtosubcode != NULL)
                    yyerror("Class has more than one %ConvertToSubClassCode directive");

                scope->convtosubcode = (yyvsp[(2) - (2)].codeb);
            }
        }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1737 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *scope = currentScope();

                if (scope->convtocode != NULL)
                    yyerror("Class has more than one %ConvertToTypeCode directive");

                scope->convtocode = (yyvsp[(2) - (2)].codeb);
            }
        }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1748 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("public section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PUBLIC | (yyvsp[(2) - (3)].number);
        }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1755 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("protected section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PROT | (yyvsp[(2) - (3)].number);
        }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1762 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("private section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_PRIVATE | (yyvsp[(2) - (3)].number);
        }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1769 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("signals section not allowed in a C module");

            if (notSkipping())
                sectionFlags = SECT_IS_SIGNAL;
        }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1778 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1781 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = SECT_IS_SLOT;
        }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1786 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Note that we allow non-virtual dtors in C modules. */

            if (notSkipping())
            {
                classDef *cd = currentScope();

                if (strcmp(classBaseName(cd),(yyvsp[(3) - (11)].text)) != 0)
                    yyerror("Destructor doesn't have the same name as its class");

                if (isDtor(cd))
                    yyerror("Destructor has already been defined");

                if (currentSpec -> genc && (yyvsp[(10) - (11)].codeb) == NULL)
                    yyerror("Destructor in C modules must include %MethodCode");

                cd -> dealloccode = (yyvsp[(10) - (11)].codeb);
                cd -> dtorcode = (yyvsp[(11) - (11)].codeb);
                cd -> dtorexceptions = (yyvsp[(6) - (11)].throwlist);

                /*
                 * Note that we don't apply the protected/public hack to dtors
                 * as it (I think) may change the behaviour of the wrapped API.
                 */
                cd->classflags |= sectionFlags;

                if ((yyvsp[(7) - (11)].number))
                {
                    if (!(yyvsp[(1) - (11)].number))
                        yyerror("Abstract destructor must be virtual");

                    setIsAbstractClass(cd);
                }

                /*
                 * The class has a shadow if we have a virtual dtor or some
                 * dtor code.
                 */
                if ((yyvsp[(1) - (11)].number) || (yyvsp[(11) - (11)].codeb) != NULL)
                {
                    if (currentSpec -> genc)
                        yyerror("Virtual destructor or %VirtualCatcherCode not allowed in a C module");

                    setHasShadow(cd);
                }

                if (getReleaseGIL(&(yyvsp[(8) - (11)].optflags)))
                    setIsReleaseGILDtor(cd);
                else if (getHoldGIL(&(yyvsp[(8) - (11)].optflags)))
                    setIsHoldGILDtor(cd);
            }
        }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1840 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {currentCtorIsExplicit = TRUE;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1844 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Note that we allow ctors in C modules. */

            if (notSkipping())
            {
                if (currentSpec -> genc)
                {
                    if ((yyvsp[(10) - (10)].codeb) == NULL && (yyvsp[(3) - (10)].signature).nrArgs != 0)
                        yyerror("Constructors with arguments in C modules must include %MethodCode");

                    if (currentCtorIsExplicit)
                        yyerror("Explicit constructors not allowed in a C module");
                }

                if ((sectionFlags & (SECT_IS_PUBLIC | SECT_IS_PROT | SECT_IS_PRIVATE)) == 0)
                    yyerror("Constructor must be in the public, private or protected sections");

                newCtor((yyvsp[(1) - (10)].text), sectionFlags, &(yyvsp[(3) - (10)].signature), &(yyvsp[(6) - (10)].optflags), (yyvsp[(10) - (10)].codeb), (yyvsp[(5) - (10)].throwlist), (yyvsp[(7) - (10)].optsignature),
                        currentCtorIsExplicit, (yyvsp[(9) - (10)].codeb));
            }

            free((yyvsp[(1) - (10)].text));

            currentCtorIsExplicit = FALSE;
        }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1871 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1874 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(3) - (5)].signature);
        }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1881 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optsignature) = NULL;
        }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1884 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optsignature) = sipMalloc(sizeof (signatureDef));

            *(yyval.optsignature) = (yyvsp[(4) - (6)].signature);
            (yyval.optsignature) -> result = (yyvsp[(2) - (6)].memArg);
        }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1892 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1895 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1900 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                applyTypeFlags(currentModule, &(yyvsp[(1) - (14)].memArg), &(yyvsp[(9) - (14)].optflags));

                (yyvsp[(4) - (14)].signature).result = (yyvsp[(1) - (14)].memArg);

                newFunction(currentSpec, currentModule, currentScope(), NULL,
                        sectionFlags, currentIsStatic, currentOverIsVirt, (yyvsp[(2) - (14)].text),
                        &(yyvsp[(4) - (14)].signature), (yyvsp[(6) - (14)].number), (yyvsp[(8) - (14)].number), &(yyvsp[(9) - (14)].optflags), (yyvsp[(13) - (14)].codeb), (yyvsp[(14) - (14)].codeb), (yyvsp[(7) - (14)].throwlist), (yyvsp[(10) - (14)].optsignature), (yyvsp[(12) - (14)].codeb));
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1915 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /*
             * It looks like an assignment operator (though we don't bother to
             * check the types) so make sure it is private.
             */
            if (notSkipping())
            {
                classDef *cd = currentScope();

                if (cd == NULL || !(sectionFlags & SECT_IS_PRIVATE))
                    yyerror("Assignment operators may only be defined as private");

                setCannotAssign(cd);
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1933 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                classDef *cd = currentScope();

                applyTypeFlags(currentModule, &(yyvsp[(1) - (14)].memArg), &(yyvsp[(10) - (14)].optflags));

                /* Handle the unary '+' and '-' operators. */
                if ((cd != NULL && (yyvsp[(5) - (14)].signature).nrArgs == 0) || (cd == NULL && (yyvsp[(5) - (14)].signature).nrArgs == 1))
                {
                    if (strcmp((yyvsp[(3) - (14)].text), "__add__") == 0)
                        (yyvsp[(3) - (14)].text) = "__pos__";
                    else if (strcmp((yyvsp[(3) - (14)].text), "__sub__") == 0)
                        (yyvsp[(3) - (14)].text) = "__neg__";
                }

                (yyvsp[(5) - (14)].signature).result = (yyvsp[(1) - (14)].memArg);

                newFunction(currentSpec, currentModule, cd, NULL,
                        sectionFlags, currentIsStatic, currentOverIsVirt, (yyvsp[(3) - (14)].text),
                        &(yyvsp[(5) - (14)].signature), (yyvsp[(7) - (14)].number), (yyvsp[(9) - (14)].number), &(yyvsp[(10) - (14)].optflags), (yyvsp[(13) - (14)].codeb), (yyvsp[(14) - (14)].codeb), (yyvsp[(8) - (14)].throwlist), (yyvsp[(11) - (14)].optsignature), NULL);
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1959 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                char *sname;
                classDef *scope = currentScope();

                if (scope == NULL || (yyvsp[(4) - (13)].signature).nrArgs != 0)
                    yyerror("Operator casts must be specified in a class and have no arguments");

                applyTypeFlags(currentModule, &(yyvsp[(2) - (13)].memArg), &(yyvsp[(9) - (13)].optflags));

                switch ((yyvsp[(2) - (13)].memArg).atype)
                {
                case defined_type:
                    sname = NULL;
                    break;

                case bool_type:
                case cbool_type:
                case short_type:
                case ushort_type:
                case int_type:
                case cint_type:
                case uint_type:
                    sname = "__int__";
                    break;

                case long_type:
                case ulong_type:
                case longlong_type:
                case ulonglong_type:
                    sname = "__long__";
                    break;

                case float_type:
                case cfloat_type:
                case double_type:
                case cdouble_type:
                    sname = "__float__";
                    break;

                default:
                    yyerror("Unsupported operator cast");
                }

                if (sname != NULL)
                {
                    (yyvsp[(4) - (13)].signature).result = (yyvsp[(2) - (13)].memArg);

                    newFunction(currentSpec, currentModule, scope, NULL,
                            sectionFlags, currentIsStatic, currentOverIsVirt,
                            sname, &(yyvsp[(4) - (13)].signature), (yyvsp[(6) - (13)].number), (yyvsp[(8) - (13)].number), &(yyvsp[(9) - (13)].optflags), (yyvsp[(12) - (13)].codeb), (yyvsp[(13) - (13)].codeb), (yyvsp[(7) - (13)].throwlist), (yyvsp[(10) - (13)].optsignature), NULL);
                }
                else
                {
                    argList *al;

                    /* Check it doesn't already exist. */
                    for (al = scope->casts; al != NULL; al = al->next)
                        if (compareScopedNames((yyvsp[(2) - (13)].memArg).u.snd, al->arg.u.snd) == 0)
                            yyerror("This operator cast has already been specified in this class");

                    al = sipMalloc(sizeof (argList));
                    al->arg = (yyvsp[(2) - (13)].memArg);
                    al->next = scope->casts;

                    scope->casts = al;
                }
            }

            currentIsStatic = FALSE;
            currentOverIsVirt = FALSE;
        }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 2034 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__add__";}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 2035 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__sub__";}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 2036 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__mul__";}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 2037 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__div__";}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 2038 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__mod__";}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 2039 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__and__";}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 2040 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__or__";}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 2041 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__xor__";}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 2042 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__lshift__";}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 2043 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__rshift__";}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 2044 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__iadd__";}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 2045 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__isub__";}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 2046 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__imul__";}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 2047 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__idiv__";}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 2048 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__imod__";}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 2049 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__iand__";}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 2050 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__ior__";}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 2051 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__ixor__";}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 2052 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__ilshift__";}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 2053 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__irshift__";}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 2054 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__invert__";}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 2055 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__call__";}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 2056 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__getitem__";}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 2057 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__lt__";}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 2058 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__le__";}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 2059 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__eq__";}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 2060 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__ne__";}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 2061 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__gt__";}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 2062 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {(yyval.text) = "__ge__";}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 2065 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = FALSE;
        }
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 2068 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = TRUE;
        }
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 2073 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 2076 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if ((yyvsp[(2) - (2)].number) != 0)
                yyerror("Abstract virtual function '= 0' expected");

            (yyval.number) = TRUE;
        }
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 2084 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optflags).nrFlags = 0;
        }
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 2087 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optflags) = (yyvsp[(2) - (3)].optflags);
        }
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 2093 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.optflags).flags[0] = (yyvsp[(1) - (1)].flag);
            (yyval.optflags).nrFlags = 1;
        }
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 2097 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check there is room. */

            if ((yyvsp[(1) - (3)].optflags).nrFlags == MAX_NR_FLAGS)
                yyerror("Too many optional flags");

            (yyval.optflags) = (yyvsp[(1) - (3)].optflags);

            (yyval.optflags).flags[(yyval.optflags).nrFlags++] = (yyvsp[(3) - (3)].flag);
        }
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 2109 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.flag).ftype = bool_flag;
            (yyval.flag).fname = (yyvsp[(1) - (1)].text);
        }
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 2113 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.flag) = (yyvsp[(3) - (3)].flag);
            (yyval.flag).fname = (yyvsp[(1) - (3)].text);
        }
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 2119 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.flag).ftype = (strchr((yyvsp[(1) - (1)].text), '.') != NULL) ? dotted_name_flag : name_flag;
            (yyval.flag).fvalue.sval = (yyvsp[(1) - (1)].text);
        }
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 2123 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            apiVersionRangeDef *avd;
            int from, to;

            (yyval.flag).ftype = api_range_flag;

            /* Check that the API is known. */
            if ((avd = findAPI(currentSpec, (yyvsp[(1) - (5)].text))) == NULL)
                yyerror("unknown API name in API annotation");

            if (inMainModule())
                setIsUsedName(avd->api_name);

            /* Unbounded values are represented by 0. */
            if ((from = (yyvsp[(3) - (5)].number)) < 0)
                from = 0;

            if ((to = (yyvsp[(5) - (5)].number)) < 0)
                to = 0;

            (yyval.flag).fvalue.aval = convertAPIRange(currentModule, avd->api_name,
                    from, to);
        }
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 2146 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.flag).ftype = string_flag;
            (yyval.flag).fvalue.sval = convertFeaturedString((yyvsp[(1) - (1)].text));
        }
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 2150 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.flag).ftype = integer_flag;
            (yyval.flag).fvalue.ival = (yyvsp[(1) - (1)].number);
        }
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 2156 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 2161 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 2167 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 2170 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 2175 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = NULL;
        }
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 2178 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.codeb) = (yyvsp[(2) - (2)].codeb);
        }
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 2183 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            int a, nrrxcon, nrrxdis, nrslotcon, nrslotdis, nrarray, nrarraysize;

            nrrxcon = nrrxdis = nrslotcon = nrslotdis = nrarray = nrarraysize = 0;

            for (a = 0; a < (yyvsp[(1) - (1)].signature).nrArgs; ++a)
            {
                argDef *ad = &(yyvsp[(1) - (1)].signature).args[a];

                switch (ad -> atype)
                {
                case rxcon_type:
                    ++nrrxcon;
                    break;

                case rxdis_type:
                    ++nrrxdis;
                    break;

                case slotcon_type:
                    ++nrslotcon;
                    break;

                case slotdis_type:
                    ++nrslotdis;
                    break;
                }

                if (isArray(ad))
                    ++nrarray;

                if (isArraySize(ad))
                    ++nrarraysize;
            }

            if (nrrxcon != nrslotcon || nrrxcon > 1)
                yyerror("SIP_RXOBJ_CON and SIP_SLOT_CON must both be given and at most once");

            if (nrrxdis != nrslotdis || nrrxdis > 1)
                yyerror("SIP_RXOBJ_DIS and SIP_SLOT_DIS must both be given and at most once");

            if (nrarray != nrarraysize || nrarray > 1)
                yyerror("/Array/ and /ArraySize/ must both be given and at most once");

            (yyval.signature) = (yyvsp[(1) - (1)].signature);
        }
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 2231 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* No arguments. */

            (yyval.signature).nrArgs = 0;
        }
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 2236 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* The single or first argument. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 2242 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check that it wasn't ...(,arg...). */
            if ((yyvsp[(1) - (3)].signature).nrArgs == 0)
                yyerror("First argument of the list is missing");

            /* Check there is nothing after an ellipsis. */
            if ((yyvsp[(1) - (3)].signature).args[(yyvsp[(1) - (3)].signature).nrArgs - 1].atype == ellipsis_type)
                yyerror("An ellipsis must be at the end of the argument list");

            /*
             * If this argument has no default value, then the
             * previous one mustn't either.
             */
            if ((yyvsp[(3) - (3)].memArg).defval == NULL && (yyvsp[(1) - (3)].signature).args[(yyvsp[(1) - (3)].signature).nrArgs - 1].defval != NULL)
                yyerror("Compulsory argument given after optional argument");

            /* Check there is room. */
            if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.signature) = (yyvsp[(1) - (3)].signature);

            (yyval.signature).args[(yyval.signature).nrArgs] = (yyvsp[(3) - (3)].memArg);
            (yyval.signature).nrArgs++;
        }
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 2269 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = signal_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (4)].text));
            (yyval.memArg).defval = (yyvsp[(4) - (4)].valp);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 2278 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = slot_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (4)].text));
            (yyval.memArg).defval = (yyvsp[(4) - (4)].valp);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 2287 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = anyslot_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (4)].text));
            (yyval.memArg).defval = (yyvsp[(4) - (4)].valp);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 2296 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = rxcon_type;
            (yyval.memArg).argflags = 0;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags), "SingleShot", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_SINGLE_SHOT;

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 2307 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = rxdis_type;
            (yyval.memArg).argflags = 0;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 2315 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = slotcon_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(5) - (6)].text));

            memset(&(yyvsp[(3) - (6)].signature).result, 0, sizeof (argDef));
            (yyvsp[(3) - (6)].signature).result.atype = void_type;

            (yyval.memArg).u.sa = sipMalloc(sizeof (signatureDef));
            *(yyval.memArg).u.sa = (yyvsp[(3) - (6)].signature);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 2329 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = slotdis_type;
            (yyval.memArg).argflags = ARG_IS_CONST;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(5) - (6)].text));

            memset(&(yyvsp[(3) - (6)].signature).result, 0, sizeof (argDef));
            (yyvsp[(3) - (6)].signature).result.atype = void_type;

            (yyval.memArg).u.sa = sipMalloc(sizeof (signatureDef));
            *(yyval.memArg).u.sa = (yyvsp[(3) - (6)].signature);

            currentSpec -> sigslots = TRUE;
        }
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 2343 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg).atype = qobject_type;
            (yyval.memArg).argflags = 0;
            (yyval.memArg).nrderefs = 0;
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));
        }
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 2349 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (2)].memArg);
            (yyval.memArg).defval = (yyvsp[(2) - (2)].valp);
        }
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 2355 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {currentIsStatic = TRUE;}
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 2363 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {currentOverIsVirt = TRUE;}
    break;

  case 325:

/* Line 1455 of yacc.c  */
#line 2367 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (notSkipping())
            {
                /* Check the section. */

                if (sectionFlags != 0)
                {
                    if ((sectionFlags & SECT_IS_PUBLIC) == 0)
                        yyerror("Class variables must be in the public section");

                    if (!currentIsStatic && (yyvsp[(5) - (7)].codeb) != NULL)
                        yyerror("%AccessCode cannot be specified for non-static class variables");
                }

                if (currentIsStatic && currentSpec -> genc)
                    yyerror("Cannot have static members in a C structure");

                applyTypeFlags(currentModule, &(yyvsp[(1) - (7)].memArg), &(yyvsp[(3) - (7)].optflags));

                if ((yyvsp[(6) - (7)].codeb) != NULL || (yyvsp[(7) - (7)].codeb) != NULL)
                {
                    if ((yyvsp[(5) - (7)].codeb) != NULL)
                        yyerror("Cannot mix %AccessCode and %GetCode or %SetCode");

                    if (currentScope() == NULL)
                        yyerror("Cannot specify %GetCode or %SetCode for global variables");
                }

                newVar(currentSpec,currentModule,(yyvsp[(2) - (7)].text),currentIsStatic,&(yyvsp[(1) - (7)].memArg),&(yyvsp[(3) - (7)].optflags),(yyvsp[(5) - (7)].codeb),(yyvsp[(6) - (7)].codeb),(yyvsp[(7) - (7)].codeb));
            }

            currentIsStatic = FALSE;
        }
    break;

  case 326:

/* Line 1455 of yacc.c  */
#line 2402 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg) = (yyvsp[(2) - (4)].memArg);
            (yyval.memArg).nrderefs += (yyvsp[(3) - (4)].number);
            (yyval.memArg).argflags |= ARG_IS_CONST | (yyvsp[(4) - (4)].number);
        }
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 2407 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (3)].memArg);
            (yyval.memArg).nrderefs += (yyvsp[(2) - (3)].number);
            (yyval.memArg).argflags |= (yyvsp[(3) - (3)].number);
        }
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 2414 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.memArg) = (yyvsp[(1) - (3)].memArg);
            (yyval.memArg).name = cacheName(currentSpec, (yyvsp[(2) - (3)].text));

            if (getAllowNone(&(yyvsp[(3) - (3)].optflags)))
                (yyval.memArg).argflags |= ARG_ALLOW_NONE;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"GetWrapper",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_GET_WRAPPER;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"Array",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_ARRAY;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"ArraySize",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_ARRAY_SIZE;

            if (getTransfer(&(yyvsp[(3) - (3)].optflags)))
                (yyval.memArg).argflags |= ARG_XFERRED;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"TransferThis",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_THIS_XFERRED;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"TransferBack",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_XFERRED_BACK;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags), "KeepReference", bool_flag) != NULL)
            {
                (yyval.memArg).argflags |= ARG_KEEP_REF;
                (yyval.memArg).key = currentModule->next_key++;
            }

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"In",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_IN;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"Out",bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_OUT;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags), "ResultSize", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_RESULT_SIZE;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags), "NoCopy", bool_flag) != NULL)
                (yyval.memArg).argflags |= ARG_NO_COPY;

            if (findOptFlag(&(yyvsp[(3) - (3)].optflags),"Constrained",bool_flag) != NULL)
            {
                (yyval.memArg).argflags |= ARG_CONSTRAINED;

                switch ((yyval.memArg).atype)
                {
                case bool_type:
                    (yyval.memArg).atype = cbool_type;
                    break;

                case int_type:
                    (yyval.memArg).atype = cint_type;
                    break;

                case float_type:
                    (yyval.memArg).atype = cfloat_type;
                    break;

                case double_type:
                    (yyval.memArg).atype = cdouble_type;
                    break;
                }
            }

            applyTypeFlags(currentModule, &(yyval.memArg), &(yyvsp[(3) - (3)].optflags));
            (yyval.memArg).docval = getDocValue(&(yyvsp[(3) - (3)].optflags));
        }
    break;

  case 329:

/* Line 1455 of yacc.c  */
#line 2486 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 330:

/* Line 1455 of yacc.c  */
#line 2489 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec -> genc)
                yyerror("References not allowed in a C module");

            (yyval.number) = ARG_IS_REF;
        }
    break;

  case 331:

/* Line 1455 of yacc.c  */
#line 2497 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = 0;
        }
    break;

  case 332:

/* Line 1455 of yacc.c  */
#line 2500 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.number) = (yyvsp[(1) - (2)].number) + 1;
        }
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 2505 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = defined_type;
            (yyval.memArg).u.snd = (yyvsp[(1) - (1)].scpvalp);

            /* Try and resolve typedefs as early as possible. */
            resolveAnyTypedef(currentSpec, &(yyval.memArg));
        }
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 2513 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            templateDef *td;

            td = sipMalloc(sizeof(templateDef));
            td->fqname = (yyvsp[(1) - (4)].scpvalp);
            td->types = (yyvsp[(3) - (4)].signature);

            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = template_type;
            (yyval.memArg).u.td = td;
        }
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 2524 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));

            /* In a C module all structures must be defined. */
            if (currentSpec -> genc)
            {
                (yyval.memArg).atype = defined_type;
                (yyval.memArg).u.snd = (yyvsp[(2) - (2)].scpvalp);
            }
            else
            {
                (yyval.memArg).atype = struct_type;
                (yyval.memArg).u.sname = (yyvsp[(2) - (2)].scpvalp);
            }
        }
    break;

  case 336:

/* Line 1455 of yacc.c  */
#line 2539 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ushort_type;
        }
    break;

  case 337:

/* Line 1455 of yacc.c  */
#line 2543 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = short_type;
        }
    break;

  case 338:

/* Line 1455 of yacc.c  */
#line 2547 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 339:

/* Line 1455 of yacc.c  */
#line 2551 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = uint_type;
        }
    break;

  case 340:

/* Line 1455 of yacc.c  */
#line 2555 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = int_type;
        }
    break;

  case 341:

/* Line 1455 of yacc.c  */
#line 2559 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = long_type;
        }
    break;

  case 342:

/* Line 1455 of yacc.c  */
#line 2563 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulong_type;
        }
    break;

  case 343:

/* Line 1455 of yacc.c  */
#line 2567 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = longlong_type;
        }
    break;

  case 344:

/* Line 1455 of yacc.c  */
#line 2571 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ulonglong_type;
        }
    break;

  case 345:

/* Line 1455 of yacc.c  */
#line 2575 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = float_type;
        }
    break;

  case 346:

/* Line 1455 of yacc.c  */
#line 2579 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = double_type;
        }
    break;

  case 347:

/* Line 1455 of yacc.c  */
#line 2583 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = bool_type;
        }
    break;

  case 348:

/* Line 1455 of yacc.c  */
#line 2587 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = sstring_type;
        }
    break;

  case 349:

/* Line 1455 of yacc.c  */
#line 2591 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ustring_type;
        }
    break;

  case 350:

/* Line 1455 of yacc.c  */
#line 2595 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = string_type;
        }
    break;

  case 351:

/* Line 1455 of yacc.c  */
#line 2599 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = wstring_type;
        }
    break;

  case 352:

/* Line 1455 of yacc.c  */
#line 2603 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = void_type;
        }
    break;

  case 353:

/* Line 1455 of yacc.c  */
#line 2607 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyobject_type;
        }
    break;

  case 354:

/* Line 1455 of yacc.c  */
#line 2611 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytuple_type;
        }
    break;

  case 355:

/* Line 1455 of yacc.c  */
#line 2615 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pylist_type;
        }
    break;

  case 356:

/* Line 1455 of yacc.c  */
#line 2619 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pydict_type;
        }
    break;

  case 357:

/* Line 1455 of yacc.c  */
#line 2623 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pycallable_type;
        }
    break;

  case 358:

/* Line 1455 of yacc.c  */
#line 2627 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pyslice_type;
        }
    break;

  case 359:

/* Line 1455 of yacc.c  */
#line 2631 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = pytype_type;
        }
    break;

  case 360:

/* Line 1455 of yacc.c  */
#line 2635 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            memset(&(yyval.memArg), 0, sizeof (argDef));
            (yyval.memArg).atype = ellipsis_type;
        }
    break;

  case 361:

/* Line 1455 of yacc.c  */
#line 2641 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* The single or first type. */

            (yyval.signature).args[0] = (yyvsp[(1) - (1)].memArg);
            (yyval.signature).nrArgs = 1;
        }
    break;

  case 362:

/* Line 1455 of yacc.c  */
#line 2647 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check there is nothing after an ellipsis. */
            if ((yyvsp[(1) - (3)].signature).args[(yyvsp[(1) - (3)].signature).nrArgs - 1].atype == ellipsis_type)
                yyerror("An ellipsis must be at the end of the argument list");

            /* Check there is room. */
            if ((yyvsp[(1) - (3)].signature).nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.signature) = (yyvsp[(1) - (3)].signature);

            (yyval.signature).args[(yyval.signature).nrArgs] = (yyvsp[(3) - (3)].memArg);
            (yyval.signature).nrArgs++;
        }
    break;

  case 363:

/* Line 1455 of yacc.c  */
#line 2663 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            (yyval.throwlist) = NULL;
        }
    break;

  case 364:

/* Line 1455 of yacc.c  */
#line 2666 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            if (currentSpec->genc)
                yyerror("Exceptions not allowed in a C module");

            (yyval.throwlist) = (yyvsp[(3) - (4)].throwlist);
        }
    break;

  case 365:

/* Line 1455 of yacc.c  */
#line 2674 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Empty list so use a blank. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 0;
        }
    break;

  case 366:

/* Line 1455 of yacc.c  */
#line 2680 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* The only or first exception. */

            (yyval.throwlist) = sipMalloc(sizeof (throwArgs));
            (yyval.throwlist) -> nrArgs = 1;
            (yyval.throwlist) -> args[0] = findException(currentSpec, (yyvsp[(1) - (1)].scpvalp), FALSE);
        }
    break;

  case 367:

/* Line 1455 of yacc.c  */
#line 2687 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"
    {
            /* Check that it wasn't ...(,arg...). */

            if ((yyvsp[(1) - (3)].throwlist) -> nrArgs == 0)
                yyerror("First exception of throw specifier is missing");

            /* Check there is room. */

            if ((yyvsp[(1) - (3)].throwlist) -> nrArgs == MAX_NR_ARGS)
                yyerror("Internal error - increase the value of MAX_NR_ARGS");

            (yyval.throwlist) = (yyvsp[(1) - (3)].throwlist);
            (yyval.throwlist) -> args[(yyval.throwlist) -> nrArgs++] = findException(currentSpec, (yyvsp[(3) - (3)].scpvalp), FALSE);
        }
    break;



/* Line 1455 of yacc.c  */
#line 6054 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 2703 "/home/phil/hg/sip/sip-4.10.2/sipgen/parser.y"



/*
 * Parse the specification.
 */
void parse(sipSpec *spec, FILE *fp, char *filename, stringList *tsl,
        stringList *xfl, int kwdArgs, int protHack)
{
    classTmplDef *tcd;

    /* Initialise the spec. */
 
    spec->modules = NULL;
    spec->namecache = NULL;
    spec->ifacefiles = NULL;
    spec->classes = NULL;
    spec->classtemplates = NULL;
    spec->exceptions = NULL;
    spec->mappedtypes = NULL;
    spec->mappedtypetemplates = NULL;
    spec->enums = NULL;
    spec->vars = NULL;
    spec->typedefs = NULL;
    spec->exphdrcode = NULL;
    spec->docs = NULL;
    spec->sigslots = FALSE;
    spec->genc = -1;
    spec->plugins = NULL;

    currentSpec = spec;
    neededQualifiers = tsl;
    excludedQualifiers = xfl;
    currentModule = NULL;
    currentMappedType = NULL;
    currentOverIsVirt = FALSE;
    currentCtorIsExplicit = FALSE;
    currentIsStatic = FALSE;
    currentIsTemplate = FALSE;
    previousFile = NULL;
    skipStackPtr = 0;
    currentScopeIdx = 0;
    sectionFlags = 0;
    defaultKwdArgs = kwdArgs;
    makeProtPublic = protHack;

    newModule(fp, filename);
    spec->module = currentModule;

    yyparse();

    handleEOF();
    handleEOM();

    /*
     * Go through each template class and remove it from the list of classes.
     */
    for (tcd = spec->classtemplates; tcd != NULL; tcd = tcd->next)
    {
        classDef **cdp;

        for (cdp = &spec->classes; *cdp != NULL; cdp = &(*cdp)->next)
            if (*cdp == tcd->cd)
            {
                ifaceFileDef **ifdp;

                /* Remove the interface file as well. */
                for (ifdp = &spec->ifacefiles; *ifdp != NULL; ifdp = &(*ifdp)->next)
                    if (*ifdp == tcd->cd->iff)
                    {
                        *ifdp = (*ifdp)->next;
                        break;
                    }

                *cdp = (*cdp)->next;
                break;
            }
    }
}


/*
 * Tell the parser that a complete file has now been read.
 */
void parserEOF(char *name, parserContext *pc)
{
    previousFile = sipStrdup(name);
    currentContext = *pc;
}


/*
 * Append a class definition to a class list if it doesn't already appear.
 * Append is needed specifically for the list of super-classes because the
 * order is important to Python.
 */
void appendToClassList(classList **clp,classDef *cd)
{
    classList *new;

    /* Find the end of the list. */

    while (*clp != NULL)
    {
        if ((*clp) -> cd == cd)
            return;

        clp = &(*clp) -> next;
    }

    new = sipMalloc(sizeof (classList));

    new -> cd = cd;
    new -> next = NULL;

    *clp = new;
}


/*
 * Create a new module for the current specification and make it current.
 */
static void newModule(FILE *fp, char *filename)
{
    moduleDef *mod;

    parseFile(fp, filename, currentModule, FALSE);

    mod = allocModule();
    mod->file = filename;

    if (currentModule != NULL)
        mod->defexception = currentModule->defexception;

    currentModule = mod;
}


/*
 * Allocate and initialise the memory for a new module.
 */
static moduleDef *allocModule()
{
    moduleDef *newmod, **tailp;

    newmod = sipMalloc(sizeof (moduleDef));

    newmod->version = -1;
    newmod->encoding = no_type;
    newmod->qobjclass = -1;
    newmod->nrvirthandlers = -1;
    newmod->next_key = 1;

    /*
     * The consolidated module support needs these to be in order that they
     * appeared.
     */
    for (tailp = &currentSpec->modules; *tailp != NULL; tailp = &(*tailp)->next)
        ;

    *tailp = newmod;

    return newmod;
}


/*
 * Switch to parsing a new file.
 */
static void parseFile(FILE *fp, char *name, moduleDef *prevmod, int optional)
{
    parserContext pc;

    pc.filename = name;
    pc.ifdepth = skipStackPtr;
    pc.prevmod = prevmod;

    if (setInputFile(fp, &pc, optional))
        currentContext = pc;
}


/*
 * Find an interface file, or create a new one.
 */
ifaceFileDef *findIfaceFile(sipSpec *pt, moduleDef *mod, scopedNameDef *fqname,
        ifaceFileType iftype, apiVersionRangeDef *api_range, argDef *ad)
{
    ifaceFileDef *iff, *first_alt = NULL;

    /* See if the name is already used. */

    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
    {
        if (compareScopedNames(iff->fqcname, fqname) != 0)
            continue;

        /*
         * If they are both versioned then assume the user knows what they are
         * doing.
         */
        if (iff->api_range != NULL && api_range != NULL && iff->module == mod)
        {
            /* Remember the first of the alternate APIs. */
            if ((first_alt = iff->first_alt) == NULL)
                first_alt = iff;

            break;
        }

        /*
         * They must be the same type except that we allow a class if we want
         * an exception.  This is because we allow classes to be used before
         * they are defined.
         */
        if (iff->type != iftype)
            if (iftype != exception_iface || iff->type != class_iface)
                yyerror("A class, exception, namespace or mapped type has already been defined with the same name");

        /* Ignore an external class declared in another module. */
        if (iftype == class_iface && iff->module != mod)
        {
            classDef *cd;

            for (cd = pt->classes; cd != NULL; cd = cd->next)
                if (cd->iff == iff)
                    break;

            if (cd != NULL && iff->module != NULL && isExternal(cd))
                continue;
        }

        /*
         * If this is a mapped type with the same name defined in a different
         * module, then check that this type isn't the same as any of the
         * mapped types defined in that module.
         */
        if (iftype == mappedtype_iface && iff->module != mod)
        {
            mappedTypeDef *mtd;

            /*
             * This is a bit of a cheat.  With consolidated modules it's
             * possible to have two implementations of a mapped type in
             * different branches of the module hierarchy.  We assume that, if
             * there really are multiple implementations in the same branch,
             * then it will be picked up in a non-consolidated build.
             */
            if (isConsolidated(pt->module))
                continue;

            for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
            {
                if (mtd->iff != iff)
                    continue;

                if (ad->atype != template_type ||
                    mtd->type.atype != template_type ||
                    sameBaseType(ad, &mtd->type))
                    yyerror("Mapped type has already been defined in another module");
            }

            /*
             * If we got here then we have a mapped type based on an existing
             * template, but with unique parameters.  We don't want to use
             * interface files from other modules, so skip this one.
             */

            continue;
        }

        /* Ignore a namespace defined in another module. */
        if (iftype == namespace_iface && iff->module != mod)
            continue;

        return iff;
    }

    iff = sipMalloc(sizeof (ifaceFileDef));

    iff->name = cacheName(pt, scopedNameToString(fqname));
    iff->api_range = api_range;

    if (first_alt != NULL)
    {
        iff->first_alt = first_alt;
        iff->next_alt = first_alt->next_alt;

        first_alt->next_alt = iff;
    }
    else
    {
        /* This is the first alternate so point to itself. */
        iff->first_alt = iff;
    }

    iff->type = iftype;
    iff->ifacenr = -1;
    iff->fqcname = fqname;
    iff->module = NULL;
    iff->hdrcode = NULL;
    iff->used = NULL;
    iff->next = pt->ifacefiles;

    pt->ifacefiles = iff;

    return iff;
}


/*
 * Find a class definition in a parse tree.
 */
static classDef *findClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname)
{
    return findClassWithInterface(pt, findIfaceFile(pt, currentModule, fqname, iftype, api_range, NULL));
}


/*
 * Find a class definition given an existing interface file.
 */
static classDef *findClassWithInterface(sipSpec *pt, ifaceFileDef *iff)
{
    classDef *cd;

    for (cd = pt -> classes; cd != NULL; cd = cd -> next)
        if (cd -> iff == iff)
            return cd;

    /* Create a new one. */
    cd = sipMalloc(sizeof (classDef));

    cd->iff = iff;
    cd->pyname = cacheName(pt, classBaseName(cd));
    cd->next = pt->classes;

    pt->classes = cd;

    return cd;
}


/*
 * Add an interface file to an interface file list if it isn't already there.
 */
void addToUsedList(ifaceFileList **ifflp, ifaceFileDef *iff)
{
    /* Make sure we don't try to add an interface file to its own list. */
    if (&iff->used != ifflp)
    {
        ifaceFileList *iffl;

        while ((iffl = *ifflp) != NULL)
        {
            /* Don't bother if it is already there. */
            if (iffl->iff == iff)
                return;

            ifflp = &iffl -> next;
        }

        iffl = sipMalloc(sizeof (ifaceFileList));

        iffl->iff = iff;
        iffl->next = NULL;

        *ifflp = iffl;
    }
}


/*
 * Find an undefined (or create a new) exception definition in a parse tree.
 */
static exceptionDef *findException(sipSpec *pt, scopedNameDef *fqname, int new)
{
    exceptionDef *xd, **tail;
    ifaceFileDef *iff;
    classDef *cd;

    iff = findIfaceFile(pt, currentModule, fqname, exception_iface, NULL, NULL);

    /* See if it is an existing one. */
    for (xd = pt->exceptions; xd != NULL; xd = xd->next)
        if (xd->iff == iff)
            return xd;

    /*
     * If it is an exception interface file then we have never seen this
     * name before.  We require that exceptions are defined before being
     * used, but don't make the same requirement of classes (for reasons of
     * backwards compatibility).  Therefore the name must be reinterpreted
     * as a (as yet undefined) class.
     */
    if (new)
    {
        if (iff->type == exception_iface)
            cd = NULL;
        else
            yyerror("There is already a class with the same name or the exception has been used before being defined");
    }
    else
    {
        if (iff->type == exception_iface)
            iff->type = class_iface;

        cd = findClassWithInterface(pt, iff);
    }

    /* Create a new one. */
    xd = sipMalloc(sizeof (exceptionDef));

    xd->exceptionnr = -1;
    xd->iff = iff;
    xd->pyname = NULL;
    xd->cd = cd;
    xd->bibase = NULL;
    xd->base = NULL;
    xd->raisecode = NULL;
    xd->next = NULL;

    /* Append it to the list. */
    for (tail = &pt->exceptions; *tail != NULL; tail = &(*tail)->next)
        ;

    *tail = xd;

    return xd;
}


/*
 * Find an undefined (or create a new) class definition in a parse tree.
 */
static classDef *newClass(sipSpec *pt, ifaceFileType iftype,
        apiVersionRangeDef *api_range, scopedNameDef *fqname)
{
    int flags;
    classDef *cd, *scope;
    codeBlock *hdrcode;

    if (sectionFlags & SECT_IS_PRIVATE)
        yyerror("Classes, structs and namespaces must be in the public or protected sections");

    flags = 0;

    if ((scope = currentScope()) != NULL)
    {
        if (sectionFlags & SECT_IS_PROT && !makeProtPublic)
        {
            flags = CLASS_IS_PROTECTED;

            if (scope->iff->type == class_iface)
                setHasShadow(scope);
        }

        /* Header code from outer scopes is also included. */
        hdrcode = scope->iff->hdrcode;
    }
    else
        hdrcode = NULL;

    if (pt -> genc)
    {
        /* C structs are always global types. */
        while (fqname -> next != NULL)
            fqname = fqname -> next;

        scope = NULL;
    }

    cd = findClass(pt, iftype, api_range, fqname);

    /* Check it hasn't already been defined. */
    if (iftype != namespace_iface && cd->iff->module != NULL)
        yyerror("The struct/class has already been defined");

    /* Complete the initialisation. */
    cd->classflags |= flags;
    cd->ecd = scope;
    cd->iff->module = currentModule;

    if (currentIsTemplate)
        setIsTemplateClass(cd);

    appendCodeBlock(&cd->iff->hdrcode, hdrcode);

    /* See if it is a namespace extender. */
    if (iftype == namespace_iface)
    {
        classDef *ns;

        for (ns = pt->classes; ns != NULL; ns = ns->next)
        {
            if (ns == cd)
                continue;

            if (ns->iff->type != namespace_iface)
                continue;

            if (compareScopedNames(ns->iff->fqcname, fqname) != 0)
                continue;

            cd->real = ns;
            break;
        }
    }

    return cd;
}


/*
 * Tidy up after finishing a class definition.
 */
static void finishClass(sipSpec *pt, moduleDef *mod, classDef *cd,
        optFlags *of)
{
    const char *pyname;
    optFlag *flg;

    /* Get the Python name and see if it is different to the C++ name. */
    pyname = getPythonName(of, classBaseName(cd));

    cd->pyname = NULL;
    checkAttributes(pt, mod, cd->ecd, NULL, pyname, FALSE);
    cd->pyname = cacheName(pt, pyname);

    if ((flg = findOptFlag(of, "Metatype", dotted_name_flag)) != NULL)
        cd->metatype = cacheName(pt, flg->fvalue.sval);

    if ((flg = findOptFlag(of, "Supertype", dotted_name_flag)) != NULL)
        cd->supertype = cacheName(pt, flg->fvalue.sval);

    if ((flg = findOptFlag(of, "PyQt4Flags", integer_flag)) != NULL)
        cd->pyqt4_flags = flg->fvalue.ival;

    if (findOptFlag(of, "PyQt4NoQMetaObject", bool_flag) != NULL)
        setPyQt4NoQMetaObject(cd);

    if (isOpaque(cd))
    {
        if (findOptFlag(of, "External", bool_flag) != NULL)
            setIsExternal(cd);
    }
    else
    {
        int seq_might, seq_not;
        memberDef *md;

        if (findOptFlag(of, "NoDefaultCtors", bool_flag) != NULL)
            setNoDefaultCtors(cd);

        if (cd -> ctors == NULL)
        {
            if (!noDefaultCtors(cd))
            {
                /* Provide a default ctor. */

                cd->ctors = sipMalloc(sizeof (ctorDef));
 
                cd->ctors->ctorflags = SECT_IS_PUBLIC;
                cd->ctors->pysig.result.atype = void_type;
                cd->ctors->cppsig = &cd->ctors->pysig;

                cd->defctor = cd->ctors;

                setCanCreate(cd);
            }
        }
        else if (cd -> defctor == NULL)
        {
            ctorDef *ct, *last = NULL;

            for (ct = cd -> ctors; ct != NULL; ct = ct -> next)
            {
                if (!isPublicCtor(ct))
                    continue;

                if (ct -> pysig.nrArgs == 0 || ct -> pysig.args[0].defval != NULL)
                {
                    cd -> defctor = ct;
                    break;
                }

                if (last == NULL)
                    last = ct;
            }

            /* The last resort is the first public ctor. */
            if (cd->defctor == NULL)
                cd->defctor = last;
        }

        if (getDeprecated(of))
            setIsDeprecatedClass(cd);

        if (cd->convtocode != NULL && getAllowNone(of))
            setClassHandlesNone(cd);

        if (findOptFlag(of,"Abstract",bool_flag) != NULL)
        {
            setIsAbstractClass(cd);
            setIsIncomplete(cd);
            resetCanCreate(cd);
        }

        /* We assume a public dtor if nothing specific was provided. */
        if (!isDtor(cd))
            setIsPublicDtor(cd);

        if (findOptFlag(of, "DelayDtor", bool_flag) != NULL)
        {
            setIsDelayedDtor(cd);
            setHasDelayedDtors(mod);
        }

        /*
         * There are subtle differences between the add and concat methods and
         * the multiply and repeat methods.  The number versions can have their
         * operands swapped and may return NotImplemented.  If the user has
         * used the /Numeric/ annotation or there are other numeric operators
         * then we use add/multiply.  Otherwise, if there are indexing
         * operators then we use concat/repeat.
         */
        seq_might = seq_not = FALSE;

        for (md = cd -> members; md != NULL; md = md -> next)
            switch (md -> slot)
            {
            case getitem_slot:
            case setitem_slot:
            case delitem_slot:
                /* This might be a sequence. */
                seq_might = TRUE;
                break;

            case sub_slot:
            case isub_slot:
            case div_slot:
            case idiv_slot:
            case mod_slot:
            case imod_slot:
            case floordiv_slot:
            case ifloordiv_slot:
            case truediv_slot:
            case itruediv_slot:
            case pos_slot:
            case neg_slot:
                /* This is definately not a sequence. */
                seq_not = TRUE;
                break;
            }

        if (!seq_not && seq_might)
            for (md = cd -> members; md != NULL; md = md -> next)
            {
                /* Ignore if the user has been explicit. */
                if (isNumeric(md))
                    continue;

                switch (md -> slot)
                {
                case add_slot:
                    md -> slot = concat_slot;
                    break;

                case iadd_slot:
                    md -> slot = iconcat_slot;
                    break;

                case mul_slot:
                    md -> slot = repeat_slot;
                    break;

                case imul_slot:
                    md -> slot = irepeat_slot;
                    break;
                }
            }
    }

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsUsedName(cd->pyname);
    }
}


/*
 * Return the encoded name of a template (ie. including its argument types) as
 * a scoped name.
 */
scopedNameDef *encodedTemplateName(templateDef *td)
{
    int a;
    scopedNameDef *snd;

    snd = copyScopedName(td->fqname);

    for (a = 0; a < td->types.nrArgs; ++a)
    {
        char buf[50];
        int flgs;
        scopedNameDef *arg_snd;
        argDef *ad = &td->types.args[a];

        flgs = 0;

        if (isConstArg(ad))
            flgs += 1;

        if (isReference(ad))
            flgs += 2;

        /* We use numbers so they don't conflict with names. */
        sprintf(buf, "%02d%d%d", ad->atype, flgs, ad->nrderefs);

        switch (ad->atype)
        {
        case defined_type:
            arg_snd = copyScopedName(ad->u.snd);
            break;

        case template_type:
            arg_snd = encodedTemplateName(ad->u.td);
            break;

        case struct_type:
            arg_snd = copyScopedName(ad->u.sname);
            break;

        default:
            arg_snd = NULL;
        }

        /*
         * Replace the first element of the argument name with a copy with the
         * encoding prepended.
         */
        if (arg_snd != NULL)
            arg_snd->name = concat(buf, arg_snd->name, NULL);
        else
            arg_snd = text2scopePart(sipStrdup(buf));

        appendScopedName(&snd, arg_snd);
    }

    return snd;
}


/*
 * Create a new mapped type.
 */
static mappedTypeDef *newMappedType(sipSpec *pt, argDef *ad, optFlags *of)
{
    mappedTypeDef *mtd;
    scopedNameDef *snd;
    ifaceFileDef *iff;
    const char *cname;

    /* Check that the type is one we want to map. */
    switch (ad->atype)
    {
    case defined_type:
        snd = ad->u.snd;
        cname = scopedNameTail(snd);
        break;

    case template_type:
        snd = encodedTemplateName(ad->u.td);
        cname = NULL;
        break;

    case struct_type:
        snd = ad->u.sname;
        cname = scopedNameTail(snd);
        break;

    default:
        yyerror("Invalid type for %MappedType");
    }

    iff = findIfaceFile(pt, currentModule, snd, mappedtype_iface,
            getAPIRange(of), ad);

    /* Check it hasn't already been defined. */
    for (mtd = pt->mappedtypes; mtd != NULL; mtd = mtd->next)
        if (mtd->iff == iff)
        {
            /*
             * We allow types based on the same template but with different
             * arguments.
             */
            if (ad->atype != template_type || sameBaseType(ad, &mtd->type))
                yyerror("Mapped type has already been defined in this module");
        }

    /* The module may not have been set yet. */
    iff->module = currentModule;

    /* Create a new mapped type. */
    mtd = allocMappedType(pt, ad);

    if (cname != NULL)
        mtd->pyname = cacheName(pt, getPythonName(of, cname));

    if (findOptFlag(of, "NoRelease", bool_flag) != NULL)
        setNoRelease(mtd);

    if (getAllowNone(of))
        setHandlesNone(mtd);

    mtd->doctype = getDocType(of);

    mtd->iff = iff;
    mtd->next = pt->mappedtypes;

    pt->mappedtypes = mtd;

    if (inMainModule())
    {
        setIsUsedName(mtd->cname);

        if (mtd->pyname)
            setIsUsedName(mtd->pyname);
    }

    return mtd;
}


/*
 * Allocate, intialise and return a mapped type structure.
 */
mappedTypeDef *allocMappedType(sipSpec *pt, argDef *type)
{
    mappedTypeDef *mtd;

    mtd = sipMalloc(sizeof (mappedTypeDef));

    mtd->type = *type;
    mtd->type.argflags = 0;
    mtd->type.nrderefs = 0;

    mtd->cname = cacheName(pt, type2string(&mtd->type));

    return mtd;
}


/*
 * Create a new enum.
 */
static enumDef *newEnum(sipSpec *pt, moduleDef *mod, mappedTypeDef *mt_scope,
        char *name, optFlags *of, int flags)
{
    enumDef *ed, *first_alt, *next_alt;
    classDef *c_scope;
    ifaceFileDef *scope;

    if (mt_scope != NULL)
    {
        scope = mt_scope->iff;
        c_scope = NULL;
    }
    else
    {
        if ((c_scope = currentScope()) != NULL)
            scope = c_scope->iff;
        else
            scope = NULL;
    }

    ed = sipMalloc(sizeof (enumDef));

    /* Assume the enum isn't versioned. */
    first_alt = ed;
    next_alt = NULL;

    if (name != NULL)
    {
        ed->pyname = cacheName(pt, getPythonName(of, name));
        checkAttributes(pt, mod, c_scope, mt_scope, ed->pyname->text, FALSE);

        ed->fqcname = text2scopedName(scope, name);
        ed->cname = cacheName(pt, scopedNameToString(ed->fqcname));

        if (inMainModule())
        {
            setIsUsedName(ed->pyname);
            setIsUsedName(ed->cname);
        }

        /* If the scope is versioned then look for any alternate. */
        if (scope != NULL && scope->api_range != NULL)
        {
            enumDef *alt;

            for (alt = pt->enums; alt != NULL; alt = alt->next)
            {
                if (alt->module != mod || alt->fqcname == NULL)
                    continue;

                if (compareScopedNames(alt->fqcname, ed->fqcname) == 0)
                {
                    first_alt = alt->first_alt;
                    next_alt = first_alt->next_alt;
                    first_alt->next_alt = ed;

                    break;
                }
            }
        }
    }
    else
    {
        ed->pyname = NULL;
        ed->fqcname = NULL;
        ed->cname = NULL;
    }

    if (flags & SECT_IS_PROT && makeProtPublic)
    {
        flags &= ~SECT_IS_PROT;
        flags |= SECT_IS_PUBLIC;
    }

    ed->enumflags = flags;
    ed->enumnr = -1;
    ed->ecd = c_scope;
    ed->emtd = mt_scope;
    ed->first_alt = first_alt;
    ed->next_alt = next_alt;
    ed->module = mod;
    ed->members = NULL;
    ed->slots = NULL;
    ed->overs = NULL;
    ed->next = pt -> enums;

    pt->enums = ed;

    return ed;
}


/*
 * Get the type values and (optionally) the type names for substitution in
 * handwritten code.
 */
void appendTypeStrings(scopedNameDef *ename, signatureDef *patt, signatureDef *src, signatureDef *known, scopedNameDef **names, scopedNameDef **values)
{
    int a;

    for (a = 0; a < patt->nrArgs; ++a)
    {
        argDef *pad = &patt->args[a];

        if (pad->atype == defined_type)
        {
            char *nam = NULL, *val;
            argDef *sad;

            /*
             * If the type names are already known then check that this is one
             * of them.
             */
            if (known == NULL)
                nam = scopedNameTail(pad->u.snd);
            else if (pad->u.snd->next == NULL)
            {
                int k;

                for (k = 0; k < known->nrArgs; ++k)
                {
                    /* Skip base types. */
                    if (known->args[k].atype != defined_type)
                        continue;

                    if (strcmp(pad->u.snd->name, known->args[k].u.snd->name) == 0)
                    {
                        nam = pad->u.snd->name;
                        break;
                    }
                }
            }

            if (nam == NULL)
                continue;

            /* Add the name. */
            appendScopedName(names, text2scopePart(nam));

            /*
             * Add the corresponding value.  For defined types we don't want 
             * any indirection or references.
             */
            sad = &src->args[a];

            if (sad->atype == defined_type)
                val = scopedNameToString(sad->u.snd);
            else
                val = type2string(sad);

            appendScopedName(values, text2scopePart(val));
        }
        else if (pad->atype == template_type)
        {
            argDef *sad = &src->args[a];

            /* These checks shouldn't be necessary, but... */
            if (sad->atype == template_type && pad->u.td->types.nrArgs == sad->u.td->types.nrArgs)
                appendTypeStrings(ename, &pad->u.td->types, &sad->u.td->types, known, names, values);
        }
    }
}


/*
 * Convert a type to a string on the heap.  The string will use the minimum
 * whitespace while still remaining valid C++.
 */
static char *type2string(argDef *ad)
{
    int i, on_heap = FALSE;
    int nr_derefs = ad->nrderefs;
    int is_reference = isReference(ad);
    char *s;

    /* Use the original type if possible. */
    if (ad->original_type != NULL && !noTypeName(ad->original_type))
    {
        s = scopedNameToString(ad->original_type->fqname);
        on_heap = TRUE;

        nr_derefs -= ad->original_type->type.nrderefs;

        if (isReference(&ad->original_type->type))
            is_reference = FALSE;
    }
    else
        switch (ad->atype)
        {
        case template_type:
            {
                templateDef *td = ad->u.td;

                s = scopedNameToString(td->fqname);
                append(&s, "<");

                for (i = 0; i < td->types.nrArgs; ++i)
                {
                    char *sub_type = type2string(&td->types.args[i]);

                    if (i > 0)
                        append(&s, ",");

                    append(&s, sub_type);
                    free(sub_type);
                }

                if (s[strlen(s) - 1] == '>')
                    append(&s, " >");
                else
                    append(&s, ">");

                on_heap = TRUE;
                break;
            }

        case struct_type:
            s = scopedNameToString(ad->u.sname);
            on_heap = TRUE;
            break;

        case defined_type:
            s = scopedNameToString(ad->u.snd);
            on_heap = TRUE;
            break;

        case ustring_type:
            s = "unsigned char";
            break;

        case ascii_string_type:
        case latin1_string_type:
        case utf8_string_type:
        case string_type:
            s = "char";
            break;

        case sstring_type:
            s = "signed char";
            break;

        case wstring_type:
            s = "wchar_t";
            break;

        case ushort_type:
            s = "unsigned short";
            break;

        case short_type:
            s = "short";
            break;

        case uint_type:
            s = "unsigned int";
            break;

        case int_type:
        case cint_type:
            s = "int";
            break;

        case ulong_type:
            s = "unsigned long";
            break;

        case long_type:
            s = "long";
            break;

        case ulonglong_type:
            s = "unsigned long long";
            break;

        case longlong_type:
            s = "long long";
            break;

        case float_type:
        case cfloat_type:
            s = "float";
            break;

        case double_type:
        case cdouble_type:
            s = "double";
            break;

        case bool_type:
        case cbool_type:
            s = "bool";
            break;

        default:
            fatal("Unsupported type argument to type2string(): %d\n", ad->atype);
        }

    /* Make sure the string is on the heap. */
    if (!on_heap)
        s = sipStrdup(s);

    while (nr_derefs-- > 0)
        append(&s, "*");

    if (is_reference)
        append(&s, "&");

    return s;
}


/*
 * Convert a scoped name to a string on the heap.
 */
static char *scopedNameToString(scopedNameDef *name)
{
    static const char scope_string[] = "::";
    size_t len;
    scopedNameDef *snd;
    char *s, *dp;

    /* Work out the length of buffer needed. */
    len = 0;

    for (snd = name; snd != NULL; snd = snd->next)
    {
        len += strlen(snd->name);

        if (snd->next != NULL)
        {
            /* Ignore the encoded part of template names. */
            if (isdigit(snd->next->name[0]))
                break;

            len += strlen(scope_string);
        }
    }

    /* Allocate and populate the buffer. */
    dp = s = sipMalloc(len + 1);

    for (snd = name; snd != NULL; snd = snd->next)
    {
        strcpy(dp, snd->name);
        dp += strlen(snd->name);

        if (snd->next != NULL)
        {
            /* Ignore the encoded part of template names. */
            if (isdigit(snd->next->name[0]))
                break;

            strcpy(dp, scope_string);
            dp += strlen(scope_string);
        }
    }

    return s;
}


/*
 * Instantiate a class template.
 */
static void instantiateClassTemplate(sipSpec *pt, moduleDef *mod,
        classDef *scope, scopedNameDef *fqname, classTmplDef *tcd,
        templateDef *td)
{
    scopedNameDef *type_names, *type_values;
    classDef *cd;
    ctorDef *oct, **cttail;
    argDef *ad;
    ifaceFileList *iffl, **used;

    type_names = type_values = NULL;
    appendTypeStrings(classFQCName(tcd->cd), &tcd->sig, &td->types, NULL, &type_names, &type_values);

    /*
     * Add a mapping from the template name to the instantiated name.  If we
     * have got this far we know there is room for it.
     */
    ad = &tcd->sig.args[tcd->sig.nrArgs++];
    memset(ad, 0, sizeof (argDef));
    ad->atype = defined_type;
    ad->u.snd = classFQCName(tcd->cd);

    appendScopedName(&type_names, text2scopePart(scopedNameTail(classFQCName(tcd->cd))));
    appendScopedName(&type_values, text2scopePart(scopedNameToString(fqname)));

    /* Create the new class. */
    cd = sipMalloc(sizeof (classDef));

    /* Start with a shallow copy. */
    *cd = *tcd->cd;

    resetIsTemplateClass(cd);
    cd->pyname = cacheName(pt, scopedNameTail(fqname));
    cd->td = td;

    /* Handle the interface file. */
    cd->iff = findIfaceFile(pt, mod, fqname, class_iface,
            (scope != NULL ? scope->iff->api_range : NULL), NULL);
    cd->iff->module = mod;

    /* Make a copy of the used list and add the enclosing scope. */
    used = &cd->iff->used;

    for (iffl = tcd->cd->iff->used; iffl != NULL; iffl = iffl->next)
        addToUsedList(used, iffl->iff);

    /* Include any scope header code. */
    if (scope != NULL)
        appendCodeBlock(&cd->iff->hdrcode, scope->iff->hdrcode);

    if (inMainModule())
    {
        setIsUsedName(cd->iff->name);
        setIsUsedName(cd->pyname);
    }

    cd->ecd = currentScope();

    /* Handle the enums. */
    instantiateTemplateEnums(pt, tcd, td, cd, used, type_names, type_values);

    /* Handle the variables. */
    instantiateTemplateVars(pt, tcd, td, cd, used, type_names, type_values);

    /* Handle the ctors. */
    cd->ctors = NULL;
    cttail = &cd->ctors;

    for (oct = tcd->cd->ctors; oct != NULL; oct = oct->next)
    {
        ctorDef *nct = sipMalloc(sizeof (ctorDef));

        /* Start with a shallow copy. */
        *nct = *oct;

        templateSignature(&nct->pysig, FALSE, tcd, td, cd);

        if (oct->cppsig == NULL)
            nct->cppsig = NULL;
        else if (oct->cppsig == &oct->pysig)
            nct->cppsig = &nct->pysig;
        else
        {
            nct->cppsig = sipMalloc(sizeof (signatureDef));

            *nct->cppsig = *oct->cppsig;

            templateSignature(nct->cppsig, FALSE, tcd, td, cd);
        }

        nct->methodcode = templateCode(pt, used, nct->methodcode, type_names, type_values);

        nct->next = NULL;
        *cttail = nct;
        cttail = &nct->next;

        /* Handle the default ctor. */
        if (tcd->cd->defctor == oct)
            cd->defctor = nct;
    }

    cd->dealloccode = templateCode(pt, used, cd->dealloccode, type_names, type_values);
    cd->dtorcode = templateCode(pt, used, cd->dtorcode, type_names, type_values);

    /* Handle the methods. */
    cd->members = instantiateTemplateMethods(tcd->cd->members, mod);
    cd->overs = instantiateTemplateOverloads(pt, tcd->cd->overs,
            tcd->cd->members, cd->members, tcd, td, cd, used, type_names,
            type_values);

    cd->cppcode = templateCode(pt, used, cd->cppcode, type_names, type_values);
    cd->iff->hdrcode = templateCode(pt, used, cd->iff->hdrcode, type_names, type_values);
    cd->convtosubcode = templateCode(pt, used, cd->convtosubcode, type_names, type_values);
    cd->convtocode = templateCode(pt, used, cd->convtocode, type_names, type_values);
    cd->travcode = templateCode(pt, used, cd->travcode, type_names, type_values);
    cd->clearcode = templateCode(pt, used, cd->clearcode, type_names, type_values);
    cd->getbufcode = templateCode(pt, used, cd->getbufcode, type_names, type_values);
    cd->releasebufcode = templateCode(pt, used, cd->releasebufcode, type_names, type_values);
    cd->readbufcode = templateCode(pt, used, cd->readbufcode, type_names, type_values);
    cd->writebufcode = templateCode(pt, used, cd->writebufcode, type_names, type_values);
    cd->segcountcode = templateCode(pt, used, cd->segcountcode, type_names, type_values);
    cd->charbufcode = templateCode(pt, used, cd->charbufcode, type_names, type_values);
    cd->picklecode = templateCode(pt, used, cd->picklecode, type_names, type_values);
    cd->next = pt->classes;

    pt->classes = cd;

    tcd->sig.nrArgs--;

    freeScopedName(type_names);
    freeScopedName(type_values);
}


/*
 * Instantiate the methods of a template class.
 */
static memberDef *instantiateTemplateMethods(memberDef *tmd, moduleDef *mod)
{
    memberDef *md, *methods, **mdtail;

    methods = NULL;
    mdtail = &methods;

    for (md = tmd; md != NULL; md = md->next)
    {
        memberDef *nmd = sipMalloc(sizeof (memberDef));

        /* Start with a shallow copy. */
        *nmd = *md;

        nmd->module = mod;

        if (inMainModule())
            setIsUsedName(nmd->pyname);

        nmd->next = NULL;
        *mdtail = nmd;
        mdtail = &nmd->next;
    }

    return methods;
}


/*
 * Instantiate the overloads of a template class.
 */
static overDef *instantiateTemplateOverloads(sipSpec *pt, overDef *tod,
        memberDef *tmethods, memberDef *methods, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    overDef *od, *overloads, **odtail;

    overloads = NULL;
    odtail = &overloads;

    for (od = tod; od != NULL; od = od->next)
    {
        overDef *nod = sipMalloc(sizeof (overDef));
        memberDef *nmd, *omd;

        /* Start with a shallow copy. */
        *nod = *od;

        for (nmd = methods, omd = tmethods; omd != NULL; omd = omd->next, nmd = nmd->next)
            if (omd == od->common)
            {
                nod->common = nmd;
                break;
            }

        templateSignature(&nod->pysig, TRUE, tcd, td, cd);

        if (od->cppsig == &od->pysig)
            nod->cppsig = &nod->pysig;
        else
        {
            nod->cppsig = sipMalloc(sizeof (signatureDef));

            *nod->cppsig = *od->cppsig;

            templateSignature(nod->cppsig, TRUE, tcd, td, cd);
        }

        nod->methodcode = templateCode(pt, used, nod->methodcode, type_names, type_values);

        /* Handle any virtual handler. */
        if (od->virthandler != NULL)
        {
            moduleDef *mod = cd->iff->module;

            nod->virthandler = sipMalloc(sizeof (virtHandlerDef));

            /* Start with a shallow copy. */
            *nod->virthandler = *od->virthandler;

            if (od->virthandler->cppsig == &od->pysig)
                nod->virthandler->cppsig = &nod->pysig;
            else
            {
                nod->virthandler->cppsig = sipMalloc(sizeof (signatureDef));

                *nod->virthandler->cppsig = *od->virthandler->cppsig;

                templateSignature(nod->virthandler->cppsig, TRUE, tcd, td, cd);
            }

            nod->virthandler->module = mod;
            nod->virthandler->virtcode = templateCode(pt, used, nod->virthandler->virtcode, type_names, type_values);
            nod->virthandler->next = mod->virthandlers;

            mod->virthandlers = nod->virthandler;
        }

        nod->next = NULL;
        *odtail = nod;
        odtail = &nod->next;
    }

    return overloads;
}


/*
 * Instantiate the enums of a template class.
 */
static void instantiateTemplateEnums(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    enumDef *ted;
    moduleDef *mod = cd->iff->module;

    for (ted = pt->enums; ted != NULL; ted = ted->next)
        if (ted->ecd == tcd->cd)
        {
            enumDef *ed;
            enumMemberDef *temd;

            ed = sipMalloc(sizeof (enumDef));

            /* Start with a shallow copy. */
            *ed = *ted;

            if (ed->fqcname != NULL)
            {
                ed->fqcname = text2scopedName(cd->iff,
                        scopedNameTail(ed->fqcname));
                ed->cname = cacheName(pt, scopedNameToString(ed->fqcname));
            }

            if (inMainModule())
            {
                if (ed->pyname != NULL)
                    setIsUsedName(ed->pyname);

                if (ed->cname != NULL)
                    setIsUsedName(ed->cname);
            }

            ed->ecd = cd;
            ed->first_alt = ed;
            ed->module = mod;
            ed->members = NULL;

            for (temd = ted->members; temd != NULL; temd = temd->next)
            {
                enumMemberDef *emd;

                emd = sipMalloc(sizeof (enumMemberDef));

                /* Start with a shallow copy. */
                *emd = *temd;
                emd->ed = ed;

                emd->next = ed->members;
                ed->members = emd;
            }

            ed->slots = instantiateTemplateMethods(ted->slots, mod);
            ed->overs = instantiateTemplateOverloads(pt, ted->overs,
                    ted->slots, ed->slots, tcd, td, cd, used, type_names,
                    type_values);

            ed->next = pt->enums;
            pt->enums = ed;
        }
}


/*
 * Instantiate the variables of a template class.
 */
static void instantiateTemplateVars(sipSpec *pt, classTmplDef *tcd,
        templateDef *td, classDef *cd, ifaceFileList **used,
        scopedNameDef *type_names, scopedNameDef *type_values)
{
    varDef *tvd;

    for (tvd = pt->vars; tvd != NULL; tvd = tvd->next)
        if (tvd->ecd == tcd->cd)
        {
            varDef *vd;

            vd = sipMalloc(sizeof (varDef));

            /* Start with a shallow copy. */
            *vd = *tvd;

            if (inMainModule())
                setIsUsedName(vd->pyname);

            vd->fqcname = text2scopedName(cd->iff,
                    scopedNameTail(vd->fqcname));
            vd->ecd = cd;
            vd->module = cd->iff->module;

            templateType(&vd->type, tcd, td, cd);

            vd->accessfunc = templateCode(pt, used, vd->accessfunc, type_names, type_values);
            vd->getcode = templateCode(pt, used, vd->getcode, type_names, type_values);
            vd->setcode = templateCode(pt, used, vd->setcode, type_names, type_values);

            addVariable(pt, vd);
        }
}


/*
 * Replace any template arguments in a signature.
 */
static void templateSignature(signatureDef *sd, int result, classTmplDef *tcd, templateDef *td, classDef *ncd)
{
    int a;

    if (result)
        templateType(&sd->result, tcd, td, ncd);

    for (a = 0; a < sd->nrArgs; ++a)
        templateType(&sd->args[a], tcd, td, ncd);
}


/*
 * Replace any template arguments in a type.
 */
static void templateType(argDef *ad, classTmplDef *tcd, templateDef *td, classDef *ncd)
{
    int a;
    char *name;

    /* Descend into any sub-templates. */
    if (ad->atype == template_type)
    {
        templateDef *new_td = sipMalloc(sizeof (templateDef));

        /* Make a deep copy of the template definition. */
        *new_td = *ad->u.td;
        ad->u.td = new_td;

        templateSignature(&ad->u.td->types, FALSE, tcd, td, ncd);

        return;
    }

    /* Ignore if it isn't an unscoped name. */
    if (ad->atype != defined_type || ad->u.snd->next != NULL)
        return;

    name = ad->u.snd->name;

    for (a = 0; a < tcd->sig.nrArgs - 1; ++a)
        if (strcmp(name, scopedNameTail(tcd->sig.args[a].u.snd)) == 0)
        {
            argDef *tad = &td->types.args[a];

            ad->atype = tad->atype;

            /* We take the constrained flag from the real type. */
            resetIsConstrained(ad);

            if (isConstrained(tad))
                setIsConstrained(ad);

            ad->u = tad->u;

            return;
        }

    /* Handle the class name itself. */
    if (strcmp(name, scopedNameTail(classFQCName(tcd->cd))) == 0)
    {
        ad->atype = class_type;
        ad->u.cd = ncd;
        ad->original_type = NULL;
    }
}


/*
 * Replace any template arguments in a literal code block.
 */
codeBlock *templateCode(sipSpec *pt, ifaceFileList **used, codeBlock *ocb,
        scopedNameDef *names, scopedNameDef *values)
{
    codeBlock *ncb = NULL, **tail = &ncb;

    while (ocb != NULL)
    {
        char *at = ocb->frag;

        do
        {
            char *first = NULL;
            codeBlock *cb;
            scopedNameDef *nam, *val, *nam_first, *val_first;

            /*
             * Go through the rest of this fragment looking for each of the
             * types and the name of the class itself.
             */
            nam = names;
            val = values;

            while (nam != NULL && val != NULL)
            {
                char *cp;

                if ((cp = strstr(at, nam->name)) != NULL)
                    if (first == NULL || first > cp)
                    {
                        nam_first = nam;
                        val_first = val;
                        first = cp;
                    }

                nam = nam->next;
                val = val->next;
            }

            /* Create the new fragment. */
            cb = sipMalloc(sizeof (codeBlock));

            if (at == ocb->frag)
            {
                cb->filename = ocb->filename;
                cb->linenr = ocb->linenr;
            }
            else
                cb->filename = NULL;

            cb->next = NULL;
            *tail = cb;
            tail = &cb->next;

            /* See if anything was found. */
            if (first == NULL)
            {
                /* We can just point to this. */
                cb->frag = at;

                /* All done with this one. */
                at = NULL;
            }
            else
            {
                static char *gen_names[] = {
                    "sipType_",
                    "sipClass_",
                    "sipEnum_",
                    "sipException_",
                    NULL
                };

                char *dp, *sp, **gn;
                int genname = FALSE;

                /*
                 * If the context in which the text is used is in the name of a
                 * SIP generated object then translate any "::" scoping to "_".
                 */
                for (gn = gen_names; *gn != NULL; ++gn)
                    if (search_back(first, at, *gn))
                    {
                        addUsedFromCode(pt, used, val_first->name);
                        genname = TRUE;
                        break;
                    }

                /* Fragment the fragment. */
                cb->frag = sipMalloc(first - at + strlen(val_first->name) + 1);

                strncpy(cb->frag, at, first - at);

                dp = &cb->frag[first - at];
                sp = val_first->name;

                if (genname)
                {
                    char gch;

                    while ((gch = *sp++) != '\0')
                        if (gch == ':' && *sp == ':')
                        {
                            *dp++ = '_';
                            ++sp;
                        }
                        else
                            *dp++ = gch;

                    *dp = '\0';
                }
                else
                    strcpy(dp, sp);

                /* Move past the replaced text. */
                at = first + strlen(nam_first->name);
            }
        }
        while (at != NULL && *at != '\0');

        ocb = ocb->next;
    }

    return ncb;
}


/*
 * Return TRUE if the text at the end of a string matches the target string.
 */
static int search_back(const char *end, const char *start, const char *target)
{
    size_t tlen = strlen(target);

    if (start + tlen >= end)
        return FALSE;

    return (strncmp(end - tlen, target, tlen) == 0);
}


/*
 * Add any needed interface files based on handwritten code.
 */
static void addUsedFromCode(sipSpec *pt, ifaceFileList **used, const char *sname)
{
    ifaceFileDef *iff;
    enumDef *ed;

    for (iff = pt->ifacefiles; iff != NULL; iff = iff->next)
    {
        if (iff->type != class_iface && iff->type != exception_iface)
            continue;

        if (sameName(iff->fqcname, sname))
        {
            addToUsedList(used, iff);
            return;
        }
    }

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        if (ed->ecd == NULL)
            continue;

        if (sameName(ed->fqcname, sname))
        {
            addToUsedList(used, ed->ecd->iff);
            return;
        }
    }
}


/*
 * Compare a scoped name with its string equivalent.
 */
static int sameName(scopedNameDef *snd, const char *sname)
{
    while (snd != NULL && *sname != '\0')
    {
        const char *sp = snd->name;

        while (*sp != '\0' && *sname != ':' && *sname != '\0')
            if (*sp++ != *sname++)
                return FALSE;

        if (*sp != '\0' || (*sname != ':' && *sname != '\0'))
            return FALSE;

        snd = snd->next;

        if (*sname == ':')
            sname += 2;
    }

    return (snd == NULL && *sname == '\0');
}


/*
 * Compare a (possibly) relative scoped name with a fully qualified scoped name
 * while taking the current scope into account.
 */
static int foundInScope(scopedNameDef *fq_name, scopedNameDef *rel_name)
{
    classDef *scope;

    for (scope = currentScope(); scope != NULL; scope = scope->ecd)
    {
        scopedNameDef *snd;
        int found;

        snd = copyScopedName(classFQCName(scope));
        appendScopedName(&snd, copyScopedName(rel_name));

        found = (compareScopedNames(fq_name, snd) == 0);

        freeScopedName(snd);

        if (found)
            return TRUE;
    }

    return compareScopedNames(fq_name, rel_name) == 0;
}


/*
 * Create a new typedef.
 */
static void newTypedef(sipSpec *pt, moduleDef *mod, char *name, argDef *type,
        optFlags *optflgs)
{
    typedefDef *td, **tdp;
    scopedNameDef *fqname;
    classDef *scope;

    scope = currentScope();
    fqname = text2scopedName((scope != NULL ? scope->iff : NULL), name);

    /* See if we are instantiating a template class. */
    if (type->atype == template_type)
    {
        classTmplDef *tcd;
        templateDef *td = type->u.td;

        for (tcd = pt->classtemplates; tcd != NULL; tcd = tcd->next)
            if (foundInScope(tcd->cd->iff->fqcname, td->fqname) &&
                sameTemplateSignature(&tcd->sig, &td->types, FALSE))
            {
                instantiateClassTemplate(pt, mod, scope, fqname, tcd, td);

                /* All done. */
                return;
            }
    }

    /*
     * Check it doesn't already exist and find the position in the sorted list
     * where it should be put.
     */
    for (tdp = &pt->typedefs; *tdp != NULL; tdp = &(*tdp)->next)
    {
        int res = compareScopedNames((*tdp)->fqname, fqname);

        if (res == 0)
        {
            fatalScopedName(fqname);
            fatal(" already defined\n");
        }

        if (res > 0)
            break;
    }

    td = sipMalloc(sizeof (typedefDef));

    td->tdflags = 0;
    td->fqname = fqname;
    td->ecd = scope;
    td->module = mod;
    td->type = *type;

    td->next = *tdp;
    *tdp = td;

    if (findOptFlag(optflgs, "NoTypeName", bool_flag) != NULL)
        setNoTypeName(td);

    mod->nrtypedefs++;
}


/*
 * Speculatively try and resolve any typedefs.  In some cases (eg. when
 * comparing template signatures) it helps to use the real type if it is known.
 * Note that this wouldn't be necessary if we required that all types be known
 * before they are used.
 */
static void resolveAnyTypedef(sipSpec *pt, argDef *ad)
{
    argDef orig = *ad;

    while (ad->atype == defined_type)
    {
        ad->atype = no_type;
        searchTypedefs(pt, ad->u.snd, ad);

        /*
         * Don't resolve to a template type as it may be superceded later on
         * by a more specific mapped type.
         */
        if (ad->atype == no_type || ad->atype == template_type)
        {
            *ad = orig;
            break;
        }
    }
}


/*
 * Return TRUE if the template signatures are the same.  A deep comparison is
 * used for mapped type templates where we want to recurse into any nested
 * templates.
 */
int sameTemplateSignature(signatureDef *tmpl_sd, signatureDef *args_sd,
        int deep)
{
    int a;

    if (tmpl_sd->nrArgs != args_sd->nrArgs)
        return FALSE;

    for (a = 0; a < tmpl_sd->nrArgs; ++a)
    {
        argDef *tmpl_ad = &tmpl_sd->args[a];
        argDef *args_ad = &args_sd->args[a];

        /*
         * If we are doing a shallow comparision (ie. for class templates) then
         * a type name in the template signature matches anything in the
         * argument signature.
         */
        if (tmpl_ad->atype == defined_type && !deep)
            continue;

        /*
         * For type names only compare the references and pointers, and do the
         * same for any nested templates.
         */
        if (tmpl_ad->atype == defined_type && args_ad->atype == defined_type)
        {
            if (isReference(tmpl_ad) != isReference(args_ad) || tmpl_ad->nrderefs != args_ad->nrderefs)
                return FALSE;
        }
        else if (tmpl_ad->atype == template_type && args_ad->atype == template_type)
        {
            if (!sameTemplateSignature(&tmpl_ad->u.td->types, &args_ad->u.td->types, deep))
                return FALSE;
        }
        else if (!sameBaseType(tmpl_ad, args_ad))
            return FALSE;
    }

    return TRUE;
}


/*
 * Create a new variable.
 */
static void newVar(sipSpec *pt,moduleDef *mod,char *name,int isstatic,
           argDef *type,optFlags *of,codeBlock *acode,codeBlock *gcode,
           codeBlock *scode)
{
    varDef *var;
    classDef *escope = currentScope();
    nameDef *nd = cacheName(pt,getPythonName(of,name));

    if (inMainModule())
        setIsUsedName(nd);

    checkAttributes(pt, mod, escope, NULL, nd->text, FALSE);

    var = sipMalloc(sizeof (varDef));

    var->pyname = nd;
    var->fqcname = text2scopedName((escope != NULL ? escope->iff : NULL),
            name);
    var->ecd = escope;
    var->module = mod;
    var->varflags = 0;
    var->type = *type;
    var->accessfunc = acode;
    var->getcode = gcode;
    var->setcode = scode;

    if (isstatic || (escope != NULL && escope->iff->type == namespace_iface))
        setIsStaticVar(var);

    addVariable(pt, var);
}


/*
 * Create a new ctor.
 */
static void newCtor(char *name, int sectFlags, signatureDef *args,
        optFlags *optflgs, codeBlock *methodcode, throwArgs *exceptions,
        signatureDef *cppsig, int explicit, codeBlock *docstring)
{
    ctorDef *ct, **ctp;
    classDef *cd = currentScope();

    /* Check the name of the constructor. */
    if (strcmp(classBaseName(cd), name) != 0)
        yyerror("Constructor doesn't have the same name as its class");

    if (docstring != NULL)
        appendCodeBlock(&cd->docstring, docstring);

    /* Add to the list of constructors. */
    ct = sipMalloc(sizeof (ctorDef));

    if (sectFlags & SECT_IS_PROT && makeProtPublic)
    {
        sectFlags &= ~SECT_IS_PROT;
        sectFlags |= SECT_IS_PUBLIC;
    }

    /* Allow the signature to be used like an function signature. */
    memset(&args->result, 0, sizeof (argDef));
    args->result.atype = void_type;

    ct->ctorflags = sectFlags;
    ct->api_range = getAPIRange(optflgs);
    ct->pysig = *args;
    ct->cppsig = (cppsig != NULL ? cppsig : &ct->pysig);
    ct->exceptions = exceptions;
    ct->methodcode = methodcode;

    if (!isPrivateCtor(ct))
        setCanCreate(cd);

    if (isProtectedCtor(ct))
        setHasShadow(cd);

    if (explicit)
        setIsExplicitCtor(ct);

    getHooks(optflgs, &ct->prehook, &ct->posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGILCtor(ct);
    else if (getHoldGIL(optflgs))
        setIsHoldGILCtor(ct);

    if (getTransfer(optflgs))
        setIsResultTransferredCtor(ct);

    if (getDeprecated(optflgs))
        setIsDeprecatedCtor(ct);

    if (!isPrivateCtor(ct) && usesKeywordArgs(optflgs, &ct->pysig))
        setUseKeywordArgsCtor(ct);

    if (findOptFlag(optflgs, "NoDerived", bool_flag) != NULL)
    {
        if (cppsig != NULL)
            yyerror("The /NoDerived/ annotation cannot be used with a C++ signature");

        if (methodcode == NULL)
            yyerror("The /NoDerived/ annotation must be used with %MethodCode");

        ct->cppsig = NULL;
    }

    if (findOptFlag(optflgs, "Default", bool_flag) != NULL)
    {
        if (cd->defctor != NULL)
            yyerror("A constructor with the /Default/ annotation has already been defined");

        cd->defctor = ct;
    }

    /* Append to the list. */
    for (ctp = &cd->ctors; *ctp != NULL; ctp = &(*ctp)->next)
        ;

    *ctp = ct;
}


/*
 * Create a new function.
 */
static void newFunction(sipSpec *pt, moduleDef *mod, classDef *c_scope,
        mappedTypeDef *mt_scope, int sflags, int isstatic, int isvirt,
        char *name, signatureDef *sig, int isconst, int isabstract,
        optFlags *optflgs, codeBlock *methodcode, codeBlock *vcode,
        throwArgs *exceptions, signatureDef *cppsig, codeBlock *docstring)
{
    int factory, xferback, no_arg_parser;
    overDef *od, **odp, **headp;
    optFlag *of;
    virtHandlerDef *vhd;

    /* Extra checks for a C module. */
    if (pt->genc)
    {
        if (c_scope != NULL)
            yyerror("Function declaration not allowed in a struct in a C module");

        if (isstatic)
            yyerror("Static functions not allowed in a C module");

        if (exceptions != NULL)
            yyerror("Exceptions not allowed in a C module");
    }

    if (mt_scope != NULL)
        headp = &mt_scope->overs;
    else if (c_scope != NULL)
        headp = &c_scope->overs;
    else
        headp = &mod->overs;

    /* See if it is a factory method. */
    if (findOptFlag(optflgs, "Factory", bool_flag) != NULL)
        factory = TRUE;
    else
    {
        int a;

        factory = FALSE;

        /* Check /TransferThis/ wasn't specified. */
        if (c_scope == NULL || isstatic)
            for (a = 0; a < sig->nrArgs; ++a)
                if (isThisTransferred(&sig->args[a]))
                    yyerror("/TransferThis/ may only be specified in constructors and class methods");
    }

    /* See if the result is to be returned to Python ownership. */
    xferback = (findOptFlag(optflgs, "TransferBack", bool_flag) != NULL);

    if (factory && xferback)
        yyerror("/TransferBack/ and /Factory/ cannot both be specified");

    /* Create a new overload definition. */

    od = sipMalloc(sizeof (overDef));

    /* Set the overload flags. */

    if (sflags & SECT_IS_PROT && makeProtPublic)
    {
        sflags &= ~SECT_IS_PROT;
        sflags |= SECT_IS_PUBLIC | OVER_REALLY_PROT;
    }

    od -> overflags = sflags;

    if (factory)
        setIsFactory(od);

    if (xferback)
        setIsResultTransferredBack(od);

    if (getTransfer(optflgs))
        setIsResultTransferred(od);

    if (findOptFlag(optflgs, "TransferThis", bool_flag) != NULL)
        setIsThisTransferredMeth(od);

    if (isProtected(od))
        setHasShadow(c_scope);

    if ((isSlot(od) || isSignal(od)) && !isPrivate(od))
    {
        if (isSignal(od))
            setHasShadow(c_scope);

        pt->sigslots = TRUE;
    }

    if (isSignal(od) && (methodcode != NULL || vcode != NULL))
        yyerror("Cannot provide code for signals");

    if (isstatic)
    {
        if (isSignal(od))
            yyerror("Static functions cannot be signals");

        if (isvirt)
            yyerror("Static functions cannot be virtual");

        setIsStatic(od);
    }

    if (isconst)
        setIsConst(od);

    if (isabstract)
    {
        if (sflags == 0)
            yyerror("Non-class function specified as abstract");

        setIsAbstract(od);
    }

    if ((of = findOptFlag(optflgs, "AutoGen", opt_name_flag)) != NULL)
    {
        if (of->fvalue.sval == NULL || isEnabledFeature(of->fvalue.sval))
            setIsAutoGen(od);
    }

    if (isvirt)
    {
        if (isSignal(od) && pluginPyQt3(pt))
            yyerror("Virtual signals aren't supported");

        setIsVirtual(od);
        setHasShadow(c_scope);

        vhd = sipMalloc(sizeof (virtHandlerDef));

        vhd->virthandlernr = -1;
        vhd->vhflags = 0;
        vhd->pysig = &od->pysig;
        vhd->cppsig = (cppsig != NULL ? cppsig : &od->pysig);
        vhd->virtcode = vcode;

        if (factory || xferback)
            setIsTransferVH(vhd);

        /*
         * Only add it to the module's virtual handlers if we are not in a
         * class template.
         */
        if (!currentIsTemplate)
        {
            vhd->module = mod;

            vhd->next = mod->virthandlers;
            mod->virthandlers = vhd;
        }
    }
    else
    {
        if (vcode != NULL)
            yyerror("%VirtualCatcherCode provided for non-virtual function");

        vhd = NULL;
    }

    od->cppname = name;
    od->pysig = *sig;
    od->cppsig = (cppsig != NULL ? cppsig : &od->pysig);
    od->exceptions = exceptions;
    od->methodcode = methodcode;
    od->virthandler = vhd;

    no_arg_parser = (findOptFlag(optflgs, "NoArgParser", bool_flag) != NULL);

    if (no_arg_parser)
    {
        if (methodcode == NULL)
            yyerror("%MethodCode must be supplied if /NoArgParser/ is specified");
    }

    if (findOptFlag(optflgs, "NoCopy", bool_flag) != NULL)
        setNoCopy(&od->pysig.result);

    od->common = findFunction(pt, mod, c_scope, mt_scope,
            getPythonName(optflgs, name), (methodcode != NULL), sig->nrArgs,
            no_arg_parser);

    if (docstring != NULL)
        appendCodeBlock(&od->common->docstring, docstring);

    od->api_range = getAPIRange(optflgs);

    if (od->api_range == NULL)
        setNotVersioned(od->common);

    if (findOptFlag(optflgs, "Numeric", bool_flag) != NULL)
        setIsNumeric(od->common);

    /* Methods that run in new threads must be virtual. */
    if (findOptFlag(optflgs, "NewThread", bool_flag) != NULL)
    {
        argDef *res;

        if (!isvirt)
            yyerror("/NewThread/ may only be specified for virtual functions");

        /*
         * This is an arbitary limitation to make the code generator slightly
         * easier - laziness on my part.
         */
        res = &od->cppsig->result;

        if (res->atype != void_type || res->nrderefs != 0)
            yyerror("/NewThread/ may only be specified for void functions");

        setIsNewThread(od);
    }

    getHooks(optflgs, &od->prehook, &od->posthook);

    if (getReleaseGIL(optflgs))
        setIsReleaseGIL(od);
    else if (getHoldGIL(optflgs))
        setIsHoldGIL(od);

    if (getDeprecated(optflgs))
        setIsDeprecated(od);

    if (!isPrivate(od) && !isSignal(od) && od->common->slot == no_slot && usesKeywordArgs(optflgs, &od->pysig))
    {
        setUseKeywordArgs(od);
        setUseKeywordArgsFunction(od->common);
    }

    od -> next = NULL;

    /* Append to the list. */
    for (odp = headp; *odp != NULL; odp = &(*odp)->next)
        ;

    *odp = od;
}


/*
 * Return the Python name based on the C/C++ name and any /PyName/ annotation.
 */
static const char *getPythonName(optFlags *optflgs, const char *cname)
{
    const char *pname;
    optFlag *of;

    if ((of = findOptFlag(optflgs, "PyName", name_flag)) != NULL)
        pname = of->fvalue.sval;
    else
        pname = cname;

    return pname;
}


/*
 * Cache a name in a module.  Entries in the cache are stored in order of
 * decreasing length.
 */
nameDef *cacheName(sipSpec *pt, const char *name)
{
    nameDef *nd, **ndp;
    size_t len;

    /* Allow callers to be lazy about checking if there is really a name. */
    if (name == NULL)
        return NULL;

    /* Skip entries that are too large. */
    ndp = &pt->namecache;
    len = strlen(name);

    while (*ndp != NULL && (*ndp)->len > len)
        ndp = &(*ndp)->next;

    /* Check entries that are the right length. */
    for (nd = *ndp; nd != NULL && nd->len == len; nd = nd->next)
        if (memcmp(nd->text, name, len) == 0)
            return nd;

    /* Create a new one. */
    nd = sipMalloc(sizeof (nameDef));

    nd->nameflags = 0;
    nd->text = name;
    nd->len = len;
    nd->next = *ndp;

    *ndp = nd;

    return nd;
}


/*
 * Find (or create) an overloaded function name.
 */
static memberDef *findFunction(sipSpec *pt, moduleDef *mod, classDef *c_scope,
        mappedTypeDef *mt_scope, const char *pname, int hwcode, int nrargs,
        int no_arg_parser)
{
    static struct slot_map {
        const char *name;   /* The slot name. */
        slotType type;      /* The corresponding type. */
        int needs_hwcode;   /* Set if handwritten code is required. */
        int nrargs;         /* Nr. of arguments. */
    } slot_table[] = {
        {"__str__", str_slot, TRUE, 0},
        {"__unicode__", unicode_slot, TRUE, 0},
        {"__int__", int_slot, FALSE, 0},
        {"__long__", long_slot, FALSE, 0},
        {"__float__", float_slot, FALSE, 0},
        {"__len__", len_slot, TRUE, 0},
        {"__contains__", contains_slot, TRUE, 1},
        {"__add__", add_slot, FALSE, 1},
        {"__sub__", sub_slot, FALSE, 1},
        {"__mul__", mul_slot, FALSE, 1},
        {"__div__", div_slot, FALSE, 1},
        {"__mod__", mod_slot, FALSE, 1},
        {"__floordiv__", floordiv_slot, TRUE, 1},
        {"__truediv__", truediv_slot, FALSE, 1},
        {"__and__", and_slot, FALSE, 1},
        {"__or__", or_slot, FALSE, 1},
        {"__xor__", xor_slot, FALSE, 1},
        {"__lshift__", lshift_slot, FALSE, 1},
        {"__rshift__", rshift_slot, FALSE, 1},
        {"__iadd__", iadd_slot, FALSE, 1},
        {"__isub__", isub_slot, FALSE, 1},
        {"__imul__", imul_slot, FALSE, 1},
        {"__idiv__", idiv_slot, FALSE, 1},
        {"__imod__", imod_slot, FALSE, 1},
        {"__ifloordiv__", ifloordiv_slot, TRUE, 1},
        {"__itruediv__", itruediv_slot, FALSE, 1},
        {"__iand__", iand_slot, FALSE, 1},
        {"__ior__", ior_slot, FALSE, 1},
        {"__ixor__", ixor_slot, FALSE, 1},
        {"__ilshift__", ilshift_slot, FALSE, 1},
        {"__irshift__", irshift_slot, FALSE, 1},
        {"__invert__", invert_slot, FALSE, 0},
        {"__call__", call_slot, FALSE, -1},
        {"__getitem__", getitem_slot, FALSE, 1},
        {"__setitem__", setitem_slot, TRUE, 2},
        {"__delitem__", delitem_slot, TRUE, 1},
        {"__lt__", lt_slot, FALSE, 1},
        {"__le__", le_slot, FALSE, 1},
        {"__eq__", eq_slot, FALSE, 1},
        {"__ne__", ne_slot, FALSE, 1},
        {"__gt__", gt_slot, FALSE, 1},
        {"__ge__", ge_slot, FALSE, 1},
        {"__cmp__", cmp_slot, FALSE, 1},
        {"__bool__", bool_slot, TRUE, 0},
        {"__nonzero__", bool_slot, TRUE, 0},
        {"__neg__", neg_slot, FALSE, 0},
        {"__pos__", pos_slot, FALSE, 0},
        {"__abs__", abs_slot, TRUE, 0},
        {"__repr__", repr_slot, TRUE, 0},
        {"__hash__", hash_slot, TRUE, 0},
        {"__index__", index_slot, TRUE, 0},
        {"__iter__", iter_slot, TRUE, 0},
        {"__next__", next_slot, TRUE, 0},
        {NULL}
    };

    memberDef *md, **flist;
    struct slot_map *sm;
    slotType st;

    /* Get the slot type. */
    st = no_slot;

    for (sm = slot_table; sm->name != NULL; ++sm)
        if (strcmp(sm->name, pname) == 0)
        {
            if (sm->needs_hwcode && !hwcode)
                yyerror("This Python slot requires %MethodCode");

            if (sm->nrargs >= 0)
            {
                if (mt_scope == NULL && c_scope == NULL)
                {
                    /* Global operators need one extra argument. */
                    if (sm -> nrargs + 1 != nrargs)
                        yyerror("Incorrect number of arguments to global operator");
                }
                else if (sm->nrargs != nrargs)
                    yyerror("Incorrect number of arguments to Python slot");
            }

            st = sm->type;

            break;
        }

    /* Check there is no name clash. */
    checkAttributes(pt, mod, c_scope, mt_scope, pname, TRUE);

    /* See if it already exists. */
    if (mt_scope != NULL)
        flist = &mt_scope->members;
    else if (c_scope != NULL)
        flist = &c_scope->members;
    else
        flist = &mod->othfuncs;

    for (md = *flist; md != NULL; md = md->next)
        if (strcmp(md->pyname->text, pname) == 0 && md->module == mod)
            break;

    if (md == NULL)
    {
        /* Create a new one. */
        md = sipMalloc(sizeof (memberDef));

        md->pyname = cacheName(pt, pname);
        md->memberflags = 0;
        md->slot = st;
        md->module = mod;
        md->next = *flist;

        *flist = md;

        if (inMainModule())
            setIsUsedName(md->pyname);

        if (no_arg_parser)
            setNoArgParser(md);
    }
    else if (noArgParser(md))
        yyerror("Another overload has already been defined that is annotated as /NoArgParser/");

    /* Global operators are a subset. */
    if (mt_scope == NULL && c_scope == NULL && st != no_slot && st != neg_slot && st != pos_slot && !isNumberSlot(md) && !isRichCompareSlot(md))
        yyerror("Global operators must be either numeric or comparison operators");

    return md;
}


/*
 * Search a set of flags for a particular one and check its type.
 */
static optFlag *findOptFlag(optFlags *flgs,char *name,flagType ft)
{
    int f;

    for (f = 0; f < flgs -> nrFlags; ++f)
    {
        optFlag *of = &flgs -> flags[f];

        if (strcmp(of -> fname,name) == 0)
        {
            /*
             * An optional name can look like a boolean or a name.
             */

            if (ft == opt_name_flag)
            {
                if (of -> ftype == bool_flag)
                {
                    of -> ftype = opt_name_flag;
                    of -> fvalue.sval = NULL;
                }
                else if (of -> ftype == name_flag)
                    of -> ftype = opt_name_flag;
            }

            if (ft != of -> ftype)
                yyerror("Optional flag has a value of the wrong type");

            return of;
        }
    }

    return NULL;
}


/*
 * A name is going to be used as a Python attribute name within a Python scope
 * (ie. a Python dictionary), so check against what we already know is going in
 * the same scope in case there is a clash.
 */
static void checkAttributes(sipSpec *pt, moduleDef *mod, classDef *py_c_scope,
        mappedTypeDef *py_mt_scope, const char *attr, int isfunc)
{
    enumDef *ed;
    varDef *vd;
    classDef *cd;

    /* Check the enums. */

    for (ed = pt->enums; ed != NULL; ed = ed->next)
    {
        enumMemberDef *emd;

        if (ed->pyname == NULL)
            continue;

        if (py_c_scope != NULL)
        {
            if (ed->ecd != py_c_scope)
                continue;
        }
        else if (py_mt_scope != NULL)
        {
            if (ed->emtd != py_mt_scope)
                continue;
        }
        else if (ed->ecd != NULL || ed->emtd != NULL)
        {
            continue;
        }

        if (strcmp(ed->pyname->text, attr) == 0)
            yyerror("There is already an enum in scope with the same Python name");

        for (emd = ed->members; emd != NULL; emd = emd->next)
            if (strcmp(emd->pyname->text, attr) == 0)
                yyerror("There is already an enum member in scope with the same Python name");
    }

    /*
     * Only check the members if this attribute isn't a member because we
     * can handle members with the same name in the same scope.
     */
    if (!isfunc)
    {
        memberDef *md, *membs;
        overDef *overs;

        if (py_mt_scope != NULL)
        {
            membs = py_mt_scope->members;
            overs = py_mt_scope->overs;
        }
        else if (py_c_scope != NULL)
        {
            membs = py_c_scope->members;
            overs = py_c_scope->overs;
        }
        else
        {
            membs = mod->othfuncs;
            overs = mod->overs;
        }

        for (md = membs; md != NULL; md = md->next)
        {
            overDef *od;

            if (strcmp(md->pyname->text, attr) != 0)
                continue;

            /* Check for a conflict with all overloads. */
            for (od = overs; od != NULL; od = od->next)
            {
                if (od->common != md)
                    continue;

                yyerror("There is already a function in scope with the same Python name");
            }
        }
    }

    /* If the scope was a mapped type then that's all we have to check. */
    if (py_mt_scope != NULL)
        return;

    /* Check the variables. */
    for (vd = pt->vars; vd != NULL; vd = vd->next)
    {
        if (vd->ecd != py_c_scope)
            continue;

        if (strcmp(vd->pyname->text,attr) == 0)
            yyerror("There is already a variable in scope with the same Python name");
    }

    /* Check the classes. */
    for (cd = pt->classes; cd != NULL; cd = cd->next)
    {
        if (cd->ecd != py_c_scope || cd->pyname == NULL)
            continue;

        if (strcmp(cd->pyname->text, attr) == 0 && !isExternal(cd))
            yyerror("There is already a class or namespace in scope with the same Python name");
    }

    /* Check the exceptions. */
    if (py_c_scope == NULL)
    {
        exceptionDef *xd;

        for (xd = pt->exceptions; xd != NULL; xd = xd->next)
            if (xd->pyname != NULL && strcmp(xd->pyname, attr) == 0)
                yyerror("There is already an exception with the same Python name");
    }
}


/*
 * Append a code block to a list of them.  Append is needed to give the
 * specifier easy control over the order of the documentation.
 */
void appendCodeBlock(codeBlock **headp, codeBlock *new)
{
    while (*headp != NULL)
        headp = &(*headp)->next;

    *headp = new;
}


/*
 * Handle the end of a fully parsed a file.
 */
static void handleEOF()
{
    /*
     * Check that the number of nested if's is the same as when we started
     * the file.
     */

    if (skipStackPtr > currentContext.ifdepth)
        fatal("Too many %%If statements in %s\n", previousFile);

    if (skipStackPtr < currentContext.ifdepth)
        fatal("Too many %%End statements in %s\n", previousFile);
}


/*
 * Handle the end of a fully parsed a module.
 */
static void handleEOM()
{
    moduleDef *from;

    /* Check it has been named. */
    if (currentModule->name == NULL)
        fatal("No %%Module has been specified for module defined in %s\n",
                previousFile);

    from = currentContext.prevmod;

    if (from != NULL && from->encoding == no_type)
        from->encoding = currentModule->encoding;

    /* The previous module is now current. */
    currentModule = from;
}


/*
 * Find an existing qualifier.
 */
static qualDef *findQualifier(const char *name)
{
    moduleDef *mod;

    for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
    {
        qualDef *qd;

        for (qd = mod->qualifiers; qd != NULL; qd = qd->next)
            if (strcmp(qd->name, name) == 0)
                return qd;
    }

    return NULL;
}


/*
 * Find an existing API.
 */
apiVersionRangeDef *findAPI(sipSpec *pt, const char *name)
{
    moduleDef *mod;

    for (mod = pt->modules; mod != NULL; mod = mod->next)
    {
        apiVersionRangeDef *avd;

        for (avd = mod->api_versions; avd != NULL; avd = avd->next)
            if (strcmp(avd->api_name->text, name) == 0)
                return avd;
    }

    return NULL;
}


/*
 * Return a copy of a scoped name.
 */
scopedNameDef *copyScopedName(scopedNameDef *snd)
{
    scopedNameDef *head;

    head = NULL;

    while (snd != NULL)
    {
        appendScopedName(&head,text2scopePart(snd -> name));
        snd = snd -> next;
    }

    return head;
}


/*
 * Append a name to a list of scopes.
 */
void appendScopedName(scopedNameDef **headp,scopedNameDef *newsnd)
{
    while (*headp != NULL)
        headp = &(*headp) -> next;

    *headp = newsnd;
}


/*
 * Free a scoped name - but not the text itself.
 */
void freeScopedName(scopedNameDef *snd)
{
    while (snd != NULL)
    {
        scopedNameDef *next = snd -> next;

        free(snd);

        snd = next;
    }
}


/*
 * Convert a text string to a scope part structure.
 */
static scopedNameDef *text2scopePart(char *text)
{
    scopedNameDef *snd;

    snd = sipMalloc(sizeof (scopedNameDef));

    snd->name = text;
    snd->next = NULL;

    return snd;
}


/*
 * Convert a text string to a fully scoped name.
 */
static scopedNameDef *text2scopedName(ifaceFileDef *scope, char *text)
{
    return scopeScopedName(scope, text2scopePart(text));
}


/*
 * Prepend any current scope to a scoped name.
 */
static scopedNameDef *scopeScopedName(ifaceFileDef *scope, scopedNameDef *name)
{
    scopedNameDef *snd;

    snd = (scope != NULL ? copyScopedName(scope->fqcname) : NULL);

    appendScopedName(&snd, name);

    return snd;
}


/*
 * Return a pointer to the tail part of a scoped name.
 */
char *scopedNameTail(scopedNameDef *snd)
{
    if (snd == NULL)
        return NULL;

    while (snd -> next != NULL)
        snd = snd -> next;

    return snd -> name;
}


/*
 * Push the given scope onto the scope stack.
 */
static void pushScope(classDef *scope)
{
    if (currentScopeIdx >= MAX_NESTED_SCOPE)
        fatal("Internal error: increase the value of MAX_NESTED_SCOPE\n");

    scopeStack[currentScopeIdx] = scope;
    sectFlagsStack[currentScopeIdx] = sectionFlags;

    ++currentScopeIdx;
}


/*
 * Pop the scope stack.
 */
static void popScope(void)
{
    if (currentScopeIdx > 0)
        sectionFlags = sectFlagsStack[--currentScopeIdx];
}


/*
 * Return non-zero if the current input should be parsed rather than be
 * skipped.
 */
static int notSkipping()
{
    return (skipStackPtr == 0 ? TRUE : skipStack[skipStackPtr - 1]);
}


/*
 * Return the value of an expression involving a time period.
 */
static int timePeriod(char *lname,char *uname)
{
    int this, line;
    qualDef *qd, *lower, *upper;
    moduleDef *mod;

    if (lname == NULL)
        lower = NULL;
    else if ((lower = findQualifier(lname)) == NULL || lower -> qtype != time_qualifier)
        yyerror("Lower bound is not a time version");

    if (uname == NULL)
        upper = NULL;
    else if ((upper = findQualifier(uname)) == NULL || upper -> qtype != time_qualifier)
        yyerror("Upper bound is not a time version");

    /* Sanity checks on the bounds. */

    if (lower == NULL && upper == NULL)
        yyerror("Lower and upper bounds cannot both be omitted");

    if (lower != NULL && upper != NULL)
    {
        if (lower -> module != upper -> module || lower -> line != upper -> line)
            yyerror("Lower and upper bounds are from different timelines");

        if (lower == upper)
            yyerror("Lower and upper bounds must be different");

        if (lower -> order > upper -> order)
            yyerror("Later version specified as lower bound");
    }

    /* Go through each slot in the relevant timeline. */

    if (lower != NULL)
    {
        mod = lower -> module;
        line = lower -> line;
    }
    else
    {
        mod = upper -> module;
        line = upper -> line;
    }

    this = FALSE;

    for (qd = mod -> qualifiers; qd != NULL; qd = qd -> next)
    {
        if (qd -> qtype != time_qualifier || qd -> line != line)
            continue;

        if (lower != NULL && qd -> order < lower -> order)
            continue;

        if (upper != NULL && qd -> order >= upper -> order)
            continue;

        /*
         * This is within the required range so if it is also needed
         * then the expression is true.
         */

        if (isNeeded(qd))
        {
            this = TRUE;
            break;
        }
    }

    return this;
}


/*
 * Return the value of an expression involving a single platform or feature.
 */
static int platOrFeature(char *name,int optnot)
{
    int this;
    qualDef *qd;

    if ((qd = findQualifier(name)) == NULL || qd -> qtype == time_qualifier)
        yyerror("No such platform or feature");

    /* Assume this sub-expression is false. */

    this = FALSE;

    if (qd -> qtype == feature_qualifier)
    {
        if (!excludedFeature(excludedQualifiers,qd))
            this = TRUE;
    }
    else if (isNeeded(qd))
        this = TRUE;

    if (optnot)
        this = !this;

    return this;
}


/*
 * Return TRUE if the given qualifier is excluded.
 */
int excludedFeature(stringList *xsl,qualDef *qd)
{
    while (xsl != NULL)
    {
        if (strcmp(qd -> name,xsl -> s) == 0)
            return TRUE;

        xsl = xsl -> next;
    }

    return FALSE;
}


/*
 * Return TRUE if the given qualifier is needed.
 */
static int isNeeded(qualDef *qd)
{
    stringList *sl;

    for (sl = neededQualifiers; sl != NULL; sl = sl -> next)
        if (strcmp(qd -> name,sl -> s) == 0)
            return TRUE;

    return FALSE;
}


/*
 * Return the current scope.  currentScope() is only valid if notSkipping()
 * returns non-zero.
 */
static classDef *currentScope(void)
{
    return (currentScopeIdx > 0 ? scopeStack[currentScopeIdx - 1] : NULL);
}


/*
 * Create a new qualifier.
 */
static void newQualifier(moduleDef *mod, int line, int order, char *name,
        qualType qt)
{
    qualDef *qd;

    /* Check it doesn't already exist. */

    if (findQualifier(name) != NULL)
        yyerror("Version is already defined");

    qd = sipMalloc(sizeof (qualDef));
    qd->name = name;
    qd->qtype = qt;
    qd->module = mod;
    qd->line = line;
    qd->order = order;
    qd->next = mod -> qualifiers;
    mod->qualifiers = qd;
}


/*
 * Create a new imported module.
 */
static void newImport(char *filename)
{
    moduleDef *from, *mod;
    moduleListDef *mld;

    /* Create a new module if it has not already been defined. */
    for (mod = currentSpec->modules; mod != NULL; mod = mod->next)
        if (strcmp(mod->file, filename) == 0)
            break;

    from = currentModule;

    if (mod == NULL)
    {
        newModule(NULL, filename);
        mod = currentModule;
    }
    else if (from->encoding == no_type)
    {
        /* Import any defaults from the already parsed module. */
        from->encoding = mod->encoding;
    }

    /* Add the new import unless it has already been imported. */
    for (mld = from->imports; mld != NULL; mld = mld->next)
        if (mld->module == mod)
            return;

    mld = sipMalloc(sizeof (moduleListDef));
    mld->module = mod;
    mld->next = from->imports;

    from->imports = mld;
}


/*
 * Set up pointers to hook names.
 */
static void getHooks(optFlags *optflgs,char **pre,char **post)
{
    optFlag *of;

    if ((of = findOptFlag(optflgs,"PreHook",name_flag)) != NULL)
        *pre = of -> fvalue.sval;
    else
        *pre = NULL;

    if ((of = findOptFlag(optflgs,"PostHook",name_flag)) != NULL)
        *post = of -> fvalue.sval;
    else
        *post = NULL;
}


/*
 * Get the /Transfer/ option flag.
 */
static int getTransfer(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "Transfer", bool_flag) != NULL);
}


/*
 * Get the /ReleaseGIL/ option flag.
 */
static int getReleaseGIL(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "ReleaseGIL", bool_flag) != NULL);
}


/*
 * Get the /HoldGIL/ option flag.
 */
static int getHoldGIL(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "HoldGIL", bool_flag) != NULL);
}


/*
 * Get the /Deprecated/ option flag.
 */
static int getDeprecated(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "Deprecated", bool_flag) != NULL);
}


/*
 * Get the /AllowNone/ option flag.
 */
static int getAllowNone(optFlags *optflgs)
{
    return (findOptFlag(optflgs, "AllowNone", bool_flag) != NULL);
}


/*
 * Get the /DocType/ option flag.
 */
static const char *getDocType(optFlags *optflgs)
{
    optFlag *of = findOptFlag(optflgs, "DocType", string_flag);

    if (of == NULL)
        return NULL;

    return of->fvalue.sval;
}


/*
 * Get the /DocValue/ option flag.
 */
static const char *getDocValue(optFlags *optflgs)
{
    optFlag *of = findOptFlag(optflgs, "DocValue", string_flag);

    if (of == NULL)
        return NULL;

    return of->fvalue.sval;
}


/*
 * Return TRUE if the PyQt3 plugin was specified.
 */
int pluginPyQt3(sipSpec *pt)
{
    return stringFind(pt->plugins, "PyQt3");
}


/*
 * Return TRUE if the PyQt4 plugin was specified.
 */
int pluginPyQt4(sipSpec *pt)
{
    return stringFind(pt->plugins, "PyQt4");
}


/*
 * Return TRUE if a list of strings contains a given entry.
 */
static int stringFind(stringList *sl, const char *s)
{
    while (sl != NULL)
    {
        if (strcmp(sl->s, s) == 0)
            return TRUE;

        sl = sl->next;
    }

    return FALSE;
}


/*
 * Set the name of a module.
 */
static void setModuleName(sipSpec *pt, moduleDef *mod, const char *fullname)
{
    mod->fullname = cacheName(pt, fullname);

    if (inMainModule())
        setIsUsedName(mod->fullname);

    if ((mod->name = strrchr(fullname, '.')) != NULL)
        mod->name++;
    else
        mod->name = fullname;
}


/*
 * Define a new class and set its name.
 */
static void defineClass(scopedNameDef *snd, classList *supers, optFlags *of)
{
    classDef *cd, *c_scope = currentScope();

    cd = newClass(currentSpec, class_iface, getAPIRange(of),
            scopeScopedName((c_scope != NULL ? c_scope->iff : NULL), snd));
    cd->supers = supers;

    pushScope(cd);
}


/*
 * Complete the definition of a class.
 */
static classDef *completeClass(scopedNameDef *snd, optFlags *of, int has_def)
{
    classDef *cd = currentScope();

    /* See if the class was defined or just declared. */
    if (has_def)
    {
        if (snd->next != NULL)
            yyerror("A scoped name cannot be given in a class/struct definition");

    }
    else if (cd->supers != NULL)
        yyerror("Class/struct has super-classes but no definition");
    else
        setIsOpaque(cd);

    finishClass(currentSpec, currentModule, cd, of);
    popScope();

    /*
     * Check that external classes have only been declared at the global scope.
     */
    if (isExternal(cd) && currentScope() != NULL)
        yyerror("External classes/structs can only be declared in the global scope");

    return cd;
}


/*
 * Add a variable to the list so that the list remains sorted.
 */
static void addVariable(sipSpec *pt, varDef *vd)
{
    varDef **at = &pt->vars;

    while (*at != NULL)
    {
        if (strcmp(vd->pyname->text, (*at)->pyname->text) < 0)
            break;

        at = &(*at)->next;
    }

    vd->next = *at;
    *at = vd;
}


/*
 * Update a type according to optional flags.
 */
static void applyTypeFlags(moduleDef *mod, argDef *ad, optFlags *flags)
{
    ad->doctype = getDocType(flags);

    if (ad->atype == string_type && !isArray(ad) && !isReference(ad))
    {
        optFlag *of;

        if ((of = findOptFlag(flags, "Encoding", string_flag)) == NULL)
        {
            if (mod->encoding != no_type)
                ad->atype = mod->encoding;
            else
                ad->atype = string_type;
        }
        else if ((ad->atype = convertEncoding(of->fvalue.sval)) == no_type)
            yyerror("The value of the /Encoding/ annotation must be one of \"ASCII\", \"Latin-1\", \"UTF-8\" or \"None\"");
    }
}


/*
 * Return the argument type for a string with the given encoding or no_type if
 * the encoding was invalid.
 */
static argType convertEncoding(const char *encoding)
{
    if (strcmp(encoding, "ASCII") == 0)
        return ascii_string_type;

    if (strcmp(encoding, "Latin-1") == 0)
        return latin1_string_type;

    if (strcmp(encoding, "UTF-8") == 0)
        return utf8_string_type;

    if (strcmp(encoding, "None") == 0)
        return string_type;

    return no_type;
}


/*
 * Get the /API/ option flag.
 */
static apiVersionRangeDef *getAPIRange(optFlags *optflgs)
{
    optFlag *of;

    if ((of = findOptFlag(optflgs, "API", api_range_flag)) == NULL)
        return NULL;

    return of->fvalue.aval;
}


/*
 * Return the API range structure and version number corresponding to the
 * given API range.
 */
static apiVersionRangeDef *convertAPIRange(moduleDef *mod, nameDef *name,
        int from, int to)
{
    int index;
    apiVersionRangeDef *avd, **avdp;

    /* Handle the trivial case. */
    if (from == 0 && to == 0)
        return NULL;

    for (index = 0, avdp = &mod->api_ranges; (*avdp) != NULL; avdp = &(*avdp)->next, ++index)
    {
        avd = *avdp;

        if (avd->api_name == name && avd->from == from && avd->to == to)
            return avd;
    }

    /* The new one must be appended so that version numbers remain valid. */
    avd = sipMalloc(sizeof (apiVersionRangeDef));

    avd->api_name = name;
    avd->from = from;
    avd->to = to;
    avd->index = index;

    avd->next = NULL;
    *avdp = avd;

    return avd;
}


/*
 * Return TRUE if a signature with annotations uses keyword arguments.
 */
static int usesKeywordArgs(optFlags *optflgs, signatureDef *sd)
{
    int kwd_args_anno, no_kwd_args_anno;

    kwd_args_anno = (findOptFlag(optflgs, "KeywordArgs", bool_flag) != NULL);
    no_kwd_args_anno = (findOptFlag(optflgs, "NoKeywordArgs", bool_flag) != NULL);

    /*
     * An ellipsis cannot be used with keyword arguments.  Only complain if it
     * has been explicitly requested.
     */
    if (kwd_args_anno && sd->nrArgs > 0 && sd->args[sd->nrArgs - 1].atype == ellipsis_type)
        yyerror("/KeywordArgs/ cannot be specified for calls with a variable number of arguments");

    if ((defaultKwdArgs || kwd_args_anno) && !no_kwd_args_anno)
    {
        int a, is_name = FALSE;

        /*
         * Mark argument names as being used and check there is at least one.
         */
        for (a = 0; a < sd->nrArgs; ++a)
        {
            nameDef *nd = sd->args[a].name;

            if (sd->args[a].name != NULL)
            {
                setIsUsedName(nd);
                is_name = TRUE;
            }
        }

        return is_name;
    }

    return FALSE;
}


/*
 * Extract the version of a string value optionally associated with a
 * particular feature.
 */
static char *convertFeaturedString(char *fs)
{
    while (fs != NULL)
    {
        char *next, *value;

        /* Individual values are ';' separated. */
        if ((next = strchr(fs, ';')) != NULL)
            *next++ = '\0';

        /* Features and values are ':' separated. */
        if ((value = strchr(fs, ':')) == NULL)
        {
            /* This is an unconditional value so just return it. */
            return strip(fs);
        }

        *value++ = '\0';

        if (isEnabledFeature(strip(fs)))
            return strip(value);

        fs = next;
    }

    /* No value was enabled. */
    return NULL;
}


/*
 * Return the stripped version of a string.
 */
static char *strip(char *s)
{
    while (*s == ' ')
        ++s;

    if (*s != '\0')
    {
        char *cp = &s[strlen(s) - 1];

        while (*cp == ' ')
            *cp-- = '\0';
    }

    return s;
}


/*
 * Return TRUE if the given feature is enabled.
 */
static int isEnabledFeature(const char *name)
{
    qualDef *qd;

    if ((qd = findQualifier(name)) == NULL || qd->qtype != feature_qualifier)
        yyerror("No such feature");

    return !excludedFeature(excludedQualifiers, qd);
}

