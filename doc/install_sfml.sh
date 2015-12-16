#!/bin/bash

echo "Please enter the directory in which SFML is to be installed."
echo "(Recommended : /usr or /usr/local)"
read sfml_prefix
sudo_maybe=sudo 
if [[ ${sfml_prefix:0:1} != / ]]; then 
    sfml_prefix=$(pwd)/${sfml_prefix};
    sudo_maybe= 
fi
${sudo_maybe} mkdir -p ${sfml_prefix}/share;
${sudo_maybe} mkdir -p ${sfml_prefix}/include;
${sudo_maybe} mkdir -p ${sfml_prefix}/lib;

sudo apt-get update -y;
sudo apt-get install -y \
    coreutils \
    libc-bin \
    wget \
    tar \
    gzip \
    bzip2 \
    libc6 \
    libx11-data \
    libx11-6 \
    libxcb1 \
    libx11-xcb1 \
    libxcb-glx0 \
    libxcb-randr0 \
    libxcb-image0 \
    libxrandr2 \
    libudev1 \
    libfreetype6 \
    libjpeg8 \
    libflac8 \
    libopenal-data \
    libopenal1 \
    libsndfile1;

function install_sfml {
    wget http://www.sfml-dev.org/files/SFML-2.3.2-linux-gcc-$1-bit.tar.gz;
    gzip -v -d SFML-2.3.2-linux-gcc-$1-bit.tar.gz;
    tar xvf SFML-2.3.2-linux-gcc-$1-bit.tar;
    rm -v SFML-2.3.2-linux-gcc-$1-bit.tar;
    cd SFML-2.3.2;
    ${sudo_maybe} mv -v share/SFML ${sfml_prefix}/share;
    ${sudo_maybe} cp -v -R include/SFML ${sfml_prefix}/include;
    ${sudo_maybe} mv -v lib/libsfml-*.so.2.3.2 ${sfml_prefix}/lib;
    old_dir=$(pwd);
    cd ${sfml_prefix}/lib;
    for i in audio graphics network system window; do
        ${sudo_maybe} ln -v -s libsfml-$i.so.2.3.2 libsfml-$i.so.2.3;
        ${sudo_maybe} ln -v -s libsfml-$i.so.2.3 libsfml-$i.so;
    done
    cd ${old_dir}/..;
    rm -vrf SFML-2.3.2;
}
function install_csfml {
    wget http://www.sfml-dev.org/files/CSFML-2.3-linux-gcc-$1-bit.tar.bz2;
    bzip2 -v -d CSFML-2.3-linux-gcc-$1-bit.tar.bz2;
    tar xvf CSFML-2.3-linux-gcc-$1-bit.tar;
    rm -v CSFML-2.3-linux-gcc-$1-bit.tar;
    cd CSFML-2.3;
    ${sudo_maybe} mkdir -p ${sfml_prefix}/share/CSFML;
    ${sudo_maybe} mv -v doc ${sfml_prefix}/share/CSFML;
    ${sudo_maybe} cp -v -R include/SFML ${sfml_prefix}/include;
    ${sudo_maybe} mv -v lib/libcsfml-*.so.2.3.0 ${sfml_prefix}/lib;
    old_dir=$(pwd);
    cd ${sfml_prefix}/lib;
    for i in audio graphics network system window; do
        ${sudo_maybe} ln -v -s libcsfml-$i.so.2.3.0 libcsfml-$i.so.2.3;
        ${sudo_maybe} ln -v -s libcsfml-$i.so.2.3 libcsfml-$i.so;
    done
    cd ${old_dir}/..;
    rm -vrf CSFML-2.3;
}

arch=$(arch);
if [[ ${arch:0:1} == x ]]; then
    bits=64
else
    bits=32
fi

echo "Now installing SFML for $arch ($bits bits).";
install_sfml  $bits
install_csfml $bits
sudo ldconfig -v;

echo;
echo "You should now be able to run the executables.";
echo "To list the shared libraries they depend on, you can run :";
echo -e "\tldd <executable>";
echo "Additionally, you can ensure you have an OpenGL driver by running :";
echo -e "\tdpkg -l *libgl1*";

