dnl $Id: config.m4,v 0.4 2004-04-18 13:35:10 turbo Exp $
dnl config.m4 for extension idn

PHP_ARG_WITH(idn,whether to include IDN support,
[  --with-idn[=DIR]        Include IDN support.  DIR is the LibIDN base 
                          install directory.])
PHP_ARG_ENABLE(idn-tld,whether to include TLD checking in IDN,
[  --disable-idn-tld       Do not include TLD checking in IDN even if
                            it is available.],yes)

if test "$PHP_IDN" != "no"; then
  PHP_EXTENSION(idn, $ext_shared)

  for i in /usr /usr/local $PHP_IDN; do
    if test -f $i/include/idna.h; then
      IDN_DIR=$i
    fi
  done

  if test -z "$IDN_DIR"; then
    AC_MSG_ERROR([Cannot find libidn])
  fi

  IDN_INCDIR=$IDN_DIR/include
  IDN_LIBDIR=$IDN_DIR/lib

  AC_CHECK_LIB(idn,
	       idna_to_unicode_8z4z,
	       [AC_DEFINE(HAVE_IDN,1,[ ])],
	       [AC_MSG_ERROR([broken libidn])],
	       [-I$IDN_INCDIR -L$IDN_LIBDIR -lidn])

if test "$PHP_IDN_TLD" != "no"; then
  AC_CHECK_LIB(idn,
	       tld_check_8z,
	       [AC_DEFINE(HAVE_IDN_TLD,1,[ ])],
	       [AC_MSG_WARN([no TLD checking support, disabling it])],
	       [-I$IDN_INCDIR -L$IDN_LIBDIR -lidn])
fi

  PHP_SUBST(IDN_SHARED_LIBADD)

  PHP_ADD_LIBRARY_WITH_PATH(idn, $IDN_LIBDIR, IDN_SHARED_LIBADD)
  PHP_ADD_INCLUDE($IDN_INCDIR)
fi
