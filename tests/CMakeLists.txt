include_directories(
    ../src
    catch
)

add_executable(curi_tests Framework.cpp Parsing.cpp)

target_link_libraries(curi_tests curi)

if(MSVC)
    set_target_properties(curi_tests PROPERTIES
        COMPILE_FLAGS /wd4297)
endif()

add_test(
  NAME Framework 
  COMMAND curi_tests -t Framework/*)

add_test(
  NAME Parsing 
  COMMAND curi_tests -t Parsing/*)