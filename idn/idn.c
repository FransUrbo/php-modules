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

/* $Id: idn.c,v 0.23 2004-01-30 07:00:30 turbo Exp $ */

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
 * Every user visible function must have an entry in idn_functions[].
 */
function_entry idn_functions[] = {
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
	PHP_FE(idn_to_ascii,					NULL)
	PHP_FE(idn_to_utf8,						NULL)

	{NULL, NULL, NULL}	/* Must be the last line in idn_functions[] */
};

/* }}} */

/* {{{ idn_module_entry
 */
zend_module_entry idn_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"idn",
	idn_functions,
	PHP_MINIT(idn),
	PHP_MSHUTDOWN(idn),
	NULL,
	NULL,
	PHP_MINFO(idn),
#if ZEND_MODULE_API_NO >= 20010901
    NO_VERSION_YET, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ php_idn_init_globals
 */
static void php_idn_init_globals(zend_idn_globals *idn_globals)
{
    idn_globals->allow_unassigned_chars = 0;
	idn_globals->use_std_3_ascii_rules = 0;

	idn_globals->default_charset = estrdup("ISO-8859-1");
}
/* }}} */

/* --------------------- */

/* {{{ PHP_INI_BEGIN
 */
PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("idn.allow_unassigned_chars",	"0",			PHP_INI_ALL, OnUpdateInt,		allow_unassigned_chars,	zend_idn_globals,	idn_globals)
  STD_PHP_INI_ENTRY("idn.use_std_3_ascii_rules",	"0",			PHP_INI_ALL, OnUpdateInt,		use_std_3_ascii_rules,	zend_idn_globals,	idn_globals)
  STD_PHP_INI_ENTRY("idn.default_charset",			"ISO-8859-1",	PHP_INI_ALL, OnUpdateString,	default_charset,		zend_idn_globals,	idn_globals)
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
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.23 2004-01-30 07:00:30 turbo Exp $" );
	php_info_print_table_end();
}
/* }}} */

/* --------------------- */
/*   Internal functions  */
/* --------------------- */

/* {{{ int idn_charset(string charset)
 * TODO: This is a dirty hack. We must be able to reset the choosen value
 *       after we're done with it
 */
static int idn_charset(char *charset)
{
	TSRMLS_FETCH();
	
	if(charset == NULL)
		// Set default charset - ISO-8859-1
		return(setenv("CHARSET", IDNG(default_charset), 1));
	else
		// Set choosen charset
		return(setenv("CHARSET", charset, 1));
}
/* }}} */

