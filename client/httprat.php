<?php
require('header.php');

$adminname=$_POST["adminname"];
$pass = $_POST["pass"];
if(($adminname=="whitepig")&&($pass=="httprat")) 
{
	$_SESSION["admin"]="whitepig";
	header("location:httprat.php");
}

if($_GET["do"]=="logout")
{
	unset($_SESSION["admin"]);
	header("location:httprat.php");
}

if(isset($_SESSION["admin"]))
{        
	switch ($_GET["do"])
	{
		case "view": require_once("view.php");break;
		default:require('help.php');break;
	}
}
else
{
	echo 
	'<form style="width:400px;margin:auto;" action = "httprat.php" method="post">
	<input style="margin-bottom:10px;" class="form-control" type="text" name="adminname" />
	<input style="margin-bottom:10px;" class="form-control" type="password" name="pass" />
	<input class="btn btn-primary btn-block" type="submit" value="login">';
}



require('footer.php');



?>