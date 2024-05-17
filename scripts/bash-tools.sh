#!/usr/bin/env bash
#!/bin/bash

# \brief High Intensity Text Foreground Colors
#
# \Example
#
#	echo -e "${i_yellow}Hello $USER!${i_def}"
#
i_black='\033[0;90m'
i_red='\033[0;91m'
i_green='\033[0;92m'
i_yellow='\033[0;93m'
i_blue='\033[0;94m'
i_purple='\033[0;95m'
i_cyan='\033[0;96m'
i_white='\033[0;97m'
i_default='\033[0m' # Resets the text foreground color to the default shell color.
i_def='\033[0m'

# TODO: Add some background color support.

# The following in bash shells is not as portable as
# $OSTYPE, although most modern up-to-date will have
# uname available.
#
kernel_name="$(uname -s)"
operating_system="$(uname)"
arch_type="$(uname -m)"

# TODO: This following is a bi more complicated. Do this later since it would
#       be  nice to have.
#os_version=TODO

# Check if the OS is Windows 64-bit
if [[ "$OSTYPE" == msys ]]; then
    WIN64=true
else
    WIN64=false
fi
WIN32=$WIN64

# Check if the OS is Linux
if [[ "$OSTYPE" == solaris* ]]; then
    LINUX=true
else
    LINUX=false
fi

# Check if the OS is macOS
if [[ "$OSTYPE" == darwin* ]]; then
    APPLE=true
else
    APPLE=false
fi

# Check if the OS is BSD
if [[ "$OSTYPE" == bsd* ]]; then
    BSD=true
else
    BSD=false
fi

UBUNTU=false
# [ -f /etc/os-release ] && grep -q "ubuntu" /etc/os-release;
# if [ -f /etc/os-release ] && grep -q "ubuntu 22.04" /etc/os-release; 
if [ -f /etc/os-release ] && grep -q "ubuntu" /etc/os-release; 
then
   UBUNTU=true
fi

FEDORA=false
if [ -f /etc/fedora-release ]; 
then
   FEDORA=true
fi

function is_x86_64()
{
   if [[ "x86_64" == ${arch_type} ]];
   then
      return 0
   fi
}

function is_arm_64()
{
   if [[ "aarch64" == ${arch_type} ]];
   then
      return 0
   fi
}

# if is_sourced;
# then
#   Script is being Sourced
# else
#   Script is being Executed
# fi
#
function is_sourced()
{
  if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
      return 0
  else
      return 1
  fi
}

# if os_windows;
# then
#     . . .
# fi
function os_windows()
{
   if [ true == $WIN64 ];
   then
      return 0 # in bash a return of 0 is true.
   fi
   return 1
}

function os_linux()
{
   if [ true == $LINUX ];
   then
      return 0
   fi
   return 1
}

function os_apple()
{
   if [ true == $APPLE ];
   then
      return 0
   fi
   return 1
}

function os_fedora()
{
   if [ true == $FEDORA ];
   then
      return 0
   fi
   return 1
}

function os_ubuntu()
{
   if [ true == $UBUNTU ];
   then
      return 0
   fi
   return 1
}

##
# \brief Checks if the current user has administrator or root privileges.
#
# \Example
#   if has_admin_or_root;
#   then
#       . . .
#   fi
#
# \return Returns 0 if the current user has administrator or root privileges,
#         and 1 otherwise.
#
# This function checks if the script is running with administrator or root privileges.
# On Windows, it uses 'net session' to check for administrator privileges.
# On Unix/macOS systems, it checks if the effective user ID is 0 (root) or if sudo can
# be executed without a password prompt.
##
function has_admin_or_root()
{
    if [ true == $WIN64 ];
    then
        if [[ $(net session 2>&1) == *"Access is denied." ]];
        then
            return 1
        else
            return 0 # A return of zero in bash is 'true'.
        fi
    else
        #
        # TODO: Test on Linux and macOS
        #
        [[ $EUID -eq 0 || $(sudo -n true 2>/dev/null) ]] && return 0 || return 1
    fi
}

##
# Fetches a package from a Git repository, either by cloning it or updating an existing clone.
#
# @param target_folder 	 The name of the target folder to store the package.
# @param url           	 The URL of the Git repository.
# @param branch_or_tag 	 The branch or tag to checkout after cloning the repository.
# @param clone_recursive Set "true" to clone --recursive.
#
# This function checks if the source folder exists. If it does, it updates it 
# with the latest changes from the repository.
#
# If it doesn't exist, it clones the repository using the specified branch or tag.
##
function fetch_package()
{
	target_folder=$1
	url=$2
	branch_or_tag=$3
	clone_recursive=$4
	
	# This filters out the path and gives us the deepest subfolder name.
	#
	folder_name=$(basename "${target_folder}")

    if [[ -d "${target_folder}" ]];
    then
		echo -e "Found Package ${folder_name}."
		cd "${target_folder}" && git pull	
    else
		if [[ "true" == "${clone_recursive}" ]];
		then
			echo -e "${i_cyan}git clone ${folder_name} --recursive . . .${i_def}"
			sleep 1
			git clone --recursive -b "${branch_or_tag}" "${url}" "${target_folder}"
		else
			git clone -b "${branch_or_tag}" "${url}" "${target_folder}"
		fi
    fi
}

# 'remove_token' 
#
#  Example:
# 
#  matplot_src="$(pwd)/${CMAKE_BINARY_DIR}/_deps/matplotplusplus-src"
#  matplot_build="$(pwd)/${CMAKE_BINARY_DIR}/_deps/matplotplusplus-build"
#
#  matplot_src=$(remove_token "${matplot_src}" "scripts")
#  matplot_build=$(remove_token "${matplot_build}" "scripts")
#
remove_token() 
{
    local string="$1"
    local token="$2"
    if [[ $string == *"$token"* ]]; 
    then
        string="${string//$token/}"
    fi
    echo ${string}
 }

if [[ -e "./config.sh" ]];
then
	. "./config.sh"
	
else
	if [[ -e "./scripts/config.sh" ]];
	then
		. "./scripts/config.sh"
	
	else
      echo -e "\n${i_yellow}Warning${i_def} - Missing Configuration Source '${i_red}./scripts/config.sh${i_def}'\n"
      
	fi
fi
