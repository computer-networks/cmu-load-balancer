/*
 * cmuloadbalancer.{cc,hh} -- L4 load balancer
 * Shawn Chen <shuoshuc@cs.cmu.edu>
 *
 * Copyright (c) 2021 Carnegie Mellon University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include "cmuloadbalancer.hh"
#include <clicknet/tcp.h>
#include <clicknet/ip.h>
#include <click/ipaddress.hh>
#include <click/glue.hh>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
CLICK_DECLS

CMULoadBalancer::CMULoadBalancer()
{
}

CMULoadBalancer::~CMULoadBalancer()
{
}

Packet *
CMULoadBalancer::handle(Packet *xp)
{
  WritablePacket *p = xp->uniqueify();
  uint32_t seq, ack, off, dlen, hlen, plen = p->length();
  char src_ip[100], dst_ip[100];
  uint16_t sport, dport;
  uint8_t tcp_flags;
  click_ip *ip;
  click_tcp *th;

  /* Drops packets smaller than no-data packets. */
  if(plen < sizeof(click_ip) + sizeof(click_tcp)) {
    goto drop;
  }

  /* Casts packet to IP. */
  ip = p->ip_header();
  /* Gets IP header length. */
  hlen = ip->ip_hl << 2;
  /* Drops packets with invalid header length. */
  if (hlen < sizeof(click_ip) || hlen > plen) {
    goto drop;
  }

  /* Casts packet to TCP. */
  th = p->tcp_header();
  off = th->th_off << 2;
  /* Computes payload size. */
  dlen = plen - hlen - off;
  /* data length can be 0, but not negative. */
  if ((int)dlen < 0) {
    goto drop;
  }

  /* src IP addr */
  strcpy(src_ip, inet_ntoa(ip->ip_src));
  /* dst IP addr */
  strcpy(dst_ip, inet_ntoa(ip->ip_dst));
  /* src port coverted to host endianness */
  sport = ntohs(th->th_sport);
  /* dst port converted to host endianness */
  dport = ntohs(th->th_dport);
  /* TCP seqnum */
  seq = ntohl(th->th_seq);
  /* TCP acknum */
  ack = ntohl(th->th_ack);
  /* TCP header flags */
  tcp_flags = th->th_flags;

  /*
  if(th->th_flags == TH_SYN){
    // TODO: handles 3-way handshake, assigns flow to a server
  } else {
    // TODO: consistent hashing
  }
  */

  /* Poor man's load balancing.
   * Clients send their requests to 172.168.1.100:9876 (vip:vport). Code below
   * statically pins all flows onto backend server 172.128.1.31:7000. It also
   * translates server responses back to vip:vport in src fields so that clients
   * believe they are talking to the same server.
   * TODO: implement RR LB and adaptive LB.
   */
  if (strcmp("172.128.1.100", dst_ip) == 0) {
    if (inet_aton("172.128.1.31", &ip->ip_dst) == 0) {
      goto drop;
    }
    /* N.B., dst IP annotation must be set whenever dst IP is changed. Click
     * router uses dst IP annotation instead of dst IP to look up the correct
     * dst MAC address. Setting it to the new dst IP tells Click router to find
     * the post load balancing server MAC, otherwise packet ends up at the
     * wrong door and gets dropped.
     */
    p->set_dst_ip_anno(IPAddress(ip->ip_dst));
  }
  if (strcmp("172.128.1.31", src_ip) == 0) {
    if (inet_aton("172.128.1.100", &ip->ip_src) == 0) {
      goto drop;
    }
  }
  if (dport == 9876) {
    th->th_dport = htons(7000);
  }
  if (sport == 7000) {
    th->th_sport = htons(9876);
  }

  return(p);

 drop:
  if(p)
    p->kill();
  return(0);
}

Packet *
CMULoadBalancer::simple_action(Packet *p)
{
  return(handle(p));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CMULoadBalancer)
