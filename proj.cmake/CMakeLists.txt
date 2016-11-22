cmake_minimum_required (VERSION 2.6)
project (Visualizer)

add_subdirectory(../../oxygine-framework oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS} -ggdb")

add_executable(Visualizer
    ../src/main.cpp  ../src/example.h ../src/example.cpp
    ../src/common.h
    ../src/metrics.h ../src/metrics.cpp
    ../src/cftree.h ../src/cftree.cpp
    ../src/cfcluster.h ../src/cfcluster.cpp
    ../src/minsquare.h
    ../src/cftreebuilder.h ../src/cftreebuilder.cpp
)
target_link_libraries(Visualizer ${OXYGINE_CORE_LIBS})
