#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "geom_serialization.h"

namespace model{

class LootObjectRepr {
public:
    LootObjectRepr() = default;

    explicit LootObjectRepr(const model::LootObject& loot_object)
        : item_(loot_object.GetPosition(), loot_object.GetWidth())
        , id_(loot_object.GetId())
        , type_(loot_object.GetType())
        , value_(loot_object.GetValue())
        , id_counter_(loot_object.GetIdCounter()) {
    }

    [[nodiscard]] model::LootObject Restore() const {
        model::LootObject loot_object(type_, value_, item_.position, item_.width);
        loot_object.SetId(id_);
        loot_object.SetIdCounter(id_counter_);
        return loot_object;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& item_;
        ar& id_;
        ar& type_;
        ar& value_;
        ar& id_counter_;
    }

private:
    collision_detector::Item item_;
    uint64_t id_;
    uint64_t type_ = 0;
    uint64_t value_ = 0;
    uint64_t id_counter_;
};
}