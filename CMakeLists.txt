# a primitive conf file, to be improved 

cmake_minimum_required(VERSION 2.6)


INCLUDE (CheckCXXSourceCompiles)
INCLUDE (CheckCSourceCompiles)

PROJECT("gextconf")


# hard set to debug currently
SET(CMAKE_BUILD_TYPE debug)

set(PROJECT_CONFIG_FILE_SOURCE ${CMAKE_SOURCE_DIR}/src/config.in.h)
set(PROJECT_CONFIG_FILE_AIM ${CMAKE_SOURCE_DIR}/src/config.h)

# define compiler args


if (MSVC)
  set ( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
  set ( CMAKE_C_FLAGS_DEBUG "/DEBUG")
  set ( CMAKE_C_FLAGS_RELEASE  "" )
endif ()

if (CMAKE_COMPILER_IS_GNUCXX)
  set ( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
  set ( CMAKE_C_FLAGS_DEBUG "-g -Wall")
  set ( CMAKE_C_FLAGS_RELEASE  "-O2")
endif ()



# tests 
CHECK_C_SOURCE_COMPILES("int main() { return 1; }" HAVE_TEST1)



set(libSources
	src/conf.c)

ADD_LIBRARY(gexconf STATIC ${libSources})


# add sample file
#ADD_EXECUTABLE(example-file)
#target_link_libraries(example-file gexconf)


configure_file(${PROJECT_CONFIG_FILE_SOURCE} ${PROJECT_CONFIG_FILE_AIM})
