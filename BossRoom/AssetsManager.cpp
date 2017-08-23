#include "AssetsManager.hpp"

#include <Windows.h>
#include <cassert>
#include <fstream>

std::map<std::string, sf::SoundBuffer> AssetsManager::_sounds;
std::map<std::string, nlohmann::json> AssetsManager::_jsons;
std::map<std::string, sf::Texture> AssetsManager::_textures;
std::map<std::string, sf::Music> AssetsManager::_musics;
std::map<std::string, sf::Image> AssetsManager::_images;
std::map<std::string, sf::Font> AssetsManager::_fonts;

AssetsManager::AssetsManager() {
}
AssetsManager::~AssetsManager() {
}

bool AssetsManager::loadTexture(const std::string &key, const std::string &path) {
	if (_textures.find(key) != std::end(_textures))
		return true;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	auto &ref = _textures[key];
	std::printf("%s ", path.c_str());

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const sf::Texture& AssetsManager::getTexture(const std::string &key) {
	auto it = _textures.find(key);
	assert(it != std::end(_textures) && "Texture don't exist");
	return it->second;
}

bool AssetsManager::loadImage(const std::string &key, const std::string &path) {
	if (_images.find(key) != std::end(_images))
		return true;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s ", path.c_str());
	auto& ref = _images[key];

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	} else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const sf::Image& AssetsManager::getImage(const std::string &key) {
	auto it = _images.find(key);
	assert(it != std::end(_images) && "Image don't exist");
	return it->second;
}

bool AssetsManager::loadFont(const std::string &key, const std::string &path) {
	if (_fonts.find(key) != std::end(_fonts))
		return true;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s", path.c_str());
	auto& ref = _fonts[key];

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	} else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const sf::Font& AssetsManager::getFont(const std::string &key) {
	auto it = _fonts.find(key);
	assert(it != std::end(_fonts) && "Font don't exist");
	return it->second;
}

bool AssetsManager::loadSound(const std::string &key, const std::string &path) {
	if (_sounds.find(key) != std::end(_sounds))
		return true;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s ", path.c_str());
	auto& ref = _sounds[key];

	const bool loaded = ref.loadFromFile(path);
	if (!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const sf::SoundBuffer& AssetsManager::getSound(const std::string &key) {
	auto it = _sounds.find(key);
	assert(it != std::end(_sounds) && "Sound don't exist");
	return it->second;
}

bool AssetsManager::loadJson(const std::string &key, const std::string &path) {
	if (_jsons.find(key) != std::end(_jsons))
		return true;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s ", path.c_str());

	std::ifstream stream;
	stream.open(path);
	const bool loaded = stream.is_open() && stream.good();

	if (!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		try {
			stream >> _jsons[key];
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
			printf(" Succes !\n");
		}
		catch (const std::exception& e) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf(" Couldn't load file /!\\ err: %s \n", e.what());
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const nlohmann::json& AssetsManager::getJson(const std::string &key) {
	auto it = _jsons.find(key);
	assert(it != std::end(_jsons) && "Sound don't exist");
	return it->second;
}



bool AssetsManager::openMusic(const std::string &key, const std::string &path) {
	if (_musics.find(key) != std::end(_musics))
		return true;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s ", path.c_str());
	auto& ref = _musics[key];

	const bool loaded = ref.openFromFile(path);
	if(!loaded) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	} else {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

sf::Music& AssetsManager::getMusic(const std::string &key) {
	auto it = _musics.find(key);
	assert(it != std::end(_musics) && "Music don't exist");
	return it->second;
}


