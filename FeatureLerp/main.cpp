#include <SFML/Graphics.hpp>
#include <iostream>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <print>

// NOT MADE BY ME. COOL LIBRARIES MADE BY CLEVER PEOPLE
#include "dependencies/external/json.hpp"

// NEWER TOOLS
#include "dependencies/internal/graph_types.hpp"
#include "dependencies/internal/interval_slice.h"

// OLDER STUFF I MADE BUT IT STILL WORKS I GUESS LOL
#include "dependencies/internal/PointerVector.h"
#include "dependencies/internal/SubCanvas.h"
#include "dependencies/internal/lalg4.hpp"

//constexpr uint32_t ALMOST_WHITE_BUT_COOLER = 0xE1E1E1FF;
constexpr uint32_t DARKISH_GOLD_COLOR = 0xB0A856FF;
constexpr uint32_t CURRENT_LIGHT_YELLOW_COLOR = 0xFBF07BFF;
constexpr uint32_t SELECTED_DARK_GRAY_COLOR = 0x242424FF;
constexpr uint32_t UNSELECTED_HOTTISH_DARK_PINK = 0x7C5295FF;
//constexpr uint32_t UNSELECTED_LIGHT_GRAY_COLOR = 0x999999FF;
//constexpr uint32_t CURRENT_SWEET_ORANGE = 0xFF8B3DFF;
constexpr const char* HEADER_TEXT_FONT = "fonts/BebasNeue-Regular.ttf";
constexpr float PADDING = 20.f;

static sf::Vector2f lalgToSf(const lalg::vec4& v) {
	return { v.r, v.g };
}

static lalg::vec4 sfToLalg(const sf::Vector2f& sfVec) {
	return { sfVec.x, sfVec.y, 1, 1 };
}

sf::Vector2f get_global_force(const sf::Vector2f& local_pos, int WIN_LENGTH, int WIN_HEIGHT, float FORCE_MULTIPLIER) {
	const lalg::mat4 bound_mouse_coords =
	{{ 2.f / WIN_LENGTH, 0.f,  -1.f,  0.f },
	 { 0.f, 2.f / WIN_HEIGHT,  -1.f,  0.f },
	 { 0.f, 0.f,                1.f,  0.f },
	 { 0.f, 0.f,                0.f,  1.f }};
	float call = 0;
	return lalgToSf(bound_mouse_coords * lalg::map_capture(sfToLalg({ (float)local_pos.x, (float)local_pos.y }), [&WIN_LENGTH, &WIN_HEIGHT, &call](float f) {
		return std::fmaxf(0, std::fminf(f, ((float)WIN_LENGTH * !(call == 1)) + (WIN_HEIGHT * (call++ == 1)))); }) * FORCE_MULTIPLIER);
}

sf::Vector2f get_global_force(const sf::Vector2i& mouse_pos, int WIN_LENGTH, int WIN_HEIGHT, float FORCE_MULTIPLIER) {
	sf::Vector2f local_pos = sf::Vector2f((float)mouse_pos.x, (float)mouse_pos.y);
	return get_global_force(local_pos, WIN_LENGTH, WIN_HEIGHT, FORCE_MULTIPLIER);
}

std::vector<sf::RectangleShape> make_rectangle_vector(float length, float width, float start_x, float start_y, size_t size) {
	std::vector<sf::RectangleShape> rect_vec;
	for (size_t i = 0; i < size; i++) {
		rect_vec.push_back(sf::RectangleShape(sf::Vector2f(length, width)));
		rect_vec[i].setFillColor(sf::Color(UNSELECTED_HOTTISH_DARK_PINK));
		rect_vec[i].setPosition(sf::Vector2f(start_x + i * width + PADDING * i, start_y));
	}
	return rect_vec;
}

template<typename T> 
std::vector<T*> make_pointer_vec(std::vector<T>& v) {
	std::vector<T*> v_ptrs;
	for (T* ptr : v | std::views::transform([](auto& in) { return &in;  })) {
		v_ptrs.push_back(ptr);
	}
	return v_ptrs;
}

