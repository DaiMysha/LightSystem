set(INCROOT ${INCROOT}/${PROJECT_NAME})
set(SRCROOT ${SRCROOT}/${PROJECT_NAME})

set(FILES_HEADER
	${INCROOT}/LightSystem.hpp
	${INCROOT}/Light.hpp
	${INCROOT}/SpotLight.hpp
	${INCROOT}/LocalAmbiantLight.hpp
	${INCROOT}/EmissiveLight.hpp
)

set(FILES_SRC
	${SRCROOT}/LightSystem.cpp
	${SRCROOT}/Light.cpp
	${SRCROOT}/SpotLight.cpp
	${SRCROOT}/LocalAmbiantLight.cpp
	${SRCROOT}/EmissiveLight.cpp
)
	
build_library(${PROJECT_NAME}
	SOURCES ${FILES_SRC} ${FILES_HEADER}
	TYPE SHARED
	EXTERNAL_LIBS ${LIBS}
)

#deploy(ROOT ${INCROOT}DESTINATION include)