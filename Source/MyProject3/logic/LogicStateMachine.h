#pragma once

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
    void operator() (StateMachineWrapper *p);
  };

public:
  LogicStateMachine ();

  std::optional<std::string> processEvent (const std::string &event);

  std::unique_ptr<StateMachineWrapper, StateMachineWrapperDeleter> sm; // only use this member inside of ".cxx". reason because of incomplete type
};