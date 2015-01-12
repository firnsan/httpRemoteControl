<?php
echo 
'<div class="usage">
<h4>端口转发用法：</h4>
<p>1、被控端(需具有公网ip)：     1234:8888</p>
<p>2、被控端所在内网目标：      htran.exe -p -slave 被控端IP 1234 127.0.0.1 3389</p>
<p>3、你的电脑：      RDP 被控端IP:8888</p>
</div>
<div class="usage">
<h4>downloader用法：</h4>
<p>1、输入要下载的资源url</p>
<p>2、输入要保存到的路径</p>
<p>3、点击按钮开始下载</p>
</div>';
?>