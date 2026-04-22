#include "inmemorydatabase.h"
#include "newsgroup.h"
#include "article.h"

#include <vector>
#include <string>
#include <map>
#include <set>

// Helper functions for groupentry
InMemoryDatabase::GroupEntry& InMemoryDatabase::getGroupEntryOrThrow(int groupId) {
	auto it = groups_.find(groupId);
	if (it == groups_.end())
		throw NGDoesNotExistException{};
	return it->second;
}

const InMemoryDatabase::GroupEntry& InMemoryDatabase::getGroupEntryOrThrow(int groupId) const {
	auto it = groups_.find(groupId);
	if (it == groups_.end())
		throw NGDoesNotExistException{};
	return it->second;
}

// Helper functions for articles
Article& InMemoryDatabase::getArticleOrThrow(GroupEntry& entry, int artId) {
	auto it = entry.articles.find(artId);
	if (it == entry.articles.end())
		throw ArtDoesNotExistException{};

	return it->second;
}

const Article& InMemoryDatabase::getArticleOrThrow(const GroupEntry& entry, int artId) const {
	auto it = entry.articles.find(artId);
	if (it == entry.articles.end())
		throw ArtDoesNotExistException{};

	return it->second;
}


// NewsGroup member functions
std::vector<NewsGroup> InMemoryDatabase::listNewsGroups() const {
	std::vector<NewsGroup> result;
	result.reserve(groups_.size());

	for (const auto& [id, entry] : groups_) {
		result.push_back(entry.group);
	}
	return result;
}

void InMemoryDatabase::createNewsGroup(const std::string& name) {
	if (groupNames_.count(name))
		throw NGAlreadyExistsException{};

	groupNames_.insert(name);
	groups_.emplace(
		nextGroupId_,
		GroupEntry{NewsGroup{nextGroupId_, name}, {}}
		);
	++nextGroupId_;
}

void InMemoryDatabase::deleteNewsGroup(int groupId) {
	auto& entry = getGroupEntryOrThrow(groupId);
	
	groupNames_.erase(entry.group.name);
	groups_.erase(groupId);
}

// Article member functions
std::vector<Article> InMemoryDatabase::listArticles(int groupId) const {
	const auto& entry = getGroupEntryOrThrow(groupId);

	std::vector<Article> result;
	result.reserve(entry.articles.size());

	for (const auto& [artId, art] : entry.articles) {
		result.push_back(art);
	}

	return result;
}

void InMemoryDatabase::createArticle(int groupId, const std::string& title, const std::string& author, const std::string& text) {
	auto& entry = getGroupEntryOrThrow(groupId);

	int artId = entry.group.nextArticleId;
	++entry.group.nextArticleId;

	entry.articles.emplace(
		artId,
		Article{artId, title, author, text}
		);
}

void InMemoryDatabase::deleteArticle(int groupId, int artId) {
	auto& entry = getGroupEntryOrThrow(groupId);
	getArticleOrThrow(entry, artId);
	entry.articles.erase(artId);
}

Article InMemoryDatabase::getArticle(int groupId, int artId) const {
	const auto& entry = getGroupEntryOrThrow(groupId);
	return getArticleOrThrow(entry, artId);
}
