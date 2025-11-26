find_package(pybind11 REQUIRED)

pybind11_add_module(pycalibmar bindings.cpp)

target_link_libraries(pycalibmar PRIVATE calibmar)
target_compile_features(pycalibmar PUBLIC cxx_std_17)
