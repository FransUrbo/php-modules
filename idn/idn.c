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
   | Authors: Turbo Fredriksson <turbo@bayour.com>                        |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id: idn.c,v 0.13 2003-11-08 11:30:58 turbo Exp $ */

/* {{{ PHP defines and includes

 */
#define IS_EXT_MODULE

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

/* }}} */

/* {{{ True/False defines
 */
#ifndef FALSE
# define FALSE   0
#endif

#ifndef TRUE
# define TRUE    1
#endif
/* }}} */

/* {{{ LibIDN includes
 */
#include <stringprep.h>
#include <punycode.h>
#include <idna.h>
/* }}} */

/* {{{ LibIDN defines
 */
#define IDN_PUNYCODE_ENCODE			 0	/* idn_punycode_encode() */
#define IDN_PUNYCODE_DECODE			 1	/* idn_punycode_decode() */

#define IDN_IDNA_TO_ASCII			 2	/* idn_unicode_to_ascii() */
#define IDN_IDNA_TO_UNICODE			 3	/* idn_ascii_to_unicode() */

#define IDN_PROFILE_PREP_NAME		 4	/* idn_prep_name() */
#define IDN_PROFILE_PREP_KRB		 5	/* idn_prep_kerberos5() */
#define IDN_PROFILE_PREP_NODE		 6	/* idn_prep_node() */
#define IDN_PROFILE_PREP_RESOURCE	 7	/* idn_prep_resource() */
#define IDN_PROFILE_PREP_PLAIN		 8	/* idn_prep_plain() */
#define IDN_PROFILE_PREP_TRACE	     9	/* idn_prep_trace() */
#define IDN_PROFILE_PREP_SASL		10	/* idn_prep_sasl() */
#define IDN_PROFILE_PREP_ISCSI		11	/* idn_prep_iscsi() */
/* }}} */

/* --------------------- */
/* Module initialization */
/* --------------------- */

/* {{{ idn_functions[]
 *
 * Every user visible function must have an entry in idn_functions[].
 */
function_entry idn_functions[] = {
	PHP_FE(idn_allow_unassigned,			NULL)
	PHP_FE(idn_use_std3_ascii_rules,		NULL)

	PHP_FE(idn_get_allow_unassigned,		NULL)
	PHP_FE(idn_get_use_std3_ascii_rules,	NULL)

	PHP_FE(idn_prep_name,					NULL)
	PHP_FE(idn_prep_kerberos5,				NULL)
	PHP_FE(idn_prep_node,					NULL)
	PHP_FE(idn_prep_resource,				NULL)
	PHP_FE(idn_prep_plain,					NULL)
	PHP_FE(idn_prep_trace,					NULL)
	PHP_FE(idn_prep_sasl,					NULL)
	PHP_FE(idn_prep_iscsi,					NULL)

	PHP_FE(idn_punycode_encode,				NULL)
	PHP_FE(idn_punycode_decode,				NULL)
	PHP_FE(idn_unicode_to_ascii,			NULL)
	PHP_FE(idn_ascii_to_unicode,			NULL)

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

/* {{{ php_idn_init_globals
 */
static void php_idn_init_globals(zend_idn_globals *idn_globals)
{
    idn_globals->allowunassigned = 0;
	idn_globals->usestd3asciirules = 0;
}
/* }}} */

/* --------------------- */

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("idn.allowunassigned",   "0", PHP_INI_ALL, OnUpdateInt, allowunassigned,   zend_idn_globals, idn_globals)
  STD_PHP_INI_ENTRY("idn.usestd3asciirules", "0", PHP_INI_ALL, OnUpdateInt, usestd3asciirules, zend_idn_globals, idn_globals)
PHP_INI_END()
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
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.13 2003-11-08 11:30:58 turbo Exp $" );
	php_info_print_table_end();
}
/* }}} */

/* --------------------- */
/*   Internal functions  */
/* --------------------- */

