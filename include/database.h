#ifndef DATABASE_H
#define DATABASE_H

#include "article.h"
#include "newsgroup.h"

#include <string>
#include <vector>

struct NGAlreadyExistsException {};
struct NGDoesNotExistException {};
struct ArtDoesNotExistException {};

class Database {
public:
	virtual ~Database() = default;

	virtual std::vector<NewsGroup> listNewsGroups() const = 0;

	virtual void createNewsGroup(const std::string&) = 0;
	virtual void deleteNewsGroup(int) = 0;

	virtual std::vector<Article> listArticles(int) const = 0;
	virtual void createArticle(int, const std::string&, const std::string&, const std::string&) = 0;
	virtual void deleteArticle(int groupId, int articleId) = 0;

	virtual Article getArticle(int groupId, int articleId) const = 0;
};

#endif
