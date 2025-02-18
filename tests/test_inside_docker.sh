#!/bin/sh -xe

OS_VERSION=$1

ls -l /home

# Clean the yum cache
yum -y clean all
yum -y clean expire-cache

# First, install all the needed packages.
rpm -Uvh https://dl.fedoraproject.org/pub/epel/epel-release-latest-${OS_VERSION}.noarch.rpm

# Broken mirror?
echo "exclude=mirror.beyondhosting.net" >> /etc/yum/pluginconf.d/fastestmirror.conf

yum -y install yum-plugin-priorities
yum -y install rpm-build gcc gcc-c++ cmake git tar gzip make autotools autoconf automake wget

if [ "$OS_VERSION" = "6" ]; then

#yum -y install http://repo.okay.com.mx/centos/6/x86_64/release/okay-release-1-1.noarch.rpm
wget -nv https://bintray.com/vicendominguez/CentOS6/rpm -O /etc/yum.repos.d/bintray-vicendominguez-CentOS6.repo
yum -y install boost-devel
yum -y install centos-release-scl
yum -y upgrade
yum -y install devtoolset-7
scl enable devtoolset-7 bash
source /opt/rh/devtoolset-7/enable
which gcc

fi

if [ "$OS_VERSION" = "6" ]; then

yum -y install centos-release-scl
yum -y upgrade
yum -y install devtoolset-7
scl enable devtoolset-7 bash
source /opt/rh/devtoolset-7/enable
which gcc

fi

# Get REDHAWK rpm
wget -nv https://github.com/RedhawkSDR/redhawk/releases/download/2.2.0/redhawk-yum-2.2.0-el${OS_VERSION}-x86_64.tar.gz
tar xzvf redhawk-yum-2.2.0-el${OS_VERSION}-x86_64.tar.gz
cd redhawk-2.2.0-el${OS_VERSION}-x86_64
yum install -y redhawk-release*.rpm

# Prepare the RPM environment
cat<<EOF|sed 's@LDIR@'`pwd`'@g'| tee /etc/yum.repos.d/redhawk.repo
[redhawk]
name=REDHAWK Repository
baseurl=file://LDIR/
enabled=1
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-redhawk
EOF

yum -y groupinstall "REDHAWK Development"

. /etc/profile.d/redhawk.sh
. /etc/profile.d/redhawk-sdrroot.sh

#sudo /usr/sbin/usermod -a -G redhawk <username>
cd /HAWK1090/ADSBdecoder_MR
./build.sh
./build.sh install

