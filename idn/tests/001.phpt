--TEST--
Check for idn presence
--SKIPIF--
<?php if (!extension_loaded("idn")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
echo "idn extension is available";
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php4/tests/README for further information on
  writing regression tests
*/
?>
--EXPECT--
idn extension is available
