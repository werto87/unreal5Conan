#pragma once

#include "LogicStateMachineDependencies.h"

#include <memory>
#include <optional>
namespace boost::asio
{
class io_context;
class thread_pool;
}
class Matchmaking;
class LogicStateMachine
{
  struct StateMachineWrapper;
  struct StateMachineWrapperDeleter
  {
    void operator() (StateMachineWrapper const *p) const;
  };

public:
  LogicStateMachine (LogicStateMachineDependencies const &logicStateMachineDependencies);

  std::optional<std::string> processEvent (std::string const &event);

  std::unique_ptr<StateMachineWrapper, StateMachineWrapperDeleter> sm; // only use this member inside of ".cxx". reason because of incomplete type
};