#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace domain {

class RetiredPlayer {
public:
    RetiredPlayer(std::string name, uint64_t score, uint64_t play_time_ms)
        : name_(std::move(name))
        , score_(score)
        , play_time_ms_(play_time_ms) {
    };

    RetiredPlayer(std::tuple<std::string, uint64_t, uint64_t> retired_player_tuple){
        std::tie(name_, score_, play_time_ms_) = retired_player_tuple;
    };

    const std::string& GetName() const noexcept {
        return name_;
    }

    uint64_t GetScore() const noexcept {
        return score_;
    }

    uint64_t GetPlayTimeMs() const noexcept {
        return play_time_ms_;
    }

private:
    std::string name_;
    uint64_t score_ = 0;
    uint64_t play_time_ms_ = 0;
};

class RetiredPlayerRepository {
public:
    virtual void SaveRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players) = 0;
    virtual std::vector<RetiredPlayer> GetRetiredPlayers(size_t offset, size_t limit) = 0;
protected:
    ~RetiredPlayerRepository() = default;
};

}
