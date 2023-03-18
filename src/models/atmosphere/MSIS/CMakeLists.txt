cmake_minimum_required (VERSION 3.1)

project(NRLMSIS)

enable_language(Fortran)

set(SOURCES alt2gph.F90
            msis_constants.F90
            msis_init.F90
            msis_gfn.F90
            msis_tfn.F90
            msis_dfn.F90
            msis_calc.F90
            msis_gtd8d.F90
            msis2.0_test.F90)

add_executable(msis2.0_test ${SOURCES})
