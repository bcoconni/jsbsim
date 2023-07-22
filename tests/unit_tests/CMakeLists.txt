set(CMAKE_CXX_STANDARD 14)

set(UNIT_TESTS FGColumnVector3Test
               StringUtilitiesTest
               FGJSBBaseTest
               FGMatrix33Test
               FGQuaternionTest
               FGLocationTest
               FGGroundCallbackTest
               FGInitialConditionTest
               FGInertialTest
               FGPropertyValueTest
               FGTableTest
               FGRealValueTest
               FGParameterTest
               FGParameterValueTest
               FGConditionTest
               FGPropertyManagerTest
               FGAtmosphereTest
               FGAuxiliaryTest
               FGMSISTest)


foreach(test ${UNIT_TESTS})
  cxxtest_add_test(${test}1 ${test}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${test}.h)
  target_link_libraries(${test}1 libJSBSim)
  target_include_directories(${test}1 PUBLIC ${CXXTEST_INCLUDE_DIR})
  add_coverage(${test}1)
endforeach()

set(MSIS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/models/atmosphere/MSIS)

if(USE_FORTRAN_MSIS)
  configure_file(${MSIS_SOURCE_DIR}/msis20.parm
                  ${CMAKE_CURRENT_BINARY_DIR}/msis20.parm COPYONLY)
  configure_file(${MSIS_SOURCE_DIR}/msis2.0_test_ref_dp.txt
                  ${CMAKE_CURRENT_BINARY_DIR}/msis2.0_test_ref_dp.txt COPYONLY)
  target_compile_definitions(FGMSISTest1 PRIVATE USE_FORTRAN_MSIS)
else(USE_FORTRAN_MSIS)
  if(WIN32 AND BUILD_SHARED_LIBS)
    # Windows cannot locate the symbol gtd7 as it is not exported in the JSBSim
    # DLL. To keep NRLMSIS source files pristine, the option chosen is to
    # compile NRLMSIS alongside the unit test FGMSISTest1.
    target_sources(FGMSISTest1 PUBLIC
                    ${MSIS_SOURCE_DIR}/nrlmsise-00.c
                    ${MSIS_SOURCE_DIR}/nrlmsise-00_data.c)
  endif()
endif(USE_FORTRAN_MSIS)

# Windows needs the DLL to be copied locally for unit tests to run.
if(WIN32 AND BUILD_SHARED_LIBS)
  # Windows needs the DLL to be copied locally for unit tests to run.
  list(GET UNIT_TESTS 0 FIRST_UNIT_TEST) # Any target name will do so we just pick the first.
  add_custom_command(TARGET ${FIRST_UNIT_TEST}1 POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different
                      $<TARGET_FILE:libJSBSim>
                      $<TARGET_FILE_DIR:${FIRST_UNIT_TEST}1>)
endif(WIN32 AND BUILD_SHARED_LIBS)
