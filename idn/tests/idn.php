<?php
/* $Id: idn.php,v 1.6 2003-11-11 13:27:29 turbo Exp $ */

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

    <a href="idna.php.txt">show code</a>
<?php
} else {
?>
    <a href="idna.php.txt">show code</a>
<?php
    die("Module IDN isn't loaded (can't find function idn_to_utf8 and/or idn_to_ascii)");
}
?>
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
