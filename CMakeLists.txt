# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

COMPONENTS_CCR_ADD_QT(5 Core Widgets Network)

# No need in QtCharts for every single project
find_package(Qt5 COMPONENTS Charts QUIET)
if(TARGET Qt5::Charts)
    COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        Qt5::Network
    )
else()
    COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
    )
    get_target_property(CURRENT_SOURCES Qt-Custom SOURCES)
    file(GLOB_RECURSE ChartsSources "${CMAKE_CURRENT_SOURCE_DIR}/src/Widgets/QtCharts/*")
    list(REMOVE_ITEM CURRENT_SOURCES ${ChartsSources})
    set_target_properties(Qt-Custom PROPERTIES SOURCES "${CURRENT_SOURCES}")
endif()



COMPONENTS_LINK_COMPONENT(Qt-Custom Logger)
COMPONENTS_LINK_COMPONENT(Qt-Custom Maths)
COMPONENTS_LINK_COMPONENT(Qt-Custom ExtraClasses)
COMPONENTS_LINK_COMPONENT(Qt-Custom Common)
