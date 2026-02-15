#pragma once
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../application_model/player_tokens.h"
#include "../application_model/game.h"
#include "../application_model/game_session.h"

#include "loot_serialization.h"
#include "game_session_serialization.h"
#include "player_serialization.h"

namespace model{

class GameSessionReprTmp {
public:
    GameSessionReprTmp() = default;

    explicit GameSessionReprTmp(const model::GameSession& game_session, const PlayerTokens& player_tokens)
                : map_id_str_(*game_session.GetMap()->GetId()) {

        for(auto [token, player_ptr] : player_tokens.GetTokenToPlayerMap()) {
            PlayerRepr player_repr(*player_ptr, token);
            players_repr_.push_back(player_repr);
        }
        for(auto [id, loot_object_ptr] : game_session.GetLootObjects()) {
            LootObjectRepr loot_object_repr(*loot_object_ptr);
            loot_objects_repr_.push_back(loot_object_repr);
        }

    }

    [[nodiscard]] std::string GetMapIdString() const {
        return map_id_str_;
    }
    [[nodiscard]] const std::vector<PlayerRepr>& GetPlayerRepr() const {
        return players_repr_;
    }
    [[nodiscard]] const std::vector<LootObjectRepr>& GetLootsObjectRepr() const {
        return loot_objects_repr_;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& map_id_str_;
        ar& players_repr_;
        ar& loot_objects_repr_;
    }

private:
    std::string map_id_str_;
    std::vector<PlayerRepr> players_repr_;
    std::vector<LootObjectRepr> loot_objects_repr_;
};

void RestoreSession(model::Game& game, const GameSessionReprTmp& session_repr);
void Restore(model::Game& game);
void Save(model::Game& game);
void Restore(model::Game& game, std::string filename);
void Save(model::Game& game, std::string filename);

}
