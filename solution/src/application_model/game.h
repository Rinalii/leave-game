#pragma once

#include <chrono>

#include "../domain_model/model_game.h"
#include "../domain_model/dog.h"
#include "player_tokens.h"
#include "game_session.h"
#include "../domain_model/constants.h"

namespace model {

class Game {
public:
    using Maps = std::vector<Map>;

    Game(double base_interval = 0.5, double probability = 0.5)
        : loot_generator_(
            std::chrono::milliseconds(static_cast<long long>(base_interval * MILLISEC_PER_SEC)),
            probability)
    {}

    void AddMap(Map map);
    const Maps& GetMaps() const noexcept;
    std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;
    std::shared_ptr<GameSession> GetGameSession(const Map::Id& id);
    std::shared_ptr<GameSession> GetGameSession(std::shared_ptr<Map> map);
    void PrintMaps() const;
    void UpdateGame(double dt);

    void GenerateLoot(double time_delta_sec);
    void SetDogRetirementTime(double dog_retirement_time);

    std::pair<std::shared_ptr<model::Player>, model::Token> JoinGame(std::shared_ptr<model::Map> map, const std::string& player_name, bool is_rand_spawn);

    std::shared_ptr<const model::Player> FindPlayer(const model::Token& token) const;
    std::shared_ptr<const model::Player> FindPlayer(uint64_t id) const;

    const std::unordered_map<std::shared_ptr<GameSession>, PlayerTokens>& GetSessions() const;
    const std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher> GetTokenToPlayerMap(std::shared_ptr<model::GameSession> session) const noexcept;
    std::shared_ptr<model::Player> AddRestoredPlayer(std::shared_ptr<model::GameSession> session, const model::Player& player, model::Token token);

    std::vector<std::tuple<std::string, int, int>> AddTimeAndLeave(std::chrono::milliseconds time);
private:
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, util::TaggedHasher<Map::Id>>;

    MapIdToIndex map_id_to_index_;
    std::vector<Map> maps_;

    std::unordered_map<std::shared_ptr<GameSession>, PlayerTokens> game_sessions_to_players_tok_;

    loot_gen::LootGenerator loot_generator_;

    std::chrono::milliseconds dog_retirement_time_ms_ = std::chrono::milliseconds{DEFAULT_DOG_RETIREMENT_TIME_MS_};
};

}