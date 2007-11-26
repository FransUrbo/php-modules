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

/* $Id: idn.c,v 0.35 2007-11-26 21:20:24 turbo Exp $ */

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
#ifdef HAVE_IDN_TLD
#include <tld.h>
#endif
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
	PHP_FE(idn_to_unicode,					NULL)

#ifdef HAVE_IDN_TLD
	PHP_FE(tld_check,						NULL)
	PHP_FE(tld_get,							NULL)
	PHP_FE(tld_get_table,					NULL)
#endif

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
#ifndef ZEND_ENGINE_2
  /* PHP4 */
  STD_PHP_INI_ENTRY("idn.allow_unassigned_chars",	"0",			PHP_INI_ALL, OnUpdateInt,		allow_unassigned_chars, zend_idn_globals,	idn_globals)
  STD_PHP_INI_ENTRY("idn.use_std_3_ascii_rules",	"0",			PHP_INI_ALL, OnUpdateInt,		use_std_3_ascii_rules,  zend_idn_globals,	idn_globals)
#else
  /* PHP5 */
  STD_PHP_INI_ENTRY("idn.allow_unassigned_chars",	"0",			PHP_INI_ALL, OnUpdateLong,		allow_unassigned_chars,	zend_idn_globals,	idn_globals)
  STD_PHP_INI_ENTRY("idn.use_std_3_ascii_rules",	"0",			PHP_INI_ALL, OnUpdateLong,		use_std_3_ascii_rules,	zend_idn_globals,	idn_globals)
#endif

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
	php_info_print_table_row(2, "RCS Version", "$Id: idn.c,v 0.35 2007-11-26 21:20:24 turbo Exp $" );
	php_info_print_table_row(2, "LibIDN version", STRINGPREP_VERSION);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* --------------------- */
/*   Internal functions  */
/* --------------------- */

/* {{{ string idn_prep(string input, string prep)
*/
static char *idn_prep(char *input, int prep, char *charset)
{
	char *output, *tmpstring, *tmpstring2;
	int rc;

	tmpstring = stringprep_convert(input, "UTF-8", charset);
	if(!tmpstring) {
		/* Could not convert from locale to UTF-8 */
		php_error(E_NOTICE, "IDN_STRINGPREP: Could not convert from locale (%s) to UTF-8", charset);
		return(NULL);
	}

	if(prep == IDN_PROFILE_PREP_NAME)
		rc = stringprep_profile(tmpstring, &tmpstring2, "Nameprep", 0);
	else if(prep == IDN_PROFILE_PREP_KRB)
		rc = stringprep_profile(tmpstring, &tmpstring2, "KRBprep", 0);
	else if(prep == IDN_PROFILE_PREP_NODE)
		rc = stringprep_profile(tmpstring, &tmpstring2, "Nodeprep", 0);
	else if(prep == IDN_PROFILE_PREP_RESOURCE)
		rc = stringprep_profile(tmpstring, &tmpstring2, "Resourceprep", 0);
	else if(prep == IDN_PROFILE_PREP_PLAIN)
		rc = stringprep_profile(tmpstring, &tmpstring2, "plain", 0);
	else if(prep == IDN_PROFILE_PREP_TRACE)
		rc = stringprep_profile(tmpstring, &tmpstring2, "generic", 0);
	else if(prep == IDN_PROFILE_PREP_SASL)
		rc = stringprep_profile(tmpstring, &tmpstring2, "SASLprep", 0);
	else if(prep == IDN_PROFILE_PREP_ISCSI)
		rc = stringprep_profile(tmpstring, &tmpstring2, "ISCSIprep", 0);
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
	
	output = stringprep_convert(tmpstring2, charset, "UTF-8");
	free(tmpstring2);
	if(!output) {
		/* Could not convert from UTF-8 to locale */
		php_error(E_NOTICE, "IDN_STRINGPREP: Could not convert from UTF-8 to %s", charset);
		return(NULL);
	}

	tmpstring = estrdup(output);
	free(output);
	return(tmpstring);
}
/* }}} */