int ncslice_demonstration() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "Feature_Lerp");

	constexpr const float RECT_W = 40.f;
	constexpr const float RECT_H = 40.f;
	constexpr const size_t TOTAL_RECTS = 20;

	std::vector<sf::RectangleShape> rvec = make_rectangle_vector(
		RECT_H, 
		RECT_W, 
		WIN_LENGTH / 2.f - (RECT_W + PADDING) * TOTAL_RECTS / 2.f, 
		WIN_HEIGHT / 2.f - RECT_H / 2, 
		TOTAL_RECTS
	);

	SubCanvas canva(sf::Vector2i(WIN_LENGTH, WIN_HEIGHT), sf::Vector2i(WIN_LENGTH, WIN_HEIGHT)); // sf::Vector2i((int) (rvec.back().getPosition().x - rvec.front().getPosition().x), RECT_H)

	NCSlice<sf::RectangleShape> rvec_view(rvec); 
	NCSlice<sf::RectangleShape>::iterator it = rvec_view.begin();

	sf::Font my_font;
	assert(my_font.loadFromFile(HEADER_TEXT_FONT) && "Was unable to load font.");

	sf::Text my_text;
	my_text.setFont(my_font);
	my_text.setCharacterSize(36);
	my_text.setPosition(rvec.front().getPosition() - sf::Vector2f(0, PADDING * 3 + my_text.getLocalBounds().getSize().y));
	my_text.setFillColor(sf::Color(SELECTED_DARK_GRAY_COLOR));
	my_text.setString("NCSlice<T> Demonstration");

	sf::Text intervals_text;
	intervals_text.setFont(my_font);
	intervals_text.setCharacterSize(24);
	intervals_text.setPosition(rvec.front().getPosition() + sf::Vector2f(0, PADDING * 2));
	intervals_text.setFillColor(sf::Color(SELECTED_DARK_GRAY_COLOR));
	intervals_text.setString("Internal Ranges = " + rvec_view.ranges_to_string());

	sf::Text index_text;
	index_text.setFont(my_font);
	index_text.setCharacterSize(24);
	index_text.setPosition(intervals_text.getPosition() + sf::Vector2f(0, PADDING));
	index_text.setFillColor(sf::Color(SELECTED_DARK_GRAY_COLOR));
	index_text.setString("Index = 0");

	PointerVector<sf::RectangleShape> rptr(make_pointer_vec(rvec));
	PointerVector<sf::Text> tptr({ &my_text, &intervals_text, &index_text });
	canva.link_group(&rptr.cast_inner<sf::Drawable>())
		.link_group(&tptr.cast_inner<sf::Drawable>())
		.setBackgroundColor(sf::Color(DARKISH_GOLD_COLOR));

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		// event handling
		sf::Event event;
		sf::Vector2f force = get_global_force(sf::Mouse::getPosition(window), window.getSize().x, window.getSize().y, 20);

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyReleased && it != rvec_view.end()) {
				// Keyboard Input Events
				if (event.key.code == sf::Keyboard::Left)  { it = it == rvec_view.begin() ? rvec_view.rbegin() : --it; }
				if (event.key.code == sf::Keyboard::Right) { it++; }
				if (event.key.code == sf::Keyboard::Space) {
					it->setFillColor(sf::Color(UNSELECTED_HOTTISH_DARK_PINK));
					it = rvec_view.remove((*it).first);
					intervals_text.setString("Internal Ranges = " + rvec_view.ranges_to_string());
				}

				// Update the Index Text
				if (it != rvec_view.end()) {
					index_text.setString("Index = " + std::to_string((*it).first));
				} else if (rvec_view.begin() != rvec_view.end()) {
					it = rvec_view.begin();
					index_text.setString("Index = " + std::to_string((*it).first));
				} else {
					intervals_text.setString("Internal Ranges = NONE");
					index_text.setString("Index = NONE");
				}
			}
		}

		// all rendering
		window.clear(sf::Color(DARKISH_GOLD_COLOR));
		for (std::pair<size_t, sf::RectangleShape&> rp : rvec_view) {
			rp.second.setFillColor(sf::Color(SELECTED_DARK_GRAY_COLOR));
		}

		if (it != rvec_view.end()) {
			it->setFillColor(sf::Color(CURRENT_LIGHT_YELLOW_COLOR));
		}

		// rendering
		window.draw(canva.pull(force));
		window.display();
	}

	window.close();
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
		AbstractGraph<int, AbstractGraph<int, std::string>> SCC = congress_network.find_sccs();
		std::cout << SCC.to_string() << std::endl;

		for (auto& SG : SCC) {
			if (SG.size() == 1) {
				for (const std::string& elem : SG) {
					std::cout << elem << std::endl;
				}
			}
		}
	}

	return 0;
}

