# Thread-pool

Простой thread-pool на С++.

## Возможности

- **C++23**
- **Perfect-forwarding**
- **std::future** для получения результата
- Логгирование ошибок

## Сборка
```
mkdir build && cd build
cmake  ..
make -j$(nproc)
./Thread-pool
```
