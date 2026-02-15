#include "use_cases_impl.h"

namespace database {

void UseCasesImpl::AddRetiredPlayers(const std::vector<domain::RetiredPlayer>& retired_players)  {
    retired_players_rep_.SaveRetiredPlayers(retired_players);
};
std::vector<domain::RetiredPlayer> UseCasesImpl::GetRetiredPlayers(size_t offset, size_t limit)  {
    return retired_players_rep_.GetRetiredPlayers(offset, limit);
};

}
