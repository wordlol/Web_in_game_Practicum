#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>

#pragma comment(lib, "ws2_32.lib")


// проверка через cmd 
// curl -X POST http://localhost:8080/leaderboard/submit -H "Content-Type: application/json" -d "{\"player_id\":1,\"score\":100}"


// Хранилище лидерборда
std::map<int, int> leaderboard;  // player_id -> score

// Простейший HTTP ответ
std::string http_response(int status, const std::string& content_type, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << " OK\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;
    return response.str();
}

// Парсинг JSON из строки (минимальный, для демонстрации)
bool parse_json_int(const std::string& json, const std::string& key, int& out) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return false;
    pos += search.size();
    // пропускаем пробелы
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    size_t end = json.find_first_not_of("-0123456789", pos);
    if (end == std::string::npos) end = json.size();
    std::string num_str = json.substr(pos, end - pos);
    out = std::stoi(num_str);
    return true;
}

// Обработка POST /leaderboard/submit
std::string handle_post_submit(const std::string& body) {
    int player_id, score;
    if (!parse_json_int(body, "player_id", player_id) || !parse_json_int(body, "score", score)) {
        return http_response(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    }
    // обновляем счёт
    auto it = leaderboard.find(player_id);
    if (it == leaderboard.end() || score > it->second) {
        leaderboard[player_id] = score;
    }
    return http_response(200, "application/json", "{\"ok\":true}");
}

// Обработка GET /leaderboard/top10
std::string handle_get_top10() {
    // копируем в вектор и сортируем
    std::vector<std::pair<int, int>> sorted(leaderboard.begin(), leaderboard.end());
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    if (sorted.size() > 10) sorted.resize(10);

    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"player_id\":" << sorted[i].first << ",\"score\":" << sorted[i].second << "}";
    }
    json << "]";
    return http_response(200, "application/json", json.str());
}

// Парсинг HTTP-запроса
void handle_client(SOCKET client_socket) {
    char buffer[4096];
    int recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_len <= 0) {
        closesocket(client_socket);
        return;
    }
    buffer[recv_len] = '\0';
    std::string request(buffer);

    // Извлекаем метод и путь
    std::istringstream req_stream(request);
    std::string method, path, version;
    req_stream >> method >> path >> version;

    std::string response;
    if (method == "GET" && path == "/") {
        response = http_response(200, "text/html",
            "<html><body><h1>Legacy C++ HTTP Server</h1><p>Use /leaderboard/top10 or POST /leaderboard/submit</p></body></html>");
    }
    else if (method == "GET" && path == "/leaderboard/top10") {
        response = handle_get_top10();
    }
    else if (method == "POST" && path == "/leaderboard/submit") {
        // Ищем тело запроса (после двух \r\n\r\n)
        size_t body_start = request.find("\r\n\r\n");
        std::string body;
        if (body_start != std::string::npos) {
            body = request.substr(body_start + 4);
        }
        response = handle_post_submit(body);
    }
    else {
        response = http_response(404, "text/plain", "Not Found");
    }

    send(client_socket, response.c_str(), response.size(), 0);
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "socket failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Legacy HTTP Server running on port 8080" << std::endl;
    std::cout << "Test with: curl http://localhost:8080/leaderboard/top10" << std::endl;

    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "accept failed" << std::endl;
            continue;
        }
        handle_client(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}