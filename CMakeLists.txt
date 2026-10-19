cmake_minimum_required(VERSION 3.16)

project(Timar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

add_subdirectory(qluent)
find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)


set(TS_FILES Timar_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        tmrhome.cpp
        tmrhome.h
        tmrhome.ui
        ${TS_FILES}
)

# Add any newly created class/source/ui/resource files here, for example:
# myclass.h myclass.cpp mydialog.ui resources.qrc
# Example shown below (replace with your actual filenames):
set(PROJECT_SOURCES
    ${PROJECT_SOURCES}
        asset.h asset.cpp
        assetmanager.h assetmanager.cpp
        subasset.h subasset.cpp
        maintenanceevent.h maintenanceevent.cpp
        maintenancemanager.h maintenancemanager.cpp
        maintenanceschedule.h
        persiandigits.h persiandigits.cpp
        persiancalendarwidget.h persiancalendarwidget.cpp
        navigationpanel.h navigationpanel.cpp
        assetoverviewwidget.h assetoverviewwidget.cpp
        asseteditdialog.h asseteditdialog.cpp
        subassetswidget.h subassetswidget.cpp
        maintenanceeventdialog.h maintenanceeventdialog.cpp
        maintenanceschedulewidget.h maintenanceschedulewidget.cpp
        customicons.h customicons.cpp
        maintenancescheduledialog.h maintenancescheduledialog.cpp
        assetsection.h assetsection.cpp
        assetwidget.h assetwidget.cpp
        assettreewidget.h assettreewidget.cpp
        timelinewidget.h timelinewidget.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Timar
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        asset.h asset.cpp
        assetmanager.h assetmanager.cpp
        subasset.h subasset.cpp
        maintenanceevent.h maintenanceevent.cpp
        maintenancemanager.h maintenancemanager.cpp
        maintenanceschedule.h
        persiandigits.h persiandigits.cpp
        persiancalendarwidget.h persiancalendarwidget.cpp
        navigationpanel.h navigationpanel.cpp
        assetoverviewwidget.h assetoverviewwidget.cpp
        asseteditdialog.h asseteditdialog.cpp
        subassetswidget.h subassetswidget.cpp
        maintenanceeventdialog.h maintenanceeventdialog.cpp
        maintenanceschedulewidget.h maintenanceschedulewidget.cpp
        customicons.h customicons.cpp
        maintenancescheduledialog.h maintenancescheduledialog.cpp
        assetsection.h assetsection.cpp
        assetwidget.h assetwidget.cpp
        assettreewidget.h assettreewidget.cpp
        timelinewidget.h timelinewidget.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Timar APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Timar SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Timar
            ${PROJECT_SOURCES}
            assetmanager.h assetmanager.cpp
            maintenancemanager.h maintenancemanager.cpp
            maintenanceeventwidget.h maintenanceeventwidget.cpp
            persiandigits.h persiandigits.cpp
            persiandateedit.h persiandateedit.cpp
            persiancalendarwidget.h persiancalendarwidget.cpp
            navigationpanel.h navigationpanel.cpp
            assetoverviewwidget.h assetoverviewwidget.cpp
            asseteditdialog.h asseteditdialog.cpp
            datamanager.h datamanager.cpp
            subassetswidget.h subassetswidget.cpp
            maintenanceeventdialog.h maintenanceeventdialog.cpp
            maintenanceschedulewidget.h maintenanceschedulewidget.cpp
            customicons.h customicons.cpp
            maintenancescheduledialog.h maintenancescheduledialog.cpp
            assetsection.h assetsection.cpp
            assetwidget.h assetwidget.cpp
            assettreewidget.h assettreewidget.cpp
            timelinewidget.h timelinewidget.cpp
            maintenancereporttypes.h
            maintenancereport.h
            maintenancereportgenerator.h maintenancereportgenerator.cpp
            reportssection.h reportssection.cpp
            reporteventwidget.h reporteventwidget.cpp
            reportcardwidget.h
            storagepart.h
            storagemanager.h storagemanager.cpp
            storagesection.h storagesection.cpp
            assettree.h assettree.cpp
            settingswidget.h settingswidget.cpp
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()



# (Condition changed: use qt_add_executable only for Qt 6, not Qt 5)

target_link_libraries(Timar PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    qluent)

target_sources(Timar PRIVATE
    resources.qrc
    resources/qlementine_icons.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Timar)
endif()
set_target_properties(Timar PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(MSVC)
    target_compile_options(Timar PRIVATE /utf-8)
endif()

include(GNUInstallDirs)
install(TARGETS Timar
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "MSVC toolset version: ${MSVC_TOOLSET_VERSION}")
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Timar)
endif()
