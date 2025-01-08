#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <limits>
#include <queue>
#include <stack>
#include <ranges>

#ifndef PROJECT_CHEST_PATH
#define PROJECT_CHEST_PATH "chest.hpp"
#endif

#ifndef PROJECT_INTERVAL_SLICE_PATH
#define PROJECT_INTERVAL_SLICE_PATH "interval_slice.h"
#endif

#include PROJECT_CHEST_PATH
#include PROJECT_INTERVAL_SLICE_PATH

struct UnequalLengthException : public std::exception {
    UnequalLengthException() {}
    const char* what() {
        return "The Key vector and Value vectors passed in must have equal length.";
    }
};

template<typename Id, typename Element>
class AbstractGraph {
public:
    struct Vertex {
        Id m_identifier;
        Element m_elem;
        std::unordered_set<Id> m_edges;

        // create an empty vertex
        static Vertex empty_vertex(Id m_identifier, Element m_elem) {
            Vertex v;
            v.m_identifier = m_identifier;
            v.m_elem = m_elem;
            return v;
        }

        // create an edge between two vertices
        static void connect(Vertex& v1, Vertex& v2) {
            v1.m_edges.insert(v2.m_identifier);
            v2.m_edges.insert(v1.m_identifier);
        }

        // remove an edge between two vertices
        static void disconnect(Vertex& v1, Vertex& v2) {
            v1.m_edges.erase(v2.m_identifier);
            v2.m_edges.erase(v1.m_identifier);
        }

        // create a directed edge from v1 to v2
        static void direct(Vertex& v1, Vertex& v2) {
            v1.m_edges.insert(v2.m_identifier);
        }

        // Return the out degree of the vertex (the size of its set of m_edges)
        int out_degree() const noexcept {
            return (int)this->m_edges.size();
        }

        // Returns the key of this vertex
        Id key() const {
            return this->m_identifier;
        }

        // Returns the value held in this vertex
        Element value() const {
            return this->m_elem;
        }

        // Returns the unordered_set of outgoing m_edges from this edge
        const std::unordered_set<Id>& branches() const {
            return this->m_edges;
        }

        // Return a string representation of the vertex
        std::string to_string() const {
            std::stringstream ss;
            ss << this->m_identifier << " : " << this->m_elem << " -> {";
            if (this->m_edges.size() > 0) {
                for (Id other_ids : this->m_edges) {
                    ss << " " << other_ids;
                }
            }
            ss << " }";
            return ss.str();
        }

        // Are two m_vertices equal?
        bool operator==(const Vertex& other) const {
            return this->m_identifier == other.m_identifier && this->m_elem == other.m_elem;
        }
    };

private:
    std::unordered_map<Id, Vertex> m_vertices;

    // Adds a vertex to the graph with Key 'm_identifier' and holding value 'm_elem'
    Vertex& add_vertex(const Id& m_identifier, const Element& m_elem) {
        this->m_vertices.insert({ m_identifier, Vertex::empty_vertex(m_identifier, m_elem) });
        return this->m_vertices.at(m_identifier);
    }

public:
    AbstractGraph() {}
    //AbstractGraph(AbstractGraph<Id, Element>&& other) : m_vertices(std::move(other.m_vertices)) {}
    ~AbstractGraph() {}

    // retrieve a element in the graph with the given Id value
    // if the vertex with the Id is present, returns a stuffed Chest with the corresponding element
    // if the vertex with the Id is not present, it returns a signed Chest with a "no vertex could be found" message
    Chest<Element> get(Id key) const {
        auto it = this->m_vertices.find(key);
        if (it != this->m_vertices.end()) {
            return Chest<Element>::stuff(it->second.value());
        }

        return Chest<Element>::sign("No Vertex matching the key could be found.");
    }

    // Returns the total number of m_vertices in the graph
    // Time Complexity: O(1)
    size_t size() const noexcept {
        return this->m_vertices.size();
    }

    // Returns a vector of all vertex keys
    // Time Complexity: O(N)
    std::vector<Id> key_set() const {
        int idx = 0;
        std::vector<Id> ks(this->m_vertices.size());
        for (std::pair<const Id, const Vertex> p : this->m_vertices) {
            ks[idx] = p.first;
            idx++;
        }

        return ks;
    }

