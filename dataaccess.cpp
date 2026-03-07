#include "dataaccess.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <string>

using namespace std;

InMemoryUserRepository::InMemoryUserRepository() : nextId(1) {
    User admin;
    admin.id = nextId++;
    admin.username = "admin";
    admin.passwordHash = "admin";
    admin.fullName = "Администратор";
    admin.accountType = AccountType::Administrator;
    users.push_back(admin);
    
    User firefighter;
    firefighter.id = nextId++;
    firefighter.username = "fireman";
    firefighter.passwordHash = "fire";
    firefighter.fullName = "Иванов Иван. Пожарник";
    firefighter.accountType = AccountType::SpecialService;
    firefighter.specialization = Specialization::Firefighter;
    firefighter.status = ServiceMemberStatus::Free;
    users.push_back(firefighter);
    
    User medic;
    medic.id = nextId++;
    medic.username = "medic";
    medic.passwordHash = "medic";
    medic.fullName = "Петрова Анна. Медик";
    medic.accountType = AccountType::SpecialService;
    medic.specialization = Specialization::Medic;
    medic.status = ServiceMemberStatus::Free;
    users.push_back(medic);
    
    User visitor;
    visitor.id = nextId++;
    visitor.username = "visitor";
    visitor.passwordHash = "pass";
    visitor.fullName = "Сидоров Петр";
    visitor.accountType = AccountType::Visitor;
    users.push_back(visitor);
}

