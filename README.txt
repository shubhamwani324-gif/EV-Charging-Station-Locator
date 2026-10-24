EV CHARGING STATION LOCATOR & AVAILABILITY SYSTEM
DSA Mini Project in C++

TEAM MEMBERS
1. Shahu Warhekar
2. Shubham Wani
3. Janhavi Vairagkar
4. Yash Patil

FEATURES
- Display sample charging stations
- Search station by ID using a hash table (unordered_map)
- List stations with free chargers
- Recommend nearest reachable stations with available chargers
- Find shortest routes using Dijkstra's algorithm
- Update simulated charger availability

DATA STRUCTURES
- Graph represented by an adjacency list
- Hash table: unordered_map<int, int>
- Min-priority queue: std::priority_queue with a comparator
- Vector-backed station records and adjacency list

COMPILE
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o ev_locator

RUN (Linux/macOS)
./ev_locator

RUN (Windows PowerShell / MinGW)
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o ev_locator.exe
.\ev_locator.exe

NOTES
- Station names, charger counts, and road distances are fictional sample data.
- Availability updates remain in memory and reset when the program restarts.
- The starting location is selected by station ID; this is not GPS navigation.
- Recommendations use shortest sample-graph distance among reachable stations with free chargers.
- No live charger API, online booking, payments, or real-time traffic is integrated.
