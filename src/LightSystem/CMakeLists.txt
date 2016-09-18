set(INCROOT ${INCROOT}/${PROJECT_NAME})
set(SRCROOT ${SRCROOT}/${PROJECT_NAME})

set(FILES_HEADER
	${INCROOT}/LightSystem.hpp
	${INCROOT}/Light.hpp
	${INCROOT}/SpotLight.hpp
	${INCROOT}/FlashLight.hpp
	${INCROOT}/LocalAmbiantLight.hpp
	${INCROOT}/SpriteLight.hpp
	${INCROOT}/ShadowSystem.hpp
)

set(FILES_SRC
	${SRCROOT}/LightSystem.cpp
	${SRCROOT}/Light.cpp
	${SRCROOT}/SpotLight.cpp
	${SRCROOT}/FlashLight.cpp
	${SRCROOT}/LocalAmbiantLight.cpp
	${SRCROOT}/SpriteLight.cpp
	${SRCROOT}/ShadowSystem.cpp
)
	
build_library(${PROJECT_NAME}
	SOURCES ${FILES_SRC} ${FILES_HEADER}
	TYPE SHARED
	EXTERNAL_LIBS ${LIBS}
)

#deploy(ROOT ${INCROOT}DESTINATION include)