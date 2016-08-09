dnl @synopsis AX_LIB_COAP([MINIMUM-VERSION])
dnl
dnl Test for the libz library of a particular version (or newer).
dnl
dnl If no path to the installed coap is given, the macro will first try
dnl using no -I or -L flags, then searches under /usr, /usr/local, /opt,
dnl and /opt/coap.
dnl If these all fail, it will try the $COAP_ROOT environment variable.
dnl
dnl This macro calls:
dnl   AC_SUBST(COAP_CPPFLAGS)
dnl   AC_SUBST(COAP_LDFLAGS)
dnl   AC_SUBST(COAP_LIBS)
dnl
dnl And (if coap is found):
dnl   AC_DEFINE(HAVE_COAP)
dnl
dnl It also leaves the shell variables "success" and "ax_have_coap"
dnl set to "yes" or "no".
dnl
dnl NOTE: This macro does not currently work for cross-compiling,
dnl       but it can be easily modified to allow it.  (grep "cross").
dnl
dnl @category InstalledPackages
dnl @category C
dnl @version 2007-09-12
dnl @license AllPermissive
dnl
dnl Copyright (C) 2009 David Reiss
dnl Copyright (C) 2016 Christopher Friedt
dnl Copying and distribution of this file, with or without modification,
dnl are permitted in any medium without royalty provided the copyright
dnl notice and this notice are preserved.

dnl Input: ax_coap_path, WANT_COAP_VERSION
dnl Output: success=yes/no
AC_DEFUN([AX_LIB_COAP_DO_CHECK],
         [
          # Save our flags.
          CPPFLAGS_SAVED="$CPPFLAGS"
          LDFLAGS_SAVED="$LDFLAGS"
          LIBS_SAVED="$LIBS"
          LD_LIBRARY_PATH_SAVED="$LD_LIBRARY_PATH"

          # Set our flags if we are checking a specific directory.
          if test -n "$ax_coap_path" ; then
            COAP_CPPFLAGS="-I$ax_coap_path/include"
            COAP_LDFLAGS="-L$ax_coap_path/lib"
            LD_LIBRARY_PATH="$ax_coap_path/lib:$LD_LIBRARY_PATH"
          else
            COAP_CPPFLAGS="-DWITH_POSIX=1"
            COAP_LDFLAGS=""
          fi

          # Required flag for coap.
          COAP_LIBS="-lcoap-1"

          # Prepare the environment for compilation.
          CPPFLAGS="$CPPFLAGS $COAP_CPPFLAGS"
          LDFLAGS="$LDFLAGS $COAP_LDFLAGS"
          LIBS="$LIBS $COAP_LIBS"
          export CPPFLAGS
          export LDFLAGS
          export LIBS
          export LD_LIBRARY_PATH

          success=no

          # Compile, link, and run the program.  This checks:
          # - coap/coap.h is available for including.
          # - coapVersion() is available for linking.
          # - LIBCOAP_PACKAGE_NAME is "libcoap"
          # XXX: @CF: Holding off on this check for now, since the version in coap.h does not reflect the library version (4.1.2 vs 1.0)
          # - LIBCOAP_PACKAGE_VERSION is greater than or equal to the desired version.
          #   (defined in the library).
          AC_LANG_PUSH([C])
          dnl This can be changed to AC_LINK_IFELSE if you are cross-compiling.
          AC_LINK_IFELSE([AC_LANG_PROGRAM([[
          #include <coap/coap.h>
          ]], [[
          const char* lib_version = coap_package_version();
          return 0;
          ]])], [
          success=yes
          ])
          AC_LANG_POP([C])

          # Restore flags.
          CPPFLAGS="$CPPFLAGS_SAVED"
          LDFLAGS="$LDFLAGS_SAVED"
          LIBS="$LIBS_SAVED"
          LD_LIBRARY_PATH="$LD_LIBRARY_PATH_SAVED"
         ])


AC_DEFUN([AX_LIB_COAP],
         [

          dnl Allow search path to be overridden on the command line.
          AC_ARG_WITH([coap],
                      AS_HELP_STRING([--with-coap@<:@=DIR@:>@], [use coap (default is yes) - it is possible to specify an alternate root directory for coap]),
                      [
                       if test "x$withval" = "xno"; then
                         want_coap="no"
                       elif test "x$withval" = "xyes"; then
                         want_coap="yes"
                         ax_coap_path=""
                       else
                         want_coap="yes"
                         ax_coap_path="$withval"
                       fi
                       ],
                       [want_coap="yes" ; ax_coap_path="" ])


          if test "$want_coap" = "yes"; then
            # Parse out the version.
            coap_version_req=ifelse([$1], ,1.2.3,$1)
            coap_version_req_major=`expr $coap_version_req : '\([[0-9]]*\)'`
            coap_version_req_minor=`expr $coap_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
            coap_version_req_patch=`expr $coap_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
            if test -z "$coap_version_req_patch" ; then
              coap_version_req_patch="0"
            fi
            WANT_COAP_VERSION=`expr $coap_version_req_major \* 1000 \+  $coap_version_req_minor \* 100 \+ $coap_version_req_patch \* 10`

            AC_MSG_CHECKING(for coap >= $coap_version_req)

            # Run tests.
            if test -n "$ax_coap_path"; then
              AX_LIB_COAP_DO_CHECK
            else
              for ax_coap_path in "" /usr /usr/local /opt /opt/coap "$COAP_ROOT" ; do
                AX_LIB_COAP_DO_CHECK
                if test "$success" = "yes"; then
                  break;
                fi
              done
            fi

            if test "$success" != "yes" ; then
              AC_MSG_RESULT(no)
              COAP_CPPFLAGS=""
              COAP_LDFLAGS=""
              COAP_LIBS=""
            else
              AC_MSG_RESULT(yes)
              AC_DEFINE(HAVE_COAP,,[define if coap is available])
            fi

            ax_have_coap="$success"

            AC_SUBST(COAP_CPPFLAGS)
            AC_SUBST(COAP_LDFLAGS)
            AC_SUBST(COAP_LIBS)
          fi

          ])
