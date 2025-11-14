
CXX ?= clang++

BOOST_VERSION = 1.89.0

BOOST_ROOT_MAC  = /opt/homebrew/Cellar/boost/$(BOOST_VERSION)
BOOST_INC_MAC   = $(BOOST_ROOT_MAC)/include
BOOST_LIB_MAC   = $(BOOST_ROOT_MAC)/lib


BOOST_ROOT_WIN ?= C:/local/boost_1_89_0
BOOST_INC_WIN  = $(BOOST_ROOT_WIN)/include
BOOST_LIB_WIN  = $(BOOST_ROOT_WIN)/lib

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    BOOST_INC_DEFAULT = $(BOOST_INC_MAC)
    BOOST_LIB_DEFAULT = $(BOOST_LIB_MAC)
else ifeq ($(UNAME_S),Linux)
    BOOST_INC_DEFAULT =
    BOOST_LIB_DEFAULT =
else
    BOOST_INC_DEFAULT = $(BOOST_INC_WIN)
    BOOST_LIB_DEFAULT = $(BOOST_LIB_WIN)
endif

BOOST_INC ?= $(BOOST_INC_DEFAULT)
BOOST_LIB ?= $(BOOST_LIB_DEFAULT)

CXXFLAGS_COMMON = -std=c++20 -Wall
CXXFLAGS        = $(CXXFLAGS_COMMON) -I $(BOOST_INC)
LDFLAGS         = -L $(BOOST_LIB) -lboost_chrono -lboost_filesystem -lboost_thread

SERVER_SRC = server/server_core.cpp server/connection_card.cpp server/server_cli.cpp
CLIENT_SRC = client/client_core.cpp

TEST_CLIENT_SRC = tests/test_client.cpp
TEST_SERVER_SRC = tests/test_server.cpp

.PHONY: all test clean install_boost_mac install_boost_win

all: serv cl

serv: $(SERVER_SRC) $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -DPROD $(SERVER_SRC) $(LDFLAGS) -o $@

cl: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -DPROD $(CLIENT_SRC) $(LDFLAGS) -o $@

launch: 
	./serv

test: test_client test_server

test_client: $(CLIENT_SRC)  $(TEST_CLIENT_SRC)
	$(CXX) $(CXXFLAGS_COMMON) -I $(BOOST_INC) $(CLIENT_SRC) $(TEST_CLIENT_SRC) $(LDFLAGS) -o $@
	./$@

test_server: server/connection_card.cpp server/server_cli.cpp server/server_core.cpp $(TEST_SERVER_SRC)
	$(CXX) $(CXXFLAGS_COMMON) -I $(BOOST_INC) server/connection_card.cpp server/server_cli.cpp server/server_core.cpp $(TEST_SERVER_SRC) $(LDFLAGS) -o $@
	./$@

clean:
	rm -f serv cl test_client test_server

install_boost_mac:
	brew update
	brew install boost
	@echo "Убедись, что установилась версия $(BOOST_VERSION) и путь совпадает с $(BOOST_ROOT_MAC)"

install_boost_win:
	@echo "== Установка Boost $(BOOST_VERSION) через vcpkg =="
	@if not exist vcpkg ( \
		echo "Скачиваю vcpkg..." && \
		git clone https://github.com/microsoft/vcpkg.git && \
		cd vcpkg && bootstrap-vcpkg.bat \
	)
	@echo "Устанавливаю Boost $(BOOST_VERSION)..."
	@vcpkg\vcpkg install boost:x64-windows --recurse
	@vcpkg\vcpkg install boost-filesystem:x64-windows
	@vcpkg\vcpkg install boost-thread:x64-windows
	@vcpkg\vcpkg install boost-chrono:x64-windows
	@echo "Boost установлен!"
	@echo "Устанавливаю пути BOOST_INC и BOOST_LIB"
	@setx BOOST_INC_WIN "vcpkg\installed\x64-windows\include"
	@setx BOOST_LIB_WIN "vcpkg\installed\x64-windows\lib"
	@echo "BOOST_INC_WIN=vcpkg\\installed\\x64-windows\\include"
	@echo "BOOST_LIB_WIN=vcpkg\\installed\\x64-windows\\lib"
	@echo "Готово: теперь можно выполнять make all"