/* {{{ string idn(string input, int rule)
   Convert the input according to rule
 */
#define __BUFSIZ_ 8192
static char *idn(char *input, int rule, char *charset)
{
	char *output, *tmpstring;
	int rc;
	uint32_t *q;
	size_t len, len2;

	TSRMLS_FETCH();

	switch(rule) {
		/* {{{ idn -e : punycode_encode() */
		case IDN_PUNYCODE_ENCODE:
			tmpstring = stringprep_convert(input, "UTF-8", charset);
			if (!tmpstring) {
                /* Could not convert from locale to UTF-8 */
                php_error(E_NOTICE, "IDN_PUNYCODE_ENCODE: Could not convert from %s to UTF-8", charset);
				return(NULL);
            }

			q = stringprep_utf8_to_ucs4(tmpstring, -1, &len);
			free(tmpstring);
			if(!q) {
				/* Could not convert from UTF-8 to UCS-4 */
				php_error(E_NOTICE, "IDN_PUNYCODE_ENCODE: Could not convert from UTF-8 to UCS-4");
				return(NULL);
			}

			output = malloc(__BUFSIZ_ * sizeof(char));
			if (!output)
			{
				free(q);
				/* Could not allocate enough memory for buffer */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not allocate memory");
				return(NULL);
			}

			len2 = __BUFSIZ_-1;
			rc = punycode_encode(len, q, NULL, &len2, output);
			free(q);

			if(rc != PUNYCODE_SUCCESS) {
				free(output);
				/* Could not Puny encode string (?) */
				php_error(E_ERROR, "IDN_PUNYCODE_ENCODE: Could not Puny encode string");
				return(NULL);
			}

			output[len2] = '\0';
			break;
			/* }}} */

		/* {{{ idn -d : punycode_decode() */
		case IDN_PUNYCODE_DECODE:
			len = __BUFSIZ_-1;
			q = (uint32_t *) emalloc(__BUFSIZ_ * sizeof(uint32_t));
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
				php_error(E_NOTICE, "IDN_PUNYCODE_DECODE: Could not convert from UCS-4 to UTF-8");
				return(NULL);
			}

			output = stringprep_convert(tmpstring, charset, "UTF-8");
			free(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_NOTICE, "IDN_PUNYCODE_DECODE: Could not convert from UTF-8 to %s", charset);
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -a : to_ascii() */
		case IDN_IDNA_TO_ASCII:
			tmpstring = stringprep_convert(input, "UTF-8", charset);
			if(!tmpstring) {
				/* Could not convert from locale to UTF-8 */
				php_error(E_NOTICE, "IDN_IDNA_TO_ASCII: Could not convert from %s to UTF-8", charset);
				return(NULL);
			}
			rc = idna_to_ascii_8z(tmpstring, &output,
								  (IDNG(allow_unassigned_chars) ? IDNA_ALLOW_UNASSIGNED : 0) |
								  (IDNG(use_std_3_ascii_rules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			free(tmpstring);
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to ASCII */
				php_error(E_NOTICE, "IDN_IDNA_TO_ASCII: Could not convert from IDNA to ASCII");
				return(NULL);
			}
			break;
			/* }}} */

		/* {{{ idn -u : to_unicode() */
		case IDN_IDNA_TO_UNICODE:
			rc = idna_to_unicode_8z8z(input, &tmpstring,
									  (IDNG(allow_unassigned_chars) ? IDNA_ALLOW_UNASSIGNED : 0) |
									  (IDNG(use_std_3_ascii_rules) ? IDNA_USE_STD3_ASCII_RULES : 0));
			if(rc != IDNA_SUCCESS) {
				/* Could not convert from IDNA to unicode */
				php_error(E_NOTICE, "IDN_IDNA_TO_UNICODE: Could not convert from IDNA to UTF-8");
				return(NULL);
			}

			output = stringprep_convert(tmpstring, charset, "UTF-8");
			free(tmpstring);
			if(!output) {
				/* Could not convert from UTF-8 to locale */
				php_error(E_NOTICE, "IDN_IDNA_TO_UNICODE: Could not convert from UTF-8 to %s", charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_NAME, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_KRB, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_NODE, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_RESOURCE, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_PLAIN, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_TRACE, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_SASL, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn_prep((*yyinput)->value.str.val, IDN_PROFILE_PREP_ISCSI, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn((*yyinput)->value.str.val, IDN_PUNYCODE_ENCODE, charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset=(*yycharset)->value.str.val;
	}

	output = idn((*yyinput)->value.str.val, IDN_PUNYCODE_DECODE,charset);
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
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset = (*yycharset)->value.str.val;
	}

	output = idn((*yyinput)->value.str.val, IDN_IDNA_TO_ASCII, charset);
	if( output == NULL ) {
		RETURN_NULL();
	} else {
		RETVAL_STRINGL(output, strlen(output), 1);
	}
	efree(output);
}
/* }}} */

/* {{{ proto string idn_to_utf8(string input [, string charset])
   Convert from ACE according to IDNA
 */
PHP_FUNCTION(idn_to_utf8)
{
	char *output;
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 1) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset=(*yycharset)->value.str.val;
	}

	output = idn((*yyinput)->value.str.val, IDN_IDNA_TO_UNICODE, "UTF-8");
	if( output == NULL ) {
		RETURN_NULL();
	} else {
		RETVAL_STRINGL(output, strlen(output), 1);
	}
	efree(output);
}
/* }}} */

/* {{{ proto string idn_to_unicode(string input [, string charset])
   Convert from ACE according to IDNA
 */
PHP_FUNCTION(idn_to_unicode)
{
	char *output;
	char *charset = IDNG(default_charset);
	pval **yyinput, **yycharset;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 0) || (argv > 2) || (zend_get_parameters_ex(argv, &yyinput, &yycharset) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(yyinput);

	if(argv == 2) {
		convert_to_string_ex(yycharset);
		charset=(*yycharset)->value.str.val;
	}

	output = idn((*yyinput)->value.str.val, IDN_IDNA_TO_UNICODE, charset);
	if( output == NULL ) {
		RETURN_NULL();
	} else {
		RETVAL_STRINGL(output, strlen(output), 1);
	}
	efree(output);
}
/* }}} */

#ifdef HAVE_IDN_TLD

/* {{{ proto boolean/null tld_check(string input , ref integer errpos[, string charset[, string tld])
   Test input for valid extra characters defined the relevant TLD,
   returning the position of the offending character in errpos and FALSE
   as a result if nonconformance is detected. Returns NULL on other error
   conditions and TRUE on success, in both cases errpos is not touched.
   Charset maybe be specified as in the other functions of this api, the tld parameter
   allows one to overwrite automatic tld detection in the input string.
 */
PHP_FUNCTION(tld_check)
{
	char *tmpstring;
	char *tmpstring2;
	const Tld_table * tld = NULL;
	uint32_t *q;
	int rc;
	size_t len, errpos = -1;
	char *charset = IDNG(default_charset);
	zval **yyinput, **yyerrpos, **yycharset, **yytld;
	int argv = ZEND_NUM_ARGS();

    if ((argv < 2) || (argv > 4) || (zend_get_parameters_ex(argv, &yyinput,
			&yyerrpos, &yycharset, &yytld) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(yyinput);

	if (!PZVAL_IS_REF(*yyerrpos)) {
		zend_error(E_ERROR, "IDN_TLD_CHECK: Parameter wasn't passed by reference");
		RETURN_NULL();
	}
		
	
	if(argv >= 3) {
		convert_to_string_ex(yycharset);
		charset=(*yycharset)->value.str.val;
	}

	if(argv == 4) {
		convert_to_string_ex(yytld);
		tld = tld_default_table((*yytld)->value.str.val, NULL);
	}

    tmpstring = stringprep_convert((*yyinput)->value.str.val, "UTF-8", charset);
	if(!tmpstring) {
		zend_error(E_WARNING, "IDN_TLD_CHECK: Could not convert from locale (%s) to UTF-8", charset);
		RETURN_NULL();
	}

	rc = stringprep_profile(tmpstring, &tmpstring2, "Nameprep", 0);
	free(tmpstring);
	if (rc != STRINGPREP_OK) {
		zend_error(E_ERROR, "IDN_TLD_CHECK: Could not setup nameprep profile: %d", rc);
		RETURN_NULL();
	}

	if (argv == 4) {
		if (tld) {
			q = stringprep_utf8_to_ucs4(tmpstring2, -1, &len);
			free(tmpstring2);
			if (!q) {
				zend_error(E_WARNING, "IDN_TLD_CHECK: Could not convert from UTF-8 to UCS-4");
				RETURN_NULL();
			}
			rc = tld_check_4t(q, len, &errpos, tld);
			free(q);
		}
		else {
			free(tmpstring2);
			rc = TLD_SUCCESS;
		}
	}
	else {
		rc = tld_check_8z(tmpstring2, &errpos, NULL);
		free(tmpstring2);
	}
	
	if (rc == TLD_SUCCESS) {
		RETURN_TRUE
	} else if (rc == TLD_INVALID) {
		ZVAL_LONG(*yyerrpos, errpos);
		RETURN_FALSE;
	}
	else
		RETURN_NULL();
}
/* }}} */

/* {{{ proto string/NULL tld_get(string input)
   Return TLD of hostname input, or NULL
   if none could be found
 */
PHP_FUNCTION(tld_get)
{
	char *tmpstring = NULL;
	int rc;
	zval **yyinput;
	int argv = ZEND_NUM_ARGS();

    if ((argv != 1) || (zend_get_parameters_ex(argv, &yyinput) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(yyinput);

    rc = tld_get_z((*yyinput)->value.str.val, &tmpstring);
	if(rc == TLD_SUCCESS && tmpstring) {
		RETVAL_STRING(tmpstring, 1);
		free(tmpstring);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto TABLE tld_get_table(string tld)
   Retrieves an array detailing restriction information on TLD tld or
   NULL if no such info could be found 
*/
PHP_FUNCTION(tld_get_table)
{
	const Tld_table * tld_table = NULL;
	int ok = 0;
	int ok_loop = 0;
	zval **yyinput;
	zval *intervals;
	zval *interval;
	const Tld_table_element *e;
	size_t pos;
	int argv = ZEND_NUM_ARGS();

    if ((argv != 1) || (zend_get_parameters_ex(argv, &yyinput) == FAILURE)) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(yyinput);

	tld_table = tld_default_table((*yyinput)->value.str.val, NULL);
	if (tld_table) {
		if(array_init(return_value) == SUCCESS)
		if(add_assoc_string(return_value, "name", (char *) tld_table->name, 1) == SUCCESS)
		if(add_assoc_string(return_value, "version", (char *) tld_table->version, 1) == SUCCESS)
		if(add_assoc_long(return_value, "nvalid", tld_table->nvalid) == SUCCESS) {
			MAKE_STD_ZVAL(intervals);
			if (array_init(intervals) == SUCCESS) {
				for (pos=0, e = tld_table->valid; pos<tld_table->nvalid; pos++,e++)
				{
					ok_loop=0;
					MAKE_STD_ZVAL(interval);
					if (array_init(interval) == SUCCESS)
					if (add_assoc_long(interval, "start", e->start) == SUCCESS)
					if (add_assoc_long(interval, "end", e->end) == SUCCESS)
					if (add_next_index_zval(intervals, interval) == SUCCESS)
						ok_loop = 1;
					if (!ok_loop)
						break;
				}
				if (ok_loop)
				if (add_assoc_zval(return_value, "valid", intervals) == SUCCESS)
					ok=1;
			}
		}
		if (ok) /* phew */
			return;
		zend_error(E_WARNING, "IDN_TLD_GET_TABLE: Couldn't create result array, maybe out of memory?");
	}
	RETURN_NULL();

}
/* }}} */

#endif /* #ifdef HAVE_TLD */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