    // Time Complexity: O(N)
    Chest<std::vector<Id>> neighborhood(const Id& key) const {
        auto it = this->m_vertices.find(key);
        if (it == this->m_vertices.end()) return Chest<std::vector<Id>>::sign("No Vertex matching the key could be found.");

        const Vertex& v = it->second;
        std::vector<Id> neighbors(v.out_degree());
        for (Id keys : v.branches()) {
            neighbors.push_back(keys);
        }

        return Chest<std::vector<Id>>::stuff(std::move(neighbors));
    }

    // Time Complexity: O(N)
    template<typename Key, typename Val>
    static AbstractGraph<Key, Val> null(const std::vector<Key>& keys, const std::vector<Val>& values) {
        if (keys.size() != values.size()) throw UnequalLengthException();
        AbstractGraph<Key, Val> null_graph;
        const int num_vertices = (int)keys.size();
        for (int i = 0; i < num_vertices; i++) {
            null_graph.add_vertex(keys[i], values[i]);
        }

        return null_graph;
    }

    // Time Complexity: O(N)
    template<typename Val>
    static AbstractGraph<int, Val> null(const std::vector<Val>& values) {
        AbstractGraph<int, Val> null_graph;
        const int num_vertices = (int)values.size();
        for (int i = 0; i < num_vertices; i++) {
            null_graph.add_vertex(i, values[i]);
        }

        return null_graph;
    }

    // Copy the vertex set of a graph as a new graph
    // Maintains the Id, Element vertex pairs but no vertices are connected
    AbstractGraph<Id, Element> null() const noexcept {
        AbstractGraph<Id, Element> null_graph;

        for (const std::pair<const Id, const Vertex>& vpair : this->m_vertices) {
            null_graph.add_vertex(vpair.first, vpair.second.m_elem);
        }

        return null_graph;
    }

    // Time Complexity: O(N^2)
    template<typename Key, typename Val>
    static AbstractGraph<Key, Val> complete(const std::vector<Key>& keys, const std::vector<Val>& values) {
        AbstractGraph<Key, Val> complete_graph = AbstractGraph<Key, Val>::null(keys, values);
        const int num_vertices = (int)keys.size();

        for (int i = 0; i < num_vertices; i++) {
            Vertex& cur = complete_graph.m_vertices[keys[i]];
            for (int j = i + 1; j < num_vertices; j++) {
                Vertex::connect(cur, complete_graph.m_vertices[keys[j]]);
            }
        }

        return complete_graph;
    }

    // Time Complexity: O(N^2)
    template<typename Val>
    static AbstractGraph<int, Val> complete(const std::vector<Val>& values) {
        AbstractGraph<int, Val> complete_graph = AbstractGraph<int, Val>::null(values);
        const int num_vertices = (int)values.size();

        for (int i = 0; i < num_vertices; i++) {
            Vertex& cur = complete_graph.m_vertices[i];
            for (int j = i + 1; j < num_vertices; j++) {
                Vertex::connect(cur, complete_graph.m_vertices[j]);
            }
        }

        return complete_graph;
    }

    // Time Complexity: O(N)
    template<typename Key, typename Val>
    static AbstractGraph<Key, Val> cycle(const std::vector<Key>& keys, const std::vector<Val>& values) {
        if (keys.size() != values.size()) throw UnequalLengthException();
        AbstractGraph<Key, Val> cycle_graph;
        const int num_vertices = (int)keys.size();

        if (num_vertices > 0) {
            Vertex* prev = &cycle_graph.add_vertex(keys[0], values[0]);
            for (int i = 1; i < num_vertices; i++) {
                Vertex* cur_vertex = &cycle_graph.add_vertex(keys[i], values[i]);
                Vertex::connect(*prev, *cur_vertex);
                prev = cur_vertex;
            }

            if (num_vertices > 1) Vertex::connect(*prev, cycle_graph.m_vertices.at(keys[0]));
        }

        return cycle_graph;
    }

    // Time Complexity: O(values.size())
    template<typename Val>
    static AbstractGraph<int, Val> cycle(const std::vector<Val>& values) {
        AbstractGraph<int, Val> cycle_graph;
        const int num_vertices = (int)values.size();

        if (num_vertices > 0) {
            Vertex* prev = &cycle_graph.add_vertex(0, values[0]);
            for (int i = 1; i < num_vertices; i++) {
                Vertex* cur_vertex = &cycle_graph.add_vertex(i, values[i]);
                Vertex::connect(*prev, *cur_vertex);
                prev = cur_vertex;
            }

            if (num_vertices > 1) Vertex::connect(*prev, cycle_graph.m_vertices.at(0));
        }

        return cycle_graph;
    }

