project("BonsaiViewer"
    VERSION 0.0.0
    LANGUAGES CXX
)

add_executable(BonsaiViewer)
add_executable(Bonsai::Viewer ALIAS BonsaiViewer)
target_compile_features(BonsaiViewer PRIVATE cxx_std_17)
target_sources(BonsaiViewer
    PRIVATE
        src/BonsaiViewer.cpp
        src/BonsaiViewer.hpp
)
target_link_libraries(BonsaiViewer
    PRIVATE
        Bonsai::Base
        Bonsai::Runtime
)
