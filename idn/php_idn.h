/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_IDN_H
#define PHP_IDN_H

#include <idna.h>

extern zend_module_entry idn_module_entry;
#define idn_module_ptr &idn_module_entry

#ifdef PHP_WIN32
#define PHP_IDN_API __declspec(dllexport)
#else
#define PHP_IDN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(idn);
PHP_MSHUTDOWN_FUNCTION(idn);
PHP_MINFO_FUNCTION(idn);

/* IDNA2ASCII functions */
PHP_FUNCTION(idna_to_ascii);
PHP_FUNCTION(idna_to_ascii_from_locale);
PHP_FUNCTION(idna_to_ascii_from_ucs4);
PHP_FUNCTION(idna_to_ascii_from_utf8);

/* IDNA2UNICODE functions */
PHP_FUNCTION(idna_to_unicode);
PHP_FUNCTION(idna_to_unicode_locale_from_locale);
PHP_FUNCTION(idna_to_unicode_locale_from_utf8);
PHP_FUNCTION(idna_to_unicode_ucs4_from_ucs4);
PHP_FUNCTION(idna_to_unicode_ucs4_from_utf8);
PHP_FUNCTION(idna_to_unicode_utf8_from_utf8);

ZEND_BEGIN_MODULE_GLOBALS(idn)
	int allowunassigned;
	int usestd3asciirules;
ZEND_END_MODULE_GLOBALS(idn)

#ifdef ZTS
# define IDNG(v) TSRMG(idn_globals_id, zend_idn_globals *, v)
#else
# define IDNG(v) (idn_globals.v)
#endif

#define phpext_idn_ptr idn_module_ptr

#endif	/* PHP_IDN_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
