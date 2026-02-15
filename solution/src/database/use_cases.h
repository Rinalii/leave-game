#pragma once

#include <string>
#include <vector>

#include "retired_player.h"

namespace database {

class UseCases {
public:
    virtual void AddRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players) = 0;
    virtual std::vector<domain::RetiredPlayer> GetRetiredPlayers(size_t offset, size_t limit) = 0;
protected:
    ~UseCases() = default;
};

}  // namespace domain