    // Time Complexity: O(values.size())
    template<typename Val>
    static AbstractGraph<int, Val> wheel(const std::vector<Val>& values) {
        AbstractGraph<int, Val> wheel_graph = AbstractGraph<int, Val>::null(values);
        const int num_vertices = (int)values.size();

        for (int i = 1; i < num_vertices; i++) {
            bool upper_lim = i == num_vertices - 1;
            bool lower_lim = i == 1;

            int prev = (i - 1) * !lower_lim + (num_vertices - 1) * lower_lim;
            int next = (i + 1) * !upper_lim + upper_lim;

            Vertex::connect(wheel_graph.m_vertices[i], wheel_graph.m_vertices[0]);
            Vertex::connect(wheel_graph.m_vertices[i], wheel_graph.m_vertices[prev]);
            Vertex::connect(wheel_graph.m_vertices[i], wheel_graph.m_vertices[next]);
        }

        return wheel_graph;
    }

    // Time Complexity: O(keys.size())
    template<typename Key, typename Val>
    static AbstractGraph<Key, Val> wheel(const std::vector<Key>& keys, const std::vector<Val>& values) {
        AbstractGraph<Key, Val> wheel_graph = AbstractGraph<Key, Val>::null(keys, values);
        const int num_vertices = (int)values.size();
        Vertex& zero_vertex = wheel_graph.m_vertices.at(keys[0]);

        for (int i = 1; i < num_vertices; i++) {
            bool upper_lim = i == num_vertices - 1;
            bool lower_lim = i == 1;

            int prev = (i - 1) * !lower_lim + (num_vertices - 1) * lower_lim;
            int next = (i + 1) * !upper_lim + upper_lim;

            Vertex& cur_vertex = wheel_graph.m_vertices.at(keys[i]);
            Vertex::connect(cur_vertex, zero_vertex);
            Vertex::connect(cur_vertex, wheel_graph.m_vertices[keys[prev]]);
            Vertex::connect(cur_vertex, wheel_graph.m_vertices[keys[next]]);
        }

        return wheel_graph;
    }

    // Time Complexity: O(N^2)
    template<typename Key, typename Val>
    static AbstractGraph<Key, Val> construct_by_predicate(const std::vector<Key>& keys, const std::vector<Val>& values,
        bool (*predicate)(const Key& key1, const Val& val1, const Key& key2, const Val& val2)) {
        AbstractGraph<Key, Val> pred_graph = AbstractGraph<Key, Val>::null(keys, values);
        const int num_vertices = (int)keys.size();

        for (int i = 0; i < num_vertices; i++) {
            Vertex& cur = pred_graph.m_vertices.at(keys[i]);
            for (int j = i + 1; j < num_vertices; j++) {
                Vertex& other = pred_graph.m_vertices.at(keys[j]);
                if (predicate(cur.m_identifier, cur.m_elem, other.m_identifier, other.m_elem)) Vertex::connect(cur, other);
            }
        }

        return pred_graph;
    }

    // Time Complexity: O(N^2)
    template<typename Val>
    static AbstractGraph<int, Val> construct_by_predicate(const std::vector<Val>& values,
        bool (*predicate)(const int& key1, const Val& val1, const int& key2, const Val& val2)) {
        AbstractGraph<int, Val> pred_graph = AbstractGraph<int, Val>::null(values);
        const int num_m_vertices = (int)values.size();

        for (int i = 0; i < num_m_vertices; i++) {
            Vertex& cur = pred_graph.m_vertices.at(i);
            for (int j = i + 1; j < num_m_vertices; j++) {
                Vertex& other = pred_graph.m_vertices.at(j);
                if (predicate(cur.m_identifier, cur.m_elem, other.m_identifier, other.m_elem)) Vertex::connect(cur, other);
            }
        }

        return pred_graph;
    }

