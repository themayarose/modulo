set(SOURCES ${SOURCES} main.c)

if (UNIX AND (NOT APPLE))
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEFAULT_SOURCE")
endif()

set(INTERNAL_LIBRARIES ${INTERNAL_LIBRARIES} modulo)

add_executable(modulo_tty ${SOURCES})

target_link_libraries(modulo_tty LINK_PUBLIC ${INTERNAL_LIBRARIES})

install(TARGETS modulo_tty DESTINATION bin)

if (APPLE)
	set_target_properties(modulo_tty PROPERTIES INSTALL_RPATH
		"@loader_path/../lib/modulo")
endif()

