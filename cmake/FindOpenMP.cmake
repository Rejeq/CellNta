if(NOT CELLNTA_OPENMP)
  return()
endif()

# MSVC compiler by default support only 2.5 version,
# So if llvm features are supported (version >= 2019), trying to use it
if(MSVC_TOOLSET_VERSION GREATER_EQUAL 142)
  set(OpenMP_CXX_FLAGS "/openmp:llvm")
  set(OpenMP_LLVM_FLAG_ENABLED ON)
  message(STATUS "Used OpenMP:LLVM flag")
endif()

list(REMOVE_ITEM CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")
find_package(OpenMP)
list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if(NOT OpenMP_FOUND)
  message(WARNING "Not found OpenMP support in your compiler, disabling")
  set(CELLNTA_OPENMP OFF)
else()
  if(OpenMP_LLVM_FLAG_ENABLED)
    target_compile_definitions(OpenMP::OpenMP_CXX INTERFACE -DCELLNTA_OPENMP_MSVC_LLVM=1)
  endif()

  # Workaround for: https://gitlab.kitware.com/cmake/cmake/-/issues/21818
  if(NOT MSVC)
    get_property(OpenMP_LINK_OPTS
      TARGET OpenMP::OpenMP_CXX
      PROPERTY INTERFACE_LINK_OPTIONS
    )

    if (NOT OpenMP_LINK_OPTS)
      get_property(OpenMP_COMPILE_OPTS
        TARGET OpenMP::OpenMP_CXX
        PROPERTY INTERFACE_COMPILE_OPTIONS
      )
      set_property(
        TARGET OpenMP::OpenMP_CXX
        PROPERTY INTERFACE_LINK_OPTIONS ${OpenMP_COMPILE_OPTS}
      )
    endif()
  endif()

  message(STATUS "OpenMP successfully enabled")
endif()
