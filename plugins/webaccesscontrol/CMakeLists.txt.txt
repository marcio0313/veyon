# plugins/webaccesscontrol/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(WebAccessControlPlugin)

# Coleta de arquivos fonte e headers
set(SOURCES
    WebAccessControlPlugin.cpp
)

set(HEADERS
    WebAccessControlPlugin.h
)

# Formulários Qt Designer e arquivos de recursos (.qrc), se houver
set(FORMS
    # WebAccessControlControlDialog.ui
)

set(RESOURCES
    # resources/webaccesscontrol.qrc
)

# Adiciona o plugin usando a macro nativa do framework Veyon
veyon_add_plugin(webaccesscontrol
    SOURCES
        ${SOURCES}
        ${HEADERS}
        ${FORMS}
        ${RESOURCES}
    LINK_LIBRARIES
        veyon-core
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
)

# Vinculações específicas para APIs nativas do Windows (Firewall / WinINet / DNS)
if(WIN32)
    target_link_libraries(webaccesscontrol PRIVATE ole32 oleaut32 dnsapi)
endif()