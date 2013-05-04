include_directories(
    ../src
    catch
)

add_executable(curi_tests Framework.cpp Parsing.cpp)

target_link_libraries(curi_tests curi)

add_test(
  NAME Framework 
  COMMAND curi_tests -t Framework/*)

add_test(
  NAME Parsing 
  COMMAND curi_tests -t Parsing/*)