#ifndef DISKDATABASE_H
#define DISKDATABASE_H

#include "database.h"
#include "article.h"
#include "newsgroup.h"

#include <filesystem>
#include <string>
#include <vector>

class DiskDatabase : public Database {
public:
	DiskDatabase(const std::string&);

	std::vector<NewsGroup> listNewsGroups() const override;
	void createNewsGroup(const std::string&) override;
	void deleteNewsGroup(int) override;

	std::vector<Article> listArticles(int) const override;
	void createArticle(int, const std::string&, const std::string&, const std::string&) override;
	void deleteArticle(int groupId, int artId) override;
	Article getArticle(int groupId, int artId) const override;
	
private:
	std::filesystem::path root_;

	std::filesystem::path groupPath(int) const;
	std::filesystem::path artPath(int groupId, int artId) const;

	void groupExistsOrThrow(int) const;
	void articleExistsOrThrow(int groupId, int artId) const;

	int readNextId(const std::filesystem::path&) const;
	void writeNextId(const std::filesystem::path&, int) const;

	std::string readName(int) const;
};

#endif
