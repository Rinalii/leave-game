#include "game_session.h"

#include <chrono>
#include <iostream>

using namespace std::literals;

namespace model {

std::shared_ptr<Map> GameSession::GetMap() const {
    return map_;
}

void GameSession::AddDog(std::shared_ptr<Dog> dog) {
    dog->GetBag().capacity = map_->GetBagCapacity();
    dogs_.emplace_back(dog);
}

const std::vector<std::shared_ptr<Dog>> GameSession::GetDogs(){
    std::vector<std::shared_ptr<Dog>> result;
    result.reserve(dogs_.size());

    auto it = dogs_.begin();
    while (it != dogs_.end()) {
        if (it->expired()) {
            it = dogs_.erase(it);
        } else {
            result.push_back(it->lock());
            ++it;
        }
    }
    return result;
}

void GameSession::UpdateDogsPosition(double dt) {
    const auto& map = GetMap();
    for (auto dog : GetDogs()) {
        if(dog->IsStandingStill()) {
            continue;
        }

        PointDouble curr_pos = dog->GetPosition();
        Point curr_pos_int = curr_pos.Round();

        std::vector<Road> roads_at_point = map->GetRoadsByPosition(curr_pos_int);

        PointDouble speed = dog->GetSpeed();
        PointDouble next_pos = curr_pos + PointDouble{speed.x * dt, speed.y * dt};

        PointDouble max_possible_pos = curr_pos;

        bool is_stop = true;

        for(const auto& road : roads_at_point) {
            if(road.IsOnArea(next_pos)) {
                is_stop = false;
                max_possible_pos = next_pos;
                break;
            } else {
                PointDouble tmp_max_possible = road.GetMaxPossiblePosition(next_pos);
                double dist = curr_pos.Distance(max_possible_pos);
                double tmp_dist = curr_pos.Distance(tmp_max_possible);
                if(dist < tmp_dist) {
                    max_possible_pos = tmp_max_possible;
                }
            }
        }
        if(!is_stop) {
            dog->SetGatherer({next_pos.x, next_pos.y});
            dog->SetPositionEndGatherer();
        } else {
            dog->SetGatherer({max_possible_pos.x, max_possible_pos.y});
            dog->SetPositionEndGatherer();
            dog->Stop();
        }
    }
    CollectAndSendItems();
}

const std::unordered_map<int, std::shared_ptr<LootObject>>& GameSession::GetLootObjects() const{
    return loot_objects_;
}
uint64_t GameSession::GetSizeLootObjects() const{
    return loot_objects_.size();
}

void GameSession::GenerateLootObjects(uint64_t number) {
    for(uint64_t i = 0; i < number; ++i) {
        std::pair<int64_t, int64_t> type_and_value = map_->GetRandomTypeAndValueOfLoot();
        PointDouble pos = map_->GetRandomPosition();
        LootObject loot_object(type_and_value.first, type_and_value.second, geom::Point2D(pos.x, pos.y));
        loot_objects_[loot_object.GetId()] = std::make_shared<LootObject>(loot_object);
    }
}

void GameSession::SetLootObjects(std::vector<LootObject>& loot_objects) {
    for(auto loot_object : loot_objects) {
        loot_objects_[loot_object.GetId()] = std::make_shared<LootObject>(loot_object);
    }
}
void GameSession::AddLootObject(LootObject& loot_object) {
    loot_objects_[loot_object.GetId()] = std::make_shared<LootObject>(loot_object);
}

ItemGathererProviderImpl GameSession::CreateProvider() {
    ItemGathererProviderImpl provider;
    for(const auto& id_to_item : loot_objects_) {
        provider.AddItem(id_to_item.second);
    }
    const std::vector<Office>& offices = map_->GetOffices();
    for(const auto& office : offices) {
        provider.AddItem(office);
    }
    const std::vector<std::shared_ptr<Dog>> dogs = GetDogs();
    for(const auto& dog : dogs) {
        provider.AddGatherer(dog);
    }
    return provider;
}
void GameSession::CollectAndSendItems() {
    ItemGathererProviderImpl provider = CreateProvider();
    auto events = collision_detector::FindGatherEvents(provider);
    
    for(const auto& event : events) {
        uint64_t item_idx = event.item_id;
        std::shared_ptr<Dog> dog = provider.GetDog(event.gatherer_id);

        std::shared_ptr<Office> office = provider.GetOffice(item_idx);
        if(office) {
            dog->CleanBag();
            continue;
        }
        std::shared_ptr<LootObject> loot_object = provider.GetLootObject(item_idx);
        if(loot_object) {
            auto it_item = loot_objects_.find(loot_object->GetId());
            if(it_item != loot_objects_.end()) {
                Bag& bag = dog->GetBag();
                if(!bag.IsFull()) {
                    bag.AddLoot(it_item->second);
                    dog->AddScore(loot_object->GetValue());
                    loot_objects_.erase(it_item);
                }
            }
        }
        

    }
    
}

const std::vector<std::shared_ptr<Dog>> GameSession::GetDogs() const {
    std::vector<std::shared_ptr<Dog>> result;

    for (const auto& weak_dog_ptr : dogs_) {
        if (auto shared_dog_ptr = weak_dog_ptr.lock()) { 
            result.push_back(shared_dog_ptr);
        }
    }
    return result;
}

}