#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../domain_model/geom.h"
#include "../domain_model/model_env.h"

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

template <typename Archive>
void serialize(Archive& ar, Vec2D& vec, [[maybe_unused]] const unsigned version) {
    ar& vec.x;
    ar& vec.y;
}

}  // namespace geom

namespace model{

template <typename Archive>
void serialize(Archive& ar, Point& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

template <typename Archive>
void serialize(Archive& ar, PointDouble& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

template <typename Archive>
void serialize(Archive& ar, Size& size, [[maybe_unused]] const unsigned version) {
    ar& size.width;
    ar& size.height;
}

template <typename Archive>
void serialize(Archive& ar, Rectangle& rectangle, [[maybe_unused]] const unsigned version) {
    ar& rectangle.position;
    ar& rectangle.size;
}

template <typename Archive>
void serialize(Archive& ar, Offset& offset, [[maybe_unused]] const unsigned version) {
    ar& offset.dx;
    ar& offset.dy;
}

}  // namespace model

namespace collision_detector{

template <typename Archive>
void serialize(Archive& ar, Item& item, [[maybe_unused]] const unsigned version) {
    ar& item.position;
    ar& item.width;
}

template <typename Archive>
void serialize(Archive& ar, Gatherer& gatherer, [[maybe_unused]] const unsigned version) {
    ar& gatherer.start_pos;
    ar& gatherer.end_pos;
    ar& gatherer.width;
}

}