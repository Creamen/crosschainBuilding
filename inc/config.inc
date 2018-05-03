#!/bin/bash

# vim: set expandtab:
# vim: set noai ts=4 sw=4:

# configuration stuff

LOCKFILE=$(pwd)/.lock

declare -A linuxKernelCodename=(
    [arm-linux-gnueabi]="arm"
    [arm-linux-gnueabihf]="arm"
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
#     With following informations :
#     [prg.ver]="xxx" where xxx is the actual version you are expecting
#    [prg.ext]="xz|bz2|gz" to force a certain compression scheme
#    [prg.url]="location" where location is the final URL without the leaf name.
#        eg. If final URL is https://ftp.gnu.org/gnu/gcc/gcc-2.95.tar.xz then prg.url will be https://ftp.gnu.org/gnu/gcc
declare -A compoments=(
    [list]="binutils gcc glibc gmp mpfr mpc isl cloog linux"

    [binutils.ver]="2.30"
    [binutils.url]="https://ftp.gnu.org/gnu/binutils"
    [binutils.ext]="xz"
#
    [gcc.ver]="8.1.0"
    [gcc.url]="https://ftp.gnu.org/gnu/gcc/gcc-[ver]"
    [gcc.ext]="xz"
#
    [glibc.ver]="2.27"
    [glibc.url]="https://ftp.gnu.org/gnu/glibc"
    [glibc.ext]="xz"
#
    [gmp.ver]="6.1.2"
    [gmp.url]="https://ftp.gnu.org/gnu/gmp"
    [gmp.ext]="xz"
#
    [mpfr.ver]="4.0.1"
    [mpfr.url]="https://ftp.gnu.org/gnu/mpfr"
    [mpfr.ext]="xz"
#
    [mpc.ver]="1.1.0"
    [mpc.url]="https://ftp.gnu.org/gnu/mpc"
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
    [linux.ver]="4.15.17"
    [linux.url]="https://www.kernel.org/pub/linux/kernel/v[ver:0:1].x"
    [linux.ext]="xz"
)

declare -A packages=(
    [list]="libffi pcre util-linux glib libunistring pkgconfig nettle gmp guile zlib git libevent libtool tmux openssh openssl libressl gnutls bash readline gettext mc make wget coreutils bison bc gawk ncurses tar diffutils findutils gdb"

    [libevent]="https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz"
    [tmux]="https://github.com/tmux/tmux/releases/download/2.6/tmux-2.6.tar.gz"
    [zlib]="https://zlib.net/zlib-1.2.11.tar.xz"
    [pkgconfig]="https://pkg-config.freedesktop.org/releases/pkg-config-0.29.2.tar.gz"
    [util-linux]="https://www.kernel.org/pub/linux/utils/util-linux/v2.31/util-linux-2.31.tar.xz"
    [glib]="https://ftp.gnome.org/pub/gnome/sources/glib/2.55/glib-2.55.1.tar.xz"

    [libffi.url]="ftp://sourceware.org/pub/libffi"
    [libffi.ext]="gz"
    [gdb.url]="https://ftp.gnu.org/gnu/gdb"
    [gdb.ext]="xz"
    [git.url]="https://www.kernel.org/pub/software/scm/git"
    [git.ext]="xz"
    [pcre.url]="https://ftp.pcre.org/pub/pcre"
    [pcre.ext]="bz2"
    [openssh.url]="https://cloudflare.cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable"
    [openssh.ext]="tar.gz"
    [openssl.url]="https://artfiles.org/openssl.org/source"
    [openssl.ext]="gz"
    [libunistring.url]="https://ftp.gnu.org/gnu/libunistring"
    [libunistring.ext]="xz"
    [libtool.url]="https://ftp.gnu.org/gnu/libtool"
    [libtool.ext]="xz"
    [gmp.url]="https://ftp.gnu.org/gnu/gmp"
    [gmp.ext]="xz"
    [guile.url]="https://ftp.gnu.org/gnu/guile"
    [guile.ext]="xz"
    [nettle.url]="https://ftp.gnu.org/gnu/nettle"
    [nettle.ext]="gz"
    [bash.url]="https://ftp.gnu.org/gnu/bash"
    [bash.ext]="gz"
    [gnutls.url]="https://ftp.gnu.org/gnu/gnutls"
    [gnutls.ext]="xz"
    [ncurses.url]="https://ftp.gnu.org/gnu/ncurses"
    [ncurses.ext]="gz"
    [tar.url]="https://ftp.gnu.org/gnu/tar"
    [tar.ext]="xz"
    [diffutils.url]="https://ftp.gnu.org/gnu/diffutils"
    [diffutils.ext]="xz"
    [findutils.url]="https://ftp.gnu.org/gnu/findutils"
    [findutils.ext]="gz"
    [bison.url]="https://ftp.gnu.org/gnu/bison"
    [bison.ext]="xz"
    [bc.url]="https://ftp.gnu.org/gnu/bc"
    [bc.ext]="gz"
    [gettext.url]="https://ftp.gnu.org/gnu/gettext"
    [gettext.ext]="xz"
    [gawk.url]="https://ftp.gnu.org/gnu/gawk"
    [gawk.ext]="xz"
    [mc.url]="https://ftp.gnu.org/gnu/mc"
    [mc.ext]="gz"
    [coreutils.url]="https://ftp.gnu.org/gnu/coreutils"
    [coreutils.ext]="xz"
    [make.url]="https://ftp.gnu.org/gnu/make"
    [make.ext]="bz2"
    [wget.url]="https://ftp.gnu.org/gnu/wget"
    [wget.ext]="xz"
    [readline.url]="https://ftp.gnu.org/gnu/readline"
    [readline.ext]="gz"
    [libressl.url]="https://ftp.openbsd.org/pub/OpenBSD/LibreSSL"
    [libressl.ext]="gz"
)

