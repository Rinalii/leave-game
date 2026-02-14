#pragma once

#include <memory>
#include <string>

#include "../domain_model/model_game.h"
#include "../domain_model/dog.h"
#include "game_session.h"

namespace model {

class Player {
public:
    Player(const std::string& name) : name_(name), id_(++id_counter_){
        dog_ = std::make_shared<Dog>();
    }
    Player(const Dog& dog, std::string name, uint64_t id) : name_(name), id_(id){
        dog_ = std::make_shared<Dog>(dog);
    }
    Player(const std::shared_ptr<Dog>& dog, std::string name, uint64_t id) : name_(name), id_(id){
        dog_ = dog;
    }
    ~Player() {}

    const std::string& GetName() const {return name_;}
    uint64_t GetId() const noexcept {return id_;}
    std::shared_ptr<Dog> GetDog() const {return dog_;}
    const std::shared_ptr<GameSession> GetPlayersSession() const {return session_;}
    std::shared_ptr<GameSession> GetPlayersSession() {return session_;}

    void AddAndPrepareGameSession(std::shared_ptr<GameSession> session, std::shared_ptr<model::Map> map, bool is_rand_spawn);
    void SetIdCounter(uint64_t id_counter);
    uint64_t GetIdCounter() const;
    void SetSession(const std::shared_ptr<GameSession>& session);

private:
    std::string name_;
    uint64_t id_;
    std::shared_ptr<Dog> dog_;
    
    std::shared_ptr<GameSession> session_;
    static uint64_t id_counter_;
};

}


