#include "wget_wrapper.hpp"

#include "config.hpp"
#include "common-feature.hpp"

#include <fstream>
#include <iostream>
#define OBJOUTPUT(a) ( std::string( std::string("WgetWrapper: <") +  __func__ + "> " + (a) ).c_str())
using std::string;

void WgetWrapper::download_to_file(const std::string& url, const std::string& file)
{
  string url_new;
  int pos1, pos2;

  pos1 = 0;
  pos2 = url.find("'");

  while (pos2 > 0) {
    url_new.append(url.substr(pos1, pos2 - pos1));
    url_new.append("'\"'\"'");
    pos1 = pos2 + 1;
    pos2 = url.find("'", pos1);
  }
  url_new.append(url.substr(pos1));

#ifdef __UCLIBC__
  run::external_program(("wget '" + url_new + "' -O " + string_format::escape_string(file)).c_str());
#else
  run::external_program(("wget -nv '" + url_new + "' -O " + string_format::escape_string(file)).c_str());
#endif
  mmsSleep(1);
}

int WgetWrapper::set_download(const std::string& url){
  string url_new;
  int pos1, pos2;
  pos1 = 0;
  pos2 = url.find("'");
  pid_t pid;
  while (pos2 > 0) {
    url_new.append(url.substr(pos1, pos2 - pos1));
    url_new.append("'\"'\"'");
    pos1 = pos2 + 1;
    pos2 = url.find("'", pos1);
  }
  url_new.append(url.substr(pos1));

  int filedesc[2];
  if (pipe(filedesc) < 0){
    fprintf(stderr, OBJOUTPUT("Could not create pipe\n"));
    return -1; /* could not create pipe */
  }

  /* filedesc[0] == read end
   * filedesc[1] == write end
   */

  /* we fork */

  if ((pid = fork()) < 0){
    /* got error */
    fprintf(stderr, OBJOUTPUT("Could not fork()\n"));
    close (filedesc[0]);
    close (filedesc[1]);
    return -1;
  }

  else if (pid == 0){ /* child process */

    close(filedesc[0]); /* we don't do any reading */

    dup2(filedesc[1], STDOUT_FILENO); /* child's output redirected to write end of pipe */
    close(filedesc[1]);

    if (S_Config::get_instance()->p_debug_level() > 2)
      fprintf(stderr, "%s%s\n", OBJOUTPUT("downloading "), url_new.c_str());
    execl ("/bin/sh", "sh", "-c", ("wget '" + url_new + "' -qO -").c_str() , NULL);

    fprintf(stderr, OBJOUTPUT("Wget failed\n"));
    close(filedesc[1]);
    _exit(1);
    /* if execl returns, there was an error */
  }

  else{ /* parent process */
    close (filedesc[1]); /* Important: if we don't do this, child's side of pipe is never closed */
    return filedesc[0];
  }
}


FILE* WgetWrapper::download_to_temp(const std::string& url){

  /* we use mkstemp to get a unique file name, read from the pipe,
   * return the fd of the temp file. Once the latter is closed, the
   * file is automagically deleted */
  Config *conf = S_Config::get_instance();
  string tmp_location = conf->p_temp_image_loc() + "/wget-mms-XXXXXX";
  
  char path_buf[tmp_location.size()+1];
  strcpy (path_buf, tmp_location.c_str());
  
  mode_t oldp = umask(077);  /* Create file with user only permissions */
  int fd = mkstemp(path_buf);

  umask(oldp); /* restore standard permissions */

  if (fd < 0){ /* mkstemp failed */
    fprintf(stderr, OBJOUTPUT("Cannot create temp file. mkstemp() failed.\n"));
    return NULL;
  }

  FILE *temp_fileptr = fdopen(fd, "w+"); /* open file stream */
  if (temp_fileptr == NULL){
    fprintf(stderr, OBJOUTPUT("Cannot open the temp file as stream\n"));
    return NULL;
  }

  if (unlink(path_buf) == -1) { /* we unlink file, but its deletion is deferred
				     while the latter is in use */
    /* couldn't unlink file */
    fprintf(stderr, OBJOUTPUT("unlink() failed.\n"));
    fclose(temp_fileptr);
    return NULL;
  }

  int fd2 = set_download(url);
  if (fd2  < 0){ /* wget failed, clean up and exit */
    fclose(temp_fileptr);
    return NULL;
  }

  FILE *stream;
  if ( (stream = fdopen( fd2, "r" )) == NULL ){
    fprintf(stderr, OBJOUTPUT("Cannot create stream with read-side of pipe\n"));
    close(fd2);
    fclose(temp_fileptr);
    return NULL;
  }

  char linebuf[1024];
  int ret;

  while (true){
    ret = fread(linebuf, sizeof(char), 1024, stream);
    if (fwrite(linebuf, sizeof(char), ret, temp_fileptr) != ret){
      fprintf(stderr, OBJOUTPUT("Write error: %s \n"), s_strerror(errno).c_str());
      fclose(stream);
      fclose(temp_fileptr);
      return NULL;
    }

    if (ret < 1024){
      if (feof(stream))
	break;
      else if (ferror(stream)){
	fprintf(stderr, OBJOUTPUT("Read error: %s \n"), s_strerror(errno).c_str());
	fclose(stream);
	fclose(temp_fileptr);
	return NULL;
      }
    }
  }
  fclose(stream);
  rewind(temp_fileptr);
  return temp_fileptr;
}

bool WgetWrapper::download(const std::string& url, std::string& contents){
  int fd = set_download(url);
  if (fd  == -1){ /* wget failed, exit */
    fprintf(stderr, OBJOUTPUT("Wget failed\n"));
    return false;
  }
  
  /* we now open a stream with fd */

  FILE *stream;
  if ( (stream = fdopen( fd, "r" )) == NULL ){
    fprintf(stderr, OBJOUTPUT("Cannot create stream\n"));
    close(fd);
    return false;
  }
  
  char linebuf[1025];
  int ret;

  while (true){
    ret = fread(linebuf, sizeof(char), 1024, stream);
    linebuf[ret] = 0;
    contents += linebuf;
    if (ret < 1024){
      if (feof(stream))
	break;
      else if (ferror(stream)){
	fprintf(stderr, OBJOUTPUT("Read error: %s \n"), s_strerror(errno).c_str());
	fclose(stream);
	return false;
      }
    }
  }

  fclose(stream);
  return true;
}

