#include <iostream>
#include <string>
#include "httplib.h"

using namespace std;

int main() {
    httplib::Client cli("http://localhost:8000");

    int command = 9;

    while (true)
    {
        system("cls");
        cout << "commands: \n" << 0 << " Exit" << endl;
        cout << 1 << " Post" << endl;
        cout << 2 << " Get" << endl;

        cin >> command;

        system("cls");
        if (command == 0)
            break;
        else if (command == 1)
        {
            int playerid = 0;
            int score = 0;
            
            cout << "playerid: ";
            cin >> playerid;

            cout << "\n" << "score: ";
            cin >> score;
            system("cls");

            string S1 = R"({"player_id": )" ;
            string S2 = R"(, "score": )";
            string json = S1 + to_string(playerid) + S2 + to_string(score) + "}";
            
            cli.Post("/leaderboard/submit", json.c_str(), "application/json");
        }
        else if (command == 2)
        {
            int target_id;
            cout << "player_id: ";
            cin >> target_id;

            auto res = cli.Get("/leaderboard/top10");
            if (res && res->status == 200)
            {
                std::string body = res->body;
                std::string search = "\"player_id\":" + std::to_string(target_id);
                size_t pos = body.find(search);
                if (pos != std::string::npos)
                {
                    pos = body.find("\"score\":", pos);
                    if (pos != std::string::npos)
                    {
                        pos += 8;
                        while (pos < body.size() && std::isspace(body[pos])) ++pos;
                        size_t end = pos;
                        while (end < body.size() && (std::isdigit(body[end]) || body[end] == '-')) ++end;
                        int score = std::stoi(body.substr(pos, end - pos));
                        std::cout << "Player " << target_id << " has score: " << score << std::endl;
                    }
                }
            }
            system("pause");
        }

    }

    return 0;
}