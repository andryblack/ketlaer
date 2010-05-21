#include "config.h"

#include "common-feature.hpp"
#include "input.hpp"

#include "plugins.hpp"
#include "SSaverobj.h"
// ugly c fork code
#include <sys/wait.h>

#include <errno.h>             /* per  errno */
#include <unistd.h>

std::string graphics::resolution_dependant_font_wrapper(int size, Config *conf)
{
  return "Vera/" + conv::itos(resolution_dependant_font_size(size, conf->p_v_res()));
}

int graphics::calculate_font_height(std::string font, Config *conf)
{
  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", font);
  return element_size.second - 5 - (conf->p_v_res()/405);
}

void aquire_exclusive_plugin_access()
{
  Plugins *plugins = S_Plugins::get_instance();
  if (!plugins->exclusive_access) {
    foreach (FeaturePlugin* plugin, plugins->fp_data)
      if (plugin->module != 0)
	plugin->module->begin_external();
    plugins->exclusive_access = true;
  }
}

void run::aquire_exclusive_access()
{
  S_ScreenUpdater::get_instance()->deactivate();
  S_BackgroundUpdater::get_instance()->deactivate();

#ifdef use_notify_area
  S_NotifyArea::get_instance()->disable();
#endif

  S_InputMaster::get_instance()->suspend();

  aquire_exclusive_plugin_access();
}

void run::release_exclusive_plugin_access()
{
  Plugins *plugins = S_Plugins::get_instance();
  if (plugins->exclusive_access) {
    foreach (FeaturePlugin* plugin, plugins->fp_data)
      if (plugin->module != 0)
	plugin->module->end_external();
    plugins->exclusive_access = false;
  }
}

void run::release_exclusive_access()
{
  S_ScreenUpdater::get_instance()->activate();
  S_BackgroundUpdater::get_instance()->activate();
#ifdef use_notify_area
  S_NotifyArea::get_instance()->enable();
#endif
  run::release_exclusive_plugin_access();
  S_InputMaster::get_instance()->wake_up();
}

// fork another program into the forground
bool run::exclusive_external_program(const std::string& command)
{
  S_SSaverobj::get_instance()->StopCounter();
  aquire_exclusive_access();

  Render *render = S_Render::get_instance();

  render->device->unlock();

  bool status = external_program(command);

  render->device->lock();

  release_exclusive_access();
  S_SSaverobj::get_instance()->ResetCounter();

  return status;
}

std::pair<bool, Pipe> run::exclusive_external_program_pipe(const std::string& command)
{
  aquire_exclusive_plugin_access();

  std::pair<bool, Pipe> status = external_program_pipe(command);

  return status;
}

// not so extreme forker :)
pid_t run::external_program(const std::string& command, bool wait_for_finish)
{
  int retstatus = 0;
  pid_t _waitpid = 0;

  pid_t pid = fork();

  if (pid == 0) {
    /* this is the child process. Execute the shell command. */
    // child inherits file descriptors
    // we don't want it to mess with them so we close them all
    // except  stdin, stdout and stderr
    for (int x = 3; x <sysconf(_SC_OPEN_MAX) - 1; x++)
      close(x);

    /* enable signals for external programs */
    /* so that they can bel killed if necessary */
    
    sigset_t signal_set;
    sigfillset( &signal_set );

    pthread_sigmask( SIG_UNBLOCK, &signal_set, NULL );
    execl ("/bin/sh", "sh", "-c", command.c_str(), NULL);

    // if we're here something went wrong
    fprintf(stderr, "Got error '%s' launching external program\n", strerror(errno));
    exit(1);
  } else if (pid < 0) {
    /* the fork failed. Report failure. */
    fprintf(stderr, "Got error %s: Couldn't fork\n", strerror(errno));
    return pid;
  } else {
    /* This is the parent process. Wait for child to end. */
    if (wait_for_finish) {
      _waitpid=waitpid(pid, &retstatus, 0);

      if (_waitpid <0)  //wait failed
	{
	  if (errno != ECHILD) /* if we get ECHILD, it's fine. It's a side effect of the thread safe 
				  zombie garbage collector */
	    fprintf(stderr, "Got error %s: Couldn't wait for child to exit\n", strerror(errno)); 
	  return pid;
	}

      //let's process return status
      if (WIFEXITED(retstatus)) {
	//child ended normally
	//      printf("Child ended with status %d\n",WEXITSTATUS(retstatus));
	return pid;
      }
      else if (WIFSIGNALED(retstatus)) { //let's see what went wrong
	fprintf(stderr, "Child exited due to %d signal\n", WTERMSIG(retstatus));
	return pid;
      }
      else if(WIFSTOPPED(retstatus)) {
	fprintf(stderr, "Child process was stopped\n");
	return pid;
      }

      //this should never be executed
      fprintf(stderr, "Undefined or unhandled child terminatio\n");
      return pid;
    }
  }

  return pid;
}

std::pair<bool, Pipe> run::external_program_pipe(const std::string& command)
{
  Pipe p;

  int inpipe[2], outpipe[2], pipefl = 0;

  if (pipe(inpipe) == -1) {
    std::cerr << "ERROR: pipe failed for inpipe" << std::endl;
    return std::make_pair(false, p);
  }
  pipefl|=1+2;
  if(pipe(outpipe)==-1) {
    std::cerr << "ERROR: pipe failed for outpipe" << std::endl;
    return std::make_pair(false, p);
  }
  pipefl|=4+8;

  pid_t pid = fork();

  if (pid == 0)
    {
      close(inpipe[1]);
      close(outpipe[0]);

      if (dup2(inpipe[0], STDIN_FILENO) < 0 ||
         dup2(outpipe[1],STDOUT_FILENO) < 0 ||
         dup2(outpipe[1],STDERR_FILENO) < 0)
	std::cerr << "ERROR: dup2() failed" << std::endl;

      close(inpipe[0]);
      close(outpipe[1]);

      /* this is the child process. Execute the shell command. */
      execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
      exit(0);
    }
  else if (pid < 0)
    /* the fork failed. Report failure. */
    return std::make_pair(false, p);

  close(inpipe[0]); pipefl&=~1;
  close(outpipe[1]); pipefl&=~8;
  fcntl(outpipe[0],F_SETFL,O_NONBLOCK);

  p.inpipe0 = inpipe[0];
  p.inpipe1 = inpipe[1];
  p.outpipe0 = outpipe[0];
  p.outpipe1 = outpipe[1];
  p.pipefl = pipefl;
  p.pid = pid;

  return std::make_pair(true, p);
}

void run::close_pipe(const Pipe& p)
{
  kill(p.pid,SIGTERM);
  if(p.pipefl&1) close(p.inpipe0);
  if(p.pipefl&2) close(p.inpipe1);
  if(p.pipefl&4) close(p.outpipe0);
  if(p.pipefl&8) close(p.outpipe1);
}
