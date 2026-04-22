#ifndef INMEMORYDATABASE_H
#define INMEMORYDATABASE_H

#include "database.h"
#include "article.h"
#include "newsgroup.h"

#include <vector>
#include <string>
#include <map>
#include <set>

class InMemoryDatabase : public Database {
public:
	InMemoryDatabase() : nextGroupId_(1) {}

	std::vector<NewsGroup> listNewsGroups() const override;
	void createNewsGroup(const std::string&) override;
	void deleteNewsGroup(int) override;

	std::vector<Article> listArticles(int) const override;
	void createArticle(int, const std::string&, const std::string&, const std::string&) override;
	void deleteArticle(int groupId, int artId) override;
	Article getArticle(int groupId, int artId) const override;

private:
	struct GroupEntry {
		NewsGroup group;
		std::map<int, Article> articles;
	};
	
	std::map<int, GroupEntry> groups_;
	std::set<std::string> groupNames_;

	int nextGroupId_;

	GroupEntry& getGroupEntryOrThrow(int);
	const GroupEntry& getGroupEntryOrThrow(int) const;

	Article& getArticleOrThrow(GroupEntry&, int);
	const Article& getArticleOrThrow(const GroupEntry&, int) const;
};

#endif
