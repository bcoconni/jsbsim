include_directories(${CXXTEST_INCLUDE_DIR})

CXXTEST_ADD_TEST(FGColumnVector3Test FGColumnVector3Test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/FGColumnVector3Test.h)

target_link_libraries(FGColumnVector3Test libJSBSim)
