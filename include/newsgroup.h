#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>

struct NewsGroup {
	int id;
	std::string name;
	int nextArticleId = 1;
};

#endif
