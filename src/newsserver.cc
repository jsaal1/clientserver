#include "newsserver.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <string>

// run newsserver
void NewsServer::run() {
	while (true) {
		auto conn = server_.waitForActivity();
		if (conn == nullptr) {
			conn = std::make_shared<Connection>();
			server_.registerConnection(conn);
		} else {
			try {
				handleClient(conn);
			} catch (ConnectionClosedException&) {
				server_.deregisterConnection(conn);
			}
		}
	}
}

// handle client
void NewsServer::handleClient(const std::shared_ptr<Connection>& conn) {
	MessageHandler mh(*conn);
	Protocol param = mh.recvCode();
	switch (param) {
		case Protocol::COM_LIST_NG:		listNewsGroups(mh); break;
		case Protocol::COM_CREATE_NG: 	createNewsGroup(mh); break;
		case Protocol::COM_DELETE_NG: 	deleteNewsGroup(mh); break;

		case Protocol::COM_LIST_ART: 	listArticles(mh); break;
		case Protocol::COM_CREATE_ART: 	createArticle(mh); break;
		case Protocol::COM_DELETE_ART: 	deleteArticle(mh); break;
		case Protocol::COM_GET_ART:		getArticle(mh); break;

		default: throw ConnectionClosedException{};
	}
}

// helper for checking end of communication
void NewsServer::recvEnd(MessageHandler& mh) {
	if (mh.recvCode() != Protocol::COM_END) 
		throw ConnectionClosedException{};
}


// member functions for different newsgroup functionality
void NewsServer::listNewsGroups(MessageHandler& mh) {
	recvEnd(mh);
	mh.sendCode(Protocol::ANS_LIST_NG);

	auto groups = db_.listNewsGroups();
	mh.sendIntParameter(groups.size());
	for (const auto& ng : groups) {
		mh.sendIntParameter(ng.id);
		mh.sendStringParameter(ng.name);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::createNewsGroup(MessageHandler& mh) {
	std::string name = mh.recvStringParameter();
	recvEnd(mh);
	
	mh.sendCode(Protocol::ANS_CREATE_NG);
	try {
		db_.createNewsGroup(name);
		mh.sendCode(Protocol::ANS_ACK);
	} catch (NGAlreadyExistsException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_ALREADY_EXISTS);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::deleteNewsGroup(MessageHandler& mh) {
	int groupId = mh.recvIntParameter();
	recvEnd(mh);

	mh.sendCode(Protocol::ANS_DELETE_NG);
	try {
		db_.deleteNewsGroup(groupId);
		mh.sendCode(Protocol::ANS_ACK);
	} catch (NGDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::listArticles(MessageHandler& mh) {
	int groupId = mh.recvIntParameter();
	recvEnd(mh);

	mh.sendCode(Protocol::ANS_LIST_ART);
	try {
		auto arts = db_.listArticles(groupId);
		mh.sendCode(Protocol::ANS_ACK);
		mh.sendIntParameter(arts.size());
		for (const auto& art : arts) {
			mh.sendIntParameter(art.id);
			mh.sendStringParameter(art.title);
		}
	} catch (NGDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::createArticle(MessageHandler& mh) {
	int groupId = mh.recvIntParameter();
	std::string title = mh.recvStringParameter();
	std::string author = mh.recvStringParameter();
	std::string text = mh.recvStringParameter();
	recvEnd(mh);

	mh.sendCode(Protocol::ANS_CREATE_ART);
	try {
		db_.createArticle(groupId, title, author, text);
		mh.sendCode(Protocol::ANS_ACK);
	} catch (NGDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::deleteArticle(MessageHandler& mh) {
	int groupId = mh.recvIntParameter();
	int artId = mh.recvIntParameter();
	recvEnd(mh);

	mh.sendCode(Protocol::ANS_DELETE_ART);
	try {
		db_.deleteArticle(groupId, artId);
		mh.sendCode(Protocol::ANS_ACK);
	} catch (NGDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	} catch (ArtDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}

void NewsServer::getArticle(MessageHandler& mh) {
	int groupId = mh.recvIntParameter();
	int artId = mh.recvIntParameter();
	recvEnd(mh);

	mh.sendCode(Protocol::ANS_GET_ART);
	try {
		auto art = db_.getArticle(groupId, artId);
		mh.sendCode(Protocol::ANS_ACK);
		mh.sendStringParameter(art.title);
		mh.sendStringParameter(art.author);
		mh.sendStringParameter(art.text);
	} catch (NGDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	} catch (ArtDoesNotExistException&) {
		mh.sendCode(Protocol::ANS_NAK);
		mh.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
	}
	mh.sendCode(Protocol::ANS_END);
}
