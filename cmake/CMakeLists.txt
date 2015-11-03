cmake_minimum_required(VERSION 2.8)

project(MiniDB)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../MiniDB)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB MiniDBSource
	${PROJECT_SOURCE_DIR}/*.h
	${PROJECT_SOURCE_DIR}/*.cpp
)

add_executable(MiniDB ${MiniDBSource})
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/BufferManager.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/CatalogManager.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/Data.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/DB_Interpreter.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/IndexManager.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/main.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/MiniDB.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/mybuffer.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/mystr.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/Page.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/RecordManager.cpp)
#add_executable(MiniDB ${PROJECT_SOURCE_DIR}/RecordManager.cpp)
