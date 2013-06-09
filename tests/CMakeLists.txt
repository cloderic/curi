include_directories(
    ../src
    catch
)

add_executable(curi_tests 
  Settings.cpp
  ParseFullUri.cpp 
  ParsePath.cpp
  ParseQuery.cpp
  UrlDecode.cpp)

target_link_libraries(curi_tests curi)

if(MSVC)
    set_target_properties(curi_tests PROPERTIES
        COMPILE_FLAGS "/wd4297 /wd4800")
endif()

add_test(
  NAME Settings 
  COMMAND curi_tests -t Settings/*)

add_test(
  NAME ParseFullUri 
  COMMAND curi_tests -t ParseFullUri/*)

add_test(
  NAME UrlDecode 
  COMMAND curi_tests -t UrlDecode/*)