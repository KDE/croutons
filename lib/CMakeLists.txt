set(croutons_SRC
    futurebase_p.cpp
)
set(croutons_HEADERS
    coroutine_integration_network.h
    coroutine_integration.h
    effects.h
    future.h
    futurebase.h
)

ecm_generate_headers(croutons_camelCase_Headers
  HEADER_NAMES
    CoroutineIntegrationNetwork
    CoroutineIntegration
    Effects
    Future
    FutureBase
  PREFIX Croutons
  REQUIRED_HEADERS croutons_HEADERS
)

add_library(Croutons ${croutons_SRC})

target_link_libraries(Croutons PUBLIC Qt5::Core Qt5::Qml)

set_target_properties(Croutons PROPERTIES VERSION ${CROUTONS_VERSION}
                                               SOVERSION ${CROUTONS_SOVERSION}
                                               EXPORT_NAME Croutons
)

install(FILES
   ${croutons_HEADERS}
   DESTINATION ${KDE_INSTALL_INCLUDEDIR}/croutons COMPONENT Devel)

install(FILES
   ${croutons_camelCase_Headers}
   DESTINATION ${KDE_INSTALL_INCLUDEDIR}/Croutons COMPONENT Devel)

install(TARGETS Croutons EXPORT CroutonsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})