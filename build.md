Протестированы два вида сборки под CMake:

1) Под Visual Studio 17 2022:
cmake -S . -B build_vs
cmake --build build_vs --config MinSizeRel

2) Под ninja (использовался компилятор Qt-MinGW gcc)
В cmake-gui выбрать cmake-multi. Конфигурация, к примеру, Release. После генерации пишем в консоли:
ninja -C build_ninja (или другая директория)

Запуск приложения:
1) ./build_vs/MinSizeRel/BMP.exe sample_24.bmp
Дальше сохранение файла уже в консоли приложения

2) ./build_ninja/Release/BMP.exe sample_24.bmp
Дальше сохранение файла уже в консоли приложения
