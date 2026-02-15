#include "postgres.h"

#include <string>
#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void RetiredPlayerRepositoryImpl::SaveRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players) {
    auto conn = connection_pool_.GetConnection();
    pqxx::work work{*conn};

    for(const auto& retired_player : retired_players) {
        work.exec_params(R"(
            INSERT INTO retired_players (name, score, play_time_ms) VALUES ($1, $2, $3);
            )"_zv, retired_player.GetName(), retired_player.GetScore(), retired_player.GetPlayTimeMs()
        );
    }

    work.commit();
};

std::vector<domain::RetiredPlayer> RetiredPlayerRepositoryImpl::GetRetiredPlayers(size_t offset, size_t limit) {
    auto conn = connection_pool_.GetConnection();
    pqxx::read_transaction read_transaction{*conn};

    std::vector<domain::RetiredPlayer> retired_players;
    auto query_text = "SELECT name, score, play_time_ms FROM retired_players ORDER BY score DESC, play_time_ms ASC, name ASC LIMIT "
        + std::to_string(limit) + " OFFSET " + std::to_string(offset) + ";";

    for (auto [name, score, play_time] : read_transaction.query<std::string, size_t, int64_t>(query_text)) {
        retired_players.emplace_back(name, score, play_time);
    }
    
    return retired_players;
};

Database::Database(size_t number_of_connection, std::string database_url)
                  : connection_pool_{
                        number_of_connection,
                        [database_url]() {
                            return std::make_shared<pqxx::connection>(database_url);
                        }
                    } 
{
    auto conn = connection_pool_.GetConnection();
    pqxx::work work{*conn};
    work.exec(R"(
CREATE TABLE IF NOT EXISTS retired_players (
    id SERIAL PRIMARY KEY,
    name varchar(40) NOT NULL,
    score integer CONSTRAINT score_positive CHECK (score >= 0),
    play_time_ms integer NOT NULL CONSTRAINT play_time_ms_positive CHECK (play_time_ms >= 0)     
);
CREATE INDEX IF NOT EXISTS retired_players_score_play_time_name_idx ON retired_players (score DESC, play_time_ms ASC, name ASC);
)"_zv);
    // коммитим изменения
    work.commit();

}

}  // namespace postgres


