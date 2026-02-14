#include "dog.h"

namespace model {

void Dog::PrepareToMove(const std::string& direction_str) {
    if(direction_str == "U"){
        direction_ = Direction::NORTH;
        SetSpeed({0., -speed_value_});
        idle_time_ = std::chrono::milliseconds{0};
    } else if(direction_str == "D"){
        direction_ = Direction::SOUTH;
        SetSpeed({0., speed_value_});
        idle_time_ = std::chrono::milliseconds{0};
    } else if(direction_str == "R"){
        direction_ = Direction::WEST;
        SetSpeed({speed_value_, 0.});
        idle_time_ = std::chrono::milliseconds{0};
    } else if(direction_str == "L"){
        direction_ = Direction::EAST;
        SetSpeed({-speed_value_, 0.});
        idle_time_ = std::chrono::milliseconds{0};
    } else if(direction_str == ""){
        SetSpeed({0., 0.});
    } else {
        throw;
    }
}

std::string Dog::GetDirection() const {
    switch(direction_){
        case Direction::NORTH: return "U";
        case Direction::SOUTH: return "D";
        case Direction::WEST: return "R";
        case Direction::EAST: return "L";
    }
    return "U";
}
void Dog::SetSpeedValue(double speed_value) {
    speed_value_ = speed_value;
}
void Dog::ApplyMapSettings(std::shared_ptr<model::Map> map, bool is_rand_spawn) {
    if(is_rand_spawn) {
        SetPosition(map->GetRandPosition());
    } else {
        SetPosition(map->GetStartPosition());
    }
    SetSpeedValue(map->GetDogSpeed());
    Stop();
}
void Dog::Stop() {
    SetSpeed({0, 0});
}

void Dog::SetPosition(PointDouble position) {
    position_ = position;
}
void Dog::SetSpeed(PointDouble speed) {
    speed_ = speed;
}
void Dog::SetDirectionAndSpeed(Direction direction, PointDouble speed) {
    direction_ = direction;
    speed_ = speed;
}
PointDouble Dog::GetPosition() const {
    return position_;
}
PointDouble Dog::GetSpeed() const {
    return speed_;
}
void Dog::SetGatherer(geom::Point2D curr_pos, geom::Point2D next_pos) {
    start_pos = curr_pos;
    end_pos = next_pos;
}
void Dog::SetGatherer(geom::Point2D next_pos) {
    start_pos = end_pos;
    end_pos = next_pos;
}
void Dog::SetWidth(double width) {
    width = width;
}
void Dog::CleanBag() {
    bag_.loot_objects.clear();
}
Bag& Dog::GetBag() {
    return bag_;
}
const Bag& Dog::GetBag() const {
    return bag_;
}
void Dog::SetPositionEndGatherer() {
    position_ = {end_pos.x, end_pos.y};
}
void Dog::AddScore(uint64_t value) {
    score += value;
}
uint64_t Dog::GetScore() const {
    return score;
}
double Dog::GetSpeedValue() const{
    return speed_value_;
}
void Dog::SetBag(const Bag& bag) {
    bag_ = bag;
}
Direction Dog::GetDirectionEnum() const {
    return direction_;
}
void Dog::AddPlayTime(std::chrono::milliseconds delta) {
    play_time_ += delta;
}
std::chrono::milliseconds Dog::GetPlayTime() const {
    return play_time_;
}
void Dog::AddIdleTime(std::chrono::milliseconds delta) {
    idle_time_ += delta;
}
void Dog::ResetIdleTime() {
    idle_time_ = std::chrono::milliseconds{0};
}
std::chrono::milliseconds Dog::GetIdleTime() const {
    return idle_time_;
}
bool Dog::IsStandingStill() const {
    return !(speed_.x || speed_.y);
}

}