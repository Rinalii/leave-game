#pragma once

#include <boost/json.hpp>
#include <cmath>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "tagged.h"
#include "collision_detector.h"

namespace model_constants{
    const std::string X = "x";
    const std::string Y = "y";
    const std::string H = "h";
    const std::string W = "w";
    const std::string X0 = "x0";
    const std::string Y0 = "y0";
    const std::string X1 = "x1";
    const std::string Y1 = "y1";
    const std::string OFFSET_X = "offsetX";
    const std::string OFFSET_Y = "offsetY";
}

namespace model {

using Dimension = int64_t;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

struct PointDouble {
    double x = 0;
    double y = 0;

    Point Round() const;
    PointDouble operator+(const PointDouble& other) const;

    template <typename T>
    PointDouble operator*(T a) {
        return PointDouble{x*a, y*a};
    }

    bool operator<(const PointDouble& other) const;
    double Norm() const;
    double Distance(const PointDouble& other) const;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept;
    Road(VerticalTag, Point start, Coord end_y) noexcept;

    bool IsHorizontal() const noexcept;
    bool IsVertical() const noexcept;
    Point GetStart() const noexcept;
    Point GetEnd() const noexcept;

    bool IsOnArea(PointDouble position) const noexcept;
    PointDouble GetMaxPossiblePosition(PointDouble position) const noexcept;
    std::pair<PointDouble, PointDouble> GetArea() const noexcept;
    PointDouble GetRandomPosition() const noexcept;

private:
    Point start_;
    Point end_;

    static double GenerateRandomNumber(double min, double max) {
        static std::random_device random_device;
        static std::mt19937 gen(random_device());
        std::uniform_real_distribution<>distr(min, max);
        return distr(gen);
    }
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office : public collision_detector::Item {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : Item({static_cast<double>(position.x), static_cast<double>(position.y)}, 0.5)
        , id_{std::move(id)}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    geom::Point2D GetPosition() const noexcept {
        return position;
    }

    double GetWidth() const {
        return width;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Offset offset_;
};


class RoadIndexes {
public:
    RoadIndexes() {}
    void AddRoadIndexes(const std::vector<Road>& roads);
    std::vector<size_t> GetRoadIndexes(Point position) const;

private:
    std::unordered_map<Coord, size_t> coord_to_idx_hor_;
    std::unordered_map<Coord, size_t> coord_to_idx_ver_;
};

struct LootType {
    std::string name;
    std::string file;
    std::string type;
    std::optional<int> rotation;
    std::string color;
    std::optional<double> scale;
    uint64_t value = 0;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

    PointDouble GetRandPosition() const;
    PointDouble GetStartPosition() const;

    void SetDogSpeed(double dog_speed);
    void SetBagCapacity(uint64_t bag_capacity = 3) {bag_capacity_ = bag_capacity;}
    double GetDogSpeed() const;
    uint64_t GetBagCapacity() const {return bag_capacity_;}
    void AddRoadIndexes();

    std::vector<size_t> GetRoadIndexes(Point position) const;
    std::vector<Road> GetRoadsByPosition(Point position) const;

    PointDouble GetRandomPosition() const;

    void SetLootTypesJson(const boost::json::array& loot_types);
    void SetLootTypes(const std::vector<LootType>& loot_types);

    uint64_t GetNumberOfLootTypes() const;
    const boost::json::array& GetLootTypesJson();
    const std::vector<LootType>& GetLootTypes() const;
    uint64_t GetRandomTypeOfLoot() const;
    std::pair<uint64_t, uint64_t> GetRandomTypeAndValueOfLoot() const;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    double dog_speed_;
    uint64_t bag_capacity_ = 3;


    RoadIndexes coords_to_road_idx_;

    boost::json::array loot_types_json_;
    std::vector<LootType> loot_types_;
    

    static uint64_t GenerateRandomNumber(uint64_t min, uint64_t max) {
        static std::random_device random_device;
        static std::mt19937 gen(random_device());
        std::uniform_int_distribution<>distr(min, max);
        return distr(gen);
    }

    static double GenerateRandomNumber(double min, double max) {
        static std::random_device random_device;
        static std::mt19937 gen(random_device());
        std::uniform_real_distribution<>distr(min, max);
        return distr(gen);
    }
};

}  // namespace model
