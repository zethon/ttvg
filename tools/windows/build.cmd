
cmake --build . --config %CONFIG% --target install
cpack -G NSIS64 -C %CONFIG% --config TTVG.CPackConfig.cmake --verbose