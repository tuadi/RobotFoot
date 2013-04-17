#include <signal.h>
#include <cstdlib>
#include <sys/time.h>

#include <iostream>

struct itimerval oldTimer;
struct itimerval newTimer;
 
void
catch_alarm (int sig)
{
  std::cout << "Alarm event\n";
  //signal (sig, catch_alarm);
}
 
int main(void)
{
  const int sdelay = 1;
  const int udelay = 0;

  signal (SIGALRM, catch_alarm);
  newTimer.it_interval.tv_sec = sdelay;
  newTimer.it_interval.tv_usec = udelay;
  newTimer.it_value.tv_sec = sdelay;
  newTimer.it_value.tv_usec = udelay;
   
  oldTimer.it_interval.tv_sec = 0;
  oldTimer.it_interval.tv_usec = 0;
  oldTimer.it_value.tv_sec = 0;
  oldTimer.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &newTimer, &oldTimer) < 0)
  {
     std::cout << "Timer init failed.\n";
     return EXIT_FAILURE;
  }

  while(1)
  {
     sleep(2);
  }

  return EXIT_SUCCESS;
}