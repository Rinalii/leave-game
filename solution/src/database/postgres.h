#pragma once
#include "retired_player.h"
#include "connection_pool.h"

#include <pqxx/connection>
#include <pqxx/transaction>

namespace postgres {

const std::string DB_URL    = "GAME_DB_URL";
const size_t DEFAULT_OFFSET = 0;
const size_t DEFAULT_LIMIT  = 100;

class RetiredPlayerRepositoryImpl : public domain::RetiredPlayerRepository {
public:
    explicit RetiredPlayerRepositoryImpl(database::ConnectionPool& connection_pool)
        : connection_pool_(connection_pool) {
    };

    void SaveRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players) override;
    std::vector<domain::RetiredPlayer> GetRetiredPlayers(size_t offset, size_t limit) override;

private:
    database::ConnectionPool& connection_pool_;
};

class Database {
public:
    Database(size_t number_of_connection, std::string database_url);

    RetiredPlayerRepositoryImpl& GetRetiredPlayerImpl() & {
        return retired_players_impl_;
    }

private:
    database::ConnectionPool connection_pool_;
    RetiredPlayerRepositoryImpl retired_players_impl_{connection_pool_};
};

}  // namespace postgres


