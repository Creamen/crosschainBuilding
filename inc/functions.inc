# Functions

function showHelp() {
	return 0
}

function cleanMess() {
	return 0
}

function cmdCheck() {
	which "$1" > /dev/null 2>&1 
	if [[ $? -ne 0 ]]; then
		echo "[FATAL] : You *must* install $1 $([ ! -z "$2" ] && echo ">= $2" ) to help me"
		exit 1
	fi
}	

function initCheck() {
	cmdCheck wget 1.16
	cmdCheck gcc
	cmdCheck g++
	cmdCheck make
	cmdCheck gawk
	cmdCheck install
	cmdCheck perl
}

function dlFile() {
	_f=${1##*/}
	if [ ! -f ../dl/${_f} ]; then 
		wget -qN --show-progress "$1" -O ../dl/${_f}
		if [[ $? -ne 0 ]]; then
			echo "[FATAL] $1 not found - ABORT"
			exit 1
		fi
	else
		echo -e "\t${_f} ... has been already dowloaded"
	fi
	ln -sf ../dl/${_f} ${_f}
}

function testRegEx() {
	grep -Eq "$2" <<< "$1"
	return $?
}

function initContext() {
	# MULTIARCH_NAME is defined as said in these links :
	# https://wiki.debian.org/Multiarch/Tuples#Used_solution
	# http://wiki.osdev.org/Target_Triplet
	#
	# So : machine-vendor-operatingsystem
	# We don't handle the case where the optional vendor word is not present
	# so we enforce it like Debian
	testRegEx "$1" "^([^-]+-){2}[^-]+$" 
	if [[ $? -ne 0 ]]; then
		echo "[FATAL] Given target architecture \"$1\" is not valid. - ABORT"
		exit 1
	fi

	MULTIARCH_NAME="$1"
	KERNEL_ARCH=${linuxKernelCodename[${MULTIARCH_NAME}]}
	if [ -z "${MULTIARCH_NAME}" -o -z "${KERNEL_ARCH}" ]; then 
		echo "[FATAL] You *must* provide me a valid target architecture. \"${MULTIARCH_NAME}\" is not - ABORT"
		exit 1
	fi
	TARGET=${MULTIARCH_NAME}
	ts_log=$(date +%Y%m%d%H%m%S)
}

function initPath() {
	#PREFIX=$HOME/cross/${MULTIARCH_NAME%%-*}/$MULTIARCH_NAME
	PREFIX=$HOME/cross/${MULTIARCH_NAME}
	mkdir -p ${PREFIX} ${MULTIARCH_NAME}/{build-binutils,build-gcc,build-glibc} dl
	export PATH=${HOME}/cross/bin:${PATH}
	cd ${MULTIARCH_NAME}
}

function initArchives() {
	echo "Download archive source files ..."
	for c in ${compoments[list]}; do
		url="${compoments[$c.url]}/$c-${compoments[$c.ver]}.tar.${compoments[$c.ext]}"
		unset _r
		
		# Substitution in URL of kind [subkey]
		_r=$(grep -oE "\[[a-z0-9]+:[0-9]+:[0-9]+\]" <<< "${url}")
		[ ! -z "$_r" ] && \
		url=$(
			eval echo $(echo $_r	\
			| sed -r "s,\[([^:]+):([^:]+):([^]]+)\],\$( echo \${compoments[$c.\1]:\2:\3} )," <<< "${url}")	\
		)
		# Substitution in URL of kind [subkey:offset:length]
		_r=$(grep -oE "\[[a-z0-9]+\]" <<< "$url")
		[ ! -z "$_r" ] && \
		url=$(	\
			eval echo $(echo $_r	\
			| sed -r "s,\[([^:]+)\],\$( echo \${compoments[$c.\1]} )," <<< "${url}")	\
		)
		dlFile ${url}
	done
	echo -e "Done !\n"
}

function unpackArchives() {
	echo "Unarchive source files ..."
	for f in *.tar.*; do
		echo -ne "\t$f ... "
		if [ ! -d ${f%%.tar*} ]; then
			tar -x -${tarComp[.${f#*tar.}]} -f $f
			echo "OK"
		else
			echo "Skipping because already unpacked !";
		fi
	done
	echo "Done !"
}

