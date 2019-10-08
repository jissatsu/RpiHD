#ifndef __RPI_IP_H
#define __RPI_IP_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <errno.h>
#include "conf.h"

int       normalize_ip( uint8_t *ip, uint8_t *dst );
uint8_t * long2ip( uint32_t long_ip );
uint8_t * getaddr( rpi_adr_type_t type, char *device, char *errbuf );
uint16_t  nhosts( uint8_t *nmask );
uint32_t  ip2long( uint8_t *ip );
uint32_t  net_off( uint8_t *ip, uint8_t *nmask );

#endif