#region Project Configuration
cmake_minimum_required(VERSION 3.16)

project(blazestudioprovs VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#endregion

#region CPM Package Manager
include(CPM.cmake)

# Configure CPM cache directory (optional)
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm_cache" CACHE PATH "Directory to store CPM packages")
#endregion

#region OpenCV Configuration
# Try different possible OpenCV paths for MSVC 2022
set(OpenCV_DIR "C:/opencv/build/x64/vc17/lib" CACHE PATH "OpenCV directory")
if(NOT EXISTS ${OpenCV_DIR})
    set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
endif()
if(NOT EXISTS ${OpenCV_DIR})
    set(OpenCV_DIR "C:/opencv/build")
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)

message(STATUS "OpenCV found: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
#endregion

#region ONNX Runtime Configuration
# Download pre-built ONNX Runtime binaries via CPM
CPMAddPackage(
    NAME onnxruntime
    URL https://github.com/microsoft/onnxruntime/releases/download/v1.17.1/onnxruntime-win-x64-1.17.1.zip
    DOWNLOAD_ONLY YES
)

if(onnxruntime_ADDED)
    set(ONNXRUNTIME_ROOT_DIR ${onnxruntime_SOURCE_DIR})
    set(ONNXRUNTIME_INCLUDE_DIRS ${ONNXRUNTIME_ROOT_DIR}/include)
    set(ONNXRUNTIME_LIB_DIR ${ONNXRUNTIME_ROOT_DIR}/lib)
    
    # Find the library
    find_library(ONNXRUNTIME_LIBRARY
        NAMES onnxruntime
        PATHS ${ONNXRUNTIME_LIB_DIR}
        NO_DEFAULT_PATH
    )
    
    if(ONNXRUNTIME_LIBRARY)
        set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_LIBRARY})
        message(STATUS "ONNX Runtime found: ${ONNXRUNTIME_LIBRARY}")
        message(STATUS "ONNX Runtime include dirs: ${ONNXRUNTIME_INCLUDE_DIRS}")
        
        # Copy DLL to output directory on Windows
        file(GLOB ONNXRUNTIME_DLLS "${ONNXRUNTIME_LIB_DIR}/*.dll")
        if(ONNXRUNTIME_DLLS)
            file(COPY ${ONNXRUNTIME_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
            message(STATUS "Copied ONNX Runtime DLLs to build directory")
        endif()
    else()
        message(WARNING "ONNX Runtime library not found in ${ONNXRUNTIME_LIB_DIR}")
        set(ONNXRUNTIME_LIBS "")
    endif()
else()
    message(WARNING "Failed to download ONNX Runtime")
    set(ONNXRUNTIME_LIBS "")
endif()
#endregion

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(OpenGL REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        videoglwidget.cpp
        videoglwidget.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(blazestudioprovs
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET blazestudioprovs APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(blazestudioprovs SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(blazestudioprovs
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(blazestudioprovs PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    ${OpenCV_LIBS}
    ${ONNXRUNTIME_LIBS}
    OpenGL::GL
)

target_include_directories(blazestudioprovs PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_INCLUDE_DIRS}
)

message(STATUS "Linking libraries: ${OpenCV_LIBS}")

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.blazestudioprovs)
endif()
set_target_properties(blazestudioprovs PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS blazestudioprovs
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(blazestudioprovs)
endif()
