#ifndef CLICK_CMULOADBALANCER_HH
#define CLICK_CMULOADBALANCER_HH
#include <click/element.hh>
#include <click/glue.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>
CLICK_DECLS

/*
 * =c
 * CMULoadBalancer()
 * =s tcp
 * L4 load balancer for video streaming.
 * =d
 * Functions as an L4 load balancer for video streaming. It does not terminate
 * TCP connections. Two load balancing schemes are supported: round robin and
 * adaptive.
 * =e
 * Example usage:
 * FromDevice(eth1, 0)
 *   -> Strip(14)
 *   -> CheckIPHeader()
 *   -> IPFilter(allow tcp && dst host 1.2.3.4 && dst port 9876, deny all)
 *   -> Print(x)
 *   -> CMULoadBalancer()
 *   -> Print(y)
 *   -> EtherEncap(0x0800, 1:1:1:1:1:1, 00:03:47:07:E9:94)
 *   -> ToDevice(eth1);
 */

class CMULoadBalancer : public Element {
 public:

  CMULoadBalancer() CLICK_COLD;
  ~CMULoadBalancer() CLICK_COLD;

  const char *class_name() const		{ return "CMULoadBalancer"; }
  const char *port_count() const		{ return PORTS_1_1; }

  Packet *simple_action(Packet *);
  Packet *handle(Packet *xp);
};

CLICK_ENDDECLS
#endif
