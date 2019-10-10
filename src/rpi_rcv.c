#include "rpi_rcv.h"


int init_sock( char *errbuf )
{
    int sock, sopt, opt;

    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock < 0 ){
        strcpy( errbuf, strerror( errno ) );
        return -1;
    }

    sopt = setsockopt( 
        sock, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        &opt, 
        sizeof( opt )
    );

    if ( sopt < 0 ){
        strcpy( errbuf, strerror( errno ) );
        return -1;
    }
    return sock;
}

void packet_handler( u_char *args, const struct pcap_pkthdr *header, 
                     const u_char *packet )
{
    short is_reply;
    uint16_t ether_type;
    struct rpi_eth_hdr *eth_hdr = (struct rpi_eth_hdr *) packet;
    struct rpi_arp_hdr *arp_hdr = (struct rpi_arp_hdr *) (packet + 14);
    struct rpi_conf *conf       = (struct rpi_conf *) args;
    struct arp_reply r, *reply  = &r;

    is_reply   = 0;
    ether_type = ntohs( eth_hdr->ether_type );
    
    if ( ether_type == ETHERTYPE_ARP && ntohs( arp_hdr->opcode ) == ARPOP_REPLY )
    {
        // if the reply is destined to us
        if ( arp_hdr->dst_hw[0] == conf->hw[0]
          && arp_hdr->dst_hw[1] == conf->hw[1]
          && arp_hdr->dst_hw[2] == conf->hw[2]
          && arp_hdr->dst_hw[3] == conf->hw[3]
          && arp_hdr->dst_hw[4] == conf->hw[4]
          && arp_hdr->dst_hw[5] == conf->hw[5] )
          {
              is_reply = 1;
              memcpy( reply->src_hw, arp_hdr->src_hw, 6 );
              memcpy( reply->src_ip, arp_hdr->src_ip, 4 );
        }
    }

    if ( is_reply )
    {
        fprintf( stdout, "%d.%d.%d.%d - ", reply->src_ip[0], reply->src_ip[1], reply->src_ip[2], reply->src_ip[3] );
        fprintf( stdout, "%02x:%02x:%02x:%02x:%02x:%02x\n", 
            reply->src_hw[0], reply->src_hw[1], reply->src_hw[2],
            reply->src_hw[3], reply->src_hw[4], reply->src_hw[5] );
    }
    is_reply = 0;
}

void * rpi_arp_sniffer( void *conf )
{   
    pcap_t *handle;
    int snaplen;
    int timeout;
    int promisc;
    struct rpi_conf *_conf = (struct rpi_conf *) conf;
    char err_buff[PCAP_ERRBUF_SIZE];
    
    if ( (sockfd = init_sock( err_buff )) < 0 ){
        _rlog( RPI_LOG_ERR, err_buff );
    }

    snaplen =  64;
    timeout = 100;
    promisc =   0;

    handle  = pcap_open_live(
        _conf->device,
        snaplen,
        promisc,
        timeout,
        err_buff
    );

    if ( !handle ) {
        _rlog( RPI_LOG_ERR, err_buff );
    }

    pcap_loop( handle, -1, packet_handler, (u_char *) _conf );
    pcap_close( handle );
    return NULL;
}

int notify_server( int *sock, char *buff )
{
    // uint8_t buff[0xFF];
    // char *end = "[arp-end]";
    return 0;
}