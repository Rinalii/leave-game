#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "model_env.h"
#include "loot_generator.h"
#include "collision_detector.h"
#include "dog.h"

namespace model {

class LootObject : public collision_detector::Item {
public:

    LootObject(geom::Point2D position = {0., 0.}, double width = 0.) 
                : Item(position, width), id_(++id_counter_) {
    }

    LootObject(uint64_t type, geom::Point2D position = {0., 0.}, double width = 0.) 
                : Item(position, width), id_(++id_counter_), type_(type) {
    }

    LootObject(uint64_t type, uint64_t value, geom::Point2D position = {0., 0.}, double width = 0.) 
                : Item(position, width), id_(++id_counter_), type_(type), value_(value) {
    }

    void SetItem(const Item& item);
    void SetId(uint64_t id);
    void SetType(uint64_t type);
    void SetValue(uint64_t value);
    void SetIdCounter(uint64_t id_counter);

    uint64_t GetValue() const;
    uint64_t GetType() const;
    uint64_t GetId() const;
    geom::Point2D GetPosition() const;
    double GetWidth() const;
    uint64_t GetIdCounter() const;

private:
    uint64_t id_;
    uint64_t type_ = 0;
    uint64_t value_ = 0;
    static uint64_t id_counter_;
};

class ItemGathererProviderImpl : public collision_detector::ItemGathererProvider {
public:
    size_t ItemsCount() const override;
    collision_detector::Item GetItem(size_t idx) const override;
    size_t GatherersCount() const override;
    collision_detector::Gatherer GetGatherer(size_t idx) const override;

    void AddItem(const std::shared_ptr<model::LootObject>& item);
    void AddItem(const model::Office& item);
    void AddGatherer(const std::shared_ptr<model::Dog>& gatherer);

    std::shared_ptr<model::Dog> GetDog(uint64_t idx) const;
    std::shared_ptr<model::Office> GetOffice(uint64_t idx) const;
    std::shared_ptr<model::LootObject> GetLootObject(uint64_t idx) const;
private:
    std::vector<std::shared_ptr<collision_detector::Item>> items_;
    std::vector<std::shared_ptr<collision_detector::Gatherer>> gatherers_;
};

}

