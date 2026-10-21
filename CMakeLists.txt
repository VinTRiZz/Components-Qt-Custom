COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom "1.0.0")
option(COMPONENTS_SW_qt_charts "" OFF)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Common)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Logger)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Maths)
if (COMPONENTS_IS_ENABLED_QT)
    target_link_libraries(Components-Qt-Custom PRIVATE Qt5::Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)
endif()
if (COMPONENTS_SW_qt_charts)
    target_link_libraries(Components-Qt-Custom PRIVATE Qt5::Charts)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/callout.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/callout.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/chartwidget.ui"
    )
endif()
