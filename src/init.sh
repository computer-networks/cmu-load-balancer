sleep 0.1
# Force traffic to go through our own router.
if [[ -z $(ip route show | grep "172.128.1.1") ]]; then
    ip route add 172.128.1.0/24 via 172.128.1.1 dev eth0
fi
# disable Linux kernel packet forwarding.
echo 0 > /proc/sys/net/ipv4/ip_forward
# change Apache port number and restart.
IP=$(hostname -i)
PORT=$((${IP##*.} + 8000))
if [[ -z $(grep "^Listen $PORT" /usr/local/apache2/conf/httpd.conf) ]] && \
   [[ $(hostname) == *"server"* ]]; then
    sed -i "s/^Listen.*/Listen $PORT/g" /usr/local/apache2/conf/httpd.conf
    /usr/local/apache2/bin/httpd -k restart
fi
