#!/bin/sh

# Copy the executable
cp src/bitcoind /usr/bin/bitcoind
cp src/bitcoin-cli /usr/bin/bitcoin-cli

# Set up directories, users, and permissions as standardized by Bitcoin Core
mkdir -p /etc/bitcoin
mkdir -p /var/lib/bitcoind
mkdir -p /var/run/bitcoind

groupadd --force --system  bitcoin
useradd -M --system --shell /usr/sbin/nologin --home /var/lib/bitcoind --gid bitcoin bitcoin

chown bitcoin:bitcoin /var/lib/bitcoind
chmod 750 /var/lib/bitcoind

chown bitcoin:bitcoin /var/run/bitcoind
chmod 755 /var/run/bitcoind

# Copy Bitcoin configuration file
cp contrib/msc-ubuntu/bitcoin.conf /etc/bitcoin
chown bitcoin:bitcoin /etc/bitcoin/bitcoin.conf
chmod 660 /etc/bitcoin/bitcoin.conf

# Copy Upstart configuration file/script
cp contrib/init/bitcoind.conf /etc/init

echo "Set rpcuser and rpcpassword in /etc/bitcoin/bitcoin.conf, then type: service bitcoind start"