    // Time Complexity: O(N^2)
    template<typename Val>
    static AbstractGraph<int, Val> direct_by_predicate(const std::vector<Val>& values, std::function<bool(const Element& e1, const Element& e2)> predicate) {
        AbstractGraph<int, Val> pred_graph = AbstractGraph<int, Val>::null(values);
        const int num_m_vertices = (int)values.size();

        for (int i = 0; i < num_m_vertices; i++) {
            Vertex& cur = pred_graph.m_vertices.at(i);
            for (int j = 0; j < num_m_vertices; j++) {
                if (j != i) {
                    Vertex& other = pred_graph.m_vertices.at(j);
                    if (predicate(cur.m_elem, other.m_elem)) Vertex::direct(cur, other);
                }
            }
        }

        return pred_graph;
    }

    template<typename Val>
    static AbstractGraph<int, Val> construct_with_list(const std::vector<Val>& values, const std::vector<std::vector<int>>& connections) {
        AbstractGraph<int, Val> connection_graph = AbstractGraph<int, Val>::null(values);
        const int edge_list_size = (int) connections.size();

        for (int i = 0; i < edge_list_size; i++) {
            Vertex& cur = connection_graph.m_vertices.at(i);
            for (int edge_target : connections[i]) {
                cur.m_edges.insert(edge_target);
            }
        }

        return connection_graph;
    }

    static AbstractGraph<int, int> construct_with_list(const std::vector<int>& values, const std::unordered_map<int, std::vector<int>>& connections) {
        AbstractGraph<int, int> connection_graph = AbstractGraph<int, int>::null(values, values);
        const int edge_list_size = (int)connections.size();

        for (int i : values) {
            AbstractGraph<int,int>::Vertex& cur = connection_graph.m_vertices.at(i);
            
            std::unordered_map<int, std::vector<int>>::const_iterator conn = connections.find(i);
            if (conn != connections.end()) {
                for (int edge_target : conn->second) {
                    cur.m_edges.insert(edge_target);
                }
            }
        }

        return connection_graph;
    }



    struct GraphUpdateResults {
        AbstractGraph* updated_graph;
        int connects;
        int disconnects;
    };

    // Time Complexity: O(N)
    const GraphUpdateResults update_by_predicate(std::function<bool (const Element& e1, const Element& e2)> pred) {
        typedef std::unordered_map<Id, Vertex>::iterator VertexIter;

        int new_disconnects = 0;
        int new_connects = 0;

        for (VertexIter outer_it = this->m_vertices.begin(); outer_it != this->m_vertices.end(); outer_it++) {
            Vertex& outer_vertex = outer_it->second;

            for (VertexIter inner_it = std::next(outer_it); inner_it != this->m_vertices.end(); inner_it++) {
                Vertex& inner_vertex = inner_it->second;
                bool outcome = pred(inner_vertex.m_elem, outer_vertex.m_elem);

                if (!outcome && outer_vertex.m_edges.count(inner_vertex.m_identifier) != 0) {
                    Vertex::disconnect(outer_vertex, inner_vertex);
                    new_disconnects++;
                } else if (outcome) {
                    Vertex::connect(outer_vertex, inner_vertex);
                    new_connects++;
                }
            }
        }

        return { this, new_connects, new_disconnects };
    }

    // Time Complexity: O(N)
    AbstractGraph<Id, Element>& update_by_predicate(std::function<bool (const Id& i1, const Element& e1, const Id& i2, const Element& e2)> pred) {
        typedef std::unordered_map<Id, Vertex>::iterator VertexIter;

        //int new_disconnects = 0;
        //int new_connects = 0;

        for (VertexIter outer_it = this->m_vertices.begin(); outer_it != this->m_vertices.end(); outer_it++) {
            Vertex& outer_vertex = outer_it->second;

            for (VertexIter inner_it = std::next(outer_it); inner_it != this->m_vertices.end(); inner_it++) {
                Vertex& inner_vertex = inner_it->second;
                bool outcome = pred(inner_vertex.m_identifier, inner_vertex.m_elem, outer_vertex.m_identifier, outer_vertex.m_elem);

                if (!outcome && outer_vertex.m_edges.count(inner_vertex.m_identifier) != 0) {
                    Vertex::disconnect(outer_vertex, inner_vertex);
                    //new_disconnects++;
                } else if (outcome) {
                    Vertex::connect(outer_vertex, inner_vertex);
                    //new_connects++;
                }
            }
        }

        return *this;
    }

