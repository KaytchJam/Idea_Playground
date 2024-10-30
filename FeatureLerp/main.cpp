#include <SFML/Graphics.hpp>
#include <iostream>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "dependencies/external/json.hpp"
#include "dependencies/internal/graph_types.hpp"

const uint32_t ALMOST_WHITE_BUT_COOLER = 0xE1E1E1FF;
const char* HEADER_TEXT_FONT = "fonts/BebasNeue-Regular.ttf";

int render_test() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "Feature Lerp");

	window.setFramerateLimit(30);
	while (window.isOpen()) {

		// event handling
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		// rendering
		window.clear(sf::Color(ALMOST_WHITE_BUT_COOLER));
		window.display();
	}

	window.close();
	return 0;
}

int build_graph(const std::string& file_path) {
	return 0;
}

int stream_json_objects(const std::string& file_path) {
	std::ifstream in;
	in.open(file_path);

	if (in.is_open()) {
		nlohmann::json data = nlohmann::json::parse(in);
		nlohmann::json out = data[0]["outList"];
		nlohmann::json names = data[0]["usernameList"];

		std::vector<std::vector<int>> out_list;
		std::vector<std::string> name_list;

		for (int i = 0; i < out.size(); i++) {
			out_list.push_back(out[i]);
			name_list.push_back(names[i]);
		}

		AbstractGraph<int, std::string> congress_network = AbstractGraph<int, std::string>::construct_with_list(name_list, out_list);
		std::cout << congress_network.to_string() << std::endl;
	}

	return 0;
}

int main() {
	return stream_json_objects("./data/congress_network_data.json");
	// render_test();
	// std::cout << "Finished rendering, hello world!" << std::endl;
}