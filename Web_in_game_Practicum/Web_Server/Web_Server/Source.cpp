#include <iostream>
#include <string>
#include "httplib.h"  // скачать с GitHub: https://github.com/yhirose/cpp-httplib/blob/master/httplib.h

using namespace std;

int main() {
    // Создаём HTTP клиент, указываем сервер (localhost, порт 8000)
    httplib::Client cli("http://localhost:8000");

    // 1. Отправляем результат игры (POST /leaderboard/submit)
    string json = R"({"player_id": 1, "score": 2500})";
    auto post_res = cli.Post("/leaderboard/submit", json, "application/json");
    if (post_res && post_res->status == 200) {
        cout << "Score sent successfully!" << endl;
    }
    else {
        cout << "Failed to send score. Status: " << (post_res ? to_string(post_res->status) : "no response") << endl;
    }

    // 2. Получаем таблицу лидеров (GET /leaderboard/top10)
    auto get_res = cli.Get("/leaderboard/top10");
    if (get_res && get_res->status == 200) {
        cout << "\n--- Leaderboard Top 10 ---\n" << get_res->body << endl;
    }
    else {
        cout << "Failed to get leaderboard." << endl;
    }

    return 0;
}