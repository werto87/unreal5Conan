#pragma once

#include "GameDependencies.h"

#include <memory>
#include <optional>
namespace boost::asio
{
class io_context;
class thread_pool;
}
class Matchmaking;
class GameLogicStateMachine
{
  struct StateMachineWrapper;
  struct StateMachineWrapperDeleter
  {
    void operator() (StateMachineWrapper const *p) const;
  };

public:
  GameLogicStateMachine (LogicStateMachineDependencies const &logicStateMachineDependencies);

  std::optional<std::string> processEvent (std::string const &event);

  std::unique_ptr<StateMachineWrapper, StateMachineWrapperDeleter> sm; // only use this member inside of ".cxx". reason because of incomplete type
};