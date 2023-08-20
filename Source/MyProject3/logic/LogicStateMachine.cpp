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

auto const createGame = [] (user_matchmaking::LoginAsGuestSuccess const &loginAsGuestSuccess, LogicStateMachineDependencies &logicStateMachineDependencies) {
  //
  //  TODO add more logic check the modern durak web client and call everything to start a game
};

// TODO fix the naming of
//  change this class name it says not much
//  change the class name of LogicStateMachineDependencies

// TODO add the durak types from shared game server durak library
struct NotInGame
{
};
struct InGame
{
};

class StateMachineImpl
{
public:
  auto
  operator() () const noexcept
  {
    using namespace boost::sml;
    namespace u_m = user_matchmaking;
    // clang-format off
    return make_transition_table (
  // Default-----------------------------------------------------------------------------------------------------------------------------------------------------------------
* state<NotInGame>                         + event<u_m::LoginAsGuestSuccess>                             / createGame  = state<InGame>
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakGameOverWon>                             /  durakGameOverWon
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakGameOverLose>                             / durakGameOverLose
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakGameOverDraw>                             / durakGameOverDraw
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::GameData>                             / gameData
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakAllowedMoves>                             / durakAllowedMoves
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakAskDefendWantToTakeCards>                             / durakAskDefendWantToTakeCards
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakTimers>                             / durakTimers
 // ,state<InGame>                            + event<HAS_TO_BE_NAMED::DurakDefendWantsToTakeCardsFromTableDoYouWantToAddCards>                             / durakDefendWantsToTakeCardsFromTableDoYouWantToAddCards

 /*
            
            
            
            
            
            
            
            
 */
      );
  }
};
// clang-format on

struct my_logger
{
  template <class SM, class TEvent>
  void
  log_process_event (TEvent const &event)
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
  log_guard (TGuard const &, TEvent const &, bool result)
  {
    printf ("[%s][guard]\t  '%s' %s\n", boost::sml::aux::get_type_name<SM> (), boost::sml::aux::get_type_name<TGuard> (), (result ? "[OK]" : "[Reject]"));
  }

  template <class SM, class TAction, class TEvent>
  void
  log_action (TAction const &, TEvent const &)
  {
    printf ("[%s][action]\t '%s' \n", boost::sml::aux::get_type_name<SM> (), boost::sml::aux::get_type_name<TAction> ());
  }

  template <class SM, class TSrcState, class TDstState>
  void
  log_state_change (TSrcState const &src, TDstState const &dst)
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
LogicStateMachine::StateMachineWrapperDeleter::operator() (StateMachineWrapper const *p) const
{
  delete p;
}

LogicStateMachine::LogicStateMachine (LogicStateMachineDependencies const &logicStateMachineDependencies) : sm { new StateMachineWrapper { this, logicStateMachineDependencies } } {}

std::optional<std::string>
LogicStateMachine::processEvent (std::string const &event)
{
  {
    std::vector<std::string> splitMessage {};
    split (splitMessage, event, boost::is_any_of ("|"));
    auto result = std::optional<std::string> {};
    if (splitMessage.size () == 2)
      {
        auto const &typeToSearch = splitMessage.at (0);
        auto const &objectAsString = splitMessage.at (1);
        bool typeFound = false;
        // TODO this only goes over user match making but we are interested in the types from game
        // TODO make ths types from example of game server into a library and use them here
        boost::hana::for_each (user_matchmaking::userMatchmaking, [&] (auto const &x) {
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