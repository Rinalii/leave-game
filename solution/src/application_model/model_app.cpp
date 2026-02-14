#include "model_app.h"

#include <iomanip>

namespace model {

uint64_t Player::id_counter_ = 0;
uint64_t LootObject::id_counter_ = 0;

void Player::AddAndPrepareGameSession(std::shared_ptr<GameSession> session, std::shared_ptr<model::Map> map, bool is_rand_spawn) {
    session_ = session;

    dog_->ApplyMapSettings(map, is_rand_spawn);
    session_->AddDog(dog_);
}

void Player::SetIdCounter(uint64_t id_counter) {
    id_counter_ = id_counter;
}

uint64_t Player::GetIdCounter() const {
    return id_counter_;
}

void Player::SetSession(const std::shared_ptr<GameSession>& session) {
    session_ = session;
}

}


