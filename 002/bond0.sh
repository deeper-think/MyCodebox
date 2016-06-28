#!/bin/bash

# $1: the 1st card for $BOND
# $2: the 2nd card for $BOND
#sh bond0.sh eth0 eth1 bond0
#sh bond0.sh eth2 eth3 bond1
FIRST_CARD=$1
SECOND_CARD=$2
BOND=$3

if [ $# != 3 ]
then 
    echo "please usage:sh bond0 first_card second_card bond_name"
    exit -1
fi 

if [ ! -f "/etc/sysconfig/network-scripts/ifcfg-${FIRST_CARD}" ]
then 
    echo "$FIRST_CARD config file not exist"
    exit -1
fi

if [ ! -f "/etc/sysconfig/network-scripts/ifcfg-${SECOND_CARD}" ]
then
    echo "$SECOND_CARD config file not exist"
fi

# 1) create ifcfg-$BOND
\cp -f /etc/sysconfig/network-scripts/ifcfg-${FIRST_CARD} /etc/sysconfig/network-scripts/ifcfg-$BOND
sed -i "/DEVICE/{s/.*/DEVICE=$BOND/}" /etc/sysconfig/network-scripts/ifcfg-$BOND
sed -i "/TYPE/{s/.*/TYPE=Bond/}" /etc/sysconfig/network-scripts/ifcfg-$BOND
sed -i "/NAME/{s/.*/NAME=$BOND/}" /etc/sysconfig/network-scripts/ifcfg-$BOND
sed -i '/HWADDR/d' /etc/sysconfig/network-scripts/ifcfg-$BOND
echo "BONDING_MASTER=yes
BONDING_OPTS=\"mode=0 miimon=100\"" >> /etc/sysconfig/network-scripts/ifcfg-$BOND
sed -i '/^$/d' /etc/sysconfig/network-scripts/ifcfg-$BOND

# 2) modify ifcfg-${FIRST_CARD} and ifcfg-${SECOND_CARD}

\cp -f /etc/sysconfig/network-scripts/ifcfg-${FIRST_CARD} /etc/sysconfig/network-scripts/bak-ifcfg-${FIRST_CARD}-before-$BOND-`date +"%Y-%m-%d-%s"`
\cp -f /etc/sysconfig/network-scripts/ifcfg-${SECOND_CARD} /etc/sysconfig/network-scripts/bak-ifcfg-${SECOND_CARD}-before-$BOND-`date +"%Y-%m-%d-%s"`

echo "DEVICE=${FIRST_CARD}
NAME=$BOND-slave
TYPE=Ethernet
BOOTPROTO=none
NM_CONTROLLED=yes
ONBOOT=yes
MASTER=$BOND
SLAVE=yes" > /etc/sysconfig/network-scripts/ifcfg-${FIRST_CARD}

echo "DEVICE=${SECOND_CARD}
NAME=$BOND-slave
TYPE=Ethernet
BOOTPROTO=none
NM_CONTROLLED=yes
ONBOOT=yes
MASTER=$BOND
SLAVE=yes" > /etc/sysconfig/network-scripts/ifcfg-${SECOND_CARD}

#3) restart network by system reboot 

reboot 



