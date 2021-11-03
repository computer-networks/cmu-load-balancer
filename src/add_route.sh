sleep 0.1
if [[ -z $(ip route show | grep '172.128.1.1') ]]; then
    ip route add 172.128.1.0/24 via 172.128.1.1 dev eth0
fi
echo 0 > /proc/sys/net/ipv4/ip_forward
