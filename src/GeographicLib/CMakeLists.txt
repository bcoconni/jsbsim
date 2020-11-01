set(SOURCES Geodesic.cpp
            GeodesicLine.cpp
            Geocentric.cpp
            Math.cpp)
set(HEADERS Config.h
            Constants.hpp
            Geodesic.hpp
            GeodesicLine.hpp
            Geocentric.hpp
            Math.hpp)

add_library(GeographicLib OBJECT ${HEADERS} ${SOURCES})
set_target_properties(GeographicLib PROPERTIES TARGET_DIRECTORY
                                    ${CMAKE_CURRENT_SOURCE_DIR})
