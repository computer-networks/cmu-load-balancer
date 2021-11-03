// IP -> out[0], ARP reply -> out[1]
c :: Classifier(12/0800, 12/0806 20/0002);
arp_q :: ARPQuerier(172.128.1.1, aa:aa:aa:00:00:01);

FromDevice(eth0)
  -> c[0]
  -> Strip(14)
  -> CheckIPHeader()
  -> DecIPTTL()
  -> CMULoadBalancer()
  -> SetTCPChecksum()
  -> SetIPChecksum()
  -> IPPrint()
  -> [0]arp_q[0] 
  -> Queue(128)
  -> ToDevice(eth0);

c[1] -> [1]arp_q;
