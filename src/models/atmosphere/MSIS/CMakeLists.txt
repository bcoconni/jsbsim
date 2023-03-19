cmake_minimum_required (VERSION 3.1)

project(NRLMSIS)

enable_language(CXX Fortran)

include(FortranCInterface)

FortranCInterface_VERIFY(CXX)

FortranCInterface_HEADER(FC_MSIS.h MACRO_NAMESPACE "FC_" SYMBOLS gtd8d msis_init:msisinit)

set(SOURCES alt2gph.F90
            msis_constants.F90
            msis_init.F90
            msis_gfn.F90
            msis_tfn.F90
            msis_dfn.F90
            msis_calc.F90
            msis_gtd8d.F90)

add_library(NRLMSIS ${SOURCES})
target_compile_definitions(NRLMSIS PRIVATE DBLE)

# FORTRAN test program
add_executable(msis2.0_test msis2.0_test.F90)
target_link_libraries(msis2.0_test NRLMSIS)

# C++ test program
add_executable(FGMSIS FGMSIS.cpp)
target_link_libraries(FGMSIS NRLMSIS)
target_include_directories(FGMSIS PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
