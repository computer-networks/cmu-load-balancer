# cmuloadbalancer
## Introduction
This folder contains the files to build a Docker testbed with all the
dependencies you need for developing the load balancer.
First make sure docker is installed on your machine. It is highly recommended
to read [Docker Document](https://docs.docker.com/) and understand how docker
works. 

## Content
| File/Folder | Use                                     |
| :---------: | :-------------------------------------: |
| apache2/    | Apache web server                       |
| www/        | A website with video                    |
| src/cmu*    | Load balancer code (copied to /click)   |
| src/echo*   | Simple echo server to test starter code |
| src/loadgen.py | Load generator script that fetches video chunks from the server |
| src/simple.load | Load config that tells loadgen.py how to generate loads |
| Dockerfile  | Docker config                           |
| README.md   | Readme file                             |
| setup.sh    | Script to set up the docker testbed     |

## Set up the testbed
  1. Execute ./setup.sh from terminal. This script builds a docker image and
  launches a set of containers.
  2. When the script finishes, you can log into each container. The complete
  set of container names is: router, client[12345], server[123]. Run this
  command to log into one of them:
  `docker exec -it [container_name] /bin/bash`.

## Click modular router
This load balancer is designed to operate on L4, which means it can balance
load based on the information in link layer, network layer and transport layer,
but *NOT* in application layer.
This is a transparent load balancer so it does not terminate TCP connections and
both clients and servers should not be aware of it. Therefore, the best way to
implement this network function is to make it look like a router box. But note
that it is not a router, for example it does not necessarily respond to pings.
(In fact, most load balancers do not allow you to ping the backend servers as 
that exposes a security risk.)

The [Click modular router](https://github.com/kohler/click#readme) is a powerful
software router that enables fast prototyping of various network functions. All
of the routing and processing is done in software so you have all the
flexibility to customize your load balancer. Since Click itself is quite
complicated, mastering the whole Click architecture is out of the scope of this
project. We provide you a working Click program with an empty load balancer
module. You only need to focus on developing the module named CMULoadBalancer.

### A very brief Click primer
In the `/click` folder of your container `router`, you will find the complete
source code of Click. The CMULoadBalancer module is implemented in two files
named `/click/elements/local/cmuloadbalancer.{cc,hh}`, you will need to keep
them where they are and make changes. Do not modify the existing function
signatures but feel free to add your own helper functions and data structures.
The existing code is heavily documented to show you how to manipulate packet
headers.

Click needs to be recompiled every time you make new changes. To do that, run:
```
$ cd /click
$ ./configure --enable-local
$ make elemlist
$ make -j $(nproc) install
```

You can start Click to test the load balancer by feeding it a configuration. The
Click configuration tells the Click binary how each routing component should be
connected to function as a router together. A working configuration is provided
in `/p3_starter/cmu_load_balancer.click`. Run the following command to launch
Click:
```
$ click /p3_starter/cmu_load_balancer.click
```
Keep in mind that routing only works when Click is running. You can ctrl-C to
stop it.

### CMU Load Balancer
The starter code implements a very simple static load balancing scheme. The
clients all connect to a virtual IP:virtual port (i.e., 172.128.1.100:9876). The
load balancer statically translates the vip:vport to a real backend IP:port and
vice versa. You can verify this behavior by running the given `echo_client.py`
and `echo_server.py` programs.
First, after you have set up the testbed, log into the router and start Click:
```
$ docker exec -it router /bin/bash
$ click /p3_starter/cmu_load_balancer.click
```
Next, log into server1 and start the `echo_server`:
```
$ docker exec -it server1 /bin/bash
$ python3 /p3_starter/echo_server.py
```
The `echo_server` will listen on port `7000` on `172.128.1.31`. Then, log into
client1 and start the `echo_client`:
```
$ docker exec -it client1 /bin/bash
$ python3 /p3_starter/echo_client.py
```
The `echo_client` will connect to port `9876` on `172.128.1.100`, which does not
exist anywhere on the testbed. But you should see that the client prints out a
"hello world" message correctly. This means the Click router is doing its job.
If you kill the Click router, `echo_server` and `echo_client` will no longer
work.
  
## Web server
The same Apache web server is reused from the adaptive bitrate project to serve
the same video traffic. Each server container has an Apache server running at
port number `8000 + $(last decimal of server's IP of eth0)`. For example,
`server1` with IP `172.128.1.31` will have its Apache running on port `8031`.
You should not need to modify the Apache config or the website content for this
project. If the Apache server crashes or needs a restart, you need to manually
restart it: `/usr/local/apache2/bin/httpd -k restart`.

## Load generator
`loadgen.py` and `simple.load` are the load generator facility we provide for
testing. To use `loadgen.py`, run
```
$ cd /p3_starter
$ python3 loadgen.py -e simple.load
```
This will generate HTTP GET requests to the server and fetch a set of video
chunks. It allows a number of ways to fetch the video chunks: (1) fetching at
a specified time, (2) fetching a specified bitrate, (3) fetching the chunks a
number of times, and (4) sleep between fetching 2 chunks. Please read the
comments of `simple.load` to get more details. Current load config only contains
one simple test scenario, follow syntax and extend it or create a different load
config for your own testing.
