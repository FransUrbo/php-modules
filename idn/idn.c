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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_idn.h"

ZEND_DECLARE_MODULE_GLOBALS(idn)

#ifdef COMPILE_DL_IDN
ZEND_GET_MODULE(idn)
#endif

#ifndef FALSE
# define FALSE   0
#endif

#ifndef TRUE
# define TRUE    1
#endif

/* {{{ idn_functions[]
 *
 * Every user visible function must have an entry in idn_functions[].
 */
function_entry idn_functions[] = {
	PHP_FE(idn_allow_unassigned,			NULL)
	PHP_FE(idn_use_std3_ascii_rules,		NULL)

	PHP_FE(idn_get_allow_unassigned,		NULL)
	PHP_FE(idn_get_use_std3_ascii_rules,	NULL)

	PHP_FE(idn,								NULL)

	{NULL, NULL, NULL}	/* Must be the last line in idn_functions[] */
};
/* }}} */

/* {{{ idn_module_entry
 */
zend_module_entry idn_module_entry = {
	STANDARD_MODULE_HEADER,
	"idn",
	idn_functions,
	PHP_MINIT(idn),
	PHP_MSHUTDOWN(idn),
	NULL,
	NULL,
	PHP_MINFO(idn),
    NO_VERSION_YET, /* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("idn.allowunassigned",   "0", PHP_INI_ALL, OnUpdateInt, allowunassigned,   zend_idn_globals, idn_globals)
  STD_PHP_INI_ENTRY("idn.usestd3asciirules", "0", PHP_INI_ALL, OnUpdateInt, usestd3asciirules, zend_idn_globals, idn_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_idn_init_globals
 */
static void php_idn_init_globals(zend_idn_globals *idn_globals)
{
    idn_globals->allowunassigned = 0;
	idn_globals->usestd3asciirules = 0;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(idn)
{
	ZEND_INIT_MODULE_GLOBALS(idn, php_idn_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(idn)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(idn)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "IDN support", "enabled");
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.3 2003-10-30 12:39:23 turbo Exp $" );
	php_info_print_table_end();
}
/* }}} */

/* --------------------- */

/* {{{ proto bool idn_allow_unassigned(mixed c)
   Toggle the Allow Unassigned value
 */
PHP_FUNCTION(idn_allow_unassigned)
{
	int c;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			/* Toggle the current value */
			if(IDNG(allowunassigned))
				IDNG(allowunassigned) = FALSE;
			else
				IDNG(allowunassigned) = TRUE;

			break;
		case 1:
			if(zend_get_parameters_ex(1, &c) == FAILURE) {
				RETURN_FALSE;
			}

			IDNG(allowunassigned) = c;
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool idn_use_std3_ascii_rules(mixed c)
   Toggle the usage of the STD3 ASCII rules
 */
PHP_FUNCTION(idn_use_std3_ascii_rules)
{
	int c;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			/* Toggle the current value */
			if(IDNG(usestd3asciirules))
				IDNG(usestd3asciirules) = FALSE;
			else
				IDNG(usestd3asciirules) = TRUE;

			break;
		case 1:
			if(zend_get_parameters_ex(1, &c) == FAILURE) {
				RETURN_FALSE;
			}

			IDNG(usestd3asciirules) = c;
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool idn_get_allow_unassigned(void)
   Get the value of unassigned
 */
PHP_FUNCTION(idn_get_allow_unassigned)
{
	RETURN_BOOL(IDNG(allowunassigned));
}
/* }}} */

/* {{{ proto bool idn_get_use_std3_ascii_rules(void)
   Get the value of unassigned
 */
PHP_FUNCTION(idn_get_use_std3_ascii_rules)
{
	RETURN_BOOL(IDNG(usestd3asciirules));
}
/* }}} */



/* {{{ proto string idn(string input, int rule)
   Convert the input according to rule
 */
PHP_FUNCTION(idn)
{
	RETURN_STRING("Function idn() not implemented", 1);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
