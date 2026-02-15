#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdint>

#include "../domain_model/model_env.h"
#include "../domain_model/dog.h"
#include "../domain_model/model_game.h"

namespace model {
class GameSession {
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;

public:
    explicit GameSession(std::shared_ptr<Map> map) : map_(map) {}

    std::shared_ptr<Map> GetMap() const;
    void AddDog(std::shared_ptr<Dog> dog);
    const std::vector<std::shared_ptr<Dog>> GetDogs();
    void UpdateDogsPosition(double dt);

    void AddLootObject(LootObject& loot_object);
    uint64_t GetSizeLootObjects() const;

    void GenerateLootObjects(uint64_t number);

    void SetLootObjects(std::vector<LootObject>& loot_objects);
    const std::unordered_map<int, std::shared_ptr<LootObject>>& GetLootObjects() const;

    const std::vector<std::shared_ptr<Dog>> GetDogs() const;

private:
    std::shared_ptr<Map> map_;
    std::vector<std::weak_ptr<Dog>> dogs_;
    std::unordered_map<int, std::shared_ptr<LootObject>> loot_objects_;

    ItemGathererProviderImpl CreateProvider();
    void CollectAndSendItems();
};

}