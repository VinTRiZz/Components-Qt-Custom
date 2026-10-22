COMPONENTS_CCR_ADD_QT(5 Gui Network Sql Widgets)
COMPONENTS_CONFIGURE_COMPONENT(Qt-Custom "1.0.0"
    Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets
)
option(COMPONENTS_SW_qtcustom_models "" ON)
option(COMPONENTS_SW_qtcustom_qt_charts "" OFF)
option(COMPONENTS_SW_qtcustom_graphicscene "" OFF)
option(COMPONENTS_SW_qtcustom_web "" OFF)
option(COMPONENTS_SW_qtcustom_orm "" OFF)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Common)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom ExtraClasses)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Logger)
COMPONENTS_LINK_COMPONENT(Components-Qt-Custom Maths)
if (NOT COMPONENTS_SW_qtcustom_models)
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
if (NOT COMPONENTS_SW_qtcustom_graphicscene)
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicScene"
    )
endif()
if (NOT COMPONENTS_SW_qtcustom_web)
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpclientbase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Web/http/httpcommon.hpp"
    )
endif()
if (NOT COMPONENTS_SW_qtcustom_orm)
    COMPONENTS_REMOVE_SOURCES(Components-Qt-Custom
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitembase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tableitemmaster.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Orm/tablesourcemodel.hpp"
    )
endif()
