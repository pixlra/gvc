###
### CMakeLists for gvc
###

cmake_minimum_required( VERSION 3.2.0 )

######################################################################################
# CMake  functions
######################################################################################
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
INCLUDE(FeatureSummary)
INCLUDE(GNUInstallDirs)

######################################################################################
# Project Definition
######################################################################################

SET( PROJECT_NAME "gvc" CACHE INTERNAL "" FORCE )
SET( PROJECT_LIBRARY "gvc" CACHE INTERNAL "" FORCE )

PROJECT( ${PROJECT_NAME} )
SET( GVC_VERSION_MAJOR "0" )
SET( GVC_VERSION_MINOR "0" )
SET( GVC_VERSION_PATCH "1" )
SET( GVC_VERSION "${GVC_VERSION_MAJOR}.${GVC_VERSION_MINOR}.${GVC_VERSION_PATCH}" CACHE INTERNAL "" FORCE )

######################################################################################
# Dependencies
######################################################################################

OPTION( BUILD_DOC "Build Documentation" OFF )
IF( BUILD_DOC )
  FIND_PACKAGE(Doxygen)
  SET( BUILD_DOC ${Doxygen_FOUND} )
ENDIF()
SET_PACKAGE_PROPERTIES(Doxygen PROPERTIES URL "http://www.doxygen.org/" DESCRIPTION "Doxygen" TYPE OPTIONAL)

OPTION( BUILD_TESTS "Build Google Tests" OFF )
IF( BUILD_TESTS )
  FIND_PACKAGE(GTest)
  SET( BUILD_DOC ${GTEST_FOUND} )
  IF( GTEST_FOUND )
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
  ENDIF()
ENDIF()

######################################################################################
# CMake Defin1itions
######################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
INCLUDE(FeatureSummary)

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()
SET( CMAKE_CXX_FLAGS "" )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

######################################################################################
# Configure targets
######################################################################################

OPTION( USE_WERROR "Warnings as errors" OFF )
OPTION( USE_STATIC "Use static libs" OFF )

SET(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=gnu++0x)
endif()

IF (NOT CMAKE_BUILD_TYPE )
    MESSAGE(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

IF( USE_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )

ADD_FEATURE_INFO(WErrors USE_WERROR "Warnings as errors" )

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( app )

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

######################################################################################
# Final information dump
######################################################################################
MESSAGE("")
MESSAGE( STATUS "${PROJECT_NAME} Information"                     )
MESSAGE( STATUS "Version: "                 "${GVC_VERSION_STRING}" )
MESSAGE( STATUS "Configuration:"                                  )
MESSAGE( STATUS "    Static libs: "         "${USE_STATIC}" )
MESSAGE( STATUS "    Build type: "          "${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "    Build flags: "         "${CMAKE_CXX_FLAGS}"  )

FEATURE_SUMMARY(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
