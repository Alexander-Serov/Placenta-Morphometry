cmake_minimum_required(VERSION 2.6)

               
#Déclaration du projet
project(plac_morph)

include_directories(.)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
SET(CMAKE_CXX_FLAGS "-DUNICODE=1")

# # Static link of -libgcc and -libstdc++ libraries
# set(CMAKE_EXE_LINKER_FLAGS " -static")


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
  plac_morph
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
if(UNIX)
	target_link_libraries(plac_morph GL GLU glut X11 pthread)
  # -static-libgcc -static-libstdc++
else()
	target_link_libraries(plac_morph Advapi32 gdi32 user32 shell32 opengl32 glu32)
endif()

