# configuration stuff

declare -A linuxKernelCodename=(
	[arm-linux-gnueabi]="arm"
	[aarch64-linux-gnu]="arm64"
	[i386-linux-gnu]="i386"
	[x86_64-linux-gnu]="x86"
)

declare -A tarComp=(
	[.xz]="J" [.txz]="J"
	[.bz2]="j" [.tbz]="j"
	[.gz]="z" [.tgz]="z"
)

# Let's define an associative array with this format :
# [list]="prg1 prg2 ... prgn" : the list of archive basename without version
# 	With following informations :
# 	[prg.ver]="xxx" where xxx is the actual version you are expecting
#	[prg.ext]="xz|bz2|gz" to force a certain compression scheme
#	[prg.url]="location" where location is the final URL without the leaf name.
#		eg. If final URL is https://ftp.gnu.org/gnu/gcc/gcc-2.95.tar.xz then prg.url will be https://ftp.gnu.org/gnu/gcc
declare -A compoments=(
	[list]="binutils gcc glibc gmp mpfr mpc isl cloog linux"

	[binutils.ver]="2.29.1"
	[binutils.url]="http://ftp.wayne.edu/gnu/binutils"
	[binutils.ext]="xz"
#
	[gcc.ver]="7.2.0"
	[gcc.url]="http://ftp.wayne.edu/gnu/gcc/gcc-[ver]"
	[gcc.ext]="xz"
#
	[glibc.ver]="2.26"
	[glibc.url]="http://ftp.wayne.edu/gnu/glibc"
	[glibc.ext]="xz"
#
	[gmp.ver]="6.1.2"
	[gmp.url]="http://ftp.wayne.edu/gnu/gmp"
	[gmp.ext]="xz"
#
	[mpfr.ver]="4.0.0"
	[mpfr.url]="http://ftp.wayne.edu/gnu/mpfr"
	[mpfr.ext]="xz"
#
	[mpc.ver]="1.1.0"
	[mpc.url]="http://ftp.wayne.edu/gnu/mpc"
	[mpc.ext]="gz"
#
	[isl.ver]="0.18"
	[isl.url]="ftp://gcc.gnu.org/pub/gcc/infrastructure"
	[isl.ext]="bz2"
#
	[cloog.ver]="0.18.1"
	[cloog.url]="ftp://gcc.gnu.org/pub/gcc/infrastructure"
	[cloog.ext]="gz"
#
	[linux.ver]="4.1.19"
	[linux.url]="https://www.kernel.org/pub/linux/kernel/v[ver:0:1].x"
	[linux.ext]="xz"
)


