#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>

using namespace std;
const double INF = numeric_limits<double>::infinity();

struct Station {
    int id;
    string name;
    string location;
    int totalChargers;
    int availableChargers;
    double chargingRate; 
};
struct Edge { int to; double distance; };
struct Candidate { int stationIndex; double distance; };
struct CompareCandidate {
    bool operator()(const Candidate& a, const Candidate& b) const {
        if (a.distance == b.distance) return a.stationIndex > b.stationIndex;
        return a.distance > b.distance;
    }
};
class EVChargingSystem {
private:
    vector<Station> stations;
    vector<vector<Edge>> graph;
    unordered_map<int, int> stationIndexById; // Hash table: station ID -> index

    int findIndexById(int id) const {
        auto it = stationIndexById.find(id);
        return it == stationIndexById.end() ? -1 : it->second;
    }
    void addSampleStation(const Station& station) {
        int index = static_cast<int>(stations.size());
        stations.push_back(station);
        graph.push_back({});
        stationIndexById[station.id] = index;
    }
    void addRoadById(int id1, int id2, double distance) {
        int a = findIndexById(id1), b = findIndexById(id2);
        if (a < 0 || b < 0 || distance <= 0) return;
        graph[a].push_back({b, distance});
        graph[b].push_back({a, distance});
    }
    vector<double> dijkstra(int source) const {
        vector<double> dist(stations.size(), INF);
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
        dist[source] = 0;
        pq.push({0, source});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            for (const Edge& e : graph[u]) {
                double nd = d + e.distance;
                if (nd < dist[e.to]) {
                    dist[e.to] = nd;
                    pq.push({nd, e.to});
                }
            }
        }
        return dist;
    }
