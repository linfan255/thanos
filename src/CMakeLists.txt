set(SERVER_SOURCE
        ${PROJECT_SOURCE_DIR}/src/buffer.cpp
        ${PROJECT_SOURCE_DIR}/src/configure.cpp
        ${PROJECT_SOURCE_DIR}/src/connection.cpp
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        ${PROJECT_SOURCE_DIR}/src/fd_handler.cpp
        ${PROJECT_SOURCE_DIR}/src/http_connection.cpp
        ${PROJECT_SOURCE_DIR}/src/http_message.cpp
        ${PROJECT_SOURCE_DIR}/src/locker.cpp
        ${PROJECT_SOURCE_DIR}/src/server.cpp
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        )

add_executable(http_server ${SERVER_SOURCE})
