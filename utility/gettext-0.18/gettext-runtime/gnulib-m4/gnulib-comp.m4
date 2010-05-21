# DO NOT EDIT! GENERATED AUTOMATICALLY!
# Copyright (C) 2002-2010 Free Software Foundation, Inc.
#
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects using CVS, this file can be treated like other built files.


# This macro should be invoked from gettext-runtime/configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([gl_EARLY],
[
  m4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace
  m4_pattern_allow([^gl_ES$])dnl a valid locale name
  m4_pattern_allow([^gl_LIBOBJS$])dnl a variable
  m4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable
  AC_REQUIRE([AC_PROG_RANLIB])
  AC_REQUIRE([AM_PROG_CC_C_O])
  # Code from module alloca-opt:
  # Code from module areadlink:
  # Code from module arg-nonnull:
  # Code from module atexit:
  # Code from module basename:
  # Code from module c++defs:
  # Code from module c-ctype:
  # Code from module c-strcase:
  # Code from module canonicalize-lgpl:
  # Code from module closeout:
  # Code from module configmake:
  # Code from module csharpcomp-script:
  # Code from module double-slash-root:
  # Code from module environ:
  # Code from module errno:
  # Code from module error:
  # Code from module extensions:
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  # Code from module fwriteerror:
  # Code from module getopt-gnu:
  # Code from module getopt-posix:
  # Code from module gettext-h:
  # Code from module gettext-runtime-misc:
  # Code from module gperf:
  # Code from module havelib:
  # Code from module iconv:
  # Code from module iconv-h:
  # Code from module iconv_open:
  # Code from module include_next:
  # Code from module intprops:
  # Code from module java:
  # Code from module javacomp-script:
  # Code from module localcharset:
  # Code from module lstat:
  # Code from module malloc-posix:
  # Code from module malloca:
  # Code from module mbchar:
  # Code from module mbiter:
  # Code from module mbrtowc:
  # Code from module mbsinit:
  # Code from module mbslen:
  # Code from module mbsstr:
  # Code from module mbuiter:
  # Code from module memchr:
  # Code from module memmove:
  # Code from module multiarch:
  # Code from module pathmax:
  # Code from module progname:
  # Code from module propername:
  # Code from module readlink:
  # Code from module relocatable-prog:
  # Code from module relocatable-prog-wrapper:
  # Code from module signal:
  # Code from module sigpipe:
  # Code from module sigprocmask:
  # Code from module ssize_t:
  # Code from module stat:
  # Code from module stdbool:
  # Code from module stddef:
  # Code from module stdint:
  # Code from module stdio:
  # Code from module stdlib:
  # Code from module streq:
  # Code from module strerror:
  # Code from module striconv:
  # Code from module string:
  # Code from module strnlen:
  # Code from module strnlen1:
  # Code from module strtol:
  # Code from module strtoul:
  # Code from module sys_stat:
  # Code from module time:
  # Code from module trim:
  # Code from module unistd:
  # Code from module unitypes:
  # Code from module uniwidth/base:
  # Code from module uniwidth/width:
  # Code from module unlocked-io:
  # Code from module verify:
  # Code from module warn-on-use:
  # Code from module wchar:
  # Code from module wctype:
  # Code from module wcwidth:
  # Code from module xalloc:
  # Code from module xreadlink:
  # Code from module xstriconv:
])

