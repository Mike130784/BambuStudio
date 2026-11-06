cmake_minimum_required(VERSION 3.20)
project(CustomStudioCertificateResearch LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT IS_DIRECTORY "${OBN_SOURCE}/include/obn")
  message(FATAL_ERROR "Set OBN_SOURCE to the separately pinned upstream checkout")
endif()
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
enable_testing()
add_executable(upstream_signing_test
  "${OBN_SOURCE}/tests/signing_test.cpp"
  "${OBN_SOURCE}/tests/config_stub.cpp"
  "${OBN_SOURCE}/src/json_lite.cpp"
  "${OBN_SOURCE}/src/signing.cpp"
  "${OBN_SOURCE}/src/log.cpp"
  "${OBN_SOURCE}/src/os_compat.cpp")
target_include_directories(upstream_signing_test PRIVATE "${OBN_SOURCE}/include")
target_link_libraries(upstream_signing_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
  target_link_libraries(upstream_signing_test PRIVATE ws2_32 crypt32)
endif()
add_executable(certificate_trust_lab certificate_trust_lab.cpp)
target_link_libraries(certificate_trust_lab PRIVATE OpenSSL::Crypto)
if(WIN32)
  target_link_libraries(certificate_trust_lab PRIVATE ws2_32 crypt32)
endif()
add_test(NAME upstream_signing_offline COMMAND upstream_signing_test)
add_test(NAME certificate_trust_offline COMMAND certificate_trust_lab "${VENDOR_ROOTS}")
set_tests_properties(upstream_signing_offline certificate_trust_offline PROPERTIES TIMEOUT 30)
