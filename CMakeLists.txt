cmake_minimum_required (VERSION 2.8.8)

# define a macro that helps defining an option

# project name
set(PROJECT_NAME "LightSystem")
project (${PROJECT_NAME})

# setup version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

include(cmake/macro.cmake)



# add the  header path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)

#add an option for building the examples
set_option(LIGHTSYSTEM_BUILD_EXAMPLES TRUE BOOL "TRUE to build the examples, FALSE to ignore them")

# add an option for building the API documentation
# set_option(LIGHTSYSTEM_BUILD_DOC TRUE BOOL "TRUE to generate the API documentation, FALSE to ignore it")

set(LIBS "")

set(DEFINES "")
set(WARNING "-Wall")
set(FLAGS "-g -std=c++11")
set(CMAKE_CXX_FLAGS "${DEFINES} ${WARNING} ${FLAGS}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LIB_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_SHARED_LIBRARY_PREFIX )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/../SFML PATH "The SFML root installation")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/SFML PATH "The SFML root installation")
else()
	set_option(SFML_ROOT ${CMAKE_INSTALL_PREFIX}/SFML PATH "The SFML root installation")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules;${CMAKE_SOURCE_DIR}")

find_package(SFML 2 COMPONENTS system graphics window REQUIRED)
if(NOT SFML_FOUND)
	message(FATAL_ERROR "SFML Modules not found. Please set the SFML_ROOT variable to your SFML root installation directory and retry")
endif()


include_directories(${SFML_INCLUDE_DIR})
list(APPEND LIBS
	${LIBS}
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

find_package(DMUtils)
if(NOT DMUTILS_FOUND)
	message(FATAL_ERROR "DMutils not found. Please set the DMUTILS_ROOT variable to your DMUtils root installation directory and retry")
endif()

include_directories(${DMUTILS_INCLUDE_DIR})

# add the subdirectories
add_subdirectory(src)
if(LIGHTSYSTEM_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
#build doc
if(LIGHTSYSTEM_BUILD_DOC)
    add_subdirectory(doc)
endif()

