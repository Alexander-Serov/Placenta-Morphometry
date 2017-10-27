cmake_minimum_required(VERSION 2.6)

               
#Déclaration du projet
project(HumanPlacenta)

include_directories(.)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
SET(CMAKE_CXX_FLAGS "-DUNICODE=1")
#Génération de la liste des fichiers sources
file(
        GLOB_RECURSE
        source_files
        core/algorithm/*.cpp
        core/algorithm/*.h
        core/data/*.cpp
        core/data/*.h
        population_sacha/*.cpp
        population_sacha/*.h
)

#Déclaration de l'exécutable
add_executable(
  Human_placenta
  ${source_files}
  main.cpp
  core/dependency/pugiconfig.hpp 
  core/dependency/CImg.h 
  core/dependency/ConvertorCImg.h 
  core/dependency/pugixml.hpp 
  core/dependency/fparser.hh 
  core/dependency/fpconfig.hh 
  core/dependency/fptypes.hh 
  core/dependency/MTRand.h 
  core/dependency/lodepng.h 
  core/dependency/bipmap.h 
  core/dependency/jpgd.h 
  core/dependency/jpge.h 
  core/dependency/tinythread.h 
  core/dependency/fparser.cpp 
  core/dependency/MTRand.cpp 
  core/dependency/pugixml.cpp 
  core/dependency/lodepng.cpp 
  core/dependency/jpgd.cpp 
  core/dependency/jpge.cpp 
  core/dependency/tinythread.cpp 
)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

#if(APPLE)
	include_directories(AFTER "/opt/X11/include")
#	include_directories(AFTER "/opt/X11/lib")
#endif()

if(UNIX)
	if(APPLE)
		target_link_libraries(Human_placenta pthread ${X11_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	else()
		target_link_libraries(Human_placenta GL GLU glut X11 pthread)
	endif()
else()
	target_link_libraries(Human_placenta Advapi32 gdi32 user32 shell32 opengl32 glu32)
endif()



