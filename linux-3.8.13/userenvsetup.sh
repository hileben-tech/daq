#!/bin/sh

export ARCH=arm 
export CROSS_COMPILE=arm-linux-gnueabi-

export ROOTFS_PATH=/media/ray/rootfs

UNAME=3.8.13-boneHLTDAQ+

function install_modules2rootfs()
{
        sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- INSTALL_MOD_PATH=${ROOTFS_PATH} modules_install;
}

function install_images2rootfs()
{
        sudo cp arch/arm/boot/zImage ${ROOTFS_PATH}/boot/vmlinuz-3.8.13;
        sudo cp .config ${ROOTFS_PATH}/boot/config-3.8.13;
	echo "Copied vmlinuz-3.8.13 & config-3.8.13 to ${ROOTFS_PATH}/boot" 
}

function make_kernel()
{
	make -j 4;
	make modules -j 4;	
}

function remake_kernel()
{
	make distclean;
	make beaglebone_defconfig;
	make -j 4;
	make modules -j 4;
}

function copy_images2tftpboot()
{
	cp arch/arm/boot/zImage /home/ray/tftpboot;
	cp arch/arm/boot/dts/am335x-boneblack.dtb /home/ray/tftpboot/dtbs;
	echo "Copied zImage & am335x-boneblack.dtb to /home/ray/tftpboot"
}

export BOARD_IP=
function install_images2board()
{
	echo "$BOARD_IP"
	if [ -n "$BOARD_IP" ]; then
        	#sudo scp arch/arm/boot/zImage root@"$BOARD_IP":/boot/vmlinuz-3.8.13;
		#sudo scp arch/arm/boot/dts/am335x-boneblack.dtb root@"$BOARD_IP":/boot/dtbs;
        	#scp arch/arm/boot/zImage root@"$BOARD_IP":/boot/vmlinuz-$UNAME;
		scp arch/arm/boot/dts/am335x-boneblack.dtb root@"$BOARD_IP":/boot/dtbs/$UNAME/;
		echo "Installed zImage & am335x-boneblack.dtb to $BOARD_IP"
	fi
}

export GEEK_TYPE=
function do_geek()
{
	case "$GEEK_TYPE" in
		2) remake_kernel;;
		3) make_kernel;;
		4) install_images2rootfs;;
		5) install_modules2rootfs;;
		6) copy_images2tftpboot;;
		7) install_images2board;;
	esac
}

function geek()
{
	GEEK_TYPE=
	ARG=$1
	if [ -n $ARG ];
	then
		case "$ARG" in
			1|2|3|4|5|6|7|q)
			   echo "Input : $1"
			   GEEK_TYPE=$ARG
			   ;;
			q) break;;
		esac
	fi
	if [ -z $GEEK_TYPE ]; then
		echo "Selections :"
		echo "	1. set rootfs path.(now: $ROOTFS_PATH)"
		echo "	2. clean & make kernel."
		echo " 	3. make kernel."
		echo "	4. install images to rootfs."
		echo "	5. install modules to rootfs."
		echo "	6. copy images to tftpboot."
		echo "	7. install images to board.(uname: $UNAME)"
		echo "	q. quit"
	fi

	INPUT_PATH=	
	while [ -z $GEEK_TYPE ];
	do
		echo -n "Which? "
		read ANSWER
		case $ANSWER in
			1)
				echo -n "Enter path : "
				read INPUT_PATH
				if [[ $INPUT_PATH != "" ]]; then # ignore Return key	
					ROOTFS_PATH=$INPUT_PATH
				fi
				echo "Rootfs path: $ROOTFS_PATH"
				break;;
			2|3|4|5|6)
				GEEK_TYPE=$ANSWER
				break;;
			7) 
				echo -n "Board IP : "
				read INPUT_IP
				if [[ $INPUT_IP != "" ]]; then	
			   	    BOARD_IP=$INPUT_IP
				    GEEK_TYPE=$ANSWER
				fi
				break;;
			q)
				break;;
		esac
	done
	
	echo ""

	do_geek
}
