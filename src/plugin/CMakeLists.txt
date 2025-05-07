add_library(backgroundappsplugin SHARED
    backgroundappsmodel.cpp
    backgroundappsmodel.h)
target_include_directories(backgroundappsplugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    
target_link_libraries(backgroundappsplugin PRIVATE
                    Qt::Core
                    Qt::DBus)

ecm_add_qml_module(backgroundappsplugin URI io.github.jinliu.backgroundappsplugin GENERATE_PLUGIN_SOURCE)

ecm_finalize_qml_module(backgroundappsplugin)

install(TARGETS backgroundappsplugin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
