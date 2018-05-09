set(SOURCE_FILE
        buffer.cpp 
        http_message.cpp 
        http_request.cpp 
        http_response.cpp 
        client.cpp 
        main.cpp)

add_executable(server ${SOURCE_FILE})
