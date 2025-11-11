### Андреев Александр Сергеевич
structure
```
cpp-remote-runner/
├─ CMakeLists.txt                 ← сборка всего проекта (или другой билд-скрипт)
├─ README.md                      ← как собрать/запустить, примеры команд консоли
├─ LICENSE                        ← если нужно
├─ common/                        ← общее для клиента и сервера
│  ├─ types.hpp                   ← базовые типы: идентификаторы, статусы, ошибки
│  ├─ message.hpp                 ← описание сообщений (структура полей)
│  ├─ serialization.hpp           ← преобразование сообщений в строки и обратно
│  ├─ net.hpp                     ← общая оболочка поверх TCP (подключение/отправка/приём)
│  ├─ command.hpp                 ← базовый класс «Команда» и интерфейсы
│  ├─ result.hpp                  ← контейнер для результата выполнения
│  └─ utils.hpp                   ← мелкие утилиты, константы, валидации
├─ server/
│  ├─ main.cpp                    ← точка входа сервера (создание и запуск)
│  ├─ server_app.hpp              ← «сердце» сервера: хранение клиентов, отправка команд
│  ├─ console.hpp                 ← консольный интерфейс (REPL) в стиле diskpart
│  ├─ client_registry.hpp         ← учёт подключённых клиентов (id, адрес, теги, статус)
│  ├─ sessions.hpp                ← управление активными соединениями с клиентами
│  └─ job_manager.hpp             ← учёт задач: статусы, таймауты, поиск по id
├─ client/
│  ├─ main.cpp                    ← точка входа клиента
│  ├─ client_app.hpp              ← цикл: подключиться → ждать команд → отвечать
│  ├─ executor.hpp                ← выполнение команд на машине (безопасный слой)
│  └─ sysinfo.hpp                 ← сбор базовой информации о системе (OS, hostname и т. п.)
├─ tests/
│  ├─ unit/
│  │  ├─ test_serialization.cpp   ← проверка формата сообщений
│  │  ├─ test_parser.cpp          ← разбор консольных команд (сервер)
│  │  ├─ test_job_states.cpp      ← переходы статусов задач
│  │  └─ test_filters.cpp         ← фильтры/поиск клиентов в реестре
│  └─ integration/
│     ├─ test_server_client.cpp   ← «сервер↔несколько клиентов»: обмен, PING/RUN/EXIT
│     └─ test_reconnect.cpp       ← отключение повторное подключение клиента                     
└─ .gitlab-ci.yml)
   └─ ci.yml

clang++ -std=c++20 -I /opt/homebrew/Cellar/boost/1.89.0/include  -L /opt/homebrew/Cellar/boost/1.89.0/lib server/server_core.cpp server/connection_card.cpp server/server_cli.cpp  -lboost_chrono -lboost_filesystem  -lboost_thread -o serv
clang++ -Wall -std=c++20 -I /opt/homebrew/Cellar/boost/1.89.0/include  -L /opt/homebrew/Cellar/boost/1.89.0/lib client/client_core.cpp  -lboost_chrono -lboost_filesystem  -lboost_thread -o cl  