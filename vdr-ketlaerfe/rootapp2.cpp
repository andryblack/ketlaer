#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

static void reset()
{
  system("/usr/local/bin/RootApp");
}

static int run_child(char **argv)
{
  int pid, status = 0;

  if ((pid = fork()) < 0) {
    perror("fork()");
    exit(1);
  }
  if (pid == 0) {
    execv(argv[0], argv);
  }
  else {
    if (wait(&status) != pid) {
      perror("wait()");
      exit(1);
    }
  }
  return status;
}

static void msg(char *msg)
{
  char cmd[1024];

  sprintf(cmd, "echo %s>/usr/local/etc/rootapp2.log", msg);
  system(cmd);
}

static bool g_bPowerdown = false;

static void run_dvd_player()
{
  char *argv[] = { 
    "/usr/local/bin/DvdPlayer", 
    NULL };

  for(;;) {
    int status;

    msg("dvdplayer>");

    reset();

    if (g_bPowerdown) {
      system("echo O | /usr/local/bin/DvdPlayer");
    }
    else {
      status = run_child(argv);
    }

    if (WEXITSTATUS(status) == 99 || g_bPowerdown) {

      g_bPowerdown = false;

      system("rmmod ieee80211_crypt");
      system("rmmod ieee80211_rtl");
      system("rmmod r8187");
      system("rmmod ohci_hcd");
      system("rmmod ehci_hcd");

      system("echo mem>/sys/power/state");
      sleep(5);

      system("rm -rf /tmp/usbmounts/* /tmp/usbmounts/.* &");
      system("/sbin/modprobe ehci-hcd && /sbin/modprobe ohci-hcd &");

      continue;

    } else if (WEXITSTATUS(status) == 88) {

      break;

    }
    system("reboot");
    sleep(20);
  }
}

static void run_other_prog()
{
  char *argv[] = { 
    "/bin/sh",
    "-c",
    "/usr/local/etc/rootapp2.sh",
    NULL };
  int status;
  
  msg("rootapp2.sh>");

  reset();
  status = run_child(argv);
  g_bPowerdown = WEXITSTATUS(status) == 99;
}

int main(int argc, char *argv[])
{
  msg("coldstart");

  for(;;) {
    run_dvd_player();
    run_other_prog();
  }

  system("reboot");
}

