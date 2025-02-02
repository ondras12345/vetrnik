cmake_minimum_required(VERSION 3.20)

project(vetrniksil VERSION 0.0.1 DESCRIPTION "wind turbine software-in-the-loop lib")

include(FetchContent)
FetchContent_Declare(fe
    GIT_REPOSITORY https://github.com/rxi/fe
    GIT_TAG ed4cda96bd582cbb08520964ba627efb40f3dd91
    #SOURCE_DIR "src/"
)
FetchContent_MakeAvailable(fe)

add_library(vetrniksil SHARED
    src/wt_sil.c
    src/hal.c
    ../vetrnik-control/lib/wt_lisp/wt_lisp.c
    ../vetrnik-control/lib/lcd_lisp/lcd_lisp.c
    ../vetrnik-control/lib/fe_utils/fe_utils.c
    ../vetrnik-control/lib/wt_hal/wt_hal.c
    "${fe_SOURCE_DIR}/src/fe.c"
)
target_link_libraries(vetrniksil m)
set_target_properties(vetrniksil PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/wt_sil.h)

target_include_directories(vetrniksil PRIVATE include)
target_include_directories(vetrniksil PRIVATE src)
target_include_directories(vetrniksil PUBLIC "${fe_SOURCE_DIR}/src")

target_include_directories(vetrniksil PRIVATE ../vetrnik-control/lib/wt_hal)
target_include_directories(vetrniksil PRIVATE ../vetrnik-control/lib/wt_lisp)
target_include_directories(vetrniksil PRIVATE ../vetrnik-control/lib/lcd_hal)
target_include_directories(vetrniksil PRIVATE ../vetrnik-control/lib/lcd_lisp)
target_include_directories(vetrniksil PRIVATE ../vetrnik-control/lib/fe_utils)

# test - repl program
#add_executable(test src/test.c)
#link_directories(.)
#target_link_libraries(test "-L./libvetrniksil.so" vetrniksil)
#target_include_directories(test PRIVATE include)
#target_include_directories(test PRIVATE ../vetrnik-control/lib/wt_hal)

# TODO does not work??
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
