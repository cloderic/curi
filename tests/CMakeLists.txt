include_directories(
    ../src
    catch
)

add_executable(curi_tests UriParsing.cpp)

target_link_libraries(curi_tests curi)

add_test(
  NAME UriParsing 
  COMMAND curi_tests)