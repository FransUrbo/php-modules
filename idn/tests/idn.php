<?php
/* $Id: idn.php,v 1.11 2008-03-18 12:14:07 turbo Exp $ */

if($_REQUEST["show"] == 'source') {
	show_source(__FILE__);
	die();
} elseif($_REQUEST["show"] == 'versions') {
   ob_start();
   phpinfo(INFO_MODULES);
   $php_info .= ob_get_contents();
   ob_end_clean();

   $php_info = ereg_replace('.*module_idn">', '<h2 align="center"><a name="MODULE_IDN">', $php_info);
   $php_info = ereg_replace('<h2 align="center"><a name="module_.*', '', $php_info);

   echo($php_info);
   die();
} elseif($_REQUEST["show"] == 'functions') {
	$funcs = get_extension_funcs("idn");
	foreach($funcs as $func)
	  echo "$func<br>";

	die();
}

// Set the locale to UTF-8
if(!$_REQUEST["charset"]) {
  $_REQUEST["charset"] = 'ISO-8859-1';
}
?>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=<?=$_REQUEST["charset"]?>">
  </head>

  <body>
<?php
function pql_execute($command, $hide=true) {
    $command = "(" . escapeshellcmd($command) . ") 2>&1";

    if(!$hide)
      echo "\n<pre>\n----\n";

    flush(passthru($command, $ret));

    if($ret)
      $code = '=> <b><u>FAILED!!</u></b>';
    else
      $code = '=> <b>SUCCESS</b>';

    if(!$hide)
      echo "----\n</pre>";

    return $ret;
}

if(function_exists("idn_to_utf8") && function_exists("idn_to_ascii")) {
  if($_REQUEST["domain"]) {
    // Convert the value
    if($_REQUEST["form"]) {
      if($_REQUEST["rule"] == 'name')
	$_REQUEST["domain_out"] = idn_prep_name($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'krb')
	$_REQUEST["domain_out"] = idn_prep_kerberos5($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'node')
	$_REQUEST["domain_out"] = idn_prep_node($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'resource')
	$_REQUEST["domain_out"] = idn_prep_resource($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'plain')
	$_REQUEST["domain_out"] = idn_prep_plain($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'trace')
	$_REQUEST["domain_out"] = idn_prep_trace($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'sasl')
	$_REQUEST["domain_out"] = idn_prep_sasl($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'iscsi')
	$_REQUEST["domain_out"] = idn_prep_iscsi($_REQUEST["domain"], $_REQUEST["charset"]);
    } else {
      if($_REQUEST["rule"] == '2ascii')
	$_REQUEST["domain_out"] = idn_to_ascii($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == '2uni')
	$_REQUEST["domain_out"] = idn_to_unicode($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == '2utf8')
	$_REQUEST["domain_out"] = idn_to_utf8($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'punyencode')
	$_REQUEST["domain_out"] = idn_punycode_encode($_REQUEST["domain"], $_REQUEST["charset"]);
      elseif($_REQUEST["rule"] == 'punydecode')
	$_REQUEST["domain_out"] = idn_punycode_decode($_REQUEST["domain"], $_REQUEST["charset"]);
      else
	die("Non supported conversion '".$_REQUEST["rule"]."'");
    }
    
    // Output the result (could contain the error message)
    echo $_REQUEST["domain"].": '".$_REQUEST["domain_out"]."'<br>";
    $_REQUEST["domain"] = $_REQUEST["domain_out"];
  }
?>
    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$_REQUEST["domain"]?>" size="50">
      <select name="rule">
        <option value="2ascii">UNICODE 2 ASCII</option>
        <option value="2uni">ASCII 2 UNICODE</option>
        <option value="2utf8">ASCII 2 UTF-8</option>
	<option value="punyencode">PUNYCODE ENCODE</option>
	<option value="punydecode">PUNYCODE DECODE</option>
      </select>
<?php include("charsets.php"); ?>
      <input type="submit" value="Convert">
    </form>

    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$_REQUEST["domain"]?>" size="50">
      <input type="hidden" name="form" value="stringprep">
      <select name="rule">
	<option value="name">Nameprep</option>
	<option value="krb">KRBprep</option>
	<option value="node">Nodeprep</option>
	<option value="resource">Resourceprep</option>
	<option value="plain">Plain</option>
	<option value="trace">Trace</option>
	<option value="sasl">SASLprep</option>
	<option value="iscsi">ISCSIprep</option>
      </select>
<?php include("charsets.php"); ?>
      <input type="submit" value="Convert">
    </form>
<?php
} else {
    die("Module IDN isn't loaded (can't find function idn_to_utf8 and/or idn_to_ascii)");
}
?>
  <p>
  <center>
    <a href="idn.php?show=source">show code</a> - 
    <a href="idn.php?show=versions">show versions</a> - 
    <a href="idn.php?show=functions">show functions</a>
  </center>
  </body>
</html>

<?php
  /*
   * Local variables:
   * tab-width: 4
   * mode: php
   * End:
   */
?>