/* {{{ string idn_prep(string input, string prep)
*/
static char *idn_prep(char *input, int prep)
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
		free(tmpstring);
		php_error(E_ERROR, "IDN_STRINGPREP: Unsupported prep profile");
		return(NULL);
	}
	free(tmpstring);

	if(rc != STRINGPREP_OK) {
		php_error(E_ERROR, "Could not setup stringprep profile: %d", rc);
		return(NULL);
	}
	
	tmpstring = stringprep_utf8_to_locale(output);
	if(!tmpstring) {
		/* Could not convert from UTF-8 to locale */
		free(output);
		php_error(E_ERROR, "IDN_STRINGPREP: Could not convert from UTF-8 to %s", stringprep_locale_charset());
		return(NULL);
	}

	output = estrdup(tmpstring);
	free(tmpstring);

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

	TSRMLS_FETCH();

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
			free(tmpstring);
			if(!q) {
				/* Could not convert from UTF-8 to UCS-4 */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not convert from UTF-8 to UCS-4");
				return(NULL);
			}

			len2 = BUFSIZ;
			rc = punycode_encode(len, q, NULL, &len2, input);
			free(q);
			if(rc != PUNYCODE_SUCCESS) {
				/* Could not Puny encode string (?) */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not Puny encode string");
				return(NULL);
			}
			
			input[len2] = '\0';
			output = stringprep_utf8_to_locale(input);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not convert from UTF-8 to %s", stringprep_locale_charset());
				return(NULL);
			}
			break;

			/* }}} */

		/* {{{ idn -d : punycode_decode() */
		case IDN_PUNYCODE_DECODE:
			len = BUFSIZ;
			q = (uint32_t *) emalloc(len * sizeof(q[0]));
			if(!q) {
				/* Could not allocate memory for q */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not allocate memory");
				return(NULL);
			}

			rc = punycode_decode(strlen(input), input, &len, q, NULL);
			if(rc != PUNYCODE_SUCCESS) {
				efree(q);
				/* Could not Puny decode string (?) */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not Puny decode string");
				return(NULL);
			}

			q[len] = 0;
			tmpstring = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			efree(q);
			if(!tmpstring) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			output = stringprep_utf8_to_locale(tmpstring);
			free(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_ERROR, "IDN_PUNYCODE_DECODE: Could not convert from UTF-8 to %s", stringprep_locale_charset());
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -a : to_ascii() */
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
								  (IDNG(allow_unassigned_chars) ? IDNA_ALLOW_UNASSIGNED : 0) |
								  (IDNG(use_std_3_ascii_rules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			free(q);
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to ASCII */
				php_error(E_ERROR, "IDN_IDNA_TO_ASCII: Could not convert from IDNA to ASCII");
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -u : to_unicode() */
		case IDN_IDNA_TO_UNICODE:
			tmpstring = stringprep_locale_to_utf8(input);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from %s to UTF-8", stringprep_locale_charset());
				return(NULL);
			}

			rc = idna_to_unicode_8z4z(tmpstring, &q,
									  (IDNG(allow_unassigned_chars) ? IDNA_ALLOW_UNASSIGNED : 0) |
									  (IDNG(use_std_3_ascii_rules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			free(tmpstring);
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to unicode */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from IDNA to unicode");
				return(NULL);
			}

			output = stringprep_ucs4_to_utf8(q, -1, NULL, NULL);
			free(q);
			if(!output) {
				/* Could not convert from UCS-4 to UTF-8 */
				php_error(E_ERROR, "IDN_IDNA_TO_UNICODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}
			break;
			/* }}} */
	}

	tmpstring = estrdup(output);
	free(output);
	return(tmpstring);
}

/* }}} */

/* --------------------- */
/*  Stringprep wrappers  */
/* --------------------- */

/* {{{ proto string idn_prep_name(string input [, string charset])
   Prepare string according to the Nameprep profile
 */
PHP_FUNCTION(idn_prep_name)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_NAME);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_kerberos5(string input [, string charset])
   Prepare string according to the Kerberos V profile
 */
PHP_FUNCTION(idn_prep_kerberos5)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_KRB);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_node(string input [, string charset])
   Prepare string according to the Nodeprep profile
 */
PHP_FUNCTION(idn_prep_node)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_NODE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_resource(string input [, string charset])
   Prepare string according to the Resourceprep profile
 */
PHP_FUNCTION(idn_prep_resource)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_RESOURCE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_plain(string input [, string charset])
   Prepare string according to the plain profile
 */
PHP_FUNCTION(idn_prep_plain)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_PLAIN);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_trace(string input [, string charset])
   Prepare string according to the trace profile
 */
PHP_FUNCTION(idn_prep_trace)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_TRACE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_sasl(string input [, string charset])
   Prepare string according to the SASLprep profile
 */
PHP_FUNCTION(idn_prep_sasl)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_SASL);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_prep_iscsi(string input [, string charset])
   Prepare string according to the iSCSI profile
 */
PHP_FUNCTION(idn_prep_iscsi)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_ISCSI);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* --------------------- */
/*   Punycode wrappers   */
/* --------------------- */

/* {{{ proto string idn_punycode_encode(string input [, string charset])
   Decode a punycode string
 */
PHP_FUNCTION(idn_punycode_encode)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn((*yyinput)->value.str.val, IDN_PUNYCODE_ENCODE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_punycode_decode(string input [, string charset])
   Encode a punycode string
 */
PHP_FUNCTION(idn_punycode_decode)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn((*yyinput)->value.str.val, IDN_PUNYCODE_DECODE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* --------------------- */
/*     IDNA wrappers     */
/* --------------------- */

/* {{{ proto string idn_to_ascii(string input [, string charset])
   Convert to ACE according to IDNA
 */
PHP_FUNCTION(idn_to_ascii)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn((*yyinput)->value.str.val, IDN_IDNA_TO_ASCII);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
}
/* }}} */

/* {{{ proto string idn_to_utf8(string input [, string charset])
   Convert from ACE according to IDNA
 */
PHP_FUNCTION(idn_to_utf8)
{
	char *output;
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		idn_charset((*yycharset)->value.str.val);
	} else
		idn_charset(NULL);

	output = idn((*yyinput)->value.str.val, IDN_IDNA_TO_UNICODE);
	RETVAL_STRINGL(output, strlen(output), 1);
	efree(output);
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
