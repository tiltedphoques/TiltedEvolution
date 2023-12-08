#include "GameVM.h"

// TODO: hook this
void GameVM::Update(float aUpdateBudget)
{
    using TUpdate = void(GameVM*, float);
    TUpdate* update = (TUpdate*)0x1429AF1C0; // 170097
    update(this, aUpdateBudget);
}
