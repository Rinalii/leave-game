#include "model_env.h"

#include <stdexcept>

#include "constants.h"

namespace model {
using namespace std::literals;

Point PointDouble::Round() const {
    return Point{static_cast<Coord>(std::round(x)), static_cast<Coord>(std::round(y))};
}

PointDouble PointDouble::operator+(const PointDouble& other) const {
    return PointDouble{x+other.x, y+other.y};
}

bool PointDouble::operator<(const PointDouble& other) const {
    if(x<other.x && y<=other.y) {
        return true;
    }
    if(x<=other.x && y<other.y) {
        return true;
    }
    return false;
}

double PointDouble::Norm() const {
    return std::sqrt(x*x+y*y);
}

double PointDouble::Distance(const PointDouble& other) const {
    return std::sqrt((x-other.x)*(x-other.x)+(y-other.y)*(y-other.y));
}

Road::Road(HorizontalTag, Point start, Coord end_x) noexcept
    : start_{start}
    , end_{end_x, start.y} {
}

Road::Road(VerticalTag, Point start, Coord end_y) noexcept
    : start_{start}
    , end_{start.x, end_y} {
}

bool Road::IsHorizontal() const noexcept {
    return start_.y == end_.y;
}

bool Road::IsVertical() const noexcept {
    return start_.x == end_.x;
}

Point Road::GetStart() const noexcept {
    return start_;
}

Point Road::GetEnd() const noexcept {
    return end_;
}

bool Road::IsOnArea(PointDouble position) const noexcept{
    bool is_hor = false;
    bool is_ver = false;

    Point start = start_;
    Point end = end_;

    if(IsHorizontal()) {
        if(start_.x > end.x) {
            start = end_;
            end = start_;
        }
        is_hor = (static_cast<double>(start.x)  - ROAD_WIDTH <= position.x) && (position.x <= static_cast<double>(end.x) + ROAD_WIDTH);
        is_ver = (static_cast<double>(start.y) - ROAD_WIDTH <= position.y) && (position.y <= static_cast<double>(end.y) + ROAD_WIDTH);
    } else {
        if(start_.y > end.y) {
            start = end_;
            end = start_;
        }
        is_ver = (static_cast<double>(start.y) - ROAD_WIDTH <= position.y) && (position.y <= static_cast<double>(end.y) + ROAD_WIDTH);
        is_hor = (static_cast<double>(start.x) - ROAD_WIDTH <= position.x) && (position.x <= static_cast<double>(end.x) + ROAD_WIDTH);
    }
    return is_hor && is_ver;
}

PointDouble Road::GetMaxPossiblePosition(PointDouble position) const noexcept {
    PointDouble max_possible = position;

    Point start = start_;
    Point end = end_;
    if(IsHorizontal()) {
        if(start_.x > end_.x) {
            start = end_;
            end = start_;
        }
        if(position.x < start.x - ROAD_WIDTH) {
            max_possible.x = start.x - ROAD_WIDTH;
        } else if(position.x > end.x+ ROAD_WIDTH) {
            max_possible.x = end.x+ ROAD_WIDTH;
        }
        if(position.y < start.y - ROAD_WIDTH) {
            max_possible.y = start.y - ROAD_WIDTH;
        } else if(position.y > start.y + ROAD_WIDTH) {
            max_possible.y = start.y + ROAD_WIDTH;
        }
    } else {
        if(start_.y > end_.y) {
            start = end_;
            end = start_;
        }
        if(position.x < start.x - ROAD_WIDTH) {
            max_possible.x = start.x - ROAD_WIDTH;
        } else if(position.x > start.x + ROAD_WIDTH) {
            max_possible.x = start.x + ROAD_WIDTH;
        }
        if(position.y < start.y - ROAD_WIDTH) {
            max_possible.y = start.y - ROAD_WIDTH;
        } else if(position.y > end.y+ ROAD_WIDTH) {
            max_possible.y = end.y+ ROAD_WIDTH;
        }
    }
    return max_possible;
}

std::pair<PointDouble, PointDouble> Road::GetArea() const noexcept {
    PointDouble min;
    PointDouble max;
    if(start_.x < end_.x) {
        min.x = start_.x - ROAD_WIDTH;
        max.x = end_.x + ROAD_WIDTH;
    } else {
        max.x = start_.x + ROAD_WIDTH;
        min.x = end_.x - ROAD_WIDTH;
    }
    if(start_.y < end_.y) {
        min.y = start_.y - ROAD_WIDTH;
        max.y = end_.y + ROAD_WIDTH;
    } else {
        max.y = start_.y + ROAD_WIDTH;
        min.y = end_.y - ROAD_WIDTH;
    }
    return {min, max};
}
PointDouble Road::GetRandomPosition() const noexcept {
    std::pair<PointDouble, PointDouble> area = GetArea();
    double x = GenerateRandomNumber(area.first.x, area.second.x);
    double y = GenerateRandomNumber(area.first.y, area.second.y);
    return PointDouble{x, y};
}

void RoadIndexes::AddRoadIndexes(const std::vector<Road>& roads) {
    for(size_t i=0; i<roads.size(); ++i) {
        const Road& road = roads[i];
        if (road.IsHorizontal()) {
            coord_to_idx_hor_[road.GetStart().y] = i;
        } else {
            coord_to_idx_ver_[road.GetStart().x] = i;
        }
    }
}

std::vector<size_t> RoadIndexes::GetRoadIndexes(Point position) const {
    std::vector<size_t> res;
    auto it = coord_to_idx_hor_.find(position.y);
    if(it != coord_to_idx_hor_.end()) {
        res.push_back(it->second);
    }
    auto it2 = coord_to_idx_ver_.find(position.x);
    if(it2 != coord_to_idx_ver_.end()) {
        res.push_back(it2->second);
    }
    return res;
}

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

PointDouble Map::GetRandPosition() const {
    if (roads_.empty()) {
        throw std::runtime_error("No roads on the map");
    }

    uint64_t rand_number_of_road = GenerateRandomNumber(0, roads_.size()-1);

    const Road& road = roads_[rand_number_of_road];
    Point road_start = road.GetStart();
    Point road_end = road.GetEnd();

    if (road.IsHorizontal()) {
        double random_x = GenerateRandomNumber(static_cast<double>(road_start.x), static_cast<double>(road_end.x));
        return PointDouble{random_x, static_cast<double>(road_start.y)};
    }
    double random_y = GenerateRandomNumber(static_cast<double>(road_start.y), static_cast<double>(road_end.y));
    return PointDouble{static_cast<double>(road_start.x), random_y};
}

PointDouble Map::GetStartPosition() const {
    if (roads_.empty()) {
        throw std::runtime_error("No roads on the map");
    }

    const Road& road = roads_[0];
    Point road_start = road.GetStart();
    return PointDouble{road_start.x*1., road_start.y*1.};
}

void Map::SetDogSpeed(double dog_speed) {
    dog_speed_ = dog_speed;
}

double Map::GetDogSpeed() const {
    return dog_speed_;
}

void Map::AddRoadIndexes() {
    coords_to_road_idx_.AddRoadIndexes(roads_);
}

std::vector<size_t> Map::GetRoadIndexes(Point position) const {
    return coords_to_road_idx_.GetRoadIndexes(position);
}

std::vector<Road> Map::GetRoadsByPosition(Point position) const {
    std::vector<Road> res;
    std::vector<size_t> roads_idxs = GetRoadIndexes(position);
    for(size_t idx : roads_idxs) {
        res.push_back(roads_[idx]);
    }
    return res;
}

PointDouble Map::GetRandomPosition() const {
    if (roads_.empty()) {
        throw std::runtime_error("No roads on the map");
    }

    uint64_t rand_number_of_road = GenerateRandomNumber(0, roads_.size()-1);

    const Road& road = roads_[rand_number_of_road];
    return road.GetRandomPosition();
}

void Map::SetLootTypesJson(const boost::json::array& loot_types) {
    loot_types_json_ = loot_types;
}

void Map::SetLootTypes(const std::vector<LootType>& loot_types) {
    loot_types_ = loot_types;
}

uint64_t Map::GetNumberOfLootTypes() const {
    return loot_types_.size();
}

const boost::json::array& Map::GetLootTypesJson() {
    return loot_types_json_;
}
const std::vector<LootType>& Map::GetLootTypes() const {
    return loot_types_;
}

uint64_t Map::GetRandomTypeOfLoot() const {
    return GenerateRandomNumber(0, GetNumberOfLootTypes()-1);
}

std::pair<uint64_t, uint64_t> Map::GetRandomTypeAndValueOfLoot() const {
    uint64_t type = GenerateRandomNumber(0, GetNumberOfLootTypes()-1);
    return {type, loot_types_[type].value};
}

}  // namespace model
