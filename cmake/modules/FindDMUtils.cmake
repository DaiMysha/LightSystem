# This script locates the UTILS library.
#
# USAGE
# find_package( DMUTILS )
#
# If DMUTILS is not installed in a standard path, you can use
# DMUTILS_ROOT CMake (or environment) variables to tell CMake where to look for
# DMUTILS.
#
#
# OUTPUT
#
# This script defines the following variables:
#   - DMUTILS_FOUND:           true if the DMUTILS library is found
#   - DMUTILS_INCLUDE_DIR:     the path where DMUTILS headers are located
#
#
# EXAMPLE
#
# find_package( DMUTILS REQUIRED )
# include_directories( ${DMUTILS_INCLUDE_DIR} )
# add_executable( myapp ... )
# target_link_libraries( myapp ${DMUTILS_LIBRARY} ... )

set( DMUTILS_FOUND false )

find_path(
	DMUTILS_INCLUDE_DIR
	DMUTILS/FixedSizeList.hpp
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
		${DMUTILSDIR}
		${DMUTILS_ROOT}
		$ENV{DMUTILS_ROOT}
		$ENV{DMUTILSDIR}
)

# find_library(
	# DMUTILS_LIBRARY
	# DMUTILS
	# PATH_SUFFIXES
		# lib
		# lib64
	# PATHS
		# /usr
		# /usr/local
		# ${DMUTILSDIR}
		# ${DMUTILS_ROOT}
		# $ENV{DMUTILS_ROOT}
		# $ENV{DMUTILSDIR}
# )

#if( NOT DMUTILS_INCLUDE_DIR OR NOT DMUTILS_LIBRARY )
if( NOT DMUTILS_INCLUDE_DIR )
	message( FATAL_ERROR "DMUtils not found. Set DMUTILS_ROOT to the installation root directory (containing inculude/ and lib/)" )
else()
	message( STATUS "DMUtils found: ${DMUTILS_INCLUDE_DIR}" )
endif()
