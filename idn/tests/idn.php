<?php /* $Id: idn.php,v 1.2 2003-11-07 15:25:41 turbo Exp $ */ ?>
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
      </select>
      <input type="submit" value="Convert">
    </form>
<?php
} else {
    die("Module IDN isn't loaded (can't find function idn_ascii_to_unicode and/or idn_unicode_to_ascii)");
}
?>
  </body>
</html>