int graph_test_two(const std::string& file_path) {
	std::ifstream in;
	in.open(file_path);

	std::string cur_line;
	if (in.is_open()) {
		typedef std::unordered_map<int, std::vector<int>>::iterator emap_iter;
		/*int lines_to_skip = 4;
		int skipped = 0;
		while (skipped < 4) {
			skipped++;
			std::getline(in, cur_line);
		}*/

		std::unordered_map<int, std::vector<int>> edges;
		int line = 0;
		while (std::getline(in, cur_line)) {
			std::istringstream is(cur_line);

			int from;
			int to;

			is >> from;
			is >> to;

			edges.try_emplace(to, std::vector<int>());
			emap_iter item = edges.find(from);
			if (item == edges.end()) {
				edges.insert(std::make_pair(from, std::vector<int>(1, to)));
			} else {
				item->second.push_back(to);
			}

			line++;
		}

		std::vector<int> nodes = std::views::iota(0, (int) edges.size()) | std::views::transform([](const int x) -> int { std::cout << x << std::endl; return x; }) | std::ranges::to<std::vector<int>>();
		AbstractGraph<int, int> G = AbstractGraph<int, int>::construct_with_list(std::move(nodes), std::move(edges));
		std::cout << G.find_sccs().to_string() << std::endl;
	}

	return 0;
}

int graph_test() {
	std::vector<int> nums(9);
	std::iota(nums.begin(), nums.end(), 0);
	std::vector<std::vector<int>> connections = { {1, 5} ,{2, 7},{0, 4},{0},{3, 1},{6},{7},{5, 8},{} };

	AbstractGraph<int,int> G = AbstractGraph<int, int>::construct_with_list(nums, connections);
	AbstractGraph<int,int> R = G.transpose();

	std::cout << G.to_string() << std::endl;
	std::cout << R.to_string() << std::endl;

	std::vector<int> post_ordering = R.post_order(0);
	AbstractGraph<int,int>::print_path(post_ordering);

	for (int b : G.dfs(8)) {
		std::cout << b << " ";
	}
	std::cout << std::endl;

	AbstractGraph<int, AbstractGraph<int, int>> sccs = G.find_sccs();
	std::cout << sccs.to_string() << std::endl;

	return 1;
}

void ranges_test() {
	std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 ,9, 10 };
	for (const int elem : v | std::ranges::views::drop(1)) {
		std::cout << elem << std::endl;
	}
}

void graph_and_range_test() {
	typedef std::pair<int, std::vector<int>> edge;
	typedef std::unordered_map<int, std::vector<int>> graph;
	AbstractGraph<int, int> G = AbstractGraph<int, int>::construct_with_list(
		std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8},
		graph{
			edge(0, { 1, 2 }),
			edge(1, { 4 }),
			edge(2, { 3,4 }),
			edge(3, { 0 }),
			edge(4, { 0, 1 }),
			edge(5, { 2 }),
			edge(6, { 5 }),
			edge(7, { 6, 8 }),
			edge(8, { 7 })
		}
	);

	float (*average_graph_func)(AbstractGraph<int, int>&) = [](AbstractGraph<int, int>& g) {
		return (float)std::ranges::fold_left(g, 0, std::plus<>()) / (float)g.size();
		};

	for (std::tuple<int, float&> i : std::views::all(G.find_sccs()) | std::views::transform(average_graph_func) | std::ranges::to<std::vector<float>>() | std::views::enumerate) {
		std::cout << "SUBGRAPH INDEX: " << std::get<0>(i) << ", SUBGRAPH AVERAGE: " << std::get<1>(i) << std::endl;
	}
}


