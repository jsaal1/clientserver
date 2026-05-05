#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "connection.h"
#include "protocol.h"

#include <string>

class MessageHandler {
public:
	explicit MessageHandler(Connection& conn) 
	 : conn_(conn) {}

	void sendCode(Protocol);
	void sendInt(int);
	void sendIntParameter(int);
	void sendStringParameter(const std::string&);

	Protocol recvCode();
	int recvInt();
	int recvIntParameter();
	std::string recvStringParameter();
	
private:
	Connection& conn_;
	
	void sendByte(unsigned char);
	unsigned char recvByte();
};

#endif
