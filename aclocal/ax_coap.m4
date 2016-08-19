dnl @synopsis AX_COAP([MINIMUM-VERSION])
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
AC_DEFUN([AX_COAP],
         [
          AC_ARG_WITH([coap],
                      AS_HELP_STRING([--with-coap], [use coap (default is yes)]),
                      [
                       if test "x$withval" = "xno"; then
                         want_coap="no"
                       elif test "x$withval" = "xyes"; then
                         want_coap="yes"
                       else
                         want_coap="yes"
                       fi
                       ],
                       [want_coap="yes"])


           if test "$want_coap" = "yes"; then
             AC_DEFINE(HAVE_COAP,,[define if coap is requested])
           fi
          ])
