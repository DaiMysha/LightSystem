set(INCROOT ${PROJECT_SOURCE_DIR}/example)
set(SRCROOT ${PROJECT_SOURCE_DIR}/example)

set(FILES_HEADER
)

set(FILES_SRC
	${SRCROOT}/main.cpp
)
	
add_executable (${PROJECT_NAME}-example
	${FILES_HEADER}
	${FILES_SRC}
)
target_link_libraries (${PROJECT_NAME}-example ${PROJECT_NAME}-shared )