    // Time Complexity: O(N)
    const AbstractGraph<Id, Element>& map(std::function<void (const Element&)> func) const {
        for (const std::pair<Id, Vertex>& p : this->m_vertices) {
            func(p.second.m_elem);
        }

        return *this;
    }

    const AbstractGraph<Id, Element>& map_vertices(std::function<void (const Vertex&)> func) const {
        for (const std::pair<Id, Vertex>& p : this->m_vertices) {
            func(p.second);
        }

        return *this;
    }

    // Time Complexity: O(N)
    AbstractGraph<Id, Element>& map_mut(std::function<void (Element&)> func) {
        for (std::pair<const Id, Vertex>& p : this->m_vertices) {
            func(p.second.m_elem);
        }

        return *this;
    }

    // Returns a vector of all unique edges (pairs of vertex ids) in this Graph
    std::vector<std::pair<Id, Id>> retrieve_edge_list() const {
        std::unordered_map<Id, std::unordered_set<Id>> id_dict;
        std::vector<std::pair<Id, Id>> edge_list;

        for (const std::pair<const Id, const Vertex> vpair : this->m_vertices) {
            const Vertex& cur_v = vpair.second;
            id_dict[cur_v.m_identifier] = std::unordered_set<Id>();

            for (Id key : cur_v.m_edges) {
                if (id_dict.count(key) == 0) {
                    edge_list.push_back({cur_v.m_identifier, key});
                }
            }
        }

        return edge_list;
    }

    size_t count_edges() const {
        std::unordered_map<Id, std::unordered_set<Id>> id_dict;
        size_t edge_count = 0;

        for (const std::pair<const Id, const Vertex> vpair : this->m_vertices) {
            const Vertex& cur_v = vpair.second;
            id_dict[cur_v.m_identifier] = std::unordered_set<Id>();

            for (Id key : cur_v.m_edges) {
                if (id_dict.count(key) == 0) {
                    edge_count++;
                }
            }
        }

        return edge_count;
    }
    
    // struct holding a bunch of degree related Graph Statistics
    // returned by Graph::get_degree_stats()
    struct GraphStatisticsBus {
        float mean;
        float std_dev;
        int degree_max;
        int degree_min;
        int degree_sum;

        // Returns the statistics in the GSB as a (nicer) formatted string
        std::string summary_statistics() const noexcept {
            std::stringstream ss;
            ss << "mean: " << this->mean 
            << "\nstdev: " << this->std_dev
            << "\nmax degree: " << this->degree_max
            << "\nmin degree: " << this->degree_min
            << "\ndegree sum: " << this->degree_sum;
            return ss.str();
        }

        // Returns the z score of a vertex's out degree 
        float degree_z_score(float degree) const {
            return (degree - this->mean) / this->std_dev;
        }

        std::vector<Id> collect_by_degree_z_score(const AbstractGraph<Id, Element>& ag, float degree_z_score_min, float degree_z_score_max) {
            std::vector<Id> candidates;
            ag.map_vertices([this, &candidates, degree_z_score_min, degree_z_score_max](const Vertex& v) {
                const float vertex_score = this->degree_z_score(v.out_degree());
                if (degree_z_score_min < vertex_score && vertex_score < degree_z_score_max) {
                    candidates.push_back(v.m_identifier);
                }
            });

            return candidates;
        }

        // Returns a string representation of the GSB
        std::string to_string() const noexcept {
            std::stringstream ss;
            ss << "{" << this->mean << ", " << this->std_dev << ", " << this->degree_max << ", " << this->degree_min << ", " << this->degree_sum << "}";
            return ss.str();
        }
    };

    // Returns a graph that is the complement of this graph in terms of edges
    // Treats the graph as if it were an UNDIRECTED GRAPH
    // Time Complexity: O(N^2) -> (N: number of vertices)
    AbstractGraph<Id, Element> complement() const noexcept {
        AbstractGraph<Id, Element> complement_graph;

        for (const std::pair<const Id, const Vertex>& vpair : this->m_vertices) {
            Vertex& new_v = complement_graph.add_vertex(vpair.first, vpair.second.m_elem);
            const Vertex& og_v = vpair.second;
            
            for (const std::pair<const Id, const Vertex>& opair : this->m_vertices) {
                if (opair.first != vpair.first && og_v.m_edges.count(opair.first) == 0) {
                    new_v.m_edges.insert(opair.first);
                }
            }
        }

        return complement_graph;
    }

