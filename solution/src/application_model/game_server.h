#pragma once

#include <boost/signals2.hpp>
#include <chrono>
#include <utility>

#include "../json_loader.h"
#include "../serialization/model_serialization.h"

#include "../database/postgres.h"
#include "../database/use_cases_impl.h"

#include "player_tokens.h"
#include "model_app.h"
#include "../domain_model/model_game.h"
#include "game.h"

namespace sig = boost::signals2;
using milliseconds = std::chrono::milliseconds;
namespace fs = std::filesystem;

using namespace std::literals;

class GameServer {
    GameServer() = delete;
    GameServer(const GameServer&) = delete;
    GameServer(GameServer&&) = delete;
    GameServer& operator=(const GameServer&) = delete;
    GameServer& operator=(GameServer&&) = delete;

public:
    using TickSignal = sig::signal<void(milliseconds delta)>;

    GameServer(fs::path config, size_t number_of_connection, std::string database_url = postgres::DB_URL) :
        game_{json_loader::LoadGame(config)}, database_(number_of_connection, database_url) {
    }

    std::pair<std::shared_ptr<model::Player>, model::Token> JoinGame(std::shared_ptr<model::Map> map, const std::string& player_name);

    std::shared_ptr<const model::Player> FindPlayer(const model::Token& token) const;
    std::shared_ptr<const model::Player> FindPlayer(int64_t id) const;
    const std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher> GetTokenToPlayerMap(std::shared_ptr<model::GameSession> session) const noexcept;

    std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
    const std::vector<model::Map>& GetMaps() const noexcept;

    void SetRandSpawn();
    void SetAutoTick();
    bool IsAutoTick() const noexcept;
    bool IsRandomSpawn() const noexcept;

    // Добавляем обработчик сигнала tick и возвращаем объект connection для управления,
    // при помощи которого можно отписаться от сигнала
    [[nodiscard]] sig::connection DoOnTick(const TickSignal::slot_type& handler);
    void Tick(milliseconds delta);

    void SetStateFile(std::string state_file);
    void SetSaveStatePeriod(uint64_t save_state_period);

    void Restore();
    void Save();

    std::optional<std::vector<domain::RetiredPlayer>> GetRetiredPlayersTable(std::optional<size_t> offset, std::optional<size_t> limit);

private:
    model::Game game_;

    bool is_rand_spawn_ = false;
    bool is_auto_tick_ = false;

    TickSignal tick_signal_;

    std::string state_file_;
    uint64_t save_state_period_ = 0;

    postgres::Database database_;
    database::UseCasesImpl use_cases_impl_{database_.GetRetiredPlayerImpl()};
};
