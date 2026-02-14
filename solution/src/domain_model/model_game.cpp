#include "model_game.h"

namespace model {

void LootObject::SetItem(const Item& item) {
    position = item.position;
    width = item.width;
}
void LootObject::SetId(uint64_t id) {
    id_ = id;
}
void LootObject::SetType(uint64_t type) {
    type_ = type;
}
void LootObject::SetValue(uint64_t value) {
    value_ = value;
}
uint64_t LootObject::GetValue() const {
    return value_;
}
uint64_t LootObject::GetType() const {
    return type_;
}
uint64_t LootObject::GetId() const {
    return id_;
}
geom::Point2D LootObject::GetPosition() const {
    return position;
}
double LootObject::GetWidth() const {
    return width;
}
uint64_t LootObject::GetIdCounter() const {
    return id_counter_;
}
void LootObject::SetIdCounter(uint64_t id_counter) {
    id_counter_ = id_counter;
}

size_t ItemGathererProviderImpl::ItemsCount() const{
    return items_.size();
}

collision_detector::Item ItemGathererProviderImpl::GetItem(size_t idx) const{
    return *items_.at(idx);
}

size_t ItemGathererProviderImpl::GatherersCount() const{
    return gatherers_.size();
}

collision_detector::Gatherer ItemGathererProviderImpl::GetGatherer(size_t idx) const{
    return *gatherers_.at(idx);
}

void ItemGathererProviderImpl::AddItem(const std::shared_ptr<model::LootObject>& item) {
    items_.push_back(item);
}

void ItemGathererProviderImpl::AddItem(const model::Office& item) {
    items_.push_back(std::make_shared<model::Office>(item));
}

void ItemGathererProviderImpl::AddGatherer(const std::shared_ptr<model::Dog>& gatherer) {
    gatherers_.push_back(gatherer);
}

std::shared_ptr<model::Dog> ItemGathererProviderImpl::GetDog(uint64_t idx) const {
    return std::dynamic_pointer_cast<model::Dog>(gatherers_.at(idx));
}
std::shared_ptr<model::Office> ItemGathererProviderImpl::GetOffice(uint64_t idx) const {
    return std::dynamic_pointer_cast<model::Office>(items_.at(idx));
}
std::shared_ptr<model::LootObject> ItemGathererProviderImpl::GetLootObject(uint64_t idx) const {
    return std::dynamic_pointer_cast<model::LootObject>(items_.at(idx));
}
}