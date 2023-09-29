

target_sources(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/imgui.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_internal.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_widgets.cpp
)
