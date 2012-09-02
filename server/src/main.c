// sdk20-eth-zb server
// Authors: Kirill Bulygin <kirill.bulygin@gmail.com>, Vasiliy Pinkevich <vass258@yandex.ru>

#include <lpc2292.h>
#include <eth.h>
#include <leds.h>
#include <spi.h>
#include <stdio_uart0.h>
#include <sw.h>
#include <systimer.h>
#include <zb.h>


#pragma pack(1) // be sure to include option "-fpack-struct" for an old gcc


#define BYTE(n,x) ( 0xFF & ( (x) >> (n << 3) ) )
#define SWAP(x) ( BYTE(0, x) << 8 | BYTE(1, x) )
#define MAX(a,b) ( (a) >= (b) ? (a) : (b) )

#define XCHG(size,a,b) do { t##size = a; a = b; b = t##size; } while(0)

#define MAX_PAYLOAD_LENGTH	104
#define ZB_MASTER_EXCHANGE_TRY_CNT	3
#define ZB_MASTER_RECEIVE_TIMEOUT	1000
#define ZB_SLAVE_RESPONSE_CNT	1
#define ZB_SLAVE_RESPONSE_DELAY	500
#define ETH_SEND_TRY_CNT	3

#define MY_SPRINTF_SLOTS_COUNT	2
#define MY_SPRINTF_SLOT_SIZE	(MAX(3*4+3, 2*6+5) + 1)
#define MY_SPRINTF_SLOT(n)		(my_sprintf_slots + n * MY_SPRINTF_SLOT_SIZE)


typedef unsigned char  b_t; // 8 bits
typedef unsigned short w_t; // 16 bits
typedef unsigned int   d_t; // 32 bits


char my_sprintf_slots[MY_SPRINTF_SLOTS_COUNT * MY_SPRINTF_SLOT_SIZE];


// Print packet in hex+ASCII (Wireshark-style)
void print_packet(const b_t *buf, w_t length) {
	w_t i, j;
	b_t c;

	printf("Packet with length %d:\n", length);

	for(i = 0; i < length; i += 16) {
		printf("%04x  ", i);
		for(j = 0; j < 16; j++) {
			if((i + j) >= length)
				printf("   ");
			else
				printf("%02x ", buf[ i + j ]);

			if(j == 7)
				printf(" ");
		}
		printf("  ");
		for(j = 0; j < 16; j++) {
			if((i + j) >= length)
				break;

			c = buf[i + j];
			printf("%c", (0x20 <= c && c <= 0x7E) ? c : '.');

			if(j == 7)
				printf(" ");
		}
		printf("\n");
	}
}


