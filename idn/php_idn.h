/*
   +----------------------------------------------------------------------+
   | LibIDN API for PHP. Copyright (c) 2003 Turbo Fredriksson             |
   +----------------------------------------------------------------------+
   |                                                                      |
   | PHP-IDN is free software; you can redistribute it and/or modify it   |
   | under the terms of the GNU General Public License as published by    |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | PHP-IDN is distributed in the hope that it will be useful, but       |
   | WITHOUT ANY WARRANTY; without even the implied warranty of           |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with PHP-IDN; if not, write to the Free Software Foundation,   |
   | Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA        |
   |                                                                      |
   +----------------------------------------------------------------------+
   | Authors: Turbo Fredriksson <turbo@bayour.com>                        |
   |          Simon Josefsson <simon@josefsson.org>                       |
   |          I stole most of idn() from his idn.c                        |
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

/* IDNA functions */
PHP_FUNCTION(idn_prep_name);
PHP_FUNCTION(idn_prep_kerberos5);
PHP_FUNCTION(idn_prep_node);
PHP_FUNCTION(idn_prep_resource);
PHP_FUNCTION(idn_prep_plain);
PHP_FUNCTION(idn_prep_trace);
PHP_FUNCTION(idn_prep_sasl);
PHP_FUNCTION(idn_prep_iscsi);

PHP_FUNCTION(idn_punycode_encode);
PHP_FUNCTION(idn_punycode_decode);
PHP_FUNCTION(idn_to_ascii);
PHP_FUNCTION(idn_to_utf8);

ZEND_BEGIN_MODULE_GLOBALS(idn)
	 int allow_unassigned_chars;	/* 0 */
	 int use_std_3_ascii_rules;		/* 0 */
	 char *default_charset;		/* ISO-8859-1 */
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
