#ifndef DATAACCESS_H
#define DATAACCESS_H

#include "models.h"
#include <vector>
#include <memory>
#include <string>

using namespace std;


class UserRepository {
public:
    virtual ~UserRepository() = default;
    
    virtual User* findByUsername(const string& username) = 0;
    virtual User* findById(int id) = 0;
    virtual vector<User> findAll() = 0;
    virtual vector<User> findPendingSpecialService() = 0;
    virtual vector<User> findFreeServiceMembers(Specialization spec) = 0;
    virtual bool create(const User& user) = 0;
    virtual bool update(const User& user) = 0;
    virtual bool updateServiceMemberStatus(int userId, ServiceMemberStatus status) = 0;
    virtual bool confirmSpecialService(int userId, Specialization spec) = 0;
};

class LocationRepository {
public:
    virtual ~LocationRepository() = default;
    
    virtual Location* findById(int id) = 0;
    virtual vector<Location> findAll() = 0;
    virtual bool create(const Location& location) = 0;
    virtual bool update(const Location& location) = 0;
    virtual bool remove(int id) = 0;
    virtual bool addIncidentToLocation(int locationId, int incidentId) = 0;
};

class EventRepository {
public:
    virtual ~EventRepository() = default;
    
    virtual Event* findById(int id) = 0;
    virtual vector<Event> findAll() = 0;
    virtual vector<Event> findByLocationId(int locationId) = 0;
    virtual vector<Event> findEventsForUser(int userId) = 0;
    virtual bool create(const Event& event) = 0;
    virtual bool update(const Event& event) = 0;
    virtual bool remove(int id) = 0;
    virtual bool registerUserForEvent(int eventId, int userId) = 0;
    virtual bool unregisterUserFromEvent(int eventId, int userId) = 0;
};

class IncidentRepository {
public:
    virtual ~IncidentRepository() = default;
    
    virtual Incident* findById(int id) = 0;
    virtual vector<Incident> findAll() = 0;
    virtual vector<Incident> findByLocationId(int locationId) = 0;
    virtual vector<Incident> findByStatus(IncidentStatus status) = 0;
    virtual vector<Incident> findCriticalIncidents() = 0;
    virtual bool create(const Incident& incident) = 0;
    virtual bool update(const Incident& incident) = 0;
    virtual bool remove(int id) = 0;
    virtual bool updateStatus(int incidentId, IncidentStatus status, int resolverId) = 0;
    virtual bool assignServiceMember(int incidentId, int serviceMemberId) = 0;
};

class InMemoryUserRepository : public UserRepository {
private:
    vector<User> users;
    int nextId;
    
public:
    InMemoryUserRepository();
    
    User* findByUsername(const string& username) override;
    User* findById(int id) override;
    vector<User> findAll() override;
    vector<User> findPendingSpecialService() override;
    vector<User> findFreeServiceMembers(Specialization spec) override;
    bool create(const User& user) override;
    bool update(const User& user) override;
    bool updateServiceMemberStatus(int userId, ServiceMemberStatus status) override;
    bool confirmSpecialService(int userId, Specialization spec) override;
};

class InMemoryLocationRepository : public LocationRepository {
private:
    vector<Location> locations;
    int nextId;
    
public:
    InMemoryLocationRepository();
    
    Location* findById(int id) override;
    vector<Location> findAll() override;
    bool create(const Location& location) override;
    bool update(const Location& location) override;
    bool remove(int id) override;
    bool addIncidentToLocation(int locationId, int incidentId) override;
};

class InMemoryEventRepository : public EventRepository {
private:
    vector<Event> events;
    int nextId;
    
public:
    InMemoryEventRepository();
    
    Event* findById(int id) override;
    vector<Event> findAll() override;
    vector<Event> findByLocationId(int locationId) override;
    vector<Event> findEventsForUser(int userId) override;
    bool create(const Event& event) override;
    bool update(const Event& event) override;
    bool remove(int id) override;
    bool registerUserForEvent(int eventId, int userId) override;
    bool unregisterUserFromEvent(int eventId, int userId) override;
};

class InMemoryIncidentRepository : public IncidentRepository {
private:
    vector<Incident> incidents;
    int nextId;
    
public:
    InMemoryIncidentRepository();
    
    Incident* findById(int id) override;
    vector<Incident> findAll() override;
    vector<Incident> findByLocationId(int locationId) override;
    vector<Incident> findByStatus(IncidentStatus status) override;
    vector<Incident> findCriticalIncidents() override;
    bool create(const Incident& incident) override;
    bool update(const Incident& incident) override;
    bool remove(int id) override;
    bool updateStatus(int incidentId, IncidentStatus status, int resolverId) override;
    bool assignServiceMember(int incidentId, int serviceMemberId) override;
};

//фасад, доступ к данным
class Database {
private:
    unique_ptr<UserRepository> users;
    unique_ptr<LocationRepository> locations;
    unique_ptr<EventRepository> events;
    unique_ptr<IncidentRepository> incidents;
    
public:
    Database();
    
    UserRepository* getUsers() { return users.get(); }
    LocationRepository* getLocations() { return locations.get(); }
    EventRepository* getEvents() { return events.get(); }
    IncidentRepository* getIncidents() { return incidents.get(); }
};

#endif 