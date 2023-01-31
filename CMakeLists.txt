# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# 设定项目
project(Transformations)
message(STATUS "[Project] ${PROJECT_NAME}")

# 修改默认的安装路径前缀
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "install prefix" FORCE)
endif()

# 在 Debug 模式下自动对 dll 和 lib 文件名后边加 d，但 exe 仍然需要手动设置
set(CMAKE_DEBUG_POSTFIX d)

# 确定 C++ 版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()

# 将生成的 exe, dll, lib 等放置在 bin, lib 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib")

# 开启 VS 的文件夹功能
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	# $ENV{XXX_INCLUDE_PATH}
	# ...
)
link_directories (
	${PROJECT_SOURCE_DIR}/lib
	# $ENV{XXX_LIB_PATH}
	# ...
)

#add_subdirectory(include)
#add_subdirectory(src)

# first create relevant static libraries required for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

message(STATUS "----------")
find_package(glfw3 REQUIRED)
if(TARGET glfw)
	message(STATUS "glfw3_FOUND: ${glfw3_FOUND}")
	message(STATUS "glfw3_DIR: ${glfw3_DIR}")
endif()
set(LIBS ${LIBS} glfw)

add_executable(${PROJECT_NAME} "src/main.cpp")
add_executable(CoordinateSystems "src/coord_sys.cpp")

target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_libraries(CoordinateSystems ${LIBS})

#file GLOB 是一个通配符，可以匹配多个文件，类似于set
file(GLOB SHADERS
            "src/shaders/*.vs"
            # "src/shaders/*.frag"
            "src/shaders/*.fs"
            "src/shaders/*.gs"
	     	"src/shaders/*.cs"
)

foreach(SHADER ${SHADERS})
        if(WIN32)
            # configure_file(${SHADER} "test")
			# 将文件复制到指定目录
            add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${PROJECT_BINARY_DIR})
        endif(WIN32)
endforeach(SHADER)