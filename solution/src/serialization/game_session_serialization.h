#pragma once
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../application_model/game_session.h"

#include "loot_serialization.h"
#include "dog_serialization.h"

namespace model{

class GameSessionRepr {
public:
    GameSessionRepr() = default;

    explicit GameSessionRepr(const model::GameSession& game_session)
        : map_id_str_(*game_session.GetMap()->GetId()) {
        for(auto dog : game_session.GetDogs()) {
            DogRepr dog_repr(*dog);
            dogs_repr_.push_back(dog_repr);
        }
        for(auto [id, loot_object_ptr] : game_session.GetLootObjects()) {
            LootObjectRepr loot_object_repr(*loot_object_ptr);
            loot_objects_repr_.push_back(loot_object_repr);
        }

    }

    [[nodiscard]] std::string GetMapIdString() const {
        return map_id_str_;
    }
    [[nodiscard]] const std::vector<DogRepr>& GetDogsRepr() const {
        return dogs_repr_;
    }
    [[nodiscard]] const std::vector<LootObjectRepr>& GetLootsObjectRepr() const {
        return loot_objects_repr_;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& map_id_str_;
        ar& dogs_repr_;
        ar& loot_objects_repr_;
    }

private:
    std::string map_id_str_;
    std::vector<DogRepr> dogs_repr_;
    std::vector<LootObjectRepr> loot_objects_repr_;
};
}