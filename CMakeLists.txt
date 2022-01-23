cmake_minimum_required(VERSION 3.21)
project(OpenVAFT)

if (NOT WIN32)
    message(SEND_ERROR "It is currently only optimized for Windows.\n")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_INSTALL_PATH "C:/Qt/6.2.2/msvc2019_64/lib/cmake")

set(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/lib/whereami/src/whereami.c
    ${PROJECT_SOURCE_DIR}/lib/whereami/src/whereami.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTAllocator.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTAllocator.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTFaceTracker.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTFaceTracker.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTGLWidget.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTGLWidget.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTLive2DManager.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTLive2DManager.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTMain.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTMainWindow.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTMainWindow.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTModel.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTModel.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTSprite.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTSprite.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTTextureManager.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTTextureManager.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTUtility.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTUtility.h
    ${PROJECT_SOURCE_DIR}/src/OVAFTView.cpp
    ${PROJECT_SOURCE_DIR}/src/OVAFTView.h
)

if (WIN32)
    set(CUBISM_DEBUG_SUFFIX d)
    set(LIBRARY_SUFFIX lib)
    set(PLATFORM Windows)
    if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
        set(ARCH x86_64)
    elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
        set(ARCH x86)
    else()
        message(FATAL_ERROR "Invalid linker flag ${CMAKE_EXE_LINKER_FLAGS}.")
    endif()
    if(MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    if(MSVC_VERSION MATCHES 1800)
        set(COMPILER 120/)
    elseif(MSVC_VERSION MATCHES 1900)
        set(COMPILER 140/)
    elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
        set(COMPILER 141/)
    elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
        set(COMPILER 142/)
    elseif(MSVC)
        message(FATAL_ERROR "[${APP_NAME}] Unsupported Visual C++ compiler used.")
    endif()
    if(CORE_CRL_MD)
        set(CRT _MD)
    else()
        set(CRT _MT)
    endif()
endif()

# Qt6
set(CMAKE_PREFIX_PATH ${QT_INSTALL_PATH})
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGLWidgets REQUIRED)

# Cubism Core
add_library(Live2DCubismCore STATIC IMPORTED)
set_target_properties(Live2DCubismCore
        PROPERTIES
        IMPORTED_LOCATION_DEBUG
        ${PROJECT_SOURCE_DIR}/lib/cubism/Core/lib/${PLATFORM}/${ARCH}/${COMPILER}Live2DCubismCore${CRT}${CUBISM_DEBUG_SUFFIX}.${LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELEASE
        ${PROJECT_SOURCE_DIR}/lib/cubism/Core/lib/${PLATFORM}/${ARCH}/${COMPILER}Live2DCubismCore${CRT}.${LIBRARY_SUFFIX}
        IMPORTED_LOCATION_MINSIZEREL
        ${PROJECT_SOURCE_DIR}/lib/cubism/Core/lib/${PLATFORM}/${ARCH}/${COMPILER}Live2DCubismCore${CRT}.${LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELWITHDEBINFO
        ${PROJECT_SOURCE_DIR}/lib/cubism/Core/lib/${PLATFORM}/${ARCH}/${COMPILER}Live2DCubismCore${CRT}.${LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/lib/cubism/Core/include
)

# GLEW
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glew/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)

# Cubism Framework
set(FRAMEWORK_SOURCE OpenGL)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/cubism/Framework ${CMAKE_CURRENT_BINARY_DIR}/Framework)
target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
target_include_directories(Framework PUBLIC ${PROJECT_SOURCE_DIR}/lib/glew/include)
target_link_libraries(Framework Live2DCubismCore glew_s)

# OpenCV
set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/lib/opencv/build)
find_package(OpenCV REQUIRED)

#dlib
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/dlib ${CMAKE_CURRENT_BINARY_DIR}/dlib)

# Main
if(WIN32)
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
else()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()
target_link_libraries(${PROJECT_NAME} dlib::dlib Framework ${OpenCV_LIBS} Qt::Core Qt::Gui Qt::Widgets Qt::OpenGLWidgets)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/stb ${PROJECT_SOURCE_DIR}/lib/whereami/src ${OpenCV_INCLUDE_DIRS})

if (WIN32)
    set(DEBUG_SUFFIX)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets OpenGL OpenGLWidgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${PROJECT_SOURCE_DIR}/lib/opencv/build/x64/vc15/bin/opencv_world455${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif ()

# Resource Directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/landmark/")

# Landmark File
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/res/shape_predictor_68_face_landmarks.dat"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/landmark/")
