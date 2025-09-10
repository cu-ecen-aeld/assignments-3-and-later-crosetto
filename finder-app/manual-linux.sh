#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

SCRIPT_DIR=`dirname $0` 
OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
cd linux-stable
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

fi

make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} mrproper
echo "make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} defonfig"
make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} defconfig
make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} -j4 all
make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} modules
make ARCH=$ARCH CROSS_COMPILE=${CROSS_COMPILE} dtbs

echo "Adding the Image in outdir"
cp -r ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}/

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
echo "mkdir ${OUTDIR}/rootfs"
mkdir ${OUTDIR}/rootfs
echo "cd ${OUTDIR}/rootfs"
cd ${OUTDIR}/rootfs
echo "mkdir -p etc bin dev home usr/bin usr/sbin usr/lib tmp sys var/log sbin proc lib lib64"
mkdir -p etc bin dev home usr/bin usr/sbin usr/lib tmp sys var/log sbin proc lib lib64


cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone https://git.busybox.net/busybox
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -j4
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

cd ${OUTDIR}/rootfs
echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
SYSROOT=`${CROSS_COMPILE}gcc -print-sysroot`
cp ${SYSROOT}/lib/ld-linux-aarch64.so.1 lib/
cp ${SYSROOT}/lib64/libc.so.6 lib64/
cp ${SYSROOT}/lib64/libm.so.6 lib64/
cp ${SYSROOT}/lib64/libresolv.so.2 lib64/



# TODO: Make device nodes

sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1

# TODO: Clean and build the writer utility

cd ${SCRIPT_DIR}
rm writer
${CROSS_COMPILE}gcc writer.c -o writer

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp writer ${OUTDIR}/rootfs/home/
cp writer.sh ${OUTDIR}/rootfs/home/
cp finder.sh ${OUTDIR}/rootfs/home/
cp finder-test.sh ${OUTDIR}/rootfs/home/
cp -r ../conf ${OUTDIR}/rootfs/home/
cp autorun-qemu.sh ${OUTDIR}/rootfs/home/

cd ${OUTDIR}/rootfs
# TODO: Chown the root directory
fakeroot -- chown root .

# TODO: Create initramfs.cpio.gz
fakeroot -- find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ..
gzip -f initramfs.cpio
