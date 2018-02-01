# Functions

function lockBuild() {
	if [ -f "${LOCKFILE}" ]; then
		printf "[FATAL] Lock [%s] present - Build already in progress or previous unclean exit.\n" "${LOCKFILE}"
		exit 0;
	fi
	touch ${LOCKFILE}
	trap "rm -f ${LOCKFILE}" EXIT SIGINT SIGTERM
}

function dlPackages() {
	for k in ${packages[list]}; do
		lastURL=$(listPackageURL $k)
		dlFile "${lastURL}"
	done
}


function showHelp() {
	return 0
}

function cleanMess() {
	return 0
}

function cmdCheck() {
	which "$1" > /dev/null 2>&1 
	if [[ $? -ne 0 ]]; then
		printf "[FATAL] : You *must* install %s %s to help me" "$1" "$([ ! -z "$2" ] && echo ">= $2" )"
		exit 1
	fi
}	

function initCheck() {
	cmdCheck wget 1.16
	cmdCheck lftp
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
			printf "[FATAL] %s not found - ABORT\n" "$1"
			exit 1
		fi
	else
		printf "\t${_f} ... has been already dowloaded\n"
	fi
	[ "$2" != "nolink" ] && ln -sf ../dl/${_f} ${_f}
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
		printf "[FATAL] Given target architecture \"%s\" is not valid. - ABORT\n" "$1"
		exit 1
	fi

	MULTIARCH_NAME="$1"
	KERNEL_ARCH=${linuxKernelCodename[${MULTIARCH_NAME}]}
	if [ -z "${MULTIARCH_NAME}" -o -z "${KERNEL_ARCH}" ]; then 
		printf "[FATAL] You *must* provide me a valid target architecture. \"%s\" is not - ABORT\n" "${MULTIARCH_NAME}"
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

function listPackageURL() {
	# 1 : package name
	if [ ! -z ${packages[$1]} ]; then
		echo ${packages[$1]}
	else
		echo ${packages[$1.url]}/$(LC_ALL=C \
			&& lftp -c "open ${packages[$1.url]}/ && ls" --norc \
			| grep -oE "$1-[0-9.]+.+\.${packages[$1.ext]}" \
			| sort -t. -k1,1 -k2n,2n -k2n,3n | tail -1
		)
	fi
}

function initArchives() {
	printf "Download archive source files ...\n"
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
	printf "Done !\n"
}

function unpackArchives() {
	printf "Unarchive source files ...\n"
	for f in *.tar.*; do
		printf "\t$f ... "
		if [ ! -d ${f%%.tar*} ]; then
			tar -x -${tarComp[.${f#*tar.}]} -f $f
			printf "OK\n"
		else
			printf "Skipping because already unpacked !\n"
		fi
	done
	printf "Done !\n"
}

