cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "2" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.1.${PATCH_VERSION})

project(graphic_vector_editor_prg VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in version.h)

add_executable(graphic_vector_editor main.cpp main.h Serializer.cpp Serializer.h)
add_library(graphic_vector_editor_prg lib.cpp)
add_executable(test_version test_version.cpp)

set_target_properties(graphic_vector_editor graphic_vector_editor_prg test_version PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(graphic_vector_editor_prg
    PRIVATE "${CMAKE_BINARY_DIR}"
)
set_target_properties(test_version PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(graphic_vector_editor PRIVATE
    graphic_vector_editor_prg
    -lboost_serialization
)
target_link_libraries(test_version
    ${Boost_LIBRARIES}
    graphic_vector_editor_prg
)

if (MSVC)
    target_compile_options(graphic_vector_editor PRIVATE
        /W4
    )
    target_compile_options(graphic_vector_editor_prg PRIVATE
        /W4
    )
    target_compile_options(test_version PRIVATE
        /W4
    )
else ()
    target_compile_options(graphic_vector_editor PRIVATE
        -Wall -Wextra -pedantic -Werror 
    )
    target_compile_options(graphic_vector_editor_prg PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS graphic_vector_editor RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_version test_version)