void post_ordering_test() {
	typedef std::pair<int, std::vector<int>> edge;
	AbstractGraph<int, int> G = AbstractGraph<int, int>::construct_with_list(
		std::vector<int>{0, 1, 2, 3, 4, 5, 6},
		std::unordered_map<int, std::vector<int>> {
			edge(0, { 1 }),
			edge(1, { 2, 3, 4}),
			edge(2, { 4 }),
			edge(3, { 5 }),
			edge(4, { 1 }),
			edge(5, { 6 }),
			edge(6, { 3 })
		}
	);

	for (size_t i = 0; i < 7; i++) {
		std::unordered_map<int, int> counts;
		std::cout << "start vertex = " << i << std::endl;
		std::vector<AbstractGraph<int,int>::scc_pair> SCC = G.color_scc_vertices(i);
		for (AbstractGraph<int, int>::scc_pair& p : SCC) {
			std::print("(VERTEX_ID: {}, SCC_IDX: {})\n", p.id, p.component_index);
			counts[p.id] = p.component_index;
		}
		std::cout << std::endl;

		std::unordered_map<int, int> component_counts;
		for (std::pair<const int, int>& cts : std::move(counts)) {
			component_counts[cts.second] += 1;
		}

		for (std::pair<const int, int>& csts : component_counts) {
			std::print("COMPONENT: {}, VERTICES: {}\n", csts.first, csts.second);
		}

		std::cout << std::endl;
	}

	AbstractGraph<int, AbstractGraph<int, int>> TRUE_SCC = G.find_sccs();
	std::cout << TRUE_SCC.to_string() << std::endl;
}

void numeric_range_test() {
	numeric_range my_range(10 + 1);
	for (int i : std::views::transform(my_range, [](int x) { return x * 2;  })) {
		std::cout << i << std::endl;
	}
}

void split_slice_test() {
	std::vector<std::string> V = {"I", "GOT", "A", "92", "FLAMETHROWER", "I", "GOT", "A", "92", "92"};

	NCSlice<std::string> slice(V);
	slice.remove_all({ 1, 3, 5, 6, 7, 9 });

	std::string& (*extract_string)(std::pair <size_t, std::string&>) = [](std::pair<size_t, std::string&> p) -> std::string& {
		return p.second;
	};;

	for (std::string& sp : std::views::transform(slice, extract_string)) {
		std::cout << sp << std::endl;
	}
}

void remove_in_for_loop_test() {
	std::vector<int> V = { 1, 2, 3, 4, 5, 6 };
	NCSlice<int> S(V);

	// we will get an error? -> Yes we do
	/*for (std::pair<size_t, int> si : S) {
		std::print("{}\n", si.second);
		S.remove(si.first);
	}*/

	for (NCSlice<int>::iterator it = S.begin(); it != S.end(); it++) {
		std::print("{}\n", (*it).second);
		it = S.remove((*it).first);
	}

	S.print_intervals();
}

int main() {
	//static_assert(std::ranges::forward_range<AbstractGraph<int, int>>);
	//ranges_test();
	// graph_and_range_test();
	//numeric_range_test();
	//split_slice_test();
	// return graph_test_two("./data/facebook_combined.txt");
	/*std::ranges::iota_view<int, int> my_iota = std::views::iota(0, 20);
	auto iter = my_iota.begin();

	while (iter != my_iota.end()) {
		std::cout << *iter << std::endl;
		iter++;
	}*/
	//post_ordering_test();
	ncslice_demonstration();
	//remove_in_for_loop_test();
	return 0;
}