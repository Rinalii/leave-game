#pragma once

#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "model_env.h"
#include "tagged.h"
#include "collision_detector.h"

#include "constants.h"

namespace model {

enum class Direction{
    NORTH, 
    SOUTH, 
    WEST, 
    EAST
};

class LootObject;

struct Bag {
    uint64_t capacity = 3;
    std::vector<std::shared_ptr<LootObject>> loot_objects;

    bool IsFull() const {
        return !(loot_objects.size() < capacity);
    }
    void AddLoot(const std::shared_ptr<LootObject>& item) {
        loot_objects.push_back(item);
    }
};

class Dog : public collision_detector::Gatherer {
public:
    explicit Dog() : Gatherer(geom::Point2D{0., 0.}, geom::Point2D{0., 0.}, DOG_WIDTH){}
    ~Dog() {}

    void ApplyMapSettings(std::shared_ptr<model::Map> map, bool is_rand_spawn);

    void PrepareToMove(const std::string& direction_str);
    void SetPosition(PointDouble position);

    PointDouble GetPosition() const;
    PointDouble GetSpeed() const;
    std::string GetDirection() const;
    void SetSpeedValue(double speed_value);

    void Stop();
    bool IsStandingStill() const;

    void SetGatherer(geom::Point2D curr_pos, geom::Point2D next_pos);
    void SetGatherer(geom::Point2D next_pos);

    void SetWidth(double width);

    void CleanBag();
    Bag& GetBag();
    const Bag& GetBag() const;
    void SetPositionEndGatherer();
    void AddScore(uint64_t value);
    uint64_t GetScore() const;
    double GetSpeedValue() const;

    void SetDirectionAndSpeed(Direction direction, PointDouble speed);
    Direction GetDirectionEnum() const;
    void SetBag(const Bag& bag);

    void AddPlayTime(std::chrono::milliseconds delta);
    void AddIdleTime(std::chrono::milliseconds delta);
    void ResetIdleTime();

    std::chrono::milliseconds GetPlayTime() const;
    std::chrono::milliseconds GetIdleTime() const;
private:
    PointDouble position_;
    PointDouble speed_;
    Direction direction_=Direction::NORTH;

    double speed_value_ = 0.;

    Bag bag_;
    uint64_t score = 0;

    std::chrono::milliseconds play_time_{0};
    std::chrono::milliseconds idle_time_{0};

    void SetSpeed(PointDouble speed);
};

}