    // Returns a graph where all its edges are in the reverse direction of the initial graph
    // Treats the graph as if it were a DIRECTED GRAPH
    // Time Complexity: O(N + M) -> (N: number of vertices, M: number of edges)
    AbstractGraph<Id, Element> transpose() const noexcept {
        //std::cout << "Building reverse graph" << std::endl;
        AbstractGraph<Id, Element> reverse_graph = this->null();
        //std::cout << "made new null graph" << std::endl;

        for (const std::pair<const Id, const Vertex>& vpair : this->m_vertices) {
            const Vertex& og_v = vpair.second;
            for (const Id& key : og_v.m_edges) {
                Vertex& target = reverse_graph.m_vertices.at(key);
                target.m_edges.insert(vpair.first); // same as: Vertex::direct(target, reverse_graph.get(vpair.first))
            }
        }

        //std::cout << "returning reverse graph" << std::endl;
        return reverse_graph;
    }

    // Return various degree related statistics about this Graph
    GraphStatisticsBus get_degree_stats() const noexcept {
        GraphStatisticsBus all_stats = {0.0f, 0.0f, 0, std::numeric_limits<int>::max(), 0};
        size_t N = this->m_vertices.size();
        
        for (const std::pair<const Id, const Vertex>& vpair : this->m_vertices) {
            const int out_degree = vpair.second.out_degree();
            all_stats.mean += out_degree;
            all_stats.degree_sum = (int) all_stats.mean;
            all_stats.std_dev += std::powf( (float) out_degree, 2);
            all_stats.degree_max = std::max(all_stats.degree_max, out_degree);
            all_stats.degree_min = std::min(all_stats.degree_min, out_degree);
        }

        all_stats.mean /= N;
        all_stats.std_dev = (float) ((all_stats.std_dev + N * pow(all_stats.mean, 2) - 2 * all_stats.mean * all_stats.degree_sum) / (N - 1));
        return all_stats;
    }


    // Returns a string representation of the graph
    // Time Complexity: O(N)
    std::string to_string() const {
        std::stringstream ss;
        ss << "# of m_Vertices: " << std::to_string(this->m_vertices.size()) << "\n";
        for (const std::pair<const Id, Vertex>& p : this->m_vertices) {
            ss << p.second.to_string() << "\n";
        }
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const AbstractGraph<Id, Element>& g) {
        os << "AbstractGraph(V: " << g.size() << ", E: " << g.count_edges() << ")";
        return os;
    }

    std::unordered_map<Id, bool> construct_visit_list() const {
        std::unordered_map<Id, bool> visited;
        size_t connected_count = 0;

        for (const std::pair<Id, Vertex>& vpair : this->m_vertices) {
            visited[vpair.first] = false;
        }

        return visited;
    }

    Id* find_unvisited(std::unordered_map<Id, bool>& uv) const {
        typedef std::unordered_map<Id, bool>::iterator IdIter;
        IdIter idx = uv.begin();

        while (idx != uv.end() && idx->second) {
            ++idx;
        }

        return idx != uv.end() ? (Id*) &(idx->first) : nullptr;
    }

    std::size_t count_connected_components() const {
        std::unordered_map<Id, bool> visited = this->construct_visit_list();
        size_t connected_count = 0;
        const Id* start_vertex = this->m_vertices.begin() != this->m_vertices.end() ? &(this->m_vertices.begin()->first) : nullptr;

        while (start_vertex != nullptr) {
            std::queue<const Id*> frontiers;
            frontiers.push(start_vertex);

            while (frontiers.size() > 0) {
                const Id* cur = frontiers.front();
                visited[*cur] = true;
                frontiers.pop();

                for (const Id& neighbor : this->m_vertices.at(*cur).m_edges) {
                    if (!visited[neighbor]) {
                        frontiers.push(&neighbor);
                    }
                }
            }

            connected_count += 1;
            start_vertex = this->find_unvisited(visited);
        }

        return connected_count;
    }

