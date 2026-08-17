# CMake generated Testfile for 
# Source directory: D:/Projects/Cube-World-Mod-Launcher/tests
# Build directory: D:/Projects/Cube-World-Mod-Launcher/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(AllTests "D:/Projects/Cube-World-Mod-Launcher/tests/Debug/test_runner.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;29;add_test;D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(AllTests "D:/Projects/Cube-World-Mod-Launcher/tests/Release/test_runner.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;29;add_test;D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(AllTests "D:/Projects/Cube-World-Mod-Launcher/tests/MinSizeRel/test_runner.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;29;add_test;D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(AllTests "D:/Projects/Cube-World-Mod-Launcher/tests/RelWithDebInfo/test_runner.exe")
  set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;29;add_test;D:/Projects/Cube-World-Mod-Launcher/tests/CMakeLists.txt;0;")
else()
  add_test(AllTests NOT_AVAILABLE)
endif()