# This macro should be invoked from gettext-runtime/configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([gl_INIT],
[
  AM_CONDITIONAL([GL_COND_LIBTOOL], [false])
  gl_cond_libtool=false
  gl_libdeps=
  gl_ltlibdeps=
  gl_m4_base='gnulib-m4'
  m4_pushdef([AC_LIBOBJ], m4_defn([gl_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([gl_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([gl_LIBSOURCES]))
  m4_pushdef([gl_LIBSOURCES_LIST], [])
  m4_pushdef([gl_LIBSOURCES_DIR], [])
  gl_COMMON
  gl_source_base='gnulib-lib'
  # Code from module alloca-opt:
  gl_FUNC_ALLOCA
  # Code from module areadlink:
  # Code from module arg-nonnull:
  # Code from module atexit:
  gl_FUNC_ATEXIT
  # Code from module basename:
  # Code from module c++defs:
  # Code from module c-ctype:
  # Code from module c-strcase:
  # Code from module canonicalize-lgpl:
  gl_CANONICALIZE_LGPL
  gl_MODULE_INDICATOR([canonicalize-lgpl])
  gl_STDLIB_MODULE_INDICATOR([canonicalize_file_name])
  gl_STDLIB_MODULE_INDICATOR([realpath])
  # Code from module closeout:
  # Code from module configmake:
  # Code from module csharpcomp-script:
  AC_REQUIRE([gt_CSHARPCOMP])
  AC_CONFIG_FILES([csharpcomp.sh:../build-aux/csharpcomp.sh.in])
  # Code from module double-slash-root:
  gl_DOUBLE_SLASH_ROOT
  # Code from module environ:
  gl_ENVIRON
  gl_UNISTD_MODULE_INDICATOR([environ])
  # Code from module errno:
  gl_HEADER_ERRNO_H
  # Code from module error:
  gl_ERROR
  m4_ifdef([AM_XGETTEXT_OPTION],
    [AM_][XGETTEXT_OPTION([--flag=error:3:c-format])
     AM_][XGETTEXT_OPTION([--flag=error_at_line:5:c-format])])
  # Code from module extensions:
  # Code from module fwriteerror:
  gl_MODULE_INDICATOR([fwriteerror])
  # Code from module getopt-gnu:
  gl_FUNC_GETOPT_GNU
  gl_MODULE_INDICATOR_FOR_TESTS([getopt-gnu])
  # Code from module getopt-posix:
  gl_FUNC_GETOPT_POSIX
  # Code from module gettext-h:
  AC_SUBST([LIBINTL])
  AC_SUBST([LTLIBINTL])
  # Code from module gettext-runtime-misc:
  # Code from module gperf:
  # Code from module havelib:
  # Code from module iconv:
  AM_ICONV
  # Code from module iconv-h:
  gl_ICONV_H
  # Code from module iconv_open:
  gl_FUNC_ICONV_OPEN
  # Code from module include_next:
  # Code from module intprops:
  # Code from module java:
  gt_JAVA_CHOICE
  # Code from module javacomp-script:
  # You need to invoke gt_JAVACOMP yourself, possibly with arguments.
  AC_CONFIG_FILES([javacomp.sh:../build-aux/javacomp.sh.in])
  # Code from module localcharset:
  gl_LOCALCHARSET
  LOCALCHARSET_TESTS_ENVIRONMENT="CHARSETALIASDIR=\"\$(top_builddir)/$gl_source_base\""
  AC_SUBST([LOCALCHARSET_TESTS_ENVIRONMENT])
  # Code from module lstat:
  gl_FUNC_LSTAT
  gl_SYS_STAT_MODULE_INDICATOR([lstat])
  # Code from module malloc-posix:
  gl_FUNC_MALLOC_POSIX
  gl_STDLIB_MODULE_INDICATOR([malloc-posix])
  # Code from module malloca:
  gl_MALLOCA
  # Code from module mbchar:
  gl_MBCHAR
  # Code from module mbiter:
  gl_MBITER
  # Code from module mbrtowc:
  gl_FUNC_MBRTOWC
  gl_WCHAR_MODULE_INDICATOR([mbrtowc])
  # Code from module mbsinit:
  gl_FUNC_MBSINIT
  gl_WCHAR_MODULE_INDICATOR([mbsinit])
  # Code from module mbslen:
  gl_FUNC_MBSLEN
  gl_STRING_MODULE_INDICATOR([mbslen])
  # Code from module mbsstr:
  gl_STRING_MODULE_INDICATOR([mbsstr])
  # Code from module mbuiter:
  gl_MBITER
  # Code from module memchr:
  gl_FUNC_MEMCHR
  gl_STRING_MODULE_INDICATOR([memchr])
  # Code from module memmove:
  gl_FUNC_MEMMOVE
  # Code from module multiarch:
  gl_MULTIARCH
  # Code from module pathmax:
  gl_PATHMAX
  # Code from module progname:
  AC_CHECK_DECLS([program_invocation_name], [], [], [#include <errno.h>])
  AC_CHECK_DECLS([program_invocation_short_name], [], [], [#include <errno.h>])
  # Code from module propername:
  m4_ifdef([AM_XGETTEXT_OPTION],
    [AM_][XGETTEXT_OPTION([--keyword='proper_name:1,\"This is a proper name. See the gettext manual, section Names.\"'])
     AM_][XGETTEXT_OPTION([--keyword='proper_name_utf8:1,\"This is a proper name. See the gettext manual, section Names.\"'])])
  # Code from module readlink:
  gl_FUNC_READLINK
  gl_UNISTD_MODULE_INDICATOR([readlink])
  # Code from module relocatable-prog:
  gl_RELOCATABLE([$gl_source_base])
  # Code from module relocatable-prog-wrapper:
  gl_FUNC_READLINK_SEPARATE
  gl_CANONICALIZE_LGPL_SEPARATE
  gl_MALLOCA
  gl_RELOCATABLE_LIBRARY_SEPARATE
  gl_FUNC_SETENV_SEPARATE
  gl_FUNC_STRERROR_SEPARATE
  # Code from module signal:
  gl_SIGNAL_H
  # Code from module sigpipe:
  gl_SIGNAL_SIGPIPE
  dnl Define the C macro GNULIB_SIGPIPE to 1.
  gl_MODULE_INDICATOR([sigpipe])
  dnl Define the substituted variable GNULIB_SIGNAL_H_SIGPIPE to 1.
  AC_REQUIRE([gl_SIGNAL_H_DEFAULTS])
  GNULIB_SIGNAL_H_SIGPIPE=1
  dnl Define the substituted variable GNULIB_STDIO_H_SIGPIPE to 1.
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  GNULIB_STDIO_H_SIGPIPE=1
  dnl Define the substituted variable GNULIB_UNISTD_H_SIGPIPE to 1.
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  GNULIB_UNISTD_H_SIGPIPE=1
  # Code from module sigprocmask:
  gl_SIGNALBLOCKING
  gl_SIGNAL_MODULE_INDICATOR([sigprocmask])
  # Code from module ssize_t:
  gt_TYPE_SSIZE_T
  # Code from module stat:
  gl_FUNC_STAT
  gl_SYS_STAT_MODULE_INDICATOR([stat])
  # Code from module stdbool:
  AM_STDBOOL_H
  # Code from module stddef:
  gl_STDDEF_H
  # Code from module stdint:
  gl_STDINT_H
  # Code from module stdio:
  gl_STDIO_H
  # Code from module stdlib:
  gl_STDLIB_H
  # Code from module streq:
  # Code from module strerror:
  gl_FUNC_STRERROR
  gl_STRING_MODULE_INDICATOR([strerror])
  # Code from module striconv:
  if test $gl_cond_libtool = false; then
    gl_ltlibdeps="$gl_ltlibdeps $LTLIBICONV"
    gl_libdeps="$gl_libdeps $LIBICONV"
  fi
  # Code from module string:
  gl_HEADER_STRING_H
  # Code from module strnlen:
  gl_FUNC_STRNLEN
  gl_STRING_MODULE_INDICATOR([strnlen])
  # Code from module strnlen1:
  # Code from module strtol:
  gl_FUNC_STRTOL
  # Code from module strtoul:
  gl_FUNC_STRTOUL
  # Code from module sys_stat:
  gl_HEADER_SYS_STAT_H
  AC_PROG_MKDIR_P
  # Code from module time:
  gl_HEADER_TIME_H
  # Code from module trim:
  # Code from module unistd:
  gl_UNISTD_H
  # Code from module unitypes:
  # Code from module uniwidth/base:
  # Code from module uniwidth/width:
  # Code from module unlocked-io:
  gl_FUNC_GLIBC_UNLOCKED_IO
  # Code from module verify:
  # Code from module warn-on-use:
  # Code from module wchar:
  gl_WCHAR_H
  # Code from module wctype:
  gl_WCTYPE_H
  # Code from module wcwidth:
  gl_FUNC_WCWIDTH
  gl_WCHAR_MODULE_INDICATOR([wcwidth])
  # Code from module xalloc:
  # Code from module xreadlink:
  # Code from module xstriconv:
  # End of code from modules
  m4_ifval(gl_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([gl_LIBSOURCES_DIR])[ ||
      for gl_file in ]gl_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([gl_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([gl_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([gl_LIBSOURCES_DIR])
  m4_popdef([gl_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    gl_libobjs=
    gl_ltlibobjs=
    if test -n "$gl_LIBOBJS"; then
      # Remove the extension.
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      for i in `for i in $gl_LIBOBJS; do echo "$i"; done | sed -e "$sed_drop_objext" | sort | uniq`; do
        gl_libobjs="$gl_libobjs $i.$ac_objext"
        gl_ltlibobjs="$gl_ltlibobjs $i.lo"
      done
    fi
    AC_SUBST([gl_LIBOBJS], [$gl_libobjs])
    AC_SUBST([gl_LTLIBOBJS], [$gl_ltlibobjs])
  ])
  gltests_libdeps=
  gltests_ltlibdeps=
  m4_pushdef([AC_LIBOBJ], m4_defn([gltests_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([gltests_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([gltests_LIBSOURCES]))
  m4_pushdef([gltests_LIBSOURCES_LIST], [])
  m4_pushdef([gltests_LIBSOURCES_DIR], [])
  gl_COMMON
  gl_source_base='tests'
changequote(,)dnl
  gltests_WITNESS=IN_`echo "${PACKAGE-$PACKAGE_TARNAME}" | LC_ALL=C tr abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ | LC_ALL=C sed -e 's/[^A-Z0-9_]/_/g'`_GNULIB_TESTS
changequote([, ])dnl
  AC_SUBST([gltests_WITNESS])
  gl_module_indicator_condition=$gltests_WITNESS
  m4_pushdef([gl_MODULE_INDICATOR_CONDITION], [$gl_module_indicator_condition])
  m4_popdef([gl_MODULE_INDICATOR_CONDITION])
  m4_ifval(gltests_LIBSOURCES_LIST, [
    m4_syscmd([test ! -d ]m4_defn([gltests_LIBSOURCES_DIR])[ ||
      for gl_file in ]gltests_LIBSOURCES_LIST[ ; do
        if test ! -r ]m4_defn([gltests_LIBSOURCES_DIR])[/$gl_file ; then
          echo "missing file ]m4_defn([gltests_LIBSOURCES_DIR])[/$gl_file" >&2
          exit 1
        fi
      done])dnl
      m4_if(m4_sysval, [0], [],
        [AC_FATAL([expected source file, required through AC_LIBSOURCES, not found])])
  ])
  m4_popdef([gltests_LIBSOURCES_DIR])
  m4_popdef([gltests_LIBSOURCES_LIST])
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    gltests_libobjs=
    gltests_ltlibobjs=
    if test -n "$gltests_LIBOBJS"; then
      # Remove the extension.
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      for i in `for i in $gltests_LIBOBJS; do echo "$i"; done | sed -e "$sed_drop_objext" | sort | uniq`; do
        gltests_libobjs="$gltests_libobjs $i.$ac_objext"
        gltests_ltlibobjs="$gltests_ltlibobjs $i.lo"
      done
    fi
    AC_SUBST([gltests_LIBOBJS], [$gltests_libobjs])
    AC_SUBST([gltests_LTLIBOBJS], [$gltests_ltlibobjs])
  ])
  LIBGRT_LIBDEPS="$gl_libdeps"
  AC_SUBST([LIBGRT_LIBDEPS])
  LIBGRT_LTLIBDEPS="$gl_ltlibdeps"
  AC_SUBST([LIBGRT_LTLIBDEPS])
])

# Like AC_LIBOBJ, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_LIBOBJ], [
  AS_LITERAL_IF([$1], [gl_LIBSOURCES([$1.c])])dnl
  gl_LIBOBJS="$gl_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [gl_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([gl_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([gl_LIBSOURCES_DIR], [gnulib-lib])
      m4_append([gl_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# Like AC_LIBOBJ, except that the module name goes
# into gltests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gltests_LIBOBJ], [
  AS_LITERAL_IF([$1], [gltests_LIBSOURCES([$1.c])])dnl
  gltests_LIBOBJS="$gltests_LIBOBJS $1.$ac_objext"
])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into gltests_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gltests_REPLACE_FUNCS], [
  m4_foreach_w([gl_NAME], [$1], [AC_LIBSOURCES(gl_NAME[.c])])dnl
  AC_CHECK_FUNCS([$1], , [gltests_LIBOBJ($ac_func)])
])

# Like AC_LIBSOURCES, except the directory where the source file is
# expected is derived from the gnulib-tool parameterization,
# and alloca is special cased (for the alloca-opt module).
# We could also entirely rely on EXTRA_lib..._SOURCES.
AC_DEFUN([gltests_LIBSOURCES], [
  m4_foreach([_gl_NAME], [$1], [
    m4_if(_gl_NAME, [alloca.c], [], [
      m4_define([gltests_LIBSOURCES_DIR], [tests])
      m4_append([gltests_LIBSOURCES_LIST], _gl_NAME, [ ])
    ])
  ])
])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([gl_FILE_LIST], [
  build-aux/arg-nonnull.h
  build-aux/c++defs.h
  build-aux/config.libpath
  build-aux/config.rpath
  build-aux/csharpcomp.sh.in
  build-aux/install-reloc
  build-aux/javacomp.sh.in
  build-aux/reloc-ldflags
  build-aux/warn-on-use.h
  doc/relocatable.texi
  lib/alloca.in.h
  lib/areadlink.c
  lib/areadlink.h
  lib/atexit.c
  lib/basename.c
  lib/basename.h
  lib/c-ctype.c
  lib/c-ctype.h
  lib/c-strcase.h
  lib/c-strcasecmp.c
  lib/c-strncasecmp.c
  lib/canonicalize-lgpl.c
  lib/closeout.c
  lib/closeout.h
  lib/config.charset
  lib/errno.in.h
  lib/error.c
  lib/error.h
  lib/fwriteerror.c
  lib/fwriteerror.h
  lib/getopt.c
  lib/getopt.in.h
  lib/getopt1.c
  lib/getopt_int.h
  lib/gettext.h
  lib/iconv.in.h
  lib/iconv_open-aix.gperf
  lib/iconv_open-hpux.gperf
  lib/iconv_open-irix.gperf
  lib/iconv_open-osf.gperf
  lib/iconv_open-solaris.gperf
  lib/iconv_open.c
  lib/intprops.h
  lib/localcharset.c
  lib/localcharset.h
  lib/lstat.c
  lib/malloc.c
  lib/malloca.c
  lib/malloca.h
  lib/malloca.valgrind
  lib/mbchar.c
  lib/mbchar.h
  lib/mbiter.h
  lib/mbrtowc.c
  lib/mbsinit.c
  lib/mbslen.c
  lib/mbsstr.c
  lib/mbuiter.h
  lib/memchr.c
  lib/memchr.valgrind
  lib/memmove.c
  lib/pathmax.h
  lib/progname.c
  lib/progname.h
  lib/progreloc.c
  lib/propername.c
  lib/propername.h
  lib/readlink.c
  lib/ref-add.sin
  lib/ref-del.sin
  lib/relocatable.c
  lib/relocatable.h
  lib/relocwrapper.c
  lib/setenv.c
  lib/signal.in.h
  lib/sigprocmask.c
  lib/stat.c
  lib/stdbool.in.h
  lib/stddef.in.h
  lib/stdint.in.h
  lib/stdio-write.c
  lib/stdio.in.h
  lib/stdlib.in.h
  lib/str-kmp.h
  lib/streq.h
  lib/strerror.c
  lib/striconv.c
  lib/striconv.h
  lib/string.in.h
  lib/strnlen.c
  lib/strnlen1.c
  lib/strnlen1.h
  lib/strtol.c
  lib/strtoul.c
  lib/sys_stat.in.h
  lib/time.in.h
  lib/trim.c
  lib/trim.h
  lib/unistd.in.h
  lib/unitypes.h
  lib/uniwidth.h
  lib/uniwidth/cjk.h
  lib/uniwidth/width.c
  lib/unlocked-io.h
  lib/verify.h
  lib/wchar.in.h
  lib/wctype.in.h
  lib/wcwidth.c
  lib/xalloc.h
  lib/xmalloc.c
  lib/xreadlink.c
  lib/xreadlink.h
  lib/xstrdup.c
  lib/xstriconv.c
  lib/xstriconv.h
  m4/00gnulib.m4
  m4/alloca.m4
  m4/atexit.m4
  m4/canonicalize.m4
  m4/codeset.m4
  m4/csharp.m4
  m4/csharpcomp.m4
  m4/dos.m4
  m4/double-slash-root.m4
  m4/eealloc.m4
  m4/environ.m4
  m4/errno_h.m4
  m4/error.m4
  m4/extensions.m4
  m4/fcntl-o.m4
  m4/getopt.m4
  m4/glibc21.m4
  m4/gnulib-common.m4
  m4/iconv.m4
  m4/iconv_h.m4
  m4/iconv_open.m4
  m4/include_next.m4
  m4/java.m4
  m4/javacomp.m4
  m4/lib-ld.m4
  m4/lib-link.m4
  m4/lib-prefix.m4
  m4/localcharset.m4
  m4/locale-fr.m4
  m4/locale-ja.m4
  m4/locale-zh.m4
  m4/longlong.m4
  m4/lstat.m4
  m4/malloc.m4
  m4/malloca.m4
  m4/mbchar.m4
  m4/mbiter.m4
  m4/mbrtowc.m4
  m4/mbsinit.m4
  m4/mbslen.m4
  m4/mbstate_t.m4
  m4/memchr.m4
  m4/memmove.m4
  m4/mmap-anon.m4
  m4/multiarch.m4
  m4/onceonly.m4
  m4/pathmax.m4
  m4/readlink.m4
  m4/relocatable-lib.m4
  m4/relocatable.m4
  m4/setenv.m4
  m4/signal_h.m4
  m4/signalblocking.m4
  m4/sigpipe.m4
  m4/ssize_t.m4
  m4/stat.m4
  m4/stdbool.m4
  m4/stddef_h.m4
  m4/stdint.m4
  m4/stdio_h.m4
  m4/stdlib_h.m4
  m4/strerror.m4
  m4/string_h.m4
  m4/strnlen.m4
  m4/strtol.m4
  m4/strtoul.m4
  m4/sys_stat_h.m4
  m4/time_h.m4
  m4/unistd_h.m4
  m4/unlocked-io.m4
  m4/warn-on-use.m4
  m4/wchar_h.m4
  m4/wchar_t.m4
  m4/wctype_h.m4
  m4/wcwidth.m4
  m4/wint_t.m4
])