// Compute IP checksum as big-endian word (buf points to the IP header)
w_t ip_chksum(const void *buf, w_t length) {
    d_t sum = 0;
    const w_t *w_p = buf;

    while(length > 1) {
        sum += *w_p;
        if(sum & 0x80000000)
            sum = (sum & 0xFFFF) + (sum >> 16);
        w_p++;
        length -= 2;
    }

    while(sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}


char *ip_str(b_t slot, d_t ip) {
	sprintf(MY_SPRINTF_SLOT(slot), "%d.%d.%d.%d", 
		BYTE(0, ip), BYTE(1, ip), BYTE(2, ip), BYTE(3, ip));
	return MY_SPRINTF_SLOT(slot);
}


char *mac_str(b_t slot, const b_t *mac) { 
	sprintf(MY_SPRINTF_SLOT(slot), "%02x:%02x:%02x:%02x:%02x:%02x", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return MY_SPRINTF_SLOT(slot);
}


b_t macs_eq(const b_t mac1[6], const b_t mac2[6]) {
	b_t i;
	for(i = 0; i < 6; i++)
		if(mac1[i] != mac2[i])
			return 0;
	return 1;
}


typedef struct { // Ethernet II
	b_t dst[6];
	b_t src[6];
	w_t type;
} mac_header_t;


typedef struct { // IPv4
	w_t w_1;
	w_t length;
	w_t ident;
	w_t w_4;
	b_t ttl;
	b_t protocol;
	w_t chksum;
	d_t src;
	d_t dst;
} ip_header_t;


typedef struct { // UDP
	w_t src;
	w_t dst;
	w_t length;
	w_t chksum;
} udp_header_t;


typedef struct { // application layer
	b_t first_byte;
} payload_t;


typedef struct { // ARP
	// constants
	w_t htype;
	w_t ptype;
	b_t hlen;
	b_t plen;

	w_t oper;

	b_t mac_src[6];
	d_t ip_src;
	b_t mac_dst[6];
	d_t ip_dst;
} arp_t;


typedef struct {
	mac_header_t mac;
	ip_header_t ip;
	udp_header_t udp;
	payload_t payload;
} udp_packet_t;


typedef struct {
	mac_header_t mac;
	arp_t arp;
} arp_packet_t;


int process_payload(b_t *payload_p, w_t *payload_length_p) {
	w_t i;
	char d = sw4_byte();

	for(i = 0; i < *payload_length_p; i++) {
		payload_p[i] += d;
	}

	return OK;
}


int process_payload_of_udp_packet(udp_packet_t *udp_p, w_t *payload_length_p) {
	b_t i, is_sent;
	w_t zb_dst;

	b_t *payload_p = &udp_p->payload.first_byte;

	if(((b_t)udp_p->ip.w_1 & 0x0F) != 0x05) {
		*payload_length_p = sprintf(payload_p, "[!] IP header must have standard length 20 bytes");
		return ERR;
	}

	//if(SWAP(udp_p->udp.length) != SWAP(udp_p->ip.length) - 20) {
	if(udp_p->ip.length - udp_p->udp.length != sizeof(udp_p->ip) << 8) {
		*payload_length_p = sprintf(payload_p, "[!] UDP length != IP payload length");
		return ERR;
	}

	if(ip_chksum(&udp_p->ip.w_1, sizeof(udp_p->ip)) != SWAP(0x0000)) {
		*payload_length_p = sprintf(payload_p, "[!] Invalid IP header checksum: %04x", udp_p->ip.chksum);
		return ERR;
	}

	*payload_length_p = SWAP(udp_p->udp.length) - sizeof(udp_p->udp);
	if(*payload_length_p > MAX_PAYLOAD_LENGTH) { // as unsigned includes negative lengths 
		// (when total length mentioned in the UDP header is less than the UDP header length).
		// So packets with empty payload are allowed.
		*payload_length_p = sprintf(payload_p, "[!] Packet have payload with length %d, not in 0..%d", (signed char)*payload_length_p, MAX_PAYLOAD_LENGTH);
		return ERR;
	}

	zb_dst = SWAP(udp_p->udp.dst);
	if(zb_dst == get_self_zb_addr()) {
		return process_payload(payload_p, payload_length_p);
	}

	set_dest_zb_addr(zb_dst);
	is_sent = 0;
	for(i = 0; i < ZB_MASTER_EXCHANGE_TRY_CNT; i++) {
		if(zb_send_packet(payload_p, *payload_length_p) != OK) {
			printf("[?] Request IS NOT sent over ZigBee\n");
		} else if(zb_get_packet(payload_p, payload_length_p, ZB_MASTER_RECEIVE_TIMEOUT) != OK) {
			is_sent = 1;
			printf("[?] Response IS NOT received over ZigBee\n");
		} else {
			return OK;
		}
	}

	if(is_sent) {
		*payload_length_p = sprintf(payload_p, "[!] Slave is not responding");
	} else {
		*payload_length_p = sprintf(payload_p, "[!] Can't send a request to the slave");
	}

	return ERR;
}


w_t my_zb_addr() {
	return sw3_byte();
}


int main(void) {
	b_t packet_p[1500 + 1];
	w_t packet_length;
	udp_packet_t *udp_p = (udp_packet_t*) packet_p;
	arp_packet_t *arp_p = (arp_packet_t*) packet_p;
	w_t payload_length;

	// addresses
	const b_t mac_broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	const b_t mac_me[6] = { 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0 };
	const d_t ip_me = 0x8f03a8c0; // 192.168.3.143
	const w_t zb_master = 1;

	b_t i;

	// temp
	b_t tb;
	w_t tw;
	d_t td;

	b_t leds_status = 0x0F;


	init_pio();
	init_led();
	init_serial();

	init_systimer0();
	init_systimer1();
	start_timer0();
	start_timer1();
	
	init_spi1();
	set_self_zb_addr(my_zb_addr());
	set_dest_zb_addr(zb_master);
	init_zb();

	printf("sizeof(udp_packet_t) = %d\n", sizeof(udp_packet_t));
	printf("My ZB address: %d\n", get_self_zb_addr());

	if(get_self_zb_addr() == zb_master) {
		printf("I'm a master!\n");
		init_eth();
		printf("My MAC address: %s\n", mac_str(0, mac_me));
		printf("My IP address: %s\n", ip_str(0, ip_me));
		while(1) { // getting Ethernet packet
			leds(leds_status ^= 0xFF);
			if(read_eth_pack(packet_p, &packet_length) == OK) {
				printf(".");
				if(!(macs_eq(udp_p->mac.dst, mac_me) || macs_eq(udp_p->mac.dst, mac_broadcast))) { // that's not for me and not for everybody
					continue; 
				}
				if(arp_p->mac.type == SWAP(0x0806) && arp_p->arp.oper == SWAP(0x0001) && arp_p->arp.ip_dst == ip_me) { // ARP request for me
					printf("[time: %d] ARP request\n", clock());
					// reply
					for(i = 0; i < 6; i++) {
						arp_p->mac.dst[i] = arp_p->arp.mac_dst[i] = arp_p->arp.mac_src[i];
						arp_p->mac.src[i] = arp_p->arp.mac_src[i] = mac_me[i];
					}
					// ARP constant fields stay unchanged
					arp_p->arp.oper = SWAP(0x0002); // reply
					XCHG(d, arp_p->arp.ip_src, arp_p->arp.ip_dst);
					packet_length = sizeof(*arp_p);

					tb = (write_eth_pack(packet_p, packet_length) == OK);
					printf("ARP reply to %s (%s): %s\n\n", ip_str(0, arp_p->arp.ip_dst), mac_str(1, arp_p->arp.mac_dst), tb ? "OK" : "ERROR");
				} else if(udp_p->mac.type == SWAP(0x0800) && udp_p->ip.protocol == 17 && udp_p->ip.dst == ip_me) { // IPv4, UDP packet for me
					printf("[time: %d] UDP packet\n", clock());
					//print_packet(packet_p, packet_length);

					if(process_payload_of_udp_packet(udp_p, &payload_length) != OK) {
						printf("Error message in response:\n%s\n", &udp_p->payload.first_byte);
					}

					// reply over Ethernet
					packet_length = 14 + 20 + 8 + payload_length;
					//packet_length = sizeof(*udp_p) - sizeof(udp_p->payload) + payload_length;

					for(i = 0; i < 6; i++) {
						XCHG(b, udp_p->mac.src[i], udp_p->mac.dst[i]);
					}

					udp_p->ip.w_1 = SWAP(0x4500); // version, header length; diff. service field
					udp_p->ip.length = SWAP(20 + 8 + payload_length); // total length
					udp_p->ip.ident = SWAP(0x0000); // identification
					udp_p->ip.w_4 = SWAP(0x4000); // flags (Don't Fragment = 1), fragment offset

					udp_p->ip.ttl = 0xFF;
					//udp_p->ip.protocol = 17; // UDP - is set already
					XCHG(d, udp_p->ip.src, udp_p->ip.dst);
					udp_p->ip.chksum = SWAP(0x0000); // assume this while actually computing checksum
					udp_p->ip.chksum = ip_chksum(&udp_p->ip.w_1, 20);

					XCHG(w, udp_p->udp.src, udp_p->udp.dst);
					udp_p->udp.length = SWAP(8 + payload_length);
					udp_p->udp.chksum = SWAP(0x0000);

					printf("Response:\n");
					print_packet(packet_p, packet_length);

					for(i = 0; i < ETH_SEND_TRY_CNT; i++) {
						if(write_eth_pack(packet_p, packet_length) == OK) {
							printf("Response is sent over Ethernet\n");
							break;
						} else {
							printf("[!] Response IS NOT sent over Ethernet\n");
						}
					}
					printf("\n");
				}
			} // if(read_packet...)
		} // while(1)
	} else {
		printf("I'm a slave!\n");
		while(1) {
			leds(leds_status ^= 0xFF);
			tb = my_zb_addr();
			if(tb != get_self_zb_addr()) {
				set_self_zb_addr(tb);
				init_zb();
				printf("My ZB address now: %d\n", get_self_zb_addr());
			}
			if(zb_get_packet(packet_p, &packet_length, 0) == OK) {
				print_packet(packet_p, packet_length);

				process_payload(packet_p, &packet_length);
				packet_p[packet_length] = 0; // terminate for printf

				for(i = 0; i < ZB_SLAVE_RESPONSE_CNT; i++) {
					if(zb_send_packet(packet_p, packet_length) == OK) {
						printf("Response '%s' is sent over ZigBee\n", packet_p);
					} else {
						printf("[!] Response IS NOT sent over ZigBee\n");
					}
					if(i < ZB_SLAVE_RESPONSE_CNT - 1) {
						delay_ms(ZB_SLAVE_RESPONSE_DELAY);
					}
				}
			}
		}
	}
	return 0;
}

// vim:ts=4
