###
### CMakeLists for gvc encoder app
###

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
)

set(GVC_ENC_SRCS
  main.cpp
  GvcEncoderApp.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME}-encoder ${GVC_ENC_SRCS} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}-encoder ${PROJECT_LIBRARY} Calyp )

INSTALL(TARGETS ${PROJECT_NAME}-encoder DESTINATION bin )
