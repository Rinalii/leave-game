#pragma once
#include "use_cases.h"

namespace database {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::RetiredPlayerRepository& retired_players_rep)
        : retired_players_rep_{retired_players_rep} {
    }

    void AddRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players) override;
    std::vector<domain::RetiredPlayer> GetRetiredPlayers(size_t offset, size_t limit) override;
private:
    domain::RetiredPlayerRepository& retired_players_rep_;
};

}  // namespace app