    // splits a graph into several connected components
    AbstractGraph<int, AbstractGraph<Id,Element>> split_graph() {
        std::unordered_map<Id, bool> visited = this->construct_visit_list();
        AbstractGraph<int, AbstractGraph<Id, Element>> connected_components;
        const Id* start_vertex = this->m_vertices.begin() != this->m_vertices.end() ? &(this->m_vertices.begin()->first) : nullptr;
        int component_idx = 0;

        while (start_vertex != nullptr) {
            std::queue<const Id*> frontiers;
            AbstractGraph<Id, Element> connected_component;
            frontiers.push(start_vertex);

            while (frontiers.size() > 0) {
                const Id* cur = frontiers.front();
                visited[*cur] = true;
                frontiers.pop();

                connected_component.m_vertices[*cur] = this->m_vertices.at(*cur);
                for (const Id& neighbor : this->m_vertices.at(*cur).m_edges) {
                    if (!visited[neighbor]) {
                        frontiers.push(&neighbor);
                    }
                }
            }

            connected_components.add_vertex(component_idx, std::move(connected_component));
            start_vertex = this->find_unvisited(visited);
            component_idx++;
        }

        return connected_components;
    }

    // Returns a post ordering of the vertices in the graph
    std::vector<Id> post_order(Id start_vertex) const {
        std::unordered_set<Id> visited;
        std::vector<Id> postordering;
        int iter = 1;

        while (visited.size() < this->size()) {
            std::stack<Id> id_stack;
            id_stack.push(start_vertex);

            while (!id_stack.empty()) {
                Id top = id_stack.top();
                if (visited.count(top) == 0) {
                    visited.insert(top);
                    for (const Id& neighbor : this->m_vertices.at(top).m_edges) {
                        if (visited.count(neighbor) == 0) {
                            id_stack.push(neighbor);
                        }
                    }
                } else {
                    postordering.push_back(top);
                    id_stack.pop();
                }
            }

            for (auto i = this->m_vertices.begin(); i != this->m_vertices.end(); i++) {
                if (visited.count(i->first) == 0) {
                    start_vertex = i->first;
                }
            }
        }

        return postordering;
    }

    // Returns the vertices in the input graph that are reachable from start_vertex
    std::unordered_set<Id> dfs(Id start_vertex) const {
        std::unordered_set<Id> visited;
        std::stack<Id> id_stack;
        id_stack.push(start_vertex);

        while (!id_stack.empty()) {
            Id top = id_stack.top();
            id_stack.pop();

            if (visited.count(top) == 0) {
                visited.insert(top);
                for (const Id& neighbor : this->m_vertices.at(top).m_edges) {
                    id_stack.push(neighbor);
                }
            }
        }

        return visited;
    }
    
    static void print_path(const std::vector<int>& path) {
        std::cout << path[0];
        for (int i = 1; i < path.size(); i++) {
            std::cout << " -> " << path[i];
        }
        std::cout << std::endl;
    }

    // Returns the Strongly Connected Components of the input graph
    AbstractGraph<int, AbstractGraph<Id, Element>> find_sccs() {
        std::vector<Id> postorder = this->transpose().post_order(this->m_vertices.begin()->first);

        typedef std::pair<Id, int> scc_mapping;

        AbstractGraph<int, AbstractGraph<Id, Element>> scc_graph;
        int scc_count = 0;

        while (postorder.size() > 0) {
            Id& sink = postorder.back();

            std::unordered_set<Id> visited = this->dfs(sink/*.first*/);
            std::vector<Id> connections;
            AbstractGraph<Id, Element> scc;

            std::vector<Id> new_postorder;

            // TODO
            // NUMBER ONE:
            // Convert "postorder" into an array of <Id, int> pairs where the second value in the pair
            // indicates the SCC the vertex is apart of
            // NUMBER TWO:
            // Implement the non-contiguous slice so we can decrease the size of our search space each iteration
            for (const Id& p : postorder) {
                if (visited.count(p) == 0) {
                    new_postorder.push_back(p);
                } else {
                    scc.m_vertices.insert({ p, this->m_vertices.at(p) });
                    for (const Id& outgoing : this->m_vertices.at(p).m_edges) {
                        for (auto& prev_sccs : scc_graph.m_vertices) {
                            if (prev_sccs.second.m_elem.m_vertices.count(outgoing) != 0) {
                                connections.push_back(prev_sccs.first);
                            }
                        }
                    }
                }
            }

            postorder = std::move(new_postorder);
            auto& newbie = scc_graph.add_vertex(scc_count, std::move(scc));
            for (const Id& conn : connections) {
                newbie.m_edges.insert(conn);
            }
            scc_count += 1;
        }

        return scc_graph;
    }

