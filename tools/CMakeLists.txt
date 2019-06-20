cmake_minimum_required(VERSION 3.11)

configure_file( ins.in ins @ONLY )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ins DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
         PERMISSIONS OWNER_READ
                     OWNER_WRITE
                     OWNER_EXECUTE
                     GROUP_READ
                     GROUP_EXECUTE
                     WORLD_READ
                     WORLD_EXECUTE
)

