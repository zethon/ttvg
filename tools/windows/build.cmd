
cmake --build . --config %CONFIG% --target install
cpack -G NSIS64 -C %CONFIG% --config TTVGCPackConfig.cmake --verbose