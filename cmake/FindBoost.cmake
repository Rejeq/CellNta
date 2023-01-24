function(LoadBoostDependices target)
  foreach(dep IN LISTS ARGN)
    set(BOOST_DEP_URL "https://github.com/boostorg/${dep}/archive/refs/tags/boost-1.80.0.tar.gz")
    message(STATUS "Loading boost_${dep} from ${BOOST_DEP_URL}")

    FetchContent_Declare(boost_${dep}
      URL ${BOOST_DEP_URL}
      DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    FetchContent_GetProperties(${dep})
    if(NOT boost_${dep}_POPULATED)
      FetchContent_Populate(boost_${dep})

      target_include_directories(${target} INTERFACE
        "${boost_${dep}_SOURCE_DIR}/include"
      )
    endif()
  endforeach()
endfunction()


# FIXME: Messy, perhaps there is a better way?
# The problem is that FindBoost.cmake is located by default in .../cmake/Modules
# but we're already using a FindBoost.cmake in the ${CELLNTA_DIR}/cmake
# otherwise recursion occurs

list(REMOVE_ITEM CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")
find_package(Boost 1.77)
list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if (NOT Boost_FOUND AND CELLNTA_FETCHING)
  message(STATUS "Loading boost_dynamic_bitset and his dependedices")
  include(FetchContent)

  add_library(BoostOnlyDynamicBitset INTERFACE IMPORTED)
  add_library(Boost::boost ALIAS BoostOnlyDynamicBitset)

  LoadBoostDependices(BoostOnlyDynamicBitset
    assert
    config
    container_hash
    core
    describe
    detail
    dynamic_bitset
    exception
    functional
    integer
    move
    static_assert
    throw_exception
    type_traits
  )

  message(STATUS "boost_dynamic_bitset configured successfully")
endif()
