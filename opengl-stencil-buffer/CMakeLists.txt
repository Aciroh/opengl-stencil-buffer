cmake_minimum_required(VERSION 3.10)
project(ImageCompositing)

# Find OpenGL and GLUT (or GLFW)
# find_package(OpenGL REQUIRED)
# find_package(GLUT REQUIRED) # Or GLFW if you're using it

# ... (other CMake commands)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# ... (rest of your CMake commands)


# Add your source files
add_executable(ImageCompositing main.cpp)

# Link OpenGL and GLUT (or GLFW) libraries
target_link_libraries(ImageCompositing ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}) # Or GLFW_LIBRARY