    struct scc_pair {
        Id id;
        int component_index;
    };

    // Mark all vertices in the graph with an index indicating the strongly connected component that vertex is apart of
    std::vector<scc_pair> color_scc_vertices(size_t start_vertex) {
        std::vector<scc_pair> postorder = this->transpose().post_order(start_vertex)
            | std::views::transform([](Id id) { return scc_pair(id, -1); })
            | std::ranges::to<std::vector<scc_pair>>();

        NCSlice<scc_pair> postorder_view(postorder);
        AbstractGraph<int, AbstractGraph<Id, Element>> scc_graph;
        int scc_count = 0;

        while (postorder_view.size() > 0) {
            scc_pair& sink = postorder_view.back();
            std::unordered_set<Id> visited = this->dfs(sink.id);

            for (auto it = postorder_view.begin(); it != postorder_view.end(); ) {
                scc_pair& p = (*it).second;
                if (visited.count(p.id) != 0) {
                    p.component_index = scc_count;
                    it = postorder_view.remove(std::move(it));
                } else {
                    it++;
                }
            }
            scc_count += 1;
        }

        return postorder;
    }
    
    template <typename Key, typename Val>
    friend class AbstractGraph;

    class AbstractGraphIterator {
    public:
        using Category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using Pointer = typename std::unordered_map<Id, Vertex>::iterator;
        using value_type = Element;
        using Reference = Element&;
    private:
        Pointer m_ptr;
    public:
        AbstractGraphIterator() : m_ptr() {}
        AbstractGraphIterator(Pointer iter) : m_ptr(iter) {}

        Reference operator*() const {
            return this->m_ptr->second.m_elem;
        }

        value_type* operator->() const {
            return &(this->m_ptr->second.m_elem);
        }

        // Pointer operator->() {
        //     return this->m_ptr;
        // }

        AbstractGraphIterator& operator++() {
            ++(this->m_ptr);
            return *this;
        }

        AbstractGraphIterator operator++(int) {
            AbstractGraphIterator temp = *this;
            ++(*this);
            return temp;
        }

        friend bool operator==(const AbstractGraphIterator& agit1, const AbstractGraphIterator& agit2) {
            return agit1.m_ptr == agit2.m_ptr;
        }

        friend bool operator!=(const AbstractGraphIterator& agit1, const AbstractGraphIterator& agit2) {
            return agit1.m_ptr != agit2.m_ptr;
        }
    };

    typedef AbstractGraphIterator iterator;

    iterator begin() {
        return AbstractGraph<Id, Element>::iterator(this->m_vertices.begin());
    }

    iterator end() {
        return AbstractGraph<Id, Element>::iterator(this->m_vertices.end());
    }
};

static_assert(std::forward_iterator<AbstractGraph<int, int>::iterator>);
static_assert(std::ranges::forward_range<AbstractGraph<int,int>>);

// SELF NOTE # 1:
// HERE I COUPLE TOGETHER VERTICES & EDGES IN THAT EACH VERTEX CONTAINS THE IDs OF THE VERTICES THAT IT IS CONNECTED TO.
// THIS MADE INTIIALLY CREATING THIS DATA STRUCTURE PRETTY SIMPLE, SINCE HAVING A VERTEX ALSO MEANT HAVING ALL ITS EDGES
// HOWEVER, NOW I'M SEEING MERIT TO DECOUPLING VERTEX AND GRAPH DATA STRUCTURES, ESPECIALLY IF YOU CONSIDER VERTICES
// AS INDEPENDENT OBJECTS IN THE SET AND EDGES AS SOME ARBITARY NOTION OF CONNECTIVITY (OR TOPOLOGY) APPLIED OVER THEM.

// DECOUPLING VERTICES AND EDGES MEANS NEW "VIEWS" OVER THE SAME GRAPH CAN BE MADE BY JUST MAINTAINING A POINTER OR
// REFERENCE TO THE ORIGINAL VERTEX SET, AND THEN DEFINING A NEW EDGE SET (TOPOLOGY) OVER THEM. THIS MEANS OPERATIONS
// LIKE TAKING THE COMPLEMENT OF A GRAPH OR REVERSE ALL EDGES OF THE GRAPH TAKE MUCH LESS SPACE.