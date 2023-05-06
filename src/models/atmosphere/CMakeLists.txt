set(SOURCES FGStandardAtmosphere.cpp
            FGWinds.cpp)

set(HEADERS FGStandardAtmosphere.h
            FGWinds.h)

add_library(Atmosphere OBJECT ${HEADERS} ${SOURCES})
set_target_properties(Atmosphere PROPERTIES TARGET_DIRECTORY
                                 ${CMAKE_CURRENT_SOURCE_DIR})

if(USE_FORTRAN_MSIS)
    include(FortranCInterface)

    FortranCInterface_VERIFY(CXX)

    FortranCInterface_HEADER(FORTRAN_MSIS.h MACRO_NAMESPACE "FORTRAN_"
                            SYMBOLS msis_calc:msiscalc)

    set(NRLMSIS_SOURCES MSIS/alt2gph.F90
                        MSIS/msis_constants.F90
                        MSIS/msis_init.F90
                        MSIS/msis_gfn.F90
                        MSIS/msis_tfn.F90
                        MSIS/msis_dfn.F90
                        MSIS/msis_calc.F90
                        MSIS/JSBSim_interface.F90
                        FGMSIS.h
                        FGMSIS.cpp)

    add_library(NRLMSIS OBJECT ${NRLMSIS_SOURCES})
    target_compile_definitions(NRLMSIS PRIVATE DBLE)
    target_include_directories(NRLMSIS PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
else(USE_FORTRAN_MSIS)
    set(NRLMSIS_SOURCES MSIS/nrlmsise-00.h
                        MSIS/nrlmsise-00.c
                        MSIS/nrlmsise-00_data.c)
    add_library(NRLMSIS OBJECT ${NRLMSIS_SOURCES})
endif(USE_FORTRAN_MSIS)

set_target_properties(NRLMSIS PROPERTIES TARGET_DIRECTORY
                    ${CMAKE_CURRENT_SOURCE_DIR})

add_coverage(Atmosphere)

install(FILES ${HEADERS} DESTINATION include/JSBSim/models/atmosphere
                         COMPONENT devel)
