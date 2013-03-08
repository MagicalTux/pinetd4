# PSRelay Protocol

pub/sub relay

## Incoming packets (from PSRelay)

* length (2 bytes, length of data after length, minimum is 17)
* type (1 byte, 0x00=JSON, other types not defined)
* channel (16 bytes)
* data

## Outgoing packets (to PSRelay)

### subscribe (0x81)

* length (16 bytes, value=17)
* type (1 byte, value=0x81)
* channel (16 bytes)

### unsubscribe (0x82)

* length (16 bytes, value=17)
* type (1 byte, value=0x82)
* channel (16 bytes)