public:
    EVChargingSystem() {
        // Fictional sample records for demonstration only.
        addSampleStation({101, "GreenCharge", "Shivajinagar", 4, 3, 60.0});
        addSampleStation({102, "VoltPoint", "Deccan", 4, 0, 50.0});
        addSampleStation({103, "EcoPlug", "Kothrud", 5, 4, 120.0});
        addSampleStation({104, "PowerDrive", "Swargate", 3, 2, 60.0});
        addSampleStation({105, "QuickCharge", "Baner", 2, 1, 30.0});
        addSampleStation({106, "CityCharge", "Aundh", 4, 2, 90.0});
        addRoadById(101, 102, 1.5);
        addRoadById(102, 103, 4.0);
        addRoadById(101, 104, 3.2);
        addRoadById(104, 103, 2.4);
        addRoadById(103, 106, 3.0);
        addRoadById(106, 105, 2.8);
        addRoadById(102, 104, 2.2);
        addRoadById(101, 106, 6.5);
    }

    void displayAllStations() const {
        cout << "\n---------------- EV CHARGING STATIONS ----------------\n";
        cout << left << setw(8) << "ID" << setw(18) << "Station" << setw(16) << "Location"
             << setw(12) << "Free/Total" << setw(12) << "Power(kW)" << '\n';
        cout << string(66, '-') << '\n';
        for (const Station& s : stations) {
            cout << left << setw(8) << s.id << setw(18) << s.name << setw(16) << s.location
                 << setw(5) << s.availableChargers << "/" << setw(7) << s.totalChargers
                 << setw(12) << fixed << setprecision(1) << s.chargingRate << '\n';
        }
    }

    void searchStation() const {
        int id; cout << "Enter station ID: "; cin >> id;
        int i = findIndexById(id);
        if (i < 0) { cout << "Station not found.\n"; return; }
        const Station& s = stations[i];
        cout << "\nStation found\nID: " << s.id << "\nName: " << s.name
             << "\nLocation: " << s.location << "\nAvailable chargers: "
             << s.availableChargers << "/" << s.totalChargers << "\nCharging power: "
             << s.chargingRate << " kW\n";
    }

    void showAvailableStations() const {
        cout << "\nAvailable charging stations:\n";
        bool found = false;
        for (const Station& s : stations) if (s.availableChargers > 0) {
            found = true;
            cout << "ID " << s.id << " | " << s.name << " | " << s.location
                 << " | Free chargers: " << s.availableChargers << '\n';
        }
        if (!found) cout << "No stations currently have free chargers.\n";
    }

    void recommendStations() const {
        int sourceId;
        cout << "Enter the ID of your starting location/station: "; cin >> sourceId;
        int source = findIndexById(sourceId);
        if (source < 0) { cout << "Unknown station ID. Use option 1 to see IDs.\n"; return; }
        vector<double> distances = dijkstra(source);
        priority_queue<Candidate, vector<Candidate>, CompareCandidate> pq;
        for (int i = 0; i < static_cast<int>(stations.size()); ++i)
            if (i != source && stations[i].availableChargers > 0 && distances[i] != INF)
                pq.push({i, distances[i]});
        cout << "\nRecommended stations (nearest reachable with free chargers):\n";
        if (pq.empty()) { cout << "No reachable station with a free charger was found.\n"; return; }
        int rank = 1;
        while (!pq.empty()) {
            Candidate c = pq.top(); pq.pop();
            const Station& s = stations[c.stationIndex];
            cout << rank++ << ". " << s.name << " (" << s.location << ")\n"
                 << "   Station ID: " << s.id << "\n   Route distance: " << fixed << setprecision(1)
                 << c.distance << " km\n   Free chargers: " << s.availableChargers
                 << "\n   Charging power: " << s.chargingRate << " kW\n";
        }
    }

    void findShortestRoute() const {
        int sourceId, destinationId;
        cout << "Enter starting station/location ID: "; cin >> sourceId;
        cout << "Enter destination station/location ID: "; cin >> destinationId;
        int source = findIndexById(sourceId), destination = findIndexById(destinationId);
        if (source < 0 || destination < 0) { cout << "Invalid station ID. Use option 1 to see IDs.\n"; return; }
        vector<double> dist(stations.size(), INF);
        vector<int> parent(stations.size(), -1);
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
        dist[source] = 0; pq.push({0, source});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            if (u == destination) break;
            for (const Edge& e : graph[u]) {
                double nd = d + e.distance;
                if (nd < dist[e.to]) { dist[e.to] = nd; parent[e.to] = u; pq.push({nd, e.to}); }
            }
        }
        if (dist[destination] == INF) { cout << "No route exists between those locations.\n"; return; }
        vector<int> path;
        for (int at = destination; at != -1; at = parent[at]) path.push_back(at);
        reverse(path.begin(), path.end());
        cout << "\nShortest route (based on sample road distances):\n";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << stations[path[i]].location;
            if (i + 1 < path.size()) cout << " -> ";
        }
        cout << "\nTotal distance: " << fixed << setprecision(1) << dist[destination] << " km\n";
    }

    void addNewStation() {
        Station s{};
        cout << "\n--- ADD NEW CHARGING STATION ---\n";
        cout << "Enter unique station ID: ";
        if (!(cin >> s.id)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Invalid ID.\n"; return; }
        if (findIndexById(s.id) >= 0) { cout << "This station ID already exists. Use a unique ID.\n"; return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter station name: "; getline(cin, s.name);
        cout << "Enter location: "; getline(cin, s.location);
        cout << "Enter total number of chargers: ";
        if (!(cin >> s.totalChargers) || s.totalChargers <= 0) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Total chargers must be greater than zero.\n"; return; }
        cout << "Enter available chargers (0 to " << s.totalChargers << "): ";
        if (!(cin >> s.availableChargers) || s.availableChargers < 0 || s.availableChargers > s.totalChargers) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Invalid available charger count.\n"; return; }
        cout << "Enter charging power in kW: ";
        if (!(cin >> s.chargingRate) || s.chargingRate <= 0) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Charging power must be greater than zero.\n"; return; }
        int newIndex = static_cast<int>(stations.size());
        stations.push_back(s); graph.push_back({}); stationIndexById[s.id] = newIndex;
        cout << "\nStation added successfully.\n";
        int links;
        cout << "How many existing stations should this station connect to? ";
        if (!(cin >> links) || links < 0) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Station was added, but the number of road links was invalid.\n"; return; }
        for (int j = 0; j < links; ++j) {
            int otherId; double distance;
            cout << "Connection " << (j + 1) << " - enter existing station ID: ";
            if (!(cin >> otherId)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Invalid ID; connection skipped.\n"; continue; }
            int otherIndex = findIndexById(otherId);
            if (otherIndex < 0 || otherIndex == newIndex) { cout << "Unknown or invalid station ID; connection skipped.\n"; continue; }
            cout << "Enter road distance in km: ";
            if (!(cin >> distance) || distance <= 0) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Distance must be positive; connection skipped.\n"; continue; }
            graph[newIndex].push_back({otherIndex, distance}); graph[otherIndex].push_back({newIndex, distance});
            cout << "Road connection added.\n";
        }
        cout << "New station is now available in station search and route calculations.\n";
        cout << "Note: changes are stored in memory and reset when the program closes.\n";
    }

    void updateAvailability() {
        int id, change;
        cout << "Enter station ID: "; cin >> id;
        int i = findIndexById(id);
        if (i < 0) { cout << "Station not found.\n"; return; }
        cout << "Enter change in available chargers (+1 frees one, -1 occupies one): "; cin >> change;
        int updated = stations[i].availableChargers + change;
        if (updated < 0 || updated > stations[i].totalChargers) {
            cout << "Invalid update. Available chargers must be between 0 and " << stations[i].totalChargers << ".\n";
            return;
        }
        stations[i].availableChargers = updated;
        cout << "Availability updated. Free chargers: " << updated << "/" << stations[i].totalChargers << '\n';
    }

    void run() {
        int choice = 0;
        do {
            cout << "\n============================================\n"
                 << " EV CHARGING STATION LOCATOR & AVAILABILITY\n"
                 << "============================================\n"
                 << "1. Display all stations\n"
                 << "2. Search station by ID (Hash Table)\n"
                 << "3. Show available stations\n"
                 << "4. Recommend nearest available stations\n"
                 << "5. Find shortest route (Graph + Dijkstra)\n"
                 << "6. Update charger availability\n"
                 << "7. Add new charging station\n"
                 << "8. Exit\nEnter choice: ";
            if (!(cin >> choice)) {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number from 1 to 8.\n"; continue;
            }
            switch (choice) {
                case 1: displayAllStations(); break;
                case 2: searchStation(); break;
                case 3: showAvailableStations(); break;
                case 4: recommendStations(); break;
                case 5: findShortestRoute(); break;
                case 6: updateAvailability(); break;
                case 7: addNewStation(); break;
                case 8: cout << "Thank you for using the EV Charging Station Locator.\n"; break;
                default: cout << "Invalid choice. Please select 1 to 8.\n";
            }
        } while (choice != 8);
    }
};

int main() {
    EVChargingSystem system;
    system.run();
    return 0;
}
