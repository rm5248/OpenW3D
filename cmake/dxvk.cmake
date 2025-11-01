file(DOWNLOAD "https://github.com/doitsujin/dxvk/releases/download/v2.7.1/dxvk-native-2.7.1-steamrt-sniper.tar.gz"
    "${CMAKE_BINARY_DIR}/dxvk-native-2.7.1-steamrt-sniper.tar.gz"
    EXPECTED_HASH MD5=06668c98da09fe63e0b667824ccaa2a0
     SHOW_PROGRESS
     STATUS download_status
     LOG download_log)

if(NOT download_status EQUAL 0)
    message(FATAL_ERROR "Failed to download dxvk: ${download_log}")
endif()

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/dxvk")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/dxvk-native-2.7.1-steamrt-sniper.tar.gz"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/dxvk"
)

add_library(dxvk INTERFACE)

target_include_directories(dxvk INTERFACE "${CMAKE_BINARY_DIR}/dxvk/usr/include/dxvk")
