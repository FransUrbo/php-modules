<?php /* $Id: idn.php,v 1.3 2003-11-08 06:45:43 turbo Exp $ */ ?>
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
	if($rule == IDN_IDNA_TO_ASCII)
	  $domain_out = idn_unicode_to_ascii($domain);
	elseif($rule == IDN_IDNA_TO_UNICODE)
	  $domain_out = idn_ascii_to_unicode($domain);
	elseif($rule == IDN_PUNYCODE_ENCODE)
	  $domain_out = idn_punycode_encode($domain);
	elseif($rule == IDN_PUNYCODE_DECODE)
	  $domain_out = idn_punycode_decode($domain);
	else
	  die("Non supported conversion '$rule'");

	// Output the result (could contain the error message)
	echo "$domain: '$domain_out'<br>";
	$domain = $domain_out;
    }
?>
    <form action="<?=$PHP_SELF?>" method="post">
      <input type="text" name="domain" value="<?=$domain?>" size="30">
      <select name="rule">
        <option value="<?=IDN_IDNA_TO_ASCII?>">UNICODE 2 ASCII</option>
        <option value="<?=IDN_IDNA_TO_UNICODE?>">ASCII 2 UNICODE</option>
	<option value="<?=IDN_PUNYCODE_ENCODE?>">PUNYCODE ENCODE</option>
	<option value="<?=IDN_PUNYCODE_DECODE?>">PUNYCODE DECODE</option>
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
