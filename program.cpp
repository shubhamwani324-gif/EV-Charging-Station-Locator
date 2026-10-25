#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <limits>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Station {
    int id;
    string name, location;
    int total, available;
    double power;
};

struct Road {
    int to;
    double distance;
};

vector<Station> stations;
vector<vector<Road>> graph;
unordered_map<int, int> stationMap;

int findStation(int id) {
    if (stationMap.find(id) == stationMap.end())
        return -1;
    return stationMap[id];
}

void addRoad(int a, int b, double distance) {
    graph[a].push_back({b, distance});
    graph[b].push_back({a, distance});
}

void displayStations() {
    cout << "\nID   Station       Location       Free/Total   Power\n";

    for (const auto &s : stations) {
        cout << s.id << "   "
             << s.name << "   "
             << s.location << "   "
             << s.available << "/" << s.total << "   "
             << s.power << " kW\n";
    }
}

void searchStation() {
    int id;
    cout << "Enter station ID: ";
    cin >> id;

    int i = findStation(id);

    if (i == -1) {
        cout << "Station not found.\n";
        return;
    }

    Station s = stations[i];

    cout << "Name: " << s.name << "\n";
    cout << "Location: " << s.location << "\n";
    cout << "Available chargers: " << s.available << "\n";
    cout << "Charging power: " << s.power << " kW\n";
}

void showAvailable() {
    cout << "\nAvailable charging stations:\n";

    for (const auto &s : stations) {
        if (s.available > 0) {
            cout << s.id << " - " << s.name
                 << " (" << s.available << " chargers free)\n";
        }
    }
}

vector<double> shortestDistances(int start, vector<int> &parent) {
    int n = stations.size();
    vector<double> dist(n, 1e9);
    parent.assign(n, -1);

    priority_queue<
        pair<double, int>,
        vector<pair<double, int>>,
        greater<pair<double, int>>
    > pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u])
            continue;

        for (const auto &road : graph[u]) {
            int v = road.to;
            double newDist = d + road.distance;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                parent[v] = u;
                pq.push({newDist, v});
            }
        }
    }

    return dist;
}

void recommendStations() {
    int id;
    cout << "Enter your current station ID: ";
    cin >> id;

    int start = findStation(id);

    if (start == -1) {
        cout << "Station not found.\n";
        return;
    }

    vector<int> parent;
    vector<double> dist = shortestDistances(start, parent);

    priority_queue<
        pair<double, int>,
        vector<pair<double, int>>,
        greater<pair<double, int>>
    > pq;

    for (int i = 0; i < (int)stations.size(); i++) {
        if (i != start && stations[i].available > 0
            && dist[i] < 1e9) {
            pq.push({dist[i], i});
        }
    }

    if (pq.empty()) {
        cout << "No reachable station has free chargers.\n";
        return;
    }

    cout << "\nNearest available stations:\n";

    while (!pq.empty()) {
        double distance = pq.top().first;
        int i = pq.top().second;
        pq.pop();

        cout << stations[i].name
             << " - " << distance << " km\n";
    }
}

void shortestRoute() {
    int sourceID, destinationID;

    cout << "Enter starting station ID: ";
    cin >> sourceID;
    cout << "Enter destination station ID: ";
    cin >> destinationID;

    int start = findStation(sourceID);
    int destination = findStation(destinationID);

    if (start == -1 || destination == -1) {
        cout << "Invalid station ID.\n";
        return;
    }

    vector<int> parent;
    vector<double> dist = shortestDistances(start, parent);

    if (dist[destination] >= 1e9) {
        cout << "No route available between these stations.\n";
        return;
    }

    vector<int> path;

    for (int i = destination; i != -1; i = parent[i])
        path.push_back(i);

    reverse(path.begin(), path.end());

    cout << "Shortest route: ";

    for (int i = 0; i < (int)path.size(); i++) {
        cout << stations[path[i]].name;
        if (i + 1 < (int)path.size())
            cout << " -> ";
    }

    cout << "\nTotal distance: " << dist[destination] << " km\n";
}

