<?php

$channel = 'c7825f96-6ad7-4bc3-bf5a-668e04650bc2';
$message = '{"hello":"world","json":"yes, this is json"}';

push_msg($channel, $message);

function push_msg($channel, $msg) {
	// encode channel
	$channel = pack('H*', str_replace('-', '', $channel));
	if (strlen($channel) != 16) return false;
	$final = chr(0).$channel.$msg;
	$final = pack('n', strlen($final)).$final;

	static $unix_sock = null;
	if (is_null($unix_sock))
		$unix_sock = stream_socket_client('udg:///tmp/ps.socket');
	
	if ($unix_sock) stream_socket_sendto($unix_sock, $final);
}

