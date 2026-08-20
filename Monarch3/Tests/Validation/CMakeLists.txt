# CMakeLists.txt for Monarch3/Tests/Validation
# Author: N.S. Oblath

if( Monarch_BUILD_PYTHON )

    message( STATUS "Configuring Monarch3 validation tests" )

    set( VALIDATION_TEST_FILES
        conftest.py
        test_cpp_write_python_read.py
        test_python_write_cpp_read.py
        test_python_write_python_read.py
    )

    # Copy Python test files to build tree so pytest can find the built monarch3 module
    foreach( tfile ${VALIDATION_TEST_FILES} )
        configure_file( ${tfile} ${CMAKE_CURRENT_BINARY_DIR}/${tfile} COPYONLY )
    endforeach()

    # Register validation tests with CTest
    # pytest is invoked in the build-tree copy so it can find monarch3.so and the executables
    add_test(
        NAME monarch3_validation
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_BINARY_DIR} -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Make the built monarch3 module and test executables available at test time
    set_tests_properties( monarch3_validation PROPERTIES
        ENVIRONMENT "PYTHONPATH=${LIB_INSTALL_DIR}:$ENV{PYTHONPATH};PATH=${BIN_INSTALL_DIR}:$ENV{PATH}"
    )

else( Monarch_BUILD_PYTHON )

    message( STATUS "Skipping Monarch3 Python validation tests (Monarch_BUILD_PYTHON is OFF)" )

endif( Monarch_BUILD_PYTHON )
