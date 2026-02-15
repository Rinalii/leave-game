#pragma once
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../domain_model/dog.h"

#include "geom_serialization.h"
#include "loot_serialization.h"

namespace model{

class BagRepr {
public:
    BagRepr() = default;

    explicit BagRepr(const model::Bag& bag)
        : capacity_(bag.capacity){
        for(auto loot_obj_ptr : bag.loot_objects) {
            loot_objects_repr_.emplace_back(*loot_obj_ptr);
        }
    }

    [[nodiscard]] model::Bag Restore() const {
        model::Bag bag;
        bag.capacity = capacity_;
        for(auto obj_repr_el : loot_objects_repr_) {
            auto loot_object = obj_repr_el.Restore();
            bag.AddLoot(std::make_shared<LootObject>(loot_object));
        }
        return bag;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& capacity_;
        ar& loot_objects_repr_;
    }

private:
    uint64_t capacity_ = 3;
    std::vector<LootObjectRepr> loot_objects_repr_;
};


template <typename Archive>
void serialize(Archive& ar, Direction& direction, [[maybe_unused]] const unsigned version) {
    ar& direction;
}

class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog)
        : position_(model::PointDouble{dog.GetPosition().x, dog.GetPosition().y})
        , speed_(model::PointDouble{dog.GetSpeed().x, dog.GetSpeed().y})
        , direction_(dog.GetDirectionEnum())
        , speed_value_(dog.GetSpeedValue())
        , bag_repr_(dog.GetBag())
        , score_(dog.GetScore())
        , gatherer_(collision_detector::Gatherer(dog.start_pos, dog.end_pos, dog.width)){

    }

    [[nodiscard]] model::Dog Restore() const {
        model::Dog dog;
        dog.SetPosition(position_);
        dog.SetSpeedValue(speed_value_);
        dog.SetDirectionAndSpeed(direction_, speed_);
        dog.SetGatherer(gatherer_.start_pos, gatherer_.end_pos);
        dog.SetWidth(gatherer_.width);
        
        Bag bag = bag_repr_.Restore();
        dog.SetBag(bag);

        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& position_;
        ar& speed_;
        ar& direction_;
        ar& speed_value_;
        ar& bag_repr_;
        ar& score_;
        ar& gatherer_;
    }

private:
    PointDouble position_;
    PointDouble speed_;
    Direction direction_=Direction::NORTH;

    double speed_value_ = 0.;

    BagRepr bag_repr_;
    uint64_t score_ = 0;
    collision_detector::Gatherer gatherer_;
};
}