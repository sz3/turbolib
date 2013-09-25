#include "LocalStreamSocketServer.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
using std::string;

LocalStreamSocketServer::LocalStreamSocketServer(const string& name)
	: _running(false)
	, _name(name)
{
}

LocalStreamSocketServer::~LocalStreamSocketServer()
{
	stop();
}

bool LocalStreamSocketServer::start()
{
	_running = true;
	_thread = std::thread( std::bind(&LocalStreamSocketServer::run, this) );

	if (!_waitForRunning.wait(5000))
		return false;
	return _running;
}

void LocalStreamSocketServer::stop()
{
	_running = false;
	if (_thread.joinable())
		_thread.join();
}

// the run function seems to contain the whole logic. Virtual-ify it and make a "SocketServer" parent class?
void LocalStreamSocketServer::run()
{
	int sock = socket(PF_UNIX, SOCK_STREAM, 0); // different than UDP server
	if (sock == -1)
	{
		fatalError("couldn't create socket!");
		return;
	}

	// also different. Remove inode for name
	::unlink(_name.c_str());

	//use different type of sockaddr
	struct sockaddr_un si_me;
	memset(&si_me, 0, sizeof(struct sockaddr_un));
	si_me.sun_family = AF_UNIX;
	snprintf(si_me.sun_path, PATH_MAX, _name.c_str());

	if (bind(sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		fatalError("couldn't bind to domain socket!");
		close(sock);
		return;
	}

	// stream socket, different behavior after here. (listen, accept, send, recv
	if(listen(sock, 5) != 0)
	{
		fatalError("listen() failed on domain socket");
		close(sock);
		return;
	}

	_waitForRunning.signal();

	struct sockaddr_un si_other;
	socklen_t slen = sizeof(si_other);

	int connection;
	while (_running && connection == accept(sock, (struct sockaddr*)&si_other, &slen) > -1)
		onConnect(connection);
	close(sock);
}

void LocalStreamSocketServer::onConnect(int fd)
{
	int nbytes;
	const int buflen = 1024;
	char buf[buflen];

	nbytes = read(fd, buf, buflen);
	buf[nbytes] = 0;

	printf("MESSAGE FROM CLIENT: %s\n", buf);
	nbytes = snprintf(buf, 256, "hello from the server");
	write(fd, buf, nbytes);

	close(fd);
}

bool LocalStreamSocketServer::isRunning() const
{
	return _running;
}

std::string LocalStreamSocketServer::lastError() const
{
	return _lastError;
}

void LocalStreamSocketServer::fatalError(const std::string& error)
{
	_lastError = error;
	_running = false;
	_waitForRunning.signal();
}
