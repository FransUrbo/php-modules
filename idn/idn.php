<?
if(!extension_loaded('idn')) {
	dl('idn.so');
}
$module = 'idn';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:<br>\n";
foreach($functions as $func) {
    echo $func."<br>\n";
}
echo "<br>\n";
?>