/* {{{ string prep(string input, string prep)
*/
static char *prep(char *input, int prep)
{
	char *output, *tmpstring;
	int rc;

	tmpstring = stringprep_locale_to_utf8(input);
	if(!tmpstring) {
		/* Could not convert from locale to UTF-8 */
		php_error(E_ERROR, "IDN_STRINGPREP: Could not convert from locale (%s) to UTF-8", stringprep_locale_charset());
		return(NULL);
	}

	if(prep == IDN_PROFILE_PREP_NAME)
		rc = stringprep_profile(tmpstring, &output, "Nameprep", 0);
	else if(prep == IDN_PROFILE_PREP_KRB)
		rc = stringprep_profile(tmpstring, &output, "KRBprep", 0);
	else if(prep == IDN_PROFILE_PREP_NODE)
		rc = stringprep_profile(tmpstring, &output, "Nodeprep", 0);
	else if(prep == IDN_PROFILE_PREP_RESOURCE)
		rc = stringprep_profile(tmpstring, &output, "Resourceprep", 0);
	else if(prep == IDN_PROFILE_PREP_PLAIN)
		rc = stringprep_profile(tmpstring, &output, "plain", 0);
	else if(prep == IDN_PROFILE_PREP_TRACE)
		rc = stringprep_profile(tmpstring, &output, "generic", 0);
	else if(prep == IDN_PROFILE_PREP_SASL)
		rc = stringprep_profile(tmpstring, &output, "SASLprep", 0);
	else if(prep == IDN_PROFILE_PREP_ISCSI)
		rc = stringprep_profile(tmpstring, &output, "ISCSIprep", 0);
	else {
		php_error(E_ERROR, "IDN_STRINGPREP: Unsupported prep profile");
		return(NULL);
	}

	if(rc != STRINGPREP_OK) {
		free(tmpstring);
		php_error(E_ERROR, "Could not setup stringprep profile: %d", rc);
		return(NULL);
	}
	
	output = stringprep_utf8_to_locale(output);
	free(tmpstring);
	if(!output) {
		/* Could not convert from UTF-8 to locale */
		php_error(E_ERROR, "IDN_STRINGPREP: Could not convert from UTF-8 to locale (%s)", stringprep_locale_charset());
		return(NULL);
	}

	return(output);
}
/* }}} */

/* {{{ string idn(string input, int rule)
   Convert the input according to rule
 */
