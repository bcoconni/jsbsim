# Import the Cython utilities for CMake
include(UseCython)

# Declare JSBSim as a C++ project
set_source_files_properties(jsbsim.pyx PROPERTIES CYTHON_IS_CXX TRUE)

# Build the Python module using Cython and the JSBSim library
cython_add_module(jsbsim jsbsim.pyx)
target_link_libraries(jsbsim libJSBSim)

# Replicate the Python files in the build dir.
# With CMake, the build tree can be separated from the source tree. For tests
# written in Python to be executed by 'make test', the sources must be
# collocated with the JSBSim Python module in the build tree.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR}
                                      FILES_MATCHING PATTERN "*.py")

# Declare the tests to CTest so that they can be executed by 'make test'
set(PYTHON_TESTS ResetOutputFiles
                 TestICOverride
                 RunCheckCases
                 TestModelLoading
                 CheckFGBug1503
                 TestGustReset
                 TestPointMassInertia
                 CheckMomentsUpdate
                 TestFuelTanksInertia
                 TestInputSocket
                 TestInitialConditions
                 CheckScripts
                 CheckAircrafts
                 CheckOutputRate
                 TestAccelerometer
                 TestOrbit
                 TestGndReactions
                 TestCosineGust)

foreach(test ${PYTHON_TESTS})
  add_test(${test} ${PYTHON_EXECUTABLE} ${test}.py ${CMAKE_SOURCE_DIR})
endforeach()

# Install the JSBSim Python module
if (INSTALL_PYTHON_MODULE)
  set(SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
  configure_file(setup.py.in ${SETUP_PY})
  install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install)")
endif()

# Run valgrind (a memory checker) on a selected subset of scripts
find_program(VALGRIND_COMMAND NAMES valgrind)

set(VALGRIND_SCRIPTS c1723
                     737_cruise
                     J2461)

if (VALGRIND_COMMAND)
  set(VALGRIND_OPTIONS --tool=memcheck --leak-check=full --leak-resolution=high)
  foreach(script ${VALGRIND_SCRIPTS})
    add_test(NAME Valgrind_${script} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
             COMMAND ${VALGRIND_COMMAND} ${VALGRIND_OPTIONS}
                     ${CMAKE_BINARY_DIR}/src/JSBSim scripts/${script}.xml)
  endforeach()
endif(VALGRIND_COMMAND)
