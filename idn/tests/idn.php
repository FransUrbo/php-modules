<?php /* $Id: idn.php,v 1.4 2003-11-08 11:32:36 turbo Exp $ */ ?>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
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

if(function_exists("idn_ascii_to_unicode") && function_exists("idn_unicode_to_ascii")) {
    if($domain) {
	// Set the locale to UTF-8
	putenv("CHARSET=ISO-8859-1");
	//pql_execute("env", false);
	
	// Convert the value
	if($form) {
	  if($rule == 'name')
	    $domain_out = idn_prep_name($domain);
	  elseif($rule == 'krb')
	    $domain_out = idn_prep_kerberos5($domain);
	  elseif($rule == 'node')
	    $domain_out = idn_prep_node($domain);
	  elseif($rule == 'resource')
	    $domain_out = idn_prep_resource($domain);
	  elseif($rule == 'plain')
	    $domain_out = idn_prep_plain($domain);
	  elseif($rule == 'trace')
	    $domain_out = idn_prep_trace($domain);
	  elseif($rule == 'sasl')
	    $domain_out = idn_prep_sasl($domain);
	  elseif($rule == 'iscsi')
	    $domain_out = idn_prep_iscsi($domain);
	} else {
	  if($rule == 'unicode2ascii')
	    $domain_out = idn_unicode_to_ascii($domain);
	  elseif($rule == 'ascii2unicode')
	    $domain_out = idn_ascii_to_unicode($domain);
	  elseif($rule == 'punyencode')
	    $domain_out = idn_punycode_encode($domain);
	  elseif($rule == 'punydecode')
	    $domain_out = idn_punycode_decode($domain);
	  else
	    die("Non supported conversion '$rule'");
	}

	// Output the result (could contain the error message)
	echo "$domain: '$domain_out'<br>";
	$domain = $domain_out;
    }
?>
    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$domain?>" size="30">
      <select name="rule">
        <option value="unicode2ascii">UNICODE 2 ASCII</option>
        <option value="ascii2unicode">ASCII 2 UNICODE</option>
	<option value="punyencode">PUNYCODE ENCODE</option>
	<option value="punydecode">PUNYCODE DECODE</option>
      </select>
      <input type="submit" value="Convert">
    </form>

    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$domain?>" size="30">
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
      <input type="submit" value="Convert">
    </form>

    <a href="idna.php.txt">show code</a>
<?php
} else {
?>
    <a href="idna.php.txt">show code</a>
<?php
    die("Module IDN isn't loaded (can't find function idn_ascii_to_unicode and/or idn_unicode_to_ascii)");
}
?>
  </body>
</html>
