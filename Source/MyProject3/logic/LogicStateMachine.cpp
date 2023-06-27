#include "LogicStateMachine.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

#include "MyProject3/UnrealMacroNuke/UndefineMacros_UE_5.1.0.h"

#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/hana/fwd/for_each.hpp>
#include <boost/sml.hpp>
#include <confu_json/to_json.hxx>
#include <confu_json/to_object.hxx>
#include <confu_json/util.hxx>
#include <login_matchmaking_game_shared_type/userMatchmakingSerialization.hxx>

#include "MyProject3/UnrealMacroNuke/RedefineMacros_UE_5.1.0.h"

#pragma clang diagnostic pop

struct LogicStateMachineDependencies
{
};

const auto SomeAction = [] () {};

struct SomeState
{
};

struct SomeEvent
{
};

class StateMachineImpl
{
public:
  auto
  operator() () const noexcept
  {
    using namespace boost::sml;
    // clang-format off
    return make_transition_table (
  // Default-----------------------------------------------------------------------------------------------------------------------------------------------------------------
* state<SomeState>                         + event<SomeEvent>                             / SomeAction
      );
  }
};
// clang-format on

struct my_logger
{
  template <class SM, class TEvent>
  void
  log_process_event (const TEvent &event)
  {
    if constexpr (boost::fusion::traits::is_sequence<TEvent>::value)
      {
        std::cout << "\n[" << boost::sml::aux::get_type_name<SM> () << "]"
                  << "[process_event] '" << objectToStringWithObjectName (event) << "'" << std::endl;
      }
    else
      {
        printf ("[%s][process_event] %s\n", boost::sml::aux::get_type_name<SM> (), boost::sml::aux::get_type_name<TEvent> ());
      }
  }

  template <class SM, class TGuard, class TEvent>
  void
  log_guard (const TGuard &, const TEvent &, bool result)
  {
    printf ("[%s][guard]\t  '%s' %s\n", boost::sml::aux::get_type_name<SM> (), boost::sml::aux::get_type_name<TGuard> (), (result ? "[OK]" : "[Reject]"));
  }

  template <class SM, class TAction, class TEvent>
  void
  log_action (const TAction &, const TEvent &)
  {
    printf ("[%s][action]\t '%s' \n", boost::sml::aux::get_type_name<SM> (), boost::sml::aux::get_type_name<TAction> ());
  }

  template <class SM, class TSrcState, class TDstState>
  void
  log_state_change (const TSrcState &src, const TDstState &dst)
  {
    printf ("[%s][transition]\t  '%s' -> '%s'\n", boost::sml::aux::get_type_name<SM> (), src.c_str (), dst.c_str ());
  }
};

struct LogicStateMachine::StateMachineWrapper
{
  StateMachineWrapper (LogicStateMachine *owner, LogicStateMachineDependencies matchmakingGameDependencies_) : matchmakingGameDependencies { std::move (matchmakingGameDependencies_) }, impl (owner, matchmakingGameDependencies) {}
  LogicStateMachineDependencies matchmakingGameDependencies;

#ifdef LOG_FOR_STATE_MACHINE
  my_logger logger;
  boost::sml::sm<StateMachineImpl, boost::sml::logger<my_logger> > impl;
#else
  boost::sml::sm<StateMachineImpl> impl;
#endif
};

void // has to be after YourClass::StateMachineWrapper definition
LogicStateMachine::StateMachineWrapperDeleter::operator() (const StateMachineWrapper *p) const
{
  delete p;
}

LogicStateMachine::LogicStateMachine () : sm { new StateMachineWrapper { this, LogicStateMachineDependencies {} } } {}

std::optional<std::string>
LogicStateMachine::processEvent (const std::string &event)
{
  {
    std::vector<std::string> SplitMessage {};
    split (SplitMessage, event, boost::is_any_of ("|"));
    auto result = std::optional<std::string> {};
    if (SplitMessage.size () == 2)
      {
        const auto &typeToSearch = SplitMessage.at (0);
        const auto &objectAsString = SplitMessage.at (1);
        bool typeFound = false;
        boost::hana::for_each (user_matchmaking::userMatchmaking, [&] (const auto &x) {
          if (typeToSearch == confu_json::type_name<std::decay_t<decltype (x)> > ())
            {
              typeFound = true;
              boost::json::error_code ec {};
              const auto messageAsObject = confu_json::read_json (objectAsString, ec);
              if (ec)
                {
                  result = "read_json error: " + ec.message ();
                }
              else
                {
                  try
                    {
                      if (not sm->impl.process_event (confu_json::to_object<std::decay_t<decltype (x)> > (messageAsObject)))
                        {
                          result = "No transition found";
                        }
                    }
                  catch (const std::exception &e)
                    {
                      auto messageForUser = std::stringstream {};
                      messageForUser << "exception: " << e.what () << '\n';
                      messageForUser << "messageAsObject: " << messageAsObject << '\n';
                      messageForUser << "example for " << confu_json::type_name<std::decay_t<decltype (x)> > () << " : '" << confu_json::to_json<> (x) << "'" << '\n';
                      result = messageForUser.str ();
                    }
                }
            }
        });
        if (not typeFound)
          {
            result = "could not find a match for typeToSearch in shared_class::gameTypes '" + typeToSearch + "'";
          }
      }
    else
      {
        result = "Not supported event. event syntax: EventName|JsonObject";
      }
    return result;
  }
}