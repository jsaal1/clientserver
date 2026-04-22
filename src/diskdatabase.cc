#include "diskdatabase.h"
#include "article.h"
#include "newsgroup.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;


// Path helpers
fs::path DiskDatabase::groupPath(int groupId) const {
	return root_ / std::to_string(groupId);
}

fs::path DiskDatabase::artPath(int groupId, int artId) const {
	return groupPath(groupId) / (std::to_string(artId) + ".art");
}


// Id -> name helper
std::string DiskDatabase::readName(int groupId) const {
	std::ifstream f(groupPath(groupId) / "name");
	std::string name;
	std::getline(f, name);
	return name;
}


// Read/write next id (agnostic of group or art);
int DiskDatabase::readNextId(const fs::path& file) const {
	std::ifstream in(file);
	if (!in)
		throw std::runtime_error("Could not open file");

	int id;
	if (! (in >> id))
		throw std::runtime_error("Failed to read id");

	return id;
} 

void DiskDatabase::writeNextId(const fs::path& file, int id) const {
	std::ofstream out(file);
	if (!out) 
		throw std::runtime_error("Could not open file");

	if (! (out << id))
		throw std::runtime_error("Failed to write id");
}


// constructor
DiskDatabase::DiskDatabase(const std::string& rootPath) : root_(rootPath) {
	fs::create_directories(root_);

	if (!fs::exists(root_ / "next_group_id"))
		writeNextId(root_ / "next_group_id", 1);

	if (!fs::exists(root_ / "group_names"))
		std::ofstream(root_ / "group_names");
}


// Get or throw helpers
void DiskDatabase::groupExistsOrThrow(int groupId) const {
	if (!fs::exists(groupPath(groupId)))
		throw NGDoesNotExistException{};
}

void DiskDatabase::articleExistsOrThrow(int groupId, int artId) const {
	groupExistsOrThrow(groupId);
	if (!fs::exists(artPath(groupId, artId)))
		throw ArtDoesNotExistException{};
}


// Newsgroup member functions
std::vector<NewsGroup> DiskDatabase::listNewsGroups() const {
	std::vector<NewsGroup> result;

	for (const auto& entry : fs::directory_iterator(root_)) {
		if (!fs::is_directory(entry))
			continue;
		int id = std::stoi(entry.path().filename().string());
		std::string name = readName(id);
		result.push_back(NewsGroup{id, name});
	}

	std::sort(result.begin(), result.end(), [](const NewsGroup& a, const NewsGroup& b) {
		return a.id < b.id;
	});

	return result;
}

void DiskDatabase::createNewsGroup(const std::string& name) {
	std::ifstream in(root_ / "group_names");

	std::string line;
	while (std::getline(in, line)) {
		if (name == line) 
			throw NGAlreadyExistsException{};
	}

	int id = readNextId(root_ / "next_group_id");

	fs::create_directory(groupPath(id));

	std::ofstream nameFile(groupPath(id) / "name");
	nameFile << name;

	writeNextId(groupPath(id) / "next_art_id", 1);

	std::ofstream groupNames(root_ / "group_names", std::ios::app);
	groupNames << name << "\n";

	writeNextId(root_ / "next_group_id", id + 1);
}

void DiskDatabase::deleteNewsGroup(int groupId) {
	groupExistsOrThrow(groupId);

	std::string name = readName(groupId);

	fs::remove_all(groupPath(groupId));

	std::ifstream in(root_ / "group_names");
	std::vector<std::string> names;
	std::string line;

	while(std::getline(in, line)) {
		if (line != name)
			names.push_back(line);
	}
	in.close();

	std::ofstream out(root_ / "group_names");
	for (const auto& n : names) {
		out << n << "\n";
	}
}


// Article member functions
std::vector<Article> DiskDatabase::listArticles(int groupId) const {
	groupExistsOrThrow(groupId);

	std::vector<Article> result;
	
	for (const auto& entry : fs::directory_iterator(groupPath(groupId))) {
		if (entry.is_directory() || entry.path().extension() != ".art")
			continue;
		
		int artId = std::stoi(entry.path().stem().string());
		
		std::ifstream in(entry.path());
		std::string title, author, text, line;
		std::getline(in, title);
		std::getline(in, author);
		while (std::getline(in, line))
			text += line + "\n";
		
		result.push_back(Article{artId, title, author, text});		
	}

	std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
		return a.id < b.id;
	});

	return result;
}

void DiskDatabase::createArticle(int groupId, const std::string& title, const std::string& author, const std::string& text) {
	groupExistsOrThrow(groupId);

	int artId = readNextId(groupPath(groupId) / "next_art_id");
	writeNextId(groupPath(groupId) / "next_art_id", artId + 1);

	std::ofstream out(artPath(groupId, artId));
	out << title << "\n" << author << "\n" << text;
}

void DiskDatabase::deleteArticle(int groupId, int artId) {
	articleExistsOrThrow(groupId, artId);

	fs::remove(artPath(groupId, artId));
}

Article DiskDatabase::getArticle(int groupId, int artId) const {
	articleExistsOrThrow(groupId, artId);

	std::ifstream in(artPath(groupId, artId));
	std::string title, author, text, line;
	std::getline(in, title);
	std::getline(in, author);

	while(std::getline(in, line))
		text += line + "\n";

	return Article{artId, title, author, text};
}
