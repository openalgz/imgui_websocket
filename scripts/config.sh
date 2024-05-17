#!/usr/bin/env bash
#!/bin/bash

CMAKE_BINARY_DIR="build"
DEFAULT_BUILD_TARGET="Debug"
DEFAULT_CMAKE_GENERATOR="Unix Makefiles"

#  Modify this file to control where third-party libraries are
#  installed. The convention here is to try not to require admin/root
#  when installing these for developer environments.
#
#  It is helpful to avoid the need for admin/root privileges when 
#  installing third-party dependencies. This makes  asier integration 
#  into various projects without imposing system-wide changes. 
#  
#  Cloning these paths to 'official' install locations require 
#  admin/root access. Therefore, "*$HOME/opt*" and "*$USERPROFILE/opt*"
#  are used for dependencies that may be shared across different projects. 
#  Larger libraries such as Eigen, Boost, FFTW, POCO, etc. are recommended 
#  to be installed in a location that does not require root/admin privileges
#  as well.
#  
#  In summary the reasons for this approach are:
#  
#  1. Project environments may be cloned and generated in CMake without 
#     root/admin access.
#  2. This enables developers to have more control over the installation
#     and configuration of the libraries they need for their projects, as
#     opposed to having the packages managed by the operating system's package
#     manager.
#  3. Not requiring root/admin privileges makes updating packages easier and 
#     allows for changes in configurations for testing.
#  
#  For additional information on this topic refer CMake's documentation on this topic:
#  [CMAKE_SYSTEM_PREFIX_PATH](https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM_PREFIX_PATH.html) 
#
SHARED_PACKAGE_INSTALL_DIR="SHARED_PACKAGE_INSTALL_DIR is undefined. See 'scripts/config.sh' to set this variable."

if os_windows;
then
   # 
   # SHARED_PACKAGE_INSTALL_DIR="$CommonProgramFiles"
   #
   user_profile="$USERPROFILE"
   SHARED_PACKAGE_INSTALL_DIR="${user_profile//\\//}/opt"
   DEFAULT_CMAKE_GENERATOR="" # chooses system default

elif os_apple;
then
    # "/usr/opt"  -> requires root/admin
    # Therefore:
    #
    SHARED_PACKAGE_INSTALL_DIR="$HOME/opt"
    DEFAULT_CMAKE_GENERATOR="Xcode"
   
elif os_linux;
then
    # "/usr/opt"  -> requires root/admin
    # Therefore:
    #
    SHARED_PACKAGE_INSTALL_DIR="$HOME/opt"

else
    #
    # Attempt to use $HOME...
    #
    SHARED_PACKAGE_INSTALL_DIR="$HOME/opt"

fi

mkdir -p "${SHARED_PACKAGE_INSTALL_DIR}"

# Ensure Defined
#
if [[ ! -d "${SHARED_PACKAGE_INSTALL_DIR}" ]];
then
   echo -e "${i_red}Error:${i_def} Directory 'SHARED_PACKAGE_INSTALL_DIR' is required but was not found: '${SHARED_PACKAGE_INSTALL_DIR}'"
   echo -e "       ${i_white}See './scrpts/config.sh' to set this variable.${i_def} "
   set -e
   exit 1
fi

echo -e "\n${i_yellow}Default CMake Build and Install Manifest:${i_def}"
echo -e "  CMAKE_BINARY_DIR: '${CMAKE_BINARY_DIR}'."
echo -e "  SHARED_PACKAGE_INSTALL_DIR: '${SHARED_PACKAGE_INSTALL_DIR}'.\n"
sleep 2
