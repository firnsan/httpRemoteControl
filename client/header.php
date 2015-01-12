<?php 
session_start();
error_reporting(E_ALL ^ E_NOTICE);//显示除去 E_NOTICE 之外的所有错误信息
echo 
'<!DOCTYPE html>
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
	<title>HTTP RAT Web Client</title>
	<script type="text/javascript" src="js/jquery.min.js"></script>
	<link href="css/bootstrap.min.css" rel="stylesheet" media="screen">
	<script type="text/javascript" src="js/bootstrap.min.js"></script>
	<link href="css/style.css" rel="stylesheet">
</head>
<body>
<div class="container">
<nav class="navbar navbar-default navbar-static-top" role="navigation">
  <div class="container-fluid">
    <!-- Brand and toggle get grouped for better mobile display -->
    <div class="navbar-header">
      <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <a class="navbar-brand" href="httprat.php">HTTP RAT Web Client</a>
    </div>
	
    <div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">';
		if(isset($_SESSION["admin"])) {
			echo
			'<ul class="nav navbar-nav navbar-right">
			<li style="float:left;"><a href="httprat.php?do=view">View Computers</a></li>
			<li style="float:left;"><a href="server.zip">Get Server.zip</a></li> 
			<li style="float:left;"><a href="httprat.php?do=logout">Log out</a></li> 
			</ul>';
		} else {
			echo 
			'<ul class="nav navbar-nav navbar-right"><li><a href="httprat.php">未登录</a></li></ul>';
		}
	echo
	'</div><!-- /.navbar-collapse -->
  </div><!-- /.container-fluid -->
</nav>';
?>