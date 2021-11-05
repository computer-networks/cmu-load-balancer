#!/bin/bash
docker build -t p3-image:latest $PWD
docker network create --subnet=172.128.0.0/16 p3-net
# Launch 1 router.
docker run --privileged -dit --hostname router --net p3-net --ip 172.128.1.1 \
    --mac-address aa:aa:aa:00:00:01 --name router p3-image:latest /bin/bash
# Launch 5 clients.
for i in {1..5}
do
    docker run --privileged -dit --hostname "client""$i" --net p3-net \
        --ip "172.128.1.2""$i" --mac-address "aa:aa:aa:00:00:2""$i" \
        --name "client""$i" p3-image:latest /bin/bash
done
# Launch 3 servers.
for i in {1..3}
do
    docker run --privileged -dit --hostname "server""$i" --net p3-net \
        --ip "172.128.1.3""$i" --mac-address "aa:aa:aa:00:00:3""$i" \
        --name "server""$i" p3-image:latest /bin/bash
done
