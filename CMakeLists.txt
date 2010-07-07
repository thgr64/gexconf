# a primitive conf file, to be improved 

cmake_minimum_required(VERSION 2.6)


INCLUDE (CheckCSourceCompiles)
INCLUDE (CheckTypeSize) 
include (TestBigEndian)

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

test_big_endian(SYSTEM_IS_BIG_ENDIAN)
if (NOT SYSTEM_IS_BIG_ENDIAN)
	set(SYSTEM_IS_LITTLE_ENDIAN true)
endif ()


# get the size of wchar_t
SET(CMAKE_EXTRA_INCLUDE_FILES wchar.h)
CHECK_TYPE_SIZE("wchar_t" WCHAR_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)

set(libSources
	src/conf.c)

ADD_LIBRARY(gexconf STATIC ${libSources})

#ADD_EXECUTABLE(conv-test src/convert.c)

# add sample file
#ADD_EXECUTABLE(example-file)
#target_link_libraries(example-file gexconf)


configure_file(${PROJECT_CONFIG_FILE_SOURCE} ${PROJECT_CONFIG_FILE_AIM})
