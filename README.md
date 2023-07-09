# UDP Server

Minimal implementation of a UDP Server. Can be set as blocking or non-blocking.

Enforces a maximum datagram size of 1200 octets (`UDP_SERVER_MAX_OCTET_SIZE`), to make it as compatible [^1] as possible with other datagram transports and to avoid datagram fragmentation over the Internet[^2].

[^1]: [Steam networking](https://partner.steamgames.com/doc/api/ISteamNetworking) had a 1200 octets maximum (now deprecated).
[^2]: [IETF draft](https://www.ietf.org/archive/id/draft-ietf-dnsop-avoid-fragmentation-07.html)
