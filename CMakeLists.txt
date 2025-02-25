cmake_minimum_required(VERSION 3.5)

project(qwidget-lua-editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
        Widgets
        Svg
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Widgets
        Svg
)

set(PROJECT_SOURCES
        # Dont forget to add this to see the icons on editor
        MagiaEditor/EditorRes.qrc
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
    views/CodeEditor.h
    views/App.h
    views/GenericEditor.h views/AIChatWidget.h views/FileExplorerWidget.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

add_subdirectory(MagiaEditor)
add_subdirectory(ais)

target_include_directories(${PROJECT_NAME} PUBLIC
    MagiaEditor/include
)

# This policy is needed to link the qt libraries to the editor
cmake_policy(SET CMP0079 NEW)
target_link_libraries(mg_editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    AIStreamer
)
#--------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    mg_editor
    AIStreamer
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
