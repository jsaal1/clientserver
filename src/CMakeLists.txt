cmake_minimum_required(VERSION 3.14)

target_sources(clientserver
    PRIVATE
        connection.cc
        server.cc
)

set(app_sources
    messagehandler.cc
    newsserver.cc
)

add_executable(server_mem
    ${app_sources}
    inmemorydatabase.cc
    server_mem_main.cc
)

add_executable(server_disk
    ${app_sources}
    diskdatabase.cc
    server_disk_main.cc
)

add_executable(client_main
    ${app_sources}
    client_main.cc
)

foreach(target server_mem server_disk client_main)
    target_compile_options(${target}
        PRIVATE ${clientserver_sanitizer_options}
    )
    target_include_directories(${target}
        PUBLIC  ${clientserver_public_include_dirs}
        PRIVATE ${clientserver_private_include_dirs}
    )
    target_link_libraries(${target}
        PUBLIC  clientserver
        PRIVATE ${clientserver_sanitizer_options}
    )
endforeach()

install(TARGETS server_mem server_disk client_main clientserver)
