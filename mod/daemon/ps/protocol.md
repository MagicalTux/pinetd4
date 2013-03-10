# PS Protocol

pub/sub provider and relay

## Data packets

Those data packets are generated and pushed on the provider socket.

They are then forwarded to relays, or directly to consumers.

* length (2 bytes, length of data after length, minimum is 17)
* type (1 byte, 0x00=JSON, 0x01=out_of_band_JSON, other types not defined)
* channel (16 bytes)
* data

## Subscription packets

### subscribe (0x81)

* length (16 bytes, value=17)
* type (1 byte, value=0x81)
* channel (16 bytes)

### unsubscribe (0x82)

* length (16 bytes, value=17)
* type (1 byte, value=0x82)
* channel (16 bytes)

