COMPONENTS_CCR_ADD_QT(5 Gui Network Sql Widgets)
COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom "1.0.0")
option(COMPONENTS_SW_qtcustom_models "" ON)
option(COMPONENTS_SW_qtcustom_qt_charts "" OFF)
option(COMPONENTS_SW_qtcustom_graphicscene "" OFF)
option(COMPONENTS_SW_qtcustom_web "" OFF)
option(COMPONENTS_SW_qtcustom_orm "" OFF)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Logger)
if (COMPONENTS_IS_ENABLED_QT)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_IS_ENABLED_QT)
    target_link_libraries(Components-Qt-Custom PRIVATE Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)
endif()
if (COMPONENTS_SW_qtcustom_models)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_models)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/proxy/treegroupingproxymodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/proxy/treegroupingproxymodel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/abstractsearchengine.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/abstractsearchengine.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/searchproxymodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Models/search/searchproxymodel.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_qt_charts)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_qt_charts)
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
if (COMPONENTS_SW_qtcustom_graphicscene)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_graphicscene)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Maths)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicScene"
    )
endif()
if (COMPONENTS_SW_qtcustom_web)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_web)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.hpp"
    )
endif()
if (COMPONENTS_SW_qtcustom_orm)
    target_compile_definitions(Components-Qt-Custom PRIVATE COMPONENTS_SW_qtcustom_orm)
    COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.hpp"
    )
endif()
