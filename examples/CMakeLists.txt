include_directories(../src)

add_executable(decode_url common.h decode_url.c)
add_executable(split_uri common.h split_uri.c)

target_link_libraries(decode_url curi)
target_link_libraries(split_uri curi)
