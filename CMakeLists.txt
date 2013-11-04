
cmake_minimum_required(VERSION 2.8)

##########
# project 

set(NAME FloodFill) 

set(SHADER_FILES
	${CMAKE_SOURCE_DIR}/data/TwoTriPixelShaders.glsl 
	${CMAKE_SOURCE_DIR}/data/RayMarch_frame.glsl 
	${CMAKE_SOURCE_DIR}/data/MandelbrotSet.glsl
) # run the DATA_COPY project after changing this data file.

set(SRC_FILSE
	main.cpp
)

if( WIN32 )
	#set this to be window not console program. the reason is the pez.windows.c
	# define the WinMain() function, so this should be window application.
	# If we do not set to the Win32 it will find the main() function, however, we donot define main().
	# so we will get this error.
	# MSVCRTD.lib(crtexe.obj) : error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
	#set( CONSOLE_SYSTEM WIN32 )    
endif()

add_executable(${NAME} 
	${CONSOLE_SYSTEM}
    ${SRC_FILSE}
)
	
target_link_libraries(
	${NAME}
	"OpenGL32"
)

if (WIN32)
  add_definitions(
      -D_USE_MATH_DEFINES 
      -D_UNICODE -D__UNICODE	
  )
  #ADD_DEFINITIONS( -DGLEW_STATIC ) #note: using with glew32s.lib, not glew32.lib
  
endif ()

################################
# Add custom target to copy all data

# this will create a DATA_COPY project, for example in .vcproject, build it then it does the copying.
set(TARGET_DATA_COPY DATA_COPY)
add_custom_target(
	${TARGET_DATA_COPY}
	COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
	#COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include #cjren	
)
