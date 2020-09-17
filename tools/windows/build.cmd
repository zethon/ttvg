
cmake --build . --config %configuration% --target install
cpack -G NSIS64 -C %configuration% --config TTVG.CPackConfig.cmake --verbose