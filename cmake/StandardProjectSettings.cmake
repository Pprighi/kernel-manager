# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050F00)

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_compile_options(-nostdlib++ -stdlib=libc++ -nodefaultlibs -fexperimental-library)
  add_link_options(-stdlib=libc++)
endif()

option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)

if(ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(
    RESULT
    result
    OUTPUT
    output)
  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(SEND_ERROR "IPO is not supported: ${output}")
  endif()
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
else()
  message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

# Enables STL container checker if not building a release.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_GLIBCXX_ASSERTIONS)
  add_definitions(-D_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS=1)
  add_definitions(-D_LIBCPP_ENABLE_ASSERTIONS=1)
endif()

# Enables dev environment.
option(ENABLE_DEVENV "Enable dev environment" ON)
if(NOT ENABLE_DEVENV)
  add_definitions(-DNDEVENV)
endif()

# Choose pkg operation implementation.
# Note: temporal fix
option(PKG_DUMMY_IMPL "Use dummy implementation of install/uninstall operations" ON)
if(PKG_DUMMY_IMPL)
  add_definitions(-DPKG_DUMMY_IMPL)
endif()

# Choose pkg operation implementation.
option(ENABLE_AUR_KERNELS "Enable AUR kernels support" ON)
if(PKG_DUMMY_IMPL AND ENABLE_AUR_KERNELS)
  add_definitions(-DENABLE_AUR_KERNELS)
elseif(NOT PKG_DUMMY_IMPL)
  if(ENABLE_AUR_KERNELS)
    message(FATAL_ERROR "Unable to enable AUR kernels support if 'PKG_DUMMY_IMPL=OFF'!")
  endif()
endif()
