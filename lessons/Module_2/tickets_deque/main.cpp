#include <algorithm>
#include <deque>
#include <string>

using namespace std;

struct Ticket {
    int id;
    string name;
};

class TicketOffice {
public:
    // добавить билет в систему
    void PushTicket(const string& name) {
        tickets_.push_back({last_id_, name});
        ++last_id_;
    }

    // получить количество доступных билетов
    int GetAvailable() const {
        return last_id_;
    }

    // получить количество доступных билетов определённого типа
    int GetAvailable(const string& name) const {
        return count_if(tickets_.begin(), tickets_.end(), [name](Ticket ticket_){return name == ticket_.name;});
    }

    // отозвать старые билеты (до определённого id)
    void Invalidate(int minimum) {
        for (int i = 0; i < minimum; ++i) {
            tickets_.pop_front();
            --last_id_;
        }
    }

private:
    int last_id_ = 0;
    deque<Ticket> tickets_;
};