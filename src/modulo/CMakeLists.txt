find_package(Threads REQUIRED)

set(SOURCES ${SOURCES} modulo/mpmc/queue.c)
set(SOURCES ${SOURCES} modulo/mpmc/controller.c)
set(SOURCES ${SOURCES} modulo/pool/pool.c)

set(SOURCES ${SOURCES} modulo/solver/node.c)
set(SOURCES ${SOURCES} modulo/solver/controller.c)
set(SOURCES ${SOURCES} modulo/solver/mpmc_types.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(modulo SHARED ${SOURCES})
target_link_libraries(modulo ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(modulo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS modulo
    LIBRARY DESTINATION lib/modulo
    INCLUDES DESTINATION include
    )
