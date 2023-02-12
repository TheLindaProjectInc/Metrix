#!/usr/bin/env bash

dirName="bdb48"
fileDesc="Berkley DB version 4.8.30"

fileUrl="http://download.oracle.com/berkeley-db/"
fileName="db-4.8.30"
fileExt=".zip"
filePath="${fileUrl}${fileName}${fileExt}"

# Patch lock for runonce
atomicPatch="atomic_patch.ack.lock"
# Script completed runonce
scriptRan="script_built.ack.lock"

# Archive unpacking package
libZip="unzip"

version="0.1"

#############################
#DO NOT EDIT BELOW THIS LINE#
#############################

#Colors
nc='\033[0m'
black='\033[0;30m'
dark_gray='\033[1;30m'
red='\033[1;31m'
dark_red='\033[0;31m'
green='\033[1;32m'
dark_green='\033[0;32m'
gold='\033[0;33m'
yellow='\033[1;33m'
blue='\033[0;34m'
light_blue='\033[1;34m'
purple='\033[0;35m'
magenta='\033[1;35m'
cyan='\033[0;36m'
light_cyan='\033[1;36m'
gray='\033[0;37m'
white='\033[1;37m'

# Console message functions
function msg {
    echo -e "${1}${nc}"
}
function msgc {
    echo -e "${2}${1}${nc}"
}

#############################

msgc "=======================================" $dark_gray
msgc "> Installing ${fileDesc}" $green
msgc ">> Using Script v${version}" $gold
sleep 3
cd ~

# Check if run already..
if [ -d "${dirName}/${fileName}" ] && [ -f "${dirName}/${fileName}/${scriptRan}" ]; then
  msgc ">> Already built and installed ${fileDesc} ..." $red
  msgc ">> Please Delete .ack.lock file to run again if needed" $yellow
  sleep 3
  exit 0
fi

#msgc "> Package 'unzip' Check..." $gold
if [ $(dpkg-query -W -f='${Status}' $libZip 2>/dev/null | grep -c "ok installed") -eq 0 ]; then
  msgc "> Package '$libZip' is Required!" $yellow
  msgc ">> Attempting to install it..." $gold
  sudo apt-get update
  sudo apt-get install -y $libZip
  sleep 1
fi

# Make dir
if [ ! -d ${dirName} ]; then
  mkdir ${dirName}
  msgc ">> Created build directory..." $dark_green
fi

cd ${dirName}

# Get file and unzip
if [ ! -d ${fileName} ]; then
  msgc "> Fetching ${fileDesc} ..." $cyan
  wget ${filePath}
  sleep 1
  msgc "> UnZipping ${fileDesc} ..." $cyan
  unzip ${fileName}${fileExt}
fi

cd ${fileName}
sleep 3

# Check app for patches, apply
if [ ! -f "${atomicPatch}" ] && [ -d "dbinc/" ]; then
  #Patch for compile issue..
  msgc ">> Patching 'atomic.h' ..." $gold
  sed -i 's/__atomic_compare_exchange/__atomic_compare_exchange_db/g' dbinc/atomic.h
  touch ${atomicPatch}
  msgc ">> Patching 'atomic.h' complete." $green
  sleep 2
fi
if [ -f ${atomicPatch} ]; then
  msgc "> Patch for 'atomic.h' was applied with success!" $cyan
  sleep 1
fi

# Build project..
cd build_unix/
msgc ">> Configuring Build for ${fileDesc} ..." $magenta
../dist/configure --prefix=/usr/local --enable-cxx
sleep 1

msgc "> Building ${fileDesc} ..." $magenta
make
sleep 1

# Install
msgc "> Installing ${fileDesc} ..." $magenta
sudo make install

msgc "=======================================" $dark_gray
msgc ">> Install of ${fileDesc} Completed! <<" $green
cd .. && touch ${scriptRan}
sleep 1

exit 0