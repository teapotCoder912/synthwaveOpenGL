cmake_minimum_required(VERSION 3.30)
project(synthwave LANGUAGES CXX)


find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED)

find_package(GLUT REQUIRED)

#find_package(glm REQUIRED)


set(MY_HEADER_FILES
        render.h

)

set(MY_SOURCE_FILES
        main.cpp
        render.cpp

)


add_executable(synthwave
        ${MY_HEADER_FILES}
        ${MY_SOURCE_FILES}
)
target_link_libraries(synthwave
        ${OPENGL_LIBRARY}
        ${GLUT_glut_LIBRARY}
        glfw


)
