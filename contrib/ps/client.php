<?php

$sock = fsockopen('54.249.253.138', 65000);
if (!$sock) die();


function pkt_ping() {
	$echodata = microtime(true);
	$packet = chr(0x80).$echodata; // 0x80 = echo
	$packet = pack('n', strlen($packet)).$packet;
	return $packet;
}

fwrite($sock, pkt_ping());

// subscribe to channel too
$channel = 'dbf1dee9-4f2e-4a08-8cb7-748919a71b21';
$packet = chr(0x81).pack('H*', str_replace('-', '', $channel)); // 0x81 = subscribe
$packet = pack('n', strlen($packet)).$packet;
fwrite($sock, $packet);

$buf = '';

while(true) {
	$tmp = fread($sock, 8192);
	if ($tmp === false) break;
	if ($tmp === '') {
		// try to do a new ping
		fwrite($sock, pkt_ping());
		continue;
	}
	$buf .= $tmp;

	while(true) {
		if (strlen($buf) < 2) break;
		list(,$len) = unpack('n', $buf);
		$len += 2;

		if (strlen($buf) < $len) break; // not enough data yet
		$packet = substr($buf, 0, $len);
		$buf = (string)substr($buf, $len);

		handle_packet($packet);
	}
}

function handle_packet($packet) {
	$type = ord($packet[2]);

	switch($type) {
		case 0x00: // json inband data
			$channel = substr($packet, 3, 16);
			$body = substr($packet, 19);
			echo "Data for channel ".bin2hex($channel).": ".$body."\n";
			break;
		case 0x80:
			// echo request, body is actually microtime(true)
			$body = substr($packet, 3);
			$time = microtime(true)-$body;
			printf("Echo reply, delay = %01.3fms\n", $time*1000);
			break;
		default:
			echo "Unhandled packet type: ".dechex($type)."\n";
	}
}

