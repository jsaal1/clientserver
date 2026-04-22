#include "messagehandler.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <string>

// private byte members
void MessageHandler::sendByte(unsigned char byte) {
	conn_.write(byte);
}

unsigned char MessageHandler::recvByte() {
	unsigned char byte = conn_.read();
	return byte;
}

// send side
void MessageHandler::sendCode(Protocol code) {
	sendByte(static_cast<unsigned char>(code));
}

void MessageHandler::sendInt(int value) {
	sendByte(value >> 24 & 0xFF);
	sendByte(value >> 16 & 0xFF);
	sendByte(value >> 8 & 0xFF);
	sendByte(value & 0xFF);
}

void MessageHandler::sendIntParameter(int param) {
	sendCode(Protocol::PAR_NUM);
	sendInt(param);
}

void MessageHandler::sendStringParameter(const std::string& param) {
	sendCode(Protocol::PAR_STRING);
	sendInt(param.size());
	for (unsigned char c : param) {
		sendByte(c);
	}
}

// recv side
Protocol MessageHandler::recvCode() {
	return static_cast<Protocol>(conn_.read());
}

int MessageHandler::recvInt() {
	unsigned int value = conn_.read();
	value = (value << 8) | conn_.read();
	value = (value << 8) | conn_.read();
	value = (value << 8) | conn_.read();
	return static_cast<int>(value);
}

int MessageHandler::recvIntParameter() {
	if (recvCode() != Protocol::PAR_NUM) {
		throw ConnectionClosedException{};
	}
	return recvInt();	
}

std::string MessageHandler::recvStringParameter() {
	if (recvCode() != Protocol::PAR_STRING) {
		throw ConnectionClosedException{};
	}
	int n = recvInt();

	std::string s;
	s.reserve(n);

	for (int i{}; i < n; ++i) {
		s.push_back(conn_.read());
	}
	return s;
}
