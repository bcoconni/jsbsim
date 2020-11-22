set(SOURCES FGStandardAtmosphere.cpp
            FGWinds.cpp)

set(HEADERS FGStandardAtmosphere.h
            FGWinds.h)

add_library(Atmosphere OBJECT ${HEADERS} ${SOURCES})
set_target_properties(Atmosphere PROPERTIES TARGET_DIRECTORY
                                 ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${HEADERS} DESTINATION include/JSBSim/models/atmosphere
                         COMPONENT devel)
