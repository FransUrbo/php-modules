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

/* Include the LibIDN headers */
#include <stringprep.h>
#include <punycode.h>
#include <idna.h>

/* idn() function rules */
#define IDN_STRINGPREP			0
#define IDN_PUNYCODE_ENCODE		1
#define IDN_PUNYCODE_DECODE		2
#define IDN_IDNA_TO_ASCII		3
#define IDN_IDNA_TO_UNICODE		4

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

	/* Constants to be used when desiding idn() rule */
	REGISTER_MAIN_LONG_CONSTANT("IDN_STRINGPREP",		IDN_STRINGPREP,			CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("IDN_PUNYCODE_ENCODE",	IDN_PUNYCODE_ENCODE,	CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("IDN_PUNYCODE_DECODE",	IDN_PUNYCODE_DECODE,	CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("IDN_IDNA_TO_ASCII",	IDN_IDNA_TO_ASCII,		CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("IDN_IDNA_TO_UNICODE",	IDN_IDNA_TO_UNICODE,	CONST_PERSISTENT | CONST_CS);

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
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.4 2003-10-30 13:54:36 turbo Exp $" );
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
   Get the value of the ascii rules
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
	char *input, *output, *tmpstring;
	int rule, rc;
	unsigned long *q;
	size_t len, len2;

	RETURN_STRING("Function idn() not implemented", 1);

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &input, &rule) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch(rule) {
		/* idn -s */
		case IDN_STRINGPREP:
			output = stringprep_locale_to_utf8(input);
			if(!output) {
				/* Could not convert from locale to UTF-8 */
				RETURN_FALSE;
			}			
			
			/* TODO: Third param! */
			rc = stringprep_profile(output, &tmpstring, "Nameprep", 0);
			if(rc != STRINGPREP_OK) {
				RETURN_LONG((long)rc);
			}

			output = stringprep_utf8_to_locale(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				RETURN_FALSE;
			}
			break;
								
		/* idn -e */
		case IDN_PUNYCODE_ENCODE:
			output = stringprep_locale_to_utf8(input);
			if(!output) {
				/* Could not convert from locale to UTF-8 */
				RETURN_FALSE;
			}			

			q = stringprep_utf8_to_ucs4(output, -1, &len);
			if(!q) {
				/* Could not convert from UTF-8 to UCS-4 */
				RETURN_FALSE;
			}

			len2 = BUFSIZ;
			rc = punycode_encode(len, q, NULL, &len2, input);
			if(rc != PUNYCODE_SUCCESS) {
				/* Could not Puny encode string (?) */
				RETURN_FALSE;
			}
			

			input[len2] = '\0';
			output = stringprep_utf8_to_locale(input);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				RETURN_FALSE;
			}
			break;

		/* idn -d */
		case IDN_PUNYCODE_DECODE:
			len = BUFSIZ;
			q = (unsigned long *) malloc(len * sizeof(q[0]));
			if(!q) {
				/* Could not allocate memory for q */
				RETURN_FALSE;
			}

			rc = punycode_decode(strlen(input), input, &len, q, NULL);
			if(rc != PUNYCODE_SUCCESS) {
				/* Could not Puny decode string (?) */
				RETURN_FALSE;
			}

			q[len] = 0;
			tmpstring = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			if(!tmpstring) {
				/* Could not convert from UCS-4 to UTF-8 */
				RETURN_FALSE;
			}

			output = stringprep_utf8_to_locale(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				RETURN_FALSE;
			}
			break;

		/* idn -a */
		case IDN_IDNA_TO_ASCII:
			tmpstring = stringprep_locale_to_utf8(input);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				RETURN_FALSE;
			}

			q = stringprep_utf8_to_ucs4(tmpstring, -1, NULL);
			if(!q) {
				/* Could not convert from UCS-4 to UTF-8 */
				RETURN_FALSE;
			}

			rc = idna_to_ascii_from_ucs4(q, &tmpstring,
										 IDNG(allowunassigned),
										 IDNG(usestd3asciirules));
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to ASCII */
				RETURN_FALSE;
			}
			break;

		/* idn -u */
		case IDN_IDNA_TO_UNICODE:
			tmpstring = stringprep_locale_to_utf8(input);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				RETURN_FALSE;
			}

			q = stringprep_utf8_to_ucs4(tmpstring, -1, NULL);
			if(!q) {
				/* Could not convert from UCS-4 to UTF-8 */
				RETURN_FALSE;
			}

			rc = idna_to_unicode_ucs4_from_utf8(tmpstring, &q,
												IDNG(allowunassigned),
												IDNG(usestd3asciirules));
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to unicode */
				RETURN_FALSE;
			}

			output = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			if(!output) {
				/* Could not convert from UCS-4 to UTF-8 */
				RETURN_FALSE;
			}
			break;
	}

	RETURN_STRING(output, 1);
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
