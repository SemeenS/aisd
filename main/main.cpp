#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <limits>
#include <queue>
#include <stack>
#include <functional>
#include <string>

using namespace std;

template<typename Vertex, typename Distance = double>
class Graph {
public:
    struct Edge {
        Vertex from;
        Vertex to;
        Distance distance;

        Edge() : from(), to(), distance() {}
        Edge(const Vertex& f, const Vertex& t, const Distance& d)
            : from(f), to(t), distance(d) {
        }

        bool operator==(const Edge& other) const {
            return from == other.from && to == other.to && distance == other.distance;
        }
    };

private:
    vector<Vertex> _vertices;
    multimap<Vertex, Edge> _edges;

public:
    bool has_vertex(const Vertex& v) const {
        return find(_vertices.begin(), _vertices.end(), v) != _vertices.end();
    }

    bool add_vertex(const Vertex& v) {
        if (has_vertex(v)) {
            return false;
        }
        _vertices.push_back(v);
        return true;
    }

    bool remove_vertex(const Vertex& v) {
        auto it = find(_vertices.begin(), _vertices.end(), v);
        if (it == _vertices.end()) {
            return false;
        }
        _vertices.erase(it);

        auto range = _edges.equal_range(v);
        _edges.erase(range.first, range.second);

        for (auto it = _edges.begin(); it != _edges.end(); ) {
            if (it->second.to == v) {
                it = _edges.erase(it);
            }
            else {
                ++it;
            }
        }
        return true;
    }

    vector<Vertex> vertices() const {
        return _vertices;
    }

    void add_edge(const Vertex& from, const Vertex& to, const Distance& d) {
        if (d < 0) {
            throw invalid_argument("Edge distance cannot be negative");
        }
        if (!has_vertex(from) || !has_vertex(to)) {
            throw invalid_argument("One or both vertices don't exist");
        }
        _edges.insert(make_pair(from, Edge(from, to, d)));
    }

    void add_undirected_edge(const Vertex& v1, const Vertex& v2, const Distance& d) {
        if (d < 0) {
            throw invalid_argument("Edge distance cannot be negative");
        }
        if (!has_vertex(v1) || !has_vertex(v2)) {
            throw invalid_argument("One or both vertices don't exist");
        }
        _edges.insert(make_pair(v1, Edge(v1, v2, d)));
        _edges.insert(make_pair(v2, Edge(v2, v1, d)));
    }

    bool remove_edge(const Vertex& from, const Vertex& to) {
        bool removed = false;
        for (auto it = _edges.begin(); it != _edges.end(); ) {
            if (it->first == from && it->second.to == to) {
                it = _edges.erase(it);
                removed = true;
            }
            else {
                ++it;
            }
        }
        return removed;
    }

    bool remove_edge(const Edge& e) {
        auto range = _edges.equal_range(e.from);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == e) {
                _edges.erase(it);
                return true;
            }
        }
        return false;
    }

    bool has_edge(const Vertex& from, const Vertex& to) const {
        auto range = _edges.equal_range(from);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second.to == to) {
                return true;
            }
        }
        return false;
    }

    bool has_edge(const Edge& e) const {
        auto range = _edges.equal_range(e.from);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == e) {
                return true;
            }
        }
        return false;
    }

    vector<Edge> edges(const Vertex& vertex) {
        vector<Edge> result;
        auto range = _edges.equal_range(vertex);
        for (auto it = range.first; it != range.second; ++it) {
            result.push_back(it->second);
        }
        return result;
    }

    size_t order() const {
        return _vertices.size();
    }

    size_t degree(const Vertex& v) const {
        return _edges.count(v);
    }

    bool is_connected() const {
        if (_vertices.empty()) return true;

        set<Vertex> visited;
        stack<Vertex> stack;
        stack.push(_vertices[0]);
        visited.insert(_vertices[0]);

        while (!stack.empty()) {
            Vertex current = stack.top();
            stack.pop();

            auto range = _edges.equal_range(current);
            for (auto it = range.first; it != range.second; ++it) {
                if (visited.find(it->second.to) == visited.end()) {
                    visited.insert(it->second.to);
                    stack.push(it->second.to);
                }
            }
        }

        if (visited.size() != _vertices.size()) {
            return false;
        }

        Graph<Vertex, Distance> transposed;
        for (size_t i = 0; i < _vertices.size(); ++i) {
            transposed.add_vertex(_vertices[i]);
        }
        for (auto it = _edges.begin(); it != _edges.end(); ++it) {
            const Edge& edge = it->second;
            transposed.add_edge(edge.to, edge.from, edge.distance);
        }

        visited.clear();
        stack.push(_vertices[0]);
        visited.insert(_vertices[0]);

        while (!stack.empty()) {
            Vertex current = stack.top();
            stack.pop();

            auto range = transposed._edges.equal_range(current);
            for (auto it = range.first; it != range.second; ++it) {
                if (visited.find(it->second.to) == visited.end()) {
                    visited.insert(it->second.to);
                    stack.push(it->second.to);
                }
            }
        }

        return visited.size() == _vertices.size();
    }

    vector<Edge> shortest_path(const Vertex& from, const Vertex& to) const {
        priority_queue<pair<Distance, Vertex>,
            vector<pair<Distance, Vertex>>,
            greater<pair<Distance, Vertex>>> pq;

        map<Vertex, Distance> distances;
        map<Vertex, Edge> predecessors;

        for (const auto& v : _vertices) {
            distances[v] = numeric_limits<Distance>::max();
        }
        distances[from] = Distance();
        pq.push(make_pair(distances[from], from));

        while (!pq.empty()) {
            Vertex u = pq.top().second;
            Distance dist_u = pq.top().first;
            pq.pop();

            if (dist_u > distances[u]) {
                continue;
            }

            auto range = _edges.equal_range(u);
            for (auto it = range.first; it != range.second; ++it) {
                const Edge& edge = it->second;
                Vertex v = edge.to;
                Distance weight = edge.distance;

                if (distances[v] > distances[u] + weight) {
                    distances[v] = distances[u] + weight;
                    predecessors[v] = edge;
                    pq.push(make_pair(distances[v], v));
                }
            }
        }

        if (distances[to] == numeric_limits<Distance>::max()) {
            return {};
        }

        vector<Edge> path;
        Vertex current = to;
        while (current != from) {
            auto it = predecessors.find(current);
            if (it == predecessors.end()) {
                return {};
            }
            path.push_back(it->second);
            current = it->second.from;
        }

        reverse(path.begin(), path.end());
        return path;
    }

    vector<Vertex> walk(const Vertex& start_vertex) const {
        vector<Vertex> result;
        if (!has_vertex(start_vertex)) {
            return result;
        }

        set<Vertex> visited;
        stack<Vertex> stack;
        stack.push(start_vertex);
        visited.insert(start_vertex);

        while (!stack.empty()) {
            Vertex current = stack.top();
            stack.pop();
            result.push_back(current);

            auto range = _edges.equal_range(current);
            for (auto it = range.first; it != range.second; ++it) {
                if (visited.find(it->second.to) == visited.end()) {
                    visited.insert(it->second.to);
                    stack.push(it->second.to);
                }
            }
        }

        return result;
    }

    double average_edge_length(const Vertex& v) const {
        auto range = _edges.equal_range(v);
        size_t count = 0;
        Distance sum = 0;

        for (auto it = range.first; it != range.second; ++it) {
            sum += it->second.distance;
            count++;
        }

        for (auto it = _edges.begin(); it != _edges.end(); ++it) {
            if (it->second.to == v) {
                sum += it->second.distance;
                count++;
            }
        }

        if (count == 0) return 0.0;
        return static_cast<double>(sum) / count;
    }
};

