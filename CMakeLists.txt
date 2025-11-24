cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# Add executable
add_executable(space_invaders main.cpp)

# Include directories
target_include_directories(space_invaders PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(space_invaders
    ${SDL2_LIBRARIES}
    SDL2_ttf
    SDL2_image
    m
)

# Platform-specific settings
if(WIN32)
    # Copy SDL2 DLLs to build directory on Windows
    if(SDL2_RUNTIME_LIBRARIES)
        foreach(dll ${SDL2_RUNTIME_LIBRARIES})
            add_custom_command(TARGET space_invaders POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${dll} $<TARGET_FILE_DIR:space_invaders>)
        endforeach()
    endif()
    
    # Set subsystem to Windows (no console window)
    # Remove this line if you want to see console output for debugging
    # set_target_properties(space_invaders PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Copy resource files to build directory
set(RESOURCE_FILES
    background.png
    boss_background.jpg
    trophy.png
    gorilla.png
    barrel.png
)

foreach(resource ${RESOURCE_FILES})
    add_custom_command(TARGET space_invaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/${resource}
        $<TARGET_FILE_DIR:space_invaders>/${resource}
    )
endforeach()

# Installation rules
install(TARGETS space_invaders DESTINATION bin)
install(FILES ${RESOURCE_FILES} DESTINATION bin)
