dnl $Id: config.m4,v 0.1 2003-11-07 18:31:08 turbo Exp $
dnl config.m4 for extension idn

PHP_ARG_WITH(idn,whether to include IDN support,
[  --with-idn[=DIR]        Include IDN support.  DIR is the LibIDN base 
                          install directory.])

if test "$PHP_IDN" != "no"; then
  PHP_EXTENSION(idn, $ext_shared)

  for i in $PHP_IDN /usr/local /usr ; do
    if test -f $i/include/idna.h; then
      IDN_DIR=$i
    fi
  done

  if test -z "$IDN_DIR"; then
    AC_MSG_ERROR(Cannot find libidn)
  fi

  IDN_INCDIR=$IDN_DIR/include
  IDN_LIBDIR=$IDN_DIR/lib

  PHP_TEMP_LDFLAGS(-L$IDN_LIBDIR,[
  AC_CHECK_LIB(idn, idna_to_unicode_8z4z, [AC_DEFINE(HAVE_IDN,1,[ ])],
    [AC_MSG_ERROR(broken libidn)])
  ])

  PHP_SUBST(IDN_SHARED_LIBADD)

  PHP_ADD_LIBRARY_WITH_PATH(idn, $IDN, IDN_SHARED_LIBADD)
  PHP_ADD_INCLUDE($IDN_INCDIR)
fi
