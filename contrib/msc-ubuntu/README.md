Master Core Upstart support for Ubuntu (tested on 14.04 LTS)

To install:

1. Build Master Core

        ./autogen.sh
        ./configure
        make

1. Run the install script

        sudo ./contrib/msc-ubuntu/install-mastercore-upstart.sh


This is based upon upstream work that is documented in these links:

* [Issue #4124](https://github.com/bitcoin/bitcoin/issues/4124)
* [Issue #4611](https://github.com/bitcoin/bitcoin/pull/4611)

This branch/PR used "git cherry-pick" to pull some of that code so it can be added to the michael-0921 branch of Master Core.

Doc on the files from the "Cherry Pick" can be found:

* [init/README.md](../init/README.md)
* [doc/init.md](../../doc/init.md)

It has been tested on Ubuntu 14.04 LTS.

There is a Vagrant script to install it here:
[https://github.com/msgilligan/install-msc/tree/vagrant](https://github.com/msgilligan/install-msc/tree/vagrant)


