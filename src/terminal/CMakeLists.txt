set(SOURCES ${SOURCES} main.c)

set(INTERNAL_LIBRARIES ${INTERNAL_LIBRARIES} modulo)

add_executable(modulo_tty ${SOURCES})

target_link_libraries(modulo_tty LINK_PUBLIC ${INTERNAL_LIBRARIES})

install(TARGETS modulo_tty DESTINATION bin)

if (APPLE)
	set_target_properties(modulo_tty PROPERTIES INSTALL_RPATH
		"@loader_path/../lib/modulo")
endif()

