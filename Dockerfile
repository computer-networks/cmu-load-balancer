# base image from gradescope
FROM gradescope/auto-builds:ubuntu-18.04

# make necessary directories
RUN mkdir /var/www/ \
  && mkdir /usr/local/apache2/ \
  && mkdir /click/ \
  && mkdir /autograder/netsim \
  && mkdir /p3_starter/

# copy apache binary, configuration and binary
ADD ./www/ /var/www/

# copy video service contents
ADD ./apache2/ /usr/local/apache2/

ADD ./src/ /p3_starter/

RUN apt-get update \
  && apt-get -y install iproute2 iputils-ping net-tools netplan \
  && apt-get -y install psmisc iptables \
  && apt-get -y install gdb \
  && apt-get -y install tcpdump \
  && apt-get -y install build-essential \
  && apt-get -y install python python-pip \
  && apt-get -y install bison flex

# install click package
RUN git clone https://github.com/kohler/click /click

ADD ./src/cmuloadbalancer.cc ./src/cmuloadbalancer.hh /click/elements/local/

RUN cd /click \
  && ./configure --enable-local \
  && make elemlist \
  && make -j $(nproc) install

RUN echo "/p3_starter/init.sh" >> /root/.bashrc
