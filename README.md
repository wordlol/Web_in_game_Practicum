# 🎮 Game Backend + Admin Panel + C++ Client Demo

Возникнут вопросы, пишите, мой tg: @MISATOMATCH

Этот проект демонстрирует взаимодействие игрового клиента на C++, веб-бекенда на FastAPI и админ-панели на Vue 3 + Vuetify. Он показывает, как современные веб-технологии используются в игровой индустрии: лидерборды, онлайн-конфигурация баланса через админку, REST API.

## 🧰 Требования

- **Python 3.14+** (с pip)
- **Node.js 20+** (с npm)
- **Docker Desktop** (для PostgreSQL)
- **Компилятор C++17** (Visual Studio 2022+)
- **Git** (для клонирования)

Все необходимые библиотеки устанавливаются автоматически через pip/npm; дополнительные библиотеки для C++ не требуются (используется только `httplib.h` – один заголовочный файл).

## 🚀 Быстрый старт

### 1. Клонируйте репозиторий

```bash
git clone https://github.com/wordlol/Web_in_game_Practicum.git
```

### 2. Запустите базу данных (PostgreSQL)
Требуется скачать Docker ```https://www.docker.com/products/docker-desktop/```

Запуск сервера через Проект Web_Server

```PowerShell (Python 3.14.4 environment)
cd С:/...../Web_in_game_Practicum-test1/Web_Server

docker run --name some-postgres -e POSTGRES_PASSWORD=postgres -e POSTGRES_DB=game_db -d -p 5432:5432 postgres:15

python -m uvicorn main:app --reload --port 8000
```

после чего сервер будет доступен по адресу http://localhost:8000. 
Swagger документация: http://localhost:8000/docs

### 3. Запуск клиента (С++)

- Запускайте исполняемый файл Web_client\Web_client\Debug\Web_client.exe
- В терменале программы можно давать команды ```POST``` и ```Get```
- смотреть результат можно по адресу http://localhost:8000/leaderboard/top10

### 4. Админ панель

Запускаем проект admin, там в терменаде PowerShell нужно поставить npm

```PowerShell
cd С:/...../Web_in_game_Practicum-test1/admin
npm install --legacy-peer-deps
```
И запустить панель

```PowerShell
npm run dev
```

- Панель админа по адресу http://localhost:5173
- Смотреть результат можно по адресу http://localhost:8000/config/weapon/sword

### 5. Просто легаси способ запустить сервак на С++
Проект Web_Legacy
МЕТОД ДЛЯ СТАРИКОВ

Запускаешь прогу

чтобы сделать ```POST``` в cmd.exe пишешь
```cmd
curl -X POST http://localhost:8080/leaderboard/submit -H "Content-Type: application/json" -d "{\"player_id\":1,\"score\":100}"
```