template<typename Vertex, typename Distance = double>
Vertex find_most_remote_clinic(const Graph<Vertex, Distance>& graph) {
    if (graph.vertices().empty()) {
        throw runtime_error("Graph is empty");
    }

    Vertex result = graph.vertices()[0];
    double max_avg = graph.average_edge_length(result);

    for (const auto& vertex : graph.vertices()) {
        double current_avg = graph.average_edge_length(vertex);
        if (current_avg > max_avg) {
            max_avg = current_avg;
            result = vertex;
        }
    }

    return result;
}

void analyze_togliatti_clinics() {
    Graph<string> tlt_graph;

    try {
        tlt_graph.add_vertex("Травмпункт ГБ №2 — ул. Мира, 125");
        tlt_graph.add_vertex("Травмпункт ГБ №5 — ул. Юбилейная, 31");
        tlt_graph.add_vertex("Травмпункт Поликлиники №3 — ул. Матросова, 17");
        tlt_graph.add_vertex("МЦ XXI век — ул. Революционная, 10");
        tlt_graph.add_vertex("Клиника Эксперт — ул. 40 лет Победы, 48");

        tlt_graph.add_undirected_edge("Травмпункт ГБ №2 — ул. Мира, 125",
            "Травмпункт ГБ №5 — ул. Юбилейная, 31", 4.2);
        tlt_graph.add_undirected_edge("Травмпункт ГБ №2 — ул. Мира, 125",
            "Травмпункт Поликлиники №3 — ул. Матросова, 17", 3.8);
        tlt_graph.add_undirected_edge("Травмпункт ГБ №5 — ул. Юбилейная, 31",
            "Клиника Эксперт — ул. 40 лет Победы, 48", 2.5);
        tlt_graph.add_undirected_edge("Травмпункт Поликлиники №3 — ул. Матросова, 17",
            "МЦ XXI век — ул. Революционная, 10", 1.7);
        tlt_graph.add_undirected_edge("МЦ XXI век — ул. Революционная, 10",
            "Клиника Эксперт — ул. 40 лет Победы, 48", 3.1);

        if (!tlt_graph.is_connected()) {
            cout << "Ошибка: граф травмпунктов несвязный! Некоторые пункты недостижимы.\n";
            return;
        }

        string remoteClinic = find_most_remote_clinic(tlt_graph);
        double avgDist = tlt_graph.average_edge_length(remoteClinic);

        cout << "Анализ травмпунктов Тольятти:\n";
        cout << "Самый удаленный травмпункт: " << remoteClinic << endl;
        cout << "Среднее расстояние до соседей: " << avgDist << " км\n\n";
        cout << "Средние расстояния для всех травмпунктов:\n";
        for (const auto& clinic : tlt_graph.vertices()) {
            cout << clinic << ": " << tlt_graph.average_edge_length(clinic) << " км\n";
        }

    }
    catch (const invalid_argument& e) {
        cerr << "Ошибка в данных: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        cerr << "Ошибка выполнения: " << e.what() << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    analyze_togliatti_clinics();
    return 0;
}