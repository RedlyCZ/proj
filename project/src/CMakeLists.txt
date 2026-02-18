cmake_minimum_required(VERSION 3.15)

project(FinancialAnalyzer)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3)
FetchContent_MakeAvailable(json)

if(WIN32)
    # Na Windows vynuť použití jejich systémového SSL (aby to nechtělo certifikáty)
    set(CPR_FORCE_WINSSL_BACKEND ON CACHE BOOL "" FORCE)
    set(CPR_FORCE_OPENSSL_BACKEND OFF CACHE BOOL "" FORCE)
    set(CPR_ENABLE_SSL ON CACHE BOOL "" FORCE)
endif()

set(CPR_USE_SYSTEM_CURL OFF CACHE BOOL "" FORCE)
set(BUILD_CPR_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 1.11.0)
FetchContent_MakeAvailable(cpr)

add_executable(financialAnalyzer main.cpp apiMiddleman.cpp)

target_link_libraries(financialAnalyzer PRIVATE nlohmann_json::nlohmann_json cpr::cpr)