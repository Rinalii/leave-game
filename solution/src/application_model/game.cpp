#include "game.h"
#include <iostream>

#include <cstdint>

using namespace std::literals;

namespace model {
    void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index);
    if (!inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    }
    try {
        maps_.emplace_back(std::move(map));
    } catch (...) {
        map_id_to_index_.erase(it);
        throw;
    }
}

const Game::Maps& Game::GetMaps() const noexcept {
    return maps_;
}

std::shared_ptr<Map> Game::FindMap(const Map::Id& id) const noexcept  {
    if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
        return std::make_shared<Map>(maps_.at(it->second));
    }
    return nullptr;
}

std::shared_ptr<GameSession> Game::GetGameSession(const Map::Id& id) {
    auto map = FindMap(id);
    return GetGameSession(map);
}

std::shared_ptr<GameSession> Game::GetGameSession(std::shared_ptr<Map> map) {
    if (map == nullptr) {
        throw std::invalid_argument("Map doesn't exist");
    }
    const auto& id = map->GetId();

    for(auto& [session, _] : game_sessions_to_players_tok_) {
        if(session->GetMap()->GetId() == id) {
            return session;
        }
    }

    auto game_session = std::make_shared<GameSession>(map);
    game_sessions_to_players_tok_[game_session];

    return game_session;
}

void Game::PrintMaps() const {
    for (Map map : maps_) {
        std::cout << "Map: " << map.GetName() << std::endl;
        for (auto road : map.GetRoads()) {
            std::cout << "{" << road.GetStart().x << ", " << road.GetStart().y << "} - {" << road.GetEnd().x << ", " << road.GetEnd().y << "}" << std::endl;
        }
        std::cout << std::endl;
    }
}

void Game::GenerateLoot(double time_delta_sec) {
    
    for(auto& [session, _] : game_sessions_to_players_tok_) {
        const std::vector<std::shared_ptr<Dog>> dogs = session->GetDogs();
        uint64_t loot_count = session->GetSizeLootObjects();
        uint64_t looter_count = dogs.size();
        uint64_t number = loot_generator_.Generate(std::chrono::milliseconds(static_cast<long long>(time_delta_sec * MILLISEC_PER_SEC))
                        , loot_count, looter_count);;
        session->GenerateLootObjects(number);
    }
}

std::pair<std::shared_ptr<model::Player>, model::Token> Game::JoinGame(std::shared_ptr<model::Map> map, const std::string& player_name, bool is_rand_spawn){
    std::shared_ptr<model::GameSession> session = GetGameSession(map);
    if (!session) {
        throw std::runtime_error("Failed to create game session.");
    }

    std::shared_ptr<model::Player> player = std::make_shared<model::Player>(player_name);
    player->AddAndPrepareGameSession(session, map, is_rand_spawn);
    auto token = game_sessions_to_players_tok_[session].AddPlayer(*player);

    return {player, token};
}

std::shared_ptr<const model::Player> Game::FindPlayer(const model::Token& token) const {
    for(auto& [_, players_to_tokens] : game_sessions_to_players_tok_) {
        std::shared_ptr<const model::Player> player = players_to_tokens.FindPlayer(token);
        if(player) {
            return player;
        }
    }
    return nullptr;
}

std::shared_ptr<const model::Player> Game::FindPlayer(uint64_t id) const {
    for(auto& [_, players_to_tokens] : game_sessions_to_players_tok_) {
        std::shared_ptr<const model::Player> player = players_to_tokens.FindPlayer(id);
        if(player) {
            return player;
        }
    }
    return nullptr;
}

const std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher> Game::GetTokenToPlayerMap(std::shared_ptr<model::GameSession> session) const noexcept {
    auto it = game_sessions_to_players_tok_.find(session);
    if(it != game_sessions_to_players_tok_.end()) {
        return it->second.GetTokenToPlayerMap();
    }
    return std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher>();
}

std::shared_ptr<model::Player> Game::AddRestoredPlayer(std::shared_ptr<model::GameSession> session, const model::Player& player, model::Token token){
    return game_sessions_to_players_tok_[session].RestorePlayer(player, token);
}

const std::unordered_map<std::shared_ptr<GameSession>, PlayerTokens>& Game::GetSessions() const {
    return game_sessions_to_players_tok_;
}

void Game::UpdateGame(double dt) {
    for(auto& [game_session, _] : game_sessions_to_players_tok_) {
        game_session->UpdateDogsPosition(dt);
    }
}

void Game::SetDogRetirementTime(double dog_retirement_time) {
    int64_t tmp = dog_retirement_time * MILLISEC_PER_SEC;
    dog_retirement_time_ms_ = std::chrono::milliseconds{tmp};
}

std::vector<std::tuple<std::string, int, int>> Game::AddTimeAndLeave(std::chrono::milliseconds time) {
    std::vector<std::tuple<std::string, int, int>> retired_players_info;
    
    for (auto& [session, players_to_tok] : game_sessions_to_players_tok_) {
        auto& token_map = players_to_tok.GetTokenToPlayerMap();
        
        // Собираем токены для удаления
        std::vector<model::Token> tokens_to_remove;
        for (const auto& [token, player_ptr] : token_map) {
            auto dog = player_ptr->GetDog();

            dog->AddPlayTime(time);
            if(dog->IsStandingStill()) {
                dog->AddIdleTime(time);
            } else {
                dog->ResetIdleTime();
                continue;
            }

            auto dur = (-dog->GetIdleTime() + dog_retirement_time_ms_);
            std::chrono::milliseconds idle_diff{std::chrono::duration_cast<std::chrono::milliseconds>(dur)};

            if (idle_diff.count() <= 0) {
                dog->AddPlayTime(idle_diff);       //if (prev_idle_time + tick_dt) > idle_time => dog_play_time = dog_move_time + idle_time 

                retired_players_info.emplace_back(
                    player_ptr->GetName(),
                    dog->GetScore(),
                    dog->GetPlayTime().count()
                );
                tokens_to_remove.push_back(token);
            }
        }
        
        // Удаляем собранные токены
        for (const auto& token : tokens_to_remove) {
            token_map.erase(token);
        }
    }
    
    return retired_players_info;
}

}