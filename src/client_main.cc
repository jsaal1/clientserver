#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"
#include "protocol.h"

#include <iostream>
#include <limits>
#include <string>
#include <cstdlib>

Connection init(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: client host port\n";
		exit(1);
	}
	Connection conn(argv[1], std::stoi(argv[2]));
	if (!conn.isConnected()) {
		std::cerr << "Connection failed\n";
		exit(2);
	}
	return conn;
}

void printSeparator() {
	std::cout << std::string(30, '-') << "\n";
}

void printMenu() {
	std::cout << "1) List newsgroups\n"
			  << "2) Create newsgroup\n"
			  << "3) Delete newsgroup\n"
			  << "4) List articles by newsgroup\n"
			  << "5) Create article\n"
			  << "6) Delete article\n"
			  << "7) Get article\n"
			  << "8) Clear screen\n"
			  << ".) Quit \n";
	printSeparator();
	std::cout << "Choice: ";
}

void recvEnd(MessageHandler& mh) {
	if (mh.recvCode() != Protocol::ANS_END)
		throw ConnectionClosedException{};
}

void handleNak(MessageHandler& mh) {
	Protocol err = mh.recvCode();
	printSeparator();
	switch (err) {
		case Protocol::ERR_NG_ALREADY_EXISTS:  std::cout << "Newsgroup already exists\n"; break;
		case Protocol::ERR_NG_DOES_NOT_EXIST:  std::cout << "Newsgroup does not exist\n"; break;
		case Protocol::ERR_ART_DOES_NOT_EXIST: std::cout << "Article does not exist\n";   break;
		default: throw ConnectionClosedException{};
	}
	printSeparator();
}

void handleAnsNoParams(MessageHandler& mh) {
	Protocol ans = mh.recvCode();
	if (ans == Protocol::ANS_ACK) {
		printSeparator();
		std::cout << "Success!\n";
		printSeparator();
	} else
		handleNak(mh);
}

int readInt(const std::string& prompt) {
	int value;
	std::cout << prompt;
	while (!(std::cin >> value)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input, please enter a number: ";
	}
	return value;
}

void getAndSendGroupId(MessageHandler& mh) {
	mh.sendIntParameter(readInt("Enter groupId: "));
}

void getAndSendArtId(MessageHandler& mh) {
	mh.sendIntParameter(readInt("Enter articleId: "));
}

void listNewsGroups(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_LIST_NG);
	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_LIST_NG)
	      throw ConnectionClosedException{};

	int count = mh.recvIntParameter();
	if (!count) 
		std::cout << "None to print\n";
		
	while (count--) {
		int id = mh.recvIntParameter();
		std::string name = mh.recvStringParameter();
		std::cout << "GroupId (" << id << "): " << name << "\n";
	}
	printSeparator();
	
	recvEnd(mh);
}

void createNewsGroup(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_CREATE_NG);
	
	std::cin.ignore();	// clear leftover newline from prev input

	std::cout << "Enter newsgroup name: ";
	std::string name;
	std::getline(std::cin, name);


	mh.sendStringParameter(name);
	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_CREATE_NG) 
		throw ConnectionClosedException{};

	handleAnsNoParams(mh);

	recvEnd(mh);
}

void deleteNewsGroup(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_DELETE_NG);

	getAndSendGroupId(mh);
	
	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_DELETE_NG) 
		throw ConnectionClosedException{};

	handleAnsNoParams(mh);

	recvEnd(mh);
}

void listArticles(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_LIST_ART);

	getAndSendGroupId(mh);
	
	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_LIST_ART) 
		throw ConnectionClosedException{};

	printSeparator();
	
	Protocol ans = mh.recvCode();
	if (ans == Protocol::ANS_ACK) {
		int count = mh.recvIntParameter();
		if (!count) 
			std::cout << "None to print\n";
		
		while (count--) {
			int artId = mh.recvIntParameter();
			std::string title = mh.recvStringParameter();
			std::cout << "ArticleId (" << artId << "): " << title << "\n";
		}
	} else {
		handleNak(mh);
	}
	printSeparator();

	recvEnd(mh);
}

void createArticle(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_CREATE_ART);

	getAndSendGroupId(mh);

	std::cin.ignore();	// clear leftover newline from prev input

	std::cout << "Enter title: ";
	std::string title;
	std::getline(std::cin, title);

	std::cout << "Enter author: ";
	std::string author;
	std::getline(std::cin, author);

	std::cout << "Text (end with a line containing only '.'): ";
	std::string text, line;
	while (std::getline(std::cin, line) && line != ".") {
		text += line + "\n";
	}
	
	mh.sendStringParameter(title);
	mh.sendStringParameter(author);
	mh.sendStringParameter(text);
	
	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_CREATE_ART)
		throw ConnectionClosedException{};

	handleAnsNoParams(mh);

	recvEnd(mh);
}

void deleteArticle(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_DELETE_ART);

	getAndSendGroupId(mh);
	getAndSendArtId(mh);

	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_DELETE_ART)
		throw ConnectionClosedException{};

	handleAnsNoParams(mh);

	recvEnd(mh);
}

void getArticle(MessageHandler& mh) {
	mh.sendCode(Protocol::COM_GET_ART);

	getAndSendGroupId(mh);
	getAndSendArtId(mh);

	mh.sendCode(Protocol::COM_END);

	if (mh.recvCode() != Protocol::ANS_GET_ART)
		throw ConnectionClosedException{};

	Protocol ans = mh.recvCode();

	if (ans == Protocol::ANS_ACK) {
		printSeparator();
		std::string title = mh.recvStringParameter();
		std::string author = mh.recvStringParameter();
		std::string text = mh.recvStringParameter();
		std::cout << title << "\n"
				  << "By: " << author << "\n";
		printSeparator();
		std::cout << text << "\n";
	} else 
		handleNak(mh);
	printSeparator();

	recvEnd(mh);
}

int main(int argc, char* argv[]) {
	Connection conn = init(argc, argv);
	MessageHandler mh(conn);

	char choice;
	while (printMenu(), std::cin >> choice) {

		printSeparator();
		try {
			switch (choice) {
				case '1': listNewsGroups(mh); break;
				case '2': createNewsGroup(mh); break;
				case '3': deleteNewsGroup(mh); break;
				case '4': listArticles(mh); break;
				case '5': createArticle(mh); break;
				case '6': deleteArticle(mh); break;
				case '7': getArticle(mh); break;
				case '8': system("clear"); break;
				case '.': return 0;
				default: std::cout << "Unknown command\n";
			}
		} catch (ConnectionClosedException&) {
			std::cerr << "Server closed connection\n";
			return 1;
		}
	}
}
