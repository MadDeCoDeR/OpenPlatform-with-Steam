cmake_minimum_required (VERSION 3.10)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
project(OpenPlatform)

file(GLOB OPEN_PLATFORM_INCLUDES "*h")
file(GLOB OPEN_PLATFORM_SOURCES "*.cpp")

source_group("Header Files" FILES ${OPEN_PLATFORM_INCLUDES})
source_group("Source Files" FILES ${OPEN_PLATFORM_SOURCES})
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/sdk/public/steam")

if (MSVC)
if(NOT SET_UP_CONFIGURATIONS_DONE)
    set(SET_UP_CONFIGURATIONS_DONE 1)
if(CMAKE_CONFIGURATION_TYPES) 
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
    endif()

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEF:../Platform.def")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEF:../Platform.def")
	endif()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS
					-D_UNICODE
					-DUNICODE)
	set(NAME "OpenPlatform")

	if (CMAKE_CL_64)
	link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin/win64")
	set (Additional_LIBRARIES steam_api64)
	else()
	link_directories ("${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin")
	set (Additional_LIBRARIES steam_api)
	endif()

	if (${CMAKE_VS_PLATFORM_NAME} STREQUAL "Win32")
	set(Target_Name ${NAME}x86)
	elseif(${CMAKE_VS_PLATFORM_NAME} STREQUAL "x64")
	set(Target_Name ${NAME}x64)
	endif()
	add_library("${Target_Name}" SHARED ${OPEN_PLATFORM_INCLUDES} ${OPEN_PLATFORM_SOURCES})
	target_link_libraries(${Target_Name} ${Additional_LIBRARIES})
	else()

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

	#set (PLATFORM_LIB "")
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	find_library (PLATFORM_LIB 
		NAME steam_api
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin/linux64)
	else()
	find_library (PLATFORM_LIB 
		NAME steam_api
		PATHS ${CMAKE_CURRENT_SOURCE_DIR}/sdk/redistributable_bin/linux32)
	endif()
	set (Additional_LIBRARIES ${PLATFORM_LIB})
	set(NAME "OpenPlatform")
	add_library (${NAME} SHARED ${OPEN_PLATFORM_INCLUDES} ${OPEN_PLATFORM_SOURCES})
	set_target_properties(${NAME} PROPERTIES PREFIX "")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(${NAME} PROPERTIES SUFFIX "x86_64.so")
	else()
	set_target_properties(${NAME} PROPERTIES SUFFIX "x86.so")
	endif()
	target_link_libraries(${NAME} ${Additional_LIBRARIES})
	endif()