static char *idn(char *input, int rule)
{
	char *output, *tmpstring;
	int rc;
	uint32_t *q;
	size_t len, len2;

	switch(rule) {
		/* {{{ idn -e : punycode_encode() */

		case IDN_PUNYCODE_ENCODE:
			tmpstring = stringprep_locale_to_utf8(input);
			if (!tmpstring) {
                /* Could not convert from locale to UTF-8 */
                php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not convert from locale (%s) to UTF-8", stringprep_locale_charset());
				return(NULL);
            }

			q = stringprep_utf8_to_ucs4(tmpstring, -1, &len);
			if(!q) {
				/* Could not convert from UTF-8 to UCS-4 */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not convert from UTF-8 to UCS-4");
				return(NULL);
			}

			len2 = BUFSIZ;
			rc = punycode_encode(len, q, NULL, &len2, input);
			if(rc != PUNYCODE_SUCCESS) {
				/* Could not Puny encode string (?) */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not Puny encode string");
				return(NULL);
			}
			
			input[len2] = '\0';
			output = stringprep_utf8_to_locale(input);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not convert from UTF-8 to locale (%s)", stringprep_locale_charset());
				return(NULL);
			}
			break;

			/* }}} */

		/* {{{ idn -d : punycode_decode() */
		case IDN_PUNYCODE_DECODE:
			len = BUFSIZ;
			q = (uint32_t *) malloc(len * sizeof(q[0]));
			if(!q) {
				/* Could not allocate memory for q */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not allocate memory");
				return(NULL);
			}

			rc = punycode_decode(strlen(input), input, &len, q, NULL);
			if(rc != PUNYCODE_SUCCESS) {
				/* Could not Puny decode string (?) */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not Puny decode string");
				return(NULL);
			}

			q[len] = 0;
			tmpstring = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			if(!tmpstring) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			output = stringprep_utf8_to_locale(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not convert from UTF-8 to locale (%s)", stringprep_locale_charset());
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -a : unicode_to_ascii() */
		case IDN_IDNA_TO_ASCII:
			tmpstring = stringprep_locale_to_utf8(input);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_ASCII: Could not convert from locale (%s) to UTF-8", stringprep_locale_charset());
				return(NULL);
			}

			q = stringprep_utf8_to_ucs4(tmpstring, -1, NULL);
			free(tmpstring);
			if(!q) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_ASCII: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			rc = idna_to_ascii_4z(q, &output,
								  (IDNG(allowunassigned) ? IDNA_ALLOW_UNASSIGNED : 0) |
								  (IDNG(usestd3asciirules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			free(q);
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to ASCII */
				php_error(E_ERROR, "IDN_IDNA_TO_ASCII: Could not convert from IDNA to ASCII");
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -u : ascii_to_unicode() */
		case IDN_IDNA_TO_UNICODE:
			tmpstring = stringprep_locale_to_utf8(input);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from %s to UTF-8", stringprep_locale_charset());
				return(NULL);
			}

			q = stringprep_utf8_to_ucs4(tmpstring, -1, NULL);
			free(tmpstring);
			if(!q) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			rc = idna_to_unicode_8z4z(tmpstring, &q,
									  (IDNG(allowunassigned) ? IDNA_ALLOW_UNASSIGNED : 0) |
									  (IDNG(usestd3asciirules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to unicode */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from IDNA to unicode");
				return(NULL);
			}

			tmpstring = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			free(q);
			if(!tmpstring) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			output = stringprep_utf8_to_locale(tmpstring);
			free(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from UTF-8 to locale (%s)", stringprep_locale_charset());
				return(NULL);
			}
			break;
			/* }}} */
	}

	return(output);
}

/* }}} */

/* --------------------- */
/*   Support functions   */
/* --------------------- */

/* {{{ proto bool idn_allow_unassigned(mixed c)
   Toggle the Allow Unassigned value
 */
PHP_FUNCTION(idn_allow_unassigned)
{
	pval **c;

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
			convert_to_long_ex(c);

			IDNG(allowunassigned) = (*c)->value.lval;
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
	pval **c;

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
			convert_to_long_ex(c);
			
			IDNG(usestd3asciirules) = (*c)->value.lval;
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

/* --------------------- */
/*  Stringprep wrappers  */
/* --------------------- */

/* {{{ proto string idn_prep_name(string input)
 */
PHP_FUNCTION(idn_prep_name)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_NAME);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_kerberos5(string input)
 */
PHP_FUNCTION(idn_prep_kerberos5)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_KRB);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_node(string input)
 */
PHP_FUNCTION(idn_prep_node)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_NODE);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_resource(string input)
 */
PHP_FUNCTION(idn_prep_resource)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_RESOURCE);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_plain(string input)
 */
PHP_FUNCTION(idn_prep_plain)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_PLAIN);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_trace(string input)
 */
PHP_FUNCTION(idn_prep_trace)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_TRACE);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_sasl(string input)
 */
PHP_FUNCTION(idn_prep_sasl)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_SASL);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_prep_iscsi(string input)
 */
PHP_FUNCTION(idn_prep_iscsi)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = prep(input, IDN_PROFILE_PREP_ISCSI);
	RETURN_STRING(output, 1);
}
/* }}} */

/* --------------------- */
/*   Punycode wrappers   */
/* --------------------- */

/* {{{ proto string idn_punycode_encode(string input)
 */
PHP_FUNCTION(idn_punycode_encode)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = idn(input, IDN_PUNYCODE_ENCODE);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_punycode_decode(string input)
 */
PHP_FUNCTION(idn_punycode_decode)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = idn(input, IDN_PUNYCODE_DECODE);
	RETURN_STRING(output, 1);
}
/* }}} */

/* --------------------- */
/*     IDNA wrappers     */
/* --------------------- */

/* {{{ proto string idn_unicode_to_ascii(string input)
 */
PHP_FUNCTION(idn_unicode_to_ascii)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = idn(input, IDN_IDNA_TO_ASCII);
	RETURN_STRING(output, 1);
}
/* }}} */

/* {{{ proto string idn_ascii_to_unicode(string input)

 */
PHP_FUNCTION(idn_ascii_to_unicode)
{
	char *input, *output;
	pval **yyinput;

    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &yyinput) == FAILURE) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);
	input = (*yyinput)->value.str.val;

	output = idn(input, IDN_IDNA_TO_UNICODE);
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
