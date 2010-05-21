#ifndef __CLIENTSTUBBASE_H_
#define __CLIENTSTUBBASE_H_

#include <string>
#include <cerrno>

int socket_connect(const std::string &sockname);

class IMMSClientStub
{
public:
    void setup(bool xidle);
    void start_song(int position, const std::string& path);
    void end_song(bool at_the_end, bool jumped, bool bad);
    void select_next();
    void playlist_changed(int length);
    virtual ~IMMSClientStub() {}
protected:
    virtual void write_command(const std::string& line) = 0;
};

#endif
