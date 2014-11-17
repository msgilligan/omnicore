Building Master Core on OS X
============================

Building Master Core on OS X should not be any different than building Bitcoin Core. However, since the Bitcoin Core Mac OS X documentation is scattered in different locations, incomplete, and inaccurate (apparently) in some places, we are creating some build instructions and notes here.

This is a *preliminary document* that will be updated as the build process is improved and replicated by more developers. As the quality and accuraccy of this document improves (and we are are able to verify the process works for unmodified Bitcoin Core) we will work to submit corrections and improvements upstream.

# Supported Mac OS X versions

We are currently focusing our efforts on building on Mavericks (10.9) and running on Mavericks and Yosemite (10.10). If you have success on earlier versions of Mac OS X, please let us know.

# There's more than one way to do it. 

There are (at least) four different ways to build and run Master Core on Mac OS X. We will document all four because each method has advantages and disadvantages and the best choice depends upon a developer's specific requirements.

1. Build on Mac OS X for Mac OS X using Homebrew and command line tools

    This is the fastest and easiest way to build a native Master Core for Mac OS X from source code. Note: We are currently working on this build on the `mscore-0.0.9` branch.

1. Build on Mac OS X for Mac OS X using Xcode

    If you are a Mac OS X developer who likes the Xcode IDE and plan on reading and writing code or debugging on OS X, this may be your preferred option. Note: We haven't done this yet.

1. Build on Linux for Mac OS X using the Gitian build process

    This is how official, audited non-determistic builds of Bitcoin Core are created. The current plan is that an official Mac OS X build of Master Core will be created using this process. Note: We haven't done this yet.

1. Build on Linux and run on Linux inside a Virtual Machine.

    If you want to work with Master Core locally, but are deployng to a Linux server, and would like to replicate your deployment environment for development, this is your best option. This is also the only recommended approach for the `mscore-0.0.8` branch.

# Command-line Build using Homebrew

This process is [documented upstream](https://github.com/bitcoin/bitcoin/blob/master/doc/build-osx.md) but we'll summarize and correct it here.

## Preparation

See the upstream instructions. Note that the upstream instructions seem to be missing a dependency that must be installed as follows:

    `brew install libqrencode`

## Headless Build (mastercored)

See the upstream insructions for complete details. Note that we've found that`--enable-tests=no` is currently necessary.

    ./autogen.sh
    ./configure --with-gui=no --enable-tests=no
    make
    make install

## GUI Build (MasterCore-Qt.app)

Again, make sure to check the upstream instructions. To build the executable itself:

    ./autogen.sh
    ./configure --with-gui=yes --enable-tests=no
    make
    make install

To package as an application and as a disk image:

    make MasterCore-Qt.dmg

Known issues:

1. Until [PR #203](https://github.com/mastercoin-MSC/mastercore/pull/203) is merged, you'll need to apply those changes manually before attempting a GUI build.
1. Until there is a better solution, you'll also need to apply the following patch manually: [050fe38](https://github.com/mastercoin-MSC/mastercore/commit/050fe3811339a13db1b2e6ced67bb4299355422d)

# Build using Xcode

There is currently a Pull Request upstream [PR #5276](https://github.com/bitcoin/bitcoin/pull/5276) with an Xcode build of Bitcoin-Qt. We haven't tried this with Master Core, yet.


# Gitian Build

For a secure, replicable, verifiable build of Master Core, we will be using the same Gitian-build process that Bitcoin uses. This build is actually done inside a Linux VM but produces a complete OS X release. For details, see the [upstream instructions](https://github.com/bitcoin/bitcoin/blob/master/doc/release-process.md). We have not tried this on Master Core, yet.

# Build and run on a Linux VM

Master Core development has been focused on the Ubuntu Linux server (14.04 LTS) platform and this is currently the most reliable build of Master Core. Running Master Core inside a Linux VM on your Mac is easy and is a great way to replicate your server environment. The fastest way to get started is to use the instructions and tools in the [Omni-devops Github](https://github.com/mastercoin-MSC/omni-devops) repository.

# Using a Mac OS X VM for testing

Note: In order to create a reproducible build environment on OS X, it can be useful to run Mac OS X inside a VM. According to Apple, this is allowed only on OS X hardware and the Macintosh version of VirtualBox provides experimental support for creating Mac OS X guest VMs. We have successfully followed these instructions to [create a Mavericks guest VM on a Mavericks system](http://engineering.bittorrent.com/2014/07/16/how-to-guide-for-mavericks-vm-on-mavericks/).

Note: The process seems to be fairly fragile. In one case we tried bumping the guest VM size from 2GB to 4GB which resulting in a "black screen of death".







