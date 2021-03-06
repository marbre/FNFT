cmake_minimum_required(VERSION 3.5)
project(fnft C)

set(FNFT_VERSION_MAJOR 0)
set(FNFT_VERSION_MINOR 1)
set(FNFT_VERSION_PATCH 1)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCSourceCompiles)
include(CheckLanguage)

# options
option(DEBUG "Compile with debugging symbols" OFF)
option(WITH_MATLAB "Build the Matlab interface" ON)
option(MACHINE_SPECIFIC_OPTIMIZATION "Activate optimizations specific for this machine" OFF)
option(ADDRESS_SANITIZER "Enable address sanitzer for known compilers" OFF)
if (MACHINE_SPECIFIC_OPTIMIZATION)
	message("Optimizing for this specific machine.")
endif()

# check for complex.h
check_include_files(complex.h HAVE_COMPLEX_H)
if (NOT HAVE_COMPLEX_H)
	message(ERROR "Compiler does not provide complex.h")
endif()

# check if math code requires us to link libm
check_function_exists(log2 HAVE_LOG2)
if (NOT HAVE_LOG2)
	message("Adding libm to the required libraries, repeating test.")
	list(APPEND CMAKE_REQUIRED_LIBRARIES m)
	check_function_exists(log2 HAVE_LOG2_2)
	if (NOT HAVE_LOG2_2)
		message(ERROR "no log2")
	else()
		set(LIBM m)	
	endif()
else()
	set(LIBM )
endif()

# check if thread local storage is available
check_c_source_compiles("_Thread_local int g; int main() { g = 1; return g; }" HAVE__THREAD_LOCAL)
if (NOT HAVE__THREAD_LOCAL)
	check_c_source_compiles("__thread int g; int main() { g = 1; return g; }" HAVE___THREAD)
	if (NOT HAVE___THREAD)
		message(WARNING "Thread local storage is not available. fnft_errwarn_setprintf will not be thread-safe.")
	endif()
endif()

# header files
include_directories(include)
include_directories(include/3rd_party/eiscor)
include(CheckCCompilerFlag)

# configure C compiler
if (CMAKE_COMPILER_IS_GNUCC) # gcc
	if (DEBUG)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
	endif()
    if (ADDRESS_SANITIZER)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
	if (MACHINE_SPECIFIC_OPTIMIZATION)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
	endif()
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic -Werror=implicit-function-declaration")
else()
	message(WARNING "Compiler is not gcc. Will try to use -O3 (or if debug) flag.")
	if (DEBUG)    
		check_c_compiler_flag("-g" HAS_C_G_FLAG)
		if (HAS_C_G_FLAG)
			set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}$ -g")
		endif()
	else()
		check_c_compiler_flag("-O3" HAS_C_O3_FLAG)
	    	if (HAS_C_O3_FLAG)
			set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
		endif()
		if (MACHINE_SPECIFIC_OPTIMIZATION)
    			message(WARNING "Compiler is not gcc. Will try to use -march=native flag.")
        		check_c_compiler_flag("-march=native" HAS_C_MARCH_NATIVE_FLAG)
        		if (HAS_C_MARCH_NATIVE_FLAG)
    				set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
        		endif()
		endif()
	endif()
endif()

# generate config header file
configure_file(include/fnft_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/fnft_config.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/private)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/3rd_party/kiss_fft)

# configure Fortran support
enable_language(Fortran)
if (CMAKE_Fortran_COMPILER_ID MATCHES GNU) # gfortran
	set (CMAKE_Fortran_FLAGS " -O3 -cpp -ffree-line-length-none")
endif ()

enable_testing()

# sources
file(GLOB SOURCES "src/*.c")
file(GLOB PRIVATE_SOURCES "src/private/*.c")
file(GLOB KISS_FFT_SOURCES "src/3rd_party/kiss_fft/*.c")
file(GLOB EISCOR_SOURCES "src/3rd_party/eiscor/*.f90")
file(GLOB EXAMPLE_SOURCES "examples/*.c")
file(GLOB_RECURSE TEST_SOURCES "test/*_test*.c")

# generate shared library
add_library(fnft SHARED ${SOURCES} ${PRIVATE_SOURCES} ${KISS_FFT_SOURCES} ${EISCOR_SOURCES})
set_target_properties(fnft PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# installation under Linux
install(TARGETS fnft DESTINATION lib)

# generate examples
foreach (srcfile ${EXAMPLE_SOURCES})
	get_filename_component(example ${srcfile} NAME_WE)
	add_executable(${example} ${srcfile})
	target_link_libraries(${example} fnft ${LIBM})
	set_target_properties(${example} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/examples")
endforeach()

# generate tests
foreach (srcfile ${TEST_SOURCES})
	get_filename_component(dir ${srcfile} DIRECTORY)
	get_filename_component(test ${srcfile} NAME_WE)
	add_executable(${test} ${srcfile})
	target_link_libraries(${test} fnft ${LIBM})
	add_test(NAME ${test} COMMAND ${test})
	set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${dir}")
endforeach()

# Try to build the Matlab interface if requested
if (WITH_MATLAB)
    check_language(CXX)
    if (CMAKE_CXX_COMPILER)
    	enable_language(CXX) # sometimes required by FindMatlab below
    else()
        message("Could not find a C++ compiler, which is sometimes required to localize Matlab.")
        message("If the build process fails, deactivate the localization with the -DWITH_MATLAB=OFF switch.")
    endif()
	find_package(Matlab COMPONENTS MX_LIBRARY)
	if (Matlab_FOUND)
		file(GLOB MEX_SOURCES "matlab/mex_*.c")
		foreach (srcfile ${MEX_SOURCES})
			get_filename_component(mexfile ${srcfile} NAME_WE)
			matlab_add_mex(NAME ${mexfile} SRC ${srcfile} LINK_TO fnft ${LIBM})
            set_target_properties(${mexfile} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/matlab")
            target_compile_options(${mexfile} PRIVATE -DMEX_DOUBLE_HANDLE)
            # -DMEX_DOUBLE_HANDLE avoids the new complex interleaved API
		endforeach()
	endif()
endif()
