#include "game_server.h"

#include "../domain_model/constants.h"

std::pair<std::shared_ptr<model::Player>, model::Token> GameServer::JoinGame(std::shared_ptr<model::Map> map, const std::string& player_name) {
    return game_.JoinGame(map, player_name, is_rand_spawn_);
}

std::shared_ptr<const model::Player> GameServer::FindPlayer(int64_t id) const {
    return game_.FindPlayer(id);
}

const std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher> GameServer::GetTokenToPlayerMap(std::shared_ptr<model::GameSession> session) const noexcept {
    return game_.GetTokenToPlayerMap(session);
}

std::shared_ptr<const model::Player> GameServer::FindPlayer(const model::Token& token) const {
    return game_.FindPlayer(token);
}

std::shared_ptr<model::Map> GameServer::FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
}

const std::vector<model::Map>& GameServer::GetMaps() const noexcept {
    return game_.GetMaps();
}

void GameServer::SetRandSpawn() {
    is_rand_spawn_ = true;
}
void GameServer::SetAutoTick() {
    is_auto_tick_ = true;
}
bool GameServer::IsAutoTick() const noexcept {
    return is_auto_tick_;
}
bool GameServer::IsRandomSpawn() const noexcept {
    return is_auto_tick_;
}

// Добавляем обработчик сигнала tick и возвращаем объект connection для управления,
// при помощи которого можно отписаться от сигнала
[[nodiscard]] sig::connection GameServer::DoOnTick(const TickSignal::slot_type& handler) {
    return tick_signal_.connect(handler);
}

void GameServer::Tick(milliseconds delta) {
    game_.GenerateLoot(delta.count());

    std::vector<std::tuple<std::string, int, int>> retired_players_info = game_.AddTimeAndLeave(delta);

    std::vector<domain::RetiredPlayer> retired_players;
    for(const auto& retired_player : retired_players_info) {
        retired_players.emplace_back(retired_player);
    }
    use_cases_impl_.AddRetiredPlayers(retired_players);

    game_.UpdateGame(static_cast<double>(delta.count())/MILLISEC_PER_SEC);
    tick_signal_(delta);
}

void GameServer::SetStateFile(std::string state_file) {
    state_file_ = state_file;
}

void GameServer::SetSaveStatePeriod(uint64_t save_state_period) {
    save_state_period_ = save_state_period;
}

void GameServer::Restore() {
    model::Restore(game_, state_file_);
}

void GameServer::Save() {
    model::Save(game_, state_file_);
}

std::optional<std::vector<domain::RetiredPlayer>> GameServer::GetRetiredPlayersTable(std::optional<size_t> offset, std::optional<size_t> limit) {
    size_t start = 0;
    size_t records_limit{postgres::DEFAULT_LIMIT};
    if(offset) {
        start = *offset;
    }
    if(limit) {
        if(*limit > postgres::DEFAULT_LIMIT) {
            return std::nullopt;
        }
        records_limit = *limit;
    }
    return use_cases_impl_.GetRetiredPlayers(start, records_limit);
}