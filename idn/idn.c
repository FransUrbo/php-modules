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
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_idn.h"

ZEND_DECLARE_MODULE_GLOBALS(idn)

/* {{{ idn_functions[]
 *
 * Every user visible function must have an entry in idn_functions[].
 */
function_entry idn_functions[] = {
	PHP_FE(confirm_idn_compiled,				NULL)		/* For testing, remove later. */

	PHP_FE(idna_to_ascii,						NULL)
	PHP_FE(idna_to_ascii_from_locale,			NULL)
	PHP_FE(idna_to_ascii_from_ucs4,				NULL)
	PHP_FE(idna_to_ascii_from_utf8,				NULL)
	
	PHP_FE(idna_to_unicode,						NULL)
	PHP_FE(idna_to_unicode_locale_from_locale,	NULL)
	PHP_FE(idna_to_unicode_locale_from_utf8,	NULL)
	PHP_FE(idna_to_unicode_ucs4_from_ucs4,		NULL)
	PHP_FE(idna_to_unicode_ucs4_from_utf8,		NULL)
	PHP_FE(idna_to_unicode_utf8_from_utf8,		NULL)

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

#ifdef COMPILE_DL_IDN
ZEND_GET_MODULE(idn)
#endif

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
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.0 2003-10-29 14:18:24 turbo Exp $" );
	php_info_print_table_end();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* {{{ proto string confirm_idn_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_idn_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char string[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = sprintf(string, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "idn", arg);
	RETURN_STRINGL(string, len, 1);
}
/* }}} */


/* {{{ idna_to_ascii(input)
 */
PHP_FUNCTION(idna_to_ascii)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_ascii_from_ucs4(input)
 */
PHP_FUNCTION(idna_to_ascii_from_ucs4)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_ascii_from_utf8(input)
 */
PHP_FUNCTION(idna_to_ascii_from_utf8)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_ascii_from_locale(input)
 */
PHP_FUNCTION(idna_to_ascii_from_locale)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* --------------------- */

/* {{{ idna_to_unicode(input)
 */
PHP_FUNCTION(idna_to_unicode)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_unicode_ucs4_from_ucs4(input)
 */
PHP_FUNCTION(idna_to_unicode_ucs4_from_ucs4)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_unicode_ucs4_from_utf8(input)
 */
PHP_FUNCTION(idna_to_unicode_ucs4_from_utf8)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_unicode_utf8_from_utf8(input)
 */
PHP_FUNCTION(idna_to_unicode_utf8_from_utf8)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_unicode_locale_from_utf8(input)
 */
PHP_FUNCTION(idna_to_unicode_locale_from_utf8)
{
	char output[256];
	int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
}
/* }}} */

/* {{{ idna_to_unicode_locale_from_locale(input)
 * 
PHP_FUNCTION(idna_to_unicode_locale_from_locale)
{
char output[256];
int len;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	len = strlen(output);
	RETURN_STRINGL(output, len, 1);
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