User* InMemoryUserRepository::findByUsername(const string& username) {
    for (auto& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

User* InMemoryUserRepository::findById(int id) {
    for (auto& user : users) {
        if (user.id == id) {
            return &user;
        }
    }
    return nullptr;
}

vector<User> InMemoryUserRepository::findAll() {
    return users;
}

vector<User> InMemoryUserRepository::findPendingSpecialService() {
    vector<User> result;
    for (const auto& user : users) {
        if (user.accountType == AccountType::PendingSpecialService) {
            result.push_back(user);
        }
    }
    return result;
}

vector<User> InMemoryUserRepository::findFreeServiceMembers(Specialization spec) {
    vector<User> result;
    for (const auto& user : users) {
        if (user.accountType == AccountType::SpecialService && 
            user.specialization == spec &&
            user.status == ServiceMemberStatus::Free) {
            result.push_back(user);
        }
    }
    return result;
}

bool InMemoryUserRepository::create(const User& user) {
    User newUser = user;
    newUser.id = nextId++;
    users.push_back(newUser);
    return true;
}

bool InMemoryUserRepository::update(const User& user) {
    for (auto& u : users) {
        if (u.id == user.id) {
            u = user;
            return true;
        }
    }
    return false;
}

bool InMemoryUserRepository::updateServiceMemberStatus(int userId, ServiceMemberStatus status) {
    for (auto& u : users) {
        if (u.id == userId && u.accountType == AccountType::SpecialService) {
            u.status = status;
            return true;
        }
    }
    return false;
}

bool InMemoryUserRepository::confirmSpecialService(int userId, Specialization spec) {
    for (auto& u : users) {
        if (u.id == userId && u.accountType == AccountType::PendingSpecialService) {
            u.accountType = AccountType::SpecialService;
            u.specialization = spec;
            u.status = ServiceMemberStatus::Free;
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////

InMemoryLocationRepository::InMemoryLocationRepository() : nextId(1) {
    Location loc;
    loc.id = nextId++;
    loc.name = "Центральный стадион";
    loc.address = "ул. Спортивная, 1";
    loc.capacity = 50000;
    locations.push_back(loc);
}

Location* InMemoryLocationRepository::findById(int id) {
    for (auto& loc : locations) {
        if (loc.id == id) {
            return &loc;
        }
    }
    return nullptr;
}

vector<Location> InMemoryLocationRepository::findAll() {
    return locations;
}

bool InMemoryLocationRepository::create(const Location& location) {
    Location newLoc = location;
    newLoc.id = nextId++;
    locations.push_back(newLoc);
    return true;
}

bool InMemoryLocationRepository::update(const Location& location) {
    for (auto& loc : locations) {
        if (loc.id == location.id) {
            loc = location;
            return true;
        }
    }
    return false;
}

bool InMemoryLocationRepository::remove(int id) {
    auto it = remove_if(locations.begin(), locations.end(),
                              [id](const Location& loc) { return loc.id == id; });
    if (it != locations.end()) {
        locations.erase(it, locations.end());
        return true;
    }
    return false;
}

bool InMemoryLocationRepository::addIncidentToLocation(int locationId, int incidentId) {
    for (auto& loc : locations) {
        if (loc.id == locationId) {
            loc.incidentIds.push_back(incidentId);
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////

InMemoryEventRepository::InMemoryEventRepository() : nextId(1) {
    Event ev;
    ev.id = nextId++;
    ev.name = "Футбольный матч";
    ev.description = "Чемпионат города по футболу";
    ev.locationId = 1;
    ev.dateTime = "2026-04-15 19:00";
    ev.maxParticipants = 45000;
    ev.currentParticipants = 0;
    events.push_back(ev);
}

Event* InMemoryEventRepository::findById(int id) {
    for (auto& ev : events) {
        if (ev.id == id) {
            return &ev;
        }
    }
    return nullptr;
}

vector<Event> InMemoryEventRepository::findAll() {
    return events;
}

vector<Event> InMemoryEventRepository::findByLocationId(int locationId) {
    vector<Event> result;
    for (const auto& ev : events) {
        if (ev.locationId == locationId) {
            result.push_back(ev);
        }
    }
    return result;
}

vector<Event> InMemoryEventRepository::findEventsForUser(int userId) {
    vector<Event> result;
    for (const auto& ev : events) {
        for (int uid : ev.registeredUserIds) {
            if (uid == userId) {
                result.push_back(ev);
                break;
            }
        }
    }
    return result;
}

bool InMemoryEventRepository::create(const Event& event) {
    Event newEvent = event;
    newEvent.id = nextId++;
    events.push_back(newEvent);
    return true;
}

bool InMemoryEventRepository::update(const Event& event) {
    for (auto& ev : events) {
        if (ev.id == event.id) {
            ev = event;
            return true;
        }
    }
    return false;
}

bool InMemoryEventRepository::remove(int id) {
    auto it = remove_if(events.begin(), events.end(),
                              [id](const Event& ev) { return ev.id == id; });
    if (it != events.end()) {
        events.erase(it, events.end());
        return true;
    }
    return false;
}

bool InMemoryEventRepository::registerUserForEvent(int eventId, int userId) {
    for (auto& ev : events) {
        if (ev.id == eventId) {
            for (int uid : ev.registeredUserIds) {
                if (uid == userId) return false;
            }
            ev.registeredUserIds.push_back(userId);
            ev.currentParticipants++;
            return true;
        }
    }
    return false;
}

bool InMemoryEventRepository::unregisterUserFromEvent(int eventId, int userId) {
    for (auto& ev : events) {
        if (ev.id == eventId) {
            auto it = find(ev.registeredUserIds.begin(), ev.registeredUserIds.end(), userId);
            if (it != ev.registeredUserIds.end()) {
                ev.registeredUserIds.erase(it);
                ev.currentParticipants--;
                return true;
            }
            return false;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////

InMemoryIncidentRepository::InMemoryIncidentRepository() : nextId(1) {}

Incident* InMemoryIncidentRepository::findById(int id) {
    for (auto& inc : incidents) {
        if (inc.id == id) {
            return &inc;
        }
    }
    return nullptr;
}

vector<Incident> InMemoryIncidentRepository::findAll() {
    return incidents;
}

vector<Incident> InMemoryIncidentRepository::findByLocationId(int locationId) {
    vector<Incident> result;
    for (const auto& inc : incidents) {
        if (inc.locationId == locationId) {
            result.push_back(inc);
        }
    }
    return result;
}

vector<Incident> InMemoryIncidentRepository::findByStatus(IncidentStatus status) {
    vector<Incident> result;
    for (const auto& inc : incidents) {
        if (inc.status == status) {
            result.push_back(inc);
        }
    }
    return result;
}

vector<Incident> InMemoryIncidentRepository::findCriticalIncidents() {
    vector<Incident> result;
    for (const auto& inc : incidents) {
        if (inc.criticality == CriticalityLevel::Critical && inc.status != IncidentStatus::Completed) {
            result.push_back(inc);
        }
    }
    return result;
}

bool InMemoryIncidentRepository::create(const Incident& incident) {
    Incident newInc = incident;
    newInc.id = nextId++;
    incidents.push_back(newInc);
    return true;
}

bool InMemoryIncidentRepository::update(const Incident& incident) {
    for (auto& inc : incidents) {
        if (inc.id == incident.id) {
            inc = incident;
            return true;
        }
    }
    return false;
}

bool InMemoryIncidentRepository::remove(int id) {
    auto it = remove_if(incidents.begin(), incidents.end(),
                              [id](const Incident& inc) { return inc.id == id; });
    if (it != incidents.end()) {
        incidents.erase(it, incidents.end());
        return true;
    }
    return false;
}

bool InMemoryIncidentRepository::updateStatus(int incidentId, IncidentStatus status, int resolverId) {
    for (auto& inc : incidents) {
        if (inc.id == incidentId) {
            inc.status = status;
            if (status == IncidentStatus::Completed || status == IncidentStatus::Rejected) {
                inc.assignedToServiceMemberId = resolverId;
            }
            return true;
        }
    }
    return false;
}

bool InMemoryIncidentRepository::assignServiceMember(int incidentId, int serviceMemberId) {
    for (auto& inc : incidents) {
        if (inc.id == incidentId) {
            inc.assignedToServiceMemberId = serviceMemberId;
            inc.status = IncidentStatus::InProgress;
            return true;
        }
    }
    return false;
}



Database::Database() 
    : users(make_unique<InMemoryUserRepository>()),
      locations(make_unique<InMemoryLocationRepository>()),
      events(make_unique<InMemoryEventRepository>()),
      incidents(make_unique<InMemoryIncidentRepository>()) {}