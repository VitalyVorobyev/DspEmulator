cmake_minimum_required (VERSION 3.1)
project (AdcFit)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

add_definitions(-Wall)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
FIND_PACKAGE(Boost 1.65 COMPONENTS program_options REQUIRED)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)
#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories (src ${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

ROOT_GENERATE_DICTIONARY(G__eclFitHit src/eclFitHit.h LINKDEF src/eclFitHitLinkDef.h)
#---Create a shared library with geneated dictionary
add_library(eclFitHit SHARED src/eclFitHit.cpp G__eclFitHit.cxx)
target_link_libraries(eclFitHit ${ROOT_LIBRARIES})

file(GLOB SOURCES src/*.cpp)
add_executable(adcfit ${SOURCES})

target_link_libraries(adcfit ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(adcfit eclFitHit)

install(TARGETS adcfit DESTINATION /usr/local/bin)
install(TARGETS eclFitHit DESTINATION /usr/local/lib)

