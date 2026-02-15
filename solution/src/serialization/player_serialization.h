#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../application_model/model_app.h"
#include "../application_model/player_tokens.h"

#include "dog_serialization.h"

namespace model{

class PlayerRepr {
public:
    PlayerRepr() = default;

    explicit PlayerRepr(const model::Player& player, Token token)
        : name_(player.GetName())
        , id_(player.GetId())
        , dog_repr_(*player.GetDog())    
        , id_counter_(player.GetIdCounter())
        , token_(*token){
    }

    [[nodiscard]] std::string GetPlayerName() const {
        return name_;
    }
    [[nodiscard]] uint64_t GetPlayerId() const {
        return id_;
    }
    [[nodiscard]] const DogRepr& GetDogRepr() const {
        return dog_repr_;
    }
    [[nodiscard]] uint64_t GetIdCounter() const {
        return id_counter_;
    }
    [[nodiscard]] std::string GetPlayerToken() const {
        return token_;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& name_;
        ar& id_;
        ar& dog_repr_;
        ar& id_counter_;
        ar& token_;
    }

private:
    std::string name_;
    uint64_t id_;
    DogRepr dog_repr_;
    uint64_t id_counter_;
    std::string token_;
};
}