void updateAvailability() {
    int id, freeChargers;

    cout << "Enter station ID: ";
    cin >> id;

    int i = findStation(id);

    if (i == -1) {
        cout << "Station not found.\n";
        return;
    }

    cout << "Total chargers: " << stations[i].total << "\n";
    cout << "Enter available chargers: ";
    cin >> freeChargers;

    if (freeChargers < 0 || freeChargers > stations[i].total) {
        cout << "Invalid charger count.\n";
        return;
    }

    stations[i].available = freeChargers;
    cout << "Availability updated successfully.\n";
}

void addStation() {
    Station s;

    cout << "Enter new station ID: ";
    cin >> s.id;

    if (findStation(s.id) != -1) {
        cout << "Station ID already exists.\n";
        return;
    }

    cout << "Enter station name: ";
    cin >> ws;
    getline(cin, s.name);

    cout << "Enter location: ";
    getline(cin, s.location);

    cout << "Enter total chargers: ";
    cin >> s.total;

    cout << "Enter available chargers: ";
    cin >> s.available;

    cout << "Enter charging power in kW: ";
    cin >> s.power;

    if (s.total <= 0 || s.available < 0 ||
        s.available > s.total || s.power <= 0) {
        cout << "Invalid station details.\n";
        return;
    }

    int newIndex = stations.size();

    stations.push_back(s);
    graph.push_back({});
    stationMap[s.id] = newIndex;

    int count;
    cout << "How many existing stations to connect? ";
    cin >> count;

    if (count < 0) {
        cout << "Invalid connection count.\n";
        return;
    }

    for (int j = 0; j < count; j++) {
        int otherID;
        double distance;

        cout << "Enter connected station ID: ";
        cin >> otherID;

        int other = findStation(otherID);

        if (other == -1 || other == newIndex) {
            cout << "Invalid station ID. Skipping connection.\n";
            continue;
        }

        cout << "Enter road distance in km: ";
        cin >> distance;

        if (distance <= 0) {
            cout << "Invalid distance. Skipping connection.\n";
            continue;
        }

        addRoad(newIndex, other, distance);
    }

    cout << "New station added successfully.\n";
}

int main() {
    stations = {
        {101, "GreenCharge", "Shivajinagar", 4, 3, 60},
        {102, "VoltPoint", "Deccan", 4, 0, 50},
        {103, "EcoPlug", "Kothrud", 5, 4, 120},
        {104, "PowerDrive", "Swargate", 3, 2, 60},
        {105, "QuickCharge", "Baner", 2, 1, 30},
        {106, "CityCharge", "Aundh", 4, 2, 90}
    };

    graph.resize(stations.size());

    for (int i = 0; i < (int)stations.size(); i++)
        stationMap[stations[i].id] = i;

    addRoad(0, 1, 2.0);
    addRoad(0, 2, 5.0);
    addRoad(1, 2, 3.0);
    addRoad(1, 3, 4.0);
    addRoad(2, 4, 6.0);
    addRoad(3, 4, 2.0);
    addRoad(4, 5, 3.0);
    addRoad(2, 5, 7.0);

    int choice;

    do {
        cout << "\n====================================\n";
        cout << " EV CHARGING STATION LOCATOR\n";
        cout << "====================================\n";
        cout << "1. Display all stations\n";
        cout << "2. Search station by ID (Hashing)\n";
        cout << "3. Show available stations\n";
        cout << "4. Recommend nearest available stations\n";
        cout << "5. Find shortest route (Graph + Dijkstra)\n";
        cout << "6. Update charger availability\n";
        cout << "7. Add new charging station\n";
        cout << "8. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: displayStations(); break;
            case 2: searchStation(); break;
            case 3: showAvailable(); break;
            case 4: recommendStations(); break;
            case 5: shortestRoute(); break;
            case 6: updateAvailability(); break;
            case 7: addStation(); break;
            case 8: cout << "Thank you!\n"; break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 8);

    return 0;
}
