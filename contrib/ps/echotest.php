<?php

$echodata = str_repeat('foo', 128);

$sock = fsockopen('127.0.0.1', 20000);
if (!$sock) die();

$packet = chr(0x80).$echodata; // 0x80 = echo
$packet = pack('n', strlen($packet)).$packet;

$start = microtime(true);
fwrite($sock, $packet);
$res = fread($sock, 65536);
$time = microtime(true)-$start;

var_dump(($res == $packet));
printf("Time: %01.3fms\n", $time*1000);

