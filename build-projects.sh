#!/usr/bin/env bash

if [[ -e "bash-tools.sh" ]];
then
	. "bash-tools.sh"
	
else
	if [[ -e "./scripts/bash-tools.sh" ]];
	then
		. "./scripts/bash-tools.sh"
	
	else
		echo -e "\nUnable to find: './scripts/bash-tools.sh library.'\n"
		#
		# The following stops execution if the last command in the pipeline had an error. 
		# Since the bash-tools.sh library was not found the script most likely cannot 
		# continue without errors so the script process is aborted.
		#
		set -e
		exit 1
	fi
fi

# Default options are:
build_dir="${CMAKE_BINARY_DIR}"
config_option="${DEFAULT_BUILD_TARGET}"
cmake_generator="${DEFAULT_CMAKE_GENERATOR}"
parallel_jobs=2
target_name="imgui_websockets"

echo parallel_jobs

# Function to show the script usage.
show_usage() {
  echo -e "\n${i_yellow}Example Usage:${i_purple} $0 ${i_white}{Optional Args: ${i_purple}-c -f ${i_white}{config}${i_purple} -t ${i_white}{target_name}${i_white}}${i_def}" \
          "\n  -h or --help                    ${i_green}# Display help documentation.${i_def}" \
          "\n  -c or --clean                   ${i_green}# Removes the '${build_dir}' directory${i_def}" \
          "\n  -f or --config {config_option}  ${i_green}# Use to define Debug, Release, MinSizeRelease, etc.${i_def}"\
          "\n  -t or --target {target_name}    ${i_green}# Defines the project name to build (default:${target_name}).${i_def}\n"\
          "\n  -j or --j {n}                   ${i_green}# Specifies the number of parallel jobs to be used when building the\n"\
          "                                 ${i_green}# project (defaults to 0 which directs cmake to use all jobs avaliable).${i_def}\n"    
          
  echo -e "${i_yellow}Default Options:${i_def}\n  --config ${config_option}\n  --target ${target_name}\n  --cmake_generator ${cmake_generator}\n  -j ${parallel_jobs}"
}

# Parse the command-line arguments
#
while [[ $# -gt 0 ]]; do
  case $1 in
    -c|--clean)
      rm -rf "${build_dir}"
      ;;
    -f|--config)
      config_option=$2
      shift
      ;;
    -t|--target)
      target_name=$2
      shift
      ;;
    -j|--parallel)
      parallel_jobs=$2
      shift
      ;;
    *)
      echo "Invalid option: $1" >&2
      clear
      show_usage
      exit 1
      ;;
  esac
  shift
done


# If no clean argument is provided or an invalid argument is provided, continue with the build process
show_usage
sleep 2

if [[ ! -d ${build_dir} ]];
then
   mkdir -p "${PWD}/${build_dir}"
fi

cd "${PWD}/${build_dir}"

if os_windows;
then
   cmake ..
else
   cmake -DCMAKE_BUILD_TYPE="${config_option}" -G "${cmake_generator}" ..
fi

cmake --build . --config "${config_option}" -j "${parallel_jobs}"
