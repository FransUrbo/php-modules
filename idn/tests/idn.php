<?php
/* $Id: idn.php,v 1.10 2004-05-26 08:47:38 turbo Exp $ */

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
if(!$charset) {
  $charset = 'ISO-8859-1';
}
?>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=<?=$charset?>">
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
  if($domain) {
    // Convert the value
    if($form) {
      if($rule == 'name')
	$domain_out = idn_prep_name($domain, $charset);
      elseif($rule == 'krb')
	$domain_out = idn_prep_kerberos5($domain, $charset);
      elseif($rule == 'node')
	$domain_out = idn_prep_node($domain, $charset);
      elseif($rule == 'resource')
	$domain_out = idn_prep_resource($domain, $charset);
      elseif($rule == 'plain')
	$domain_out = idn_prep_plain($domain, $charset);
      elseif($rule == 'trace')
	$domain_out = idn_prep_trace($domain, $charset);
      elseif($rule == 'sasl')
	$domain_out = idn_prep_sasl($domain, $charset);
      elseif($rule == 'iscsi')
	$domain_out = idn_prep_iscsi($domain, $charset);
    } else {
      if($rule == '2ascii')
	$domain_out = idn_to_ascii($domain, $charset);
      elseif($rule == '2uni')
	$domain_out = idn_to_unicode($domain, $charset);
      elseif($rule == '2utf8')
	$domain_out = idn_to_utf8($domain, $charset);
      elseif($rule == 'punyencode')
	$domain_out = idn_punycode_encode($domain, $charset);
      elseif($rule == 'punydecode')
	$domain_out = idn_punycode_decode($domain, $charset);
      else
	die("Non supported conversion '$rule'");
    }
    
    // Output the result (could contain the error message)
    echo "$domain: '$domain_out'<br>";
    $domain = $domain_out;
  }
?>
    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$domain?>" size="50">
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
      <input type="text" name="domain" value="<?=$domain?>" size="50">
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
