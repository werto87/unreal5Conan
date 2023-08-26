#pragma once

struct LogicStateMachineDependencies
{
  std::function<void (FString const &message)> onMessage {};
};
