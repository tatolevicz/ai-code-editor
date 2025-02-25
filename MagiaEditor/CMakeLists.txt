project(mg_editor VERSION 0.1 LANGUAGES CXX)

set(PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MgStyles.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MagiaEditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MagiaDebugger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ConsoleOutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MagiaEditorWidget.h
        )

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MagiaEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MagiaDebugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MagiaEditorWiget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConsoleOutput.cpp
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/scintilla)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sol2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/lexilla)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lexilla/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lexilla/access
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Lua/lua-5.4.6/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    scintilla-qt-edit
    lua_static
    sol2
    lexilla
)
