#include <cassert>
#include <sstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

istream& operator>>(istream& is, Query& q) {
    string s;
    is >> s;
    q.bus = ""s;
    q.stop = ""s;
    q.stops = {};

    if (s == "NEW_BUS"s) q.type = QueryType::NewBus;
    else if (s == "BUSES_FOR_STOP"s) q.type = QueryType::BusesForStop;
    else if (s == "STOPS_FOR_BUS"s) q.type = QueryType::StopsForBus;
    else q.type = QueryType::AllBuses;   
    
    if ((q.type == QueryType::NewBus) || (q.type == QueryType::StopsForBus)) {
        is >> q.bus;
    
        if (q.type == QueryType::NewBus) {       
            int stop_count;
            is >> stop_count;
            string line;
            getline(is, line);
            q.stops = SplitIntoWords(line);
        }

    } else if (q.type == QueryType::BusesForStop){
        is >> q.stop;
    }   
    
    return is;
}

struct BusesForStopResponse {
    string stop;
    vector<string> buses;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.buses.empty()) {
        os << "No stop"s;
    } else {
        for (const string& bus : r.buses) {
            os << bus << " "s;
        }
    }
    return os;
}

struct StopsForBusResponse {
    string bus;
    vector<string> stops;
    map<string, vector<string>> stops_to_buses;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.stops.empty()) {
        os << "No bus"s;
    } else {
        int count = 0;
        for (const string& stop : r.stops) {
            ++count;
            os << "Stop "s << stop << ": "s;
            if (r.stops_to_buses.at(stop).size() == 1) {
                os << "no interchange"s;
            } else {                
                for (const string& other_bus : r.stops_to_buses.at(stop)) {
                    if (r.bus != other_bus) {
                        os << other_bus << " "s;
                    }   
                }         
            }
            bool next_line = static_cast<int>(r.stops.size()) == count;
            if(!next_line) os << endl;      
        }
    }
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> buses_to_stops;
    map<string, vector<string>> stops_to_buses;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.buses_to_stops.empty()) {
        os << "No buses"s;
    } else {
        int count =0;
        for (const auto& bus_item : r.buses_to_stops) {
            ++count;
            os << "Bus "s << bus_item.first << ": "s;
            for (const string& stop : bus_item.second) {
                os << stop << " "s;
            }
            bool next_line = static_cast<int>(r.buses_to_stops.size()) == count;
            if(!next_line) os << endl; 
        }
              
    }
    return os;
}

class BusManager {
private:
    map<string, vector<string>> buses_to_stops_, stops_to_buses_;
                                //#1 Авт - Ост     #2 Ост - Авт
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        buses_to_stops_[bus] = stops;
        for (const string& stop : stops) {
            stops_to_buses_[stop].push_back(bus);
        }  
    } 

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse r;
        r.stop = stop;
        if (stops_to_buses_.count(stop) > 0) {
            r.buses = stops_to_buses_.at(stop);
        }         
        return r;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse r;
        r.bus = bus;
        r.stops_to_buses = stops_to_buses_;
        if (buses_to_stops_.count(bus) > 0) {
            r.stops = buses_to_stops_.at(bus);
        }        
        return r;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse r; 
        r.buses_to_stops = buses_to_stops_;
        r.stops_to_buses = stops_to_buses_;
        return r;
    }
};

int main() {
    #ifndef FILES
        freopen("inp.txt", "r", stdin);
        freopen("outp.txt", "w", stdout);
    #endif

    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}