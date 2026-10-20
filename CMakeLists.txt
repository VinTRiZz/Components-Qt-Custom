COMPONENTS_CCR_ADD_QT(5 Core Widgets Network Sql)

# Ignore QtCharts (to be edited in future)
COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom "1.0.0"
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
)
get_target_property(CURRENT_SOURCES Components-Qt-Custom SOURCES)
file(GLOB_RECURSE ChartsSources "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/*")
list(REMOVE_ITEM CURRENT_SOURCES ${ChartsSources})
set_target_properties(Components-Qt-Custom PROPERTIES SOURCES "${CURRENT_SOURCES}")



COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Logger)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Maths)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Common)
