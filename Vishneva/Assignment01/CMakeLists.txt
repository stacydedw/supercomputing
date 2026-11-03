find_package(OpenMP REQUIRED)

add_executable(minirt-test minirt_test_openmp.cpp)

target_link_libraries(minirt-test
    PUBLIC
    minirt
    OpenMP::OpenMP_CXX
)