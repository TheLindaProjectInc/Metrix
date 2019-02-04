Table of Contents
------------------

- [Setting up Debian for Gitian building](#setting-up-debian-for-gitian-building)
- [Setting up Ubuntu for Gitian building](#setting-up-ubuntu-for-gitian-building)
- [Installing Gitian](#installing-gitian)
- [Setting up the Gitian image](#setting-up-the-gitian-image)
- [Building](#building)
- [Signing](#signing)


Setting up Debian for Gitian building
--------------------------------------

In this section we will be setting up the Debian installation for Gitian building.
We assume that a user `gitianuser` was previously added.

First we need to set up dependencies. Type/paste the following in the terminal:

```bash
sudo apt-get install git ruby apt-cacher-ng qemu-utils debootstrap lxc python-cheetah parted kpartx bridge-utils make ubuntu-archive-keyring curl firewalld
```

Then set up LXC and the rest with the following, which is a complex jumble of settings and workarounds:

```bash
sudo -s
# the version of lxc-start in Debian needs to run as root, so make sure
# that the build script can execute it without providing a password
echo "%sudo ALL=NOPASSWD: /usr/bin/lxc-start" > /etc/sudoers.d/gitian-lxc
echo "%sudo ALL=NOPASSWD: /usr/bin/lxc-execute" >> /etc/sudoers.d/gitian-lxc
# make /etc/rc.local script that sets up bridge between guest and host
echo '#!/bin/sh -e' > /etc/rc.local
echo 'brctl addbr br0' >> /etc/rc.local
echo 'ip addr add 10.0.3.1/24 broadcast 10.0.3.255 dev br0' >> /etc/rc.local
echo 'ip link set br0 up' >> /etc/rc.local
echo 'firewall-cmd --zone=external --add-interface=br0' >> /etc/rc.local
echo 'exit 0' >> /etc/rc.local
chmod +x /etc/rc.local
# make sure that USE_LXC is always set when logging in as gitianuser,
# and configure LXC IP addresses
echo 'export USE_LXC=1' >> /home/gitianuser/.profile
echo 'export GITIAN_HOST_IP=10.0.3.1' >> /home/gitianuser/.profile
echo 'export LXC_GUEST_IP=10.0.3.5' >> /home/gitianuser/.profile
sysctl -w net.ipv4.ip_forward=1
sed -i 's/lxcbr0/br0/' /etc/default/lxc-net
reboot
```

At the end Debian is rebooted to make sure that the changes take effect. The steps in this
section only need to be performed once.

**Note**: When sudo asks for a password, enter the password for the user `gitianuser` not for `root`.

Setting up Ubuntu for Gitian building
--------------------------------------

In this section we will be setting up the Ubuntu installation for Gitian building.
We assume that a user `gitianuser` was previously created and added to the sudo group. This guide is based on Ubuntu 16.04.

First we need to set up dependencies. Type/paste the following in the terminal:

```bash
sudo apt-get install git ruby apt-cacher-ng qemu-utils debootstrap lxc python-cheetah parted kpartx bridge-utils make curl ufw python-vm-builder
```

Then set up LXC and the rest with the following, which is a complex jumble of settings and workarounds:

```bash
sudo -s
# the version of lxc-start in Debian needs to run as root, so make sure
# that the build script can execute it without providing a password
echo "%sudo ALL=NOPASSWD: /usr/bin/lxc-start" > /etc/sudoers.d/gitian-lxc
echo "%sudo ALL=NOPASSWD: /usr/bin/lxc-execute" >> /etc/sudoers.d/gitian-lxc
# make /etc/rc.local script that sets up bridge between guest and host
echo '#!/bin/sh -e' > /etc/rc.local
echo 'brctl addbr br0' >> /etc/rc.local
echo 'ip addr add 10.0.3.1/24 broadcast 10.0.3.255 dev br0' >> /etc/rc.local
echo 'ip link set br0 up' >> /etc/rc.local
echo 'exit 0' >> /etc/rc.local
chmod +x /etc/rc.local
# make sure that USE_LXC is always set when logging in as gitianuser,
# and configure LXC IP addresses
echo 'export USE_LXC=1' >> /home/gitianuser/.profile
echo 'export GITIAN_HOST_IP=10.0.3.1' >> /home/gitianuser/.profile
echo 'export LXC_GUEST_IP=10.0.3.5' >> /home/gitianuser/.profile
sysctl -w net.ipv4.ip_forward=1
ufw allow ssh
ufw allow in on br0
sed -i 's/lxcbr0/br0/' /etc/default/lxc-net
```

Edit the /etc/ufw/before.rules file to allow natting to the new virtual interface, this is required to allow the LXC container internet access to download the dependancies.

```bash
sudo vi /etc/ufw/before.rules
```

Add the below lines before the *filter section, make sure to update the interface name to correspond with your internet facing interface.

```bash
# NAT table rules
*nat
:POSTROUTING ACCEPT [0:0]

# Forward traffic through ens3 - Change to match your out-interface e.g. eth0
-A POSTROUTING -s 10.0.3.0/24 -o ens3 -j MASQUERADE
COMMIT

```

At the end Debian is rebooted to make sure that the changes take effect. The steps in this
section only need to be performed once.

```
sudo reboot
```

**Note**: When sudo asks for a password, enter the password for the user `gitianuser` not for `root`.


Installing Gitian
------------------

Re-login as the user `gitianuser` that was created during installation.
The rest of the steps in this guide will be performed as that user.

There is no `python-vm-builder` package in Debian, so we need to install it from source ourselves, **there should be no requirement to perform this for Ubuntu**.

```bash
wget http://archive.ubuntu.com/ubuntu/pool/universe/v/vm-builder/vm-builder_0.12.4+bzr494.orig.tar.gz
echo "76cbf8c52c391160b2641e7120dbade5afded713afaa6032f733a261f13e6a8e  vm-builder_0.12.4+bzr494.orig.tar.gz" | sha256sum -c
# (verification -- must return OK)
tar -zxvf vm-builder_0.12.4+bzr494.orig.tar.gz
cd vm-builder-0.12.4+bzr494
sudo python setup.py install
cd ..
```

**Note**: When sudo asks for a password, enter the password for the user `gitianuser` not for `root`.

Clone the git repositories for Linda and Gitian.

```bash
git clone https://github.com/devrandom/gitian-builder.git
git clone https://github.com/TheLindaProjectInc/linda.git
```

Setting up the Gitian image
-------------------------

Gitian needs a virtual image of the operating system to build in.
Currently this is Ubuntu Xenial x86_64.
This image will be copied and used every time that a build is started to
make sure that the build is deterministic.
Creating the image will take a while, but only has to be done once.

**Note. Debian Only**

If you run the "make-base-vm" command below and get an error about sudo and ubuntu-minimal then apply this fix.

Debian fix for issues building vm.
can you find in file /usr/lib/python2.7/dist-packages/VMBuilder/plugins/ubuntu/dapper.py next string:

    self.run_in_target('apt-get', '-y', '--force-yes', 'dist-upgrade',

and replace with:

    self.run_in_target('apt-get', '-y', '--force-yes', '--option=Dpkg::Options::=--force-confnew', 'dist-upgrade',


**Ubuntu and Debian continue here.**

Execute the following as user `gitianuser`:

```bash
cd gitian-builder
PATH=$PATH:$(pwd)/libexec
bin/make-base-vm --lxc --arch amd64 --suite xenial
```

There will be a lot of warnings printed during the build of the image. These can be ignored.

**Note**: When sudo asks for a password, enter the password for the user `gitianuser` not for `root`.


## Building

Copy any additional build inputs into a directory named _inputs_.

Then execute the build using a `YAML` description file (can be run as non-root):
```
export USE_LXC=1 # LXC only
bin/gbuild <package>.yml
```
e.g.
```
bin/gbuild ../linda/contrib/gitian-descriptors/gitian-linux.yml
```
or if you need to specify a commit for one of the git remotes:
```
bin/gbuild --commit <dir>=<hash/branch> <package>.yml
```
e.g.
```
bin/gbuild --commit linda=master ../linda/contrib/gitian-descriptors/gitian-linux.yml
```

The build stage may take some time to complete depending on the speed your machine can compile.
If you wish to actively follow the build process then once the build script starts you can open a second terminal session an run a tail on the var/build.log.

Logged in as ```gitianuser```
```bash
tail -f ~/gitian-builder/var/build.log
```

On completion you will see the output files and a completion report in ~/gitian-builder/build/out/

Signing
-------

Signing would normally only be done by a single codesigner with access to commit to the distribution repository.

To sign the result, perform:

```bash
bin/gsign --signer <signer> --release <release-name> <package>.yml
```

Where <signer> is your signing PGP key ID and <release-name> is the name for the current release. This will put the result and signature in the sigs/<package>/<release-name>. The sigs/<package> directory can be managed through git to coordinate multiple signers.

After you've merged everybody's signatures, verify them:

```bash
bin/gverify --release <release-name> <package>.yml
```