#ifndef NEWSSERVER_H
#define NEWSSERVER_H

#include "server.h"
#include "connection.h"
#include "messagehandler.h"
#include "database.h"

#include <memory>

class NewsServer {
public:
	NewsServer(Server& server, Database& db)
		: server_(server), db_(db) {}

	void run();

private:
	Server& server_;
	Database& db_;

	void handleClient(const std::shared_ptr<Connection>&);

	void listNewsGroups(MessageHandler&);
	void createNewsGroup(MessageHandler&);
	void deleteNewsGroup(MessageHandler&);

	void listArticles(MessageHandler&);
	void createArticle(MessageHandler&);
	void deleteArticle(MessageHandler&);
	void getArticle(MessageHandler&);

	void recvEnd(MessageHandler& mh);
};

#endif
