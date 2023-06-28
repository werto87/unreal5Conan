#include "util.h"
void
printExceptionHelper (std::exception_ptr eptr)
{
  try
    {
      if (eptr)
        {
          std::rethrow_exception (eptr);
        }
    }
  catch (std::exception const &e)
    {
      UE_LOG (LogTemp, Error, TEXT ("co_spawn exception: %s"), *FString { e.what () });
    }
}