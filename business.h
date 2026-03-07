#ifndef BUSINESS_H
#define BUSINESS_H

#include "dataaccess.h"
#include <memory>
#include <string>
using namespace std;

class AuthService {
private:
    Database& db;
    User* currentUser;
    
public:
    AuthService(Database& database);
    
    bool login(const string& username, const string& password);
    void logout();
    bool isAuthenticated() const { return currentUser != nullptr; }
    User* getCurrentUser() const { return currentUser; }
    
    bool hasRole(AccountType requiredRole) const;
    bool isAdmin() const;
    bool isSpecialService() const;
    bool isVisitor() const;
    bool isPendingSpecialService() const;
};

class EventService {
private:
    Database& db;
    AuthService& auth;
    
public:
    EventService(Database& database, AuthService& authService);
    
    vector<Event> getAllEvents();
    vector<Event> getEventsByLocation(int locationId);
    
    bool registerForEvent(int eventId, int userId);
    bool unregisterFromEvent(int eventId, int userId);
    vector<Event> getMyEvents(int userId);
    
    bool createEvent(const string& name, const string& description, 
                     int locationId, const string& dateTime, int maxParticipants);
    bool updateEvent(const Event& event);
    bool deleteEvent(int eventId);
    Event* getEventById(int eventId);
};

class IncidentService {
private:
    Database& db;
    AuthService& auth;

    void notifySpecialServices(const Incident& incident);
    
public:
    IncidentService(Database& database, AuthService& authService);
    
    bool reportIncident(const string& description, const string& category,
                        int locationId, int eventId, int userId);
    vector<Incident> getAllIncidents();
    vector<Incident> getIncidentsByLocation(int locationId);
    
    bool createIncident(const string& description, const string& category,
                       int locationId, int eventId, CriticalityLevel criticality);
    bool updateIncident(const Incident& incident);
    bool deleteIncident(int incidentId);
    bool updateIncidentStatus(int incidentId, IncidentStatus status, int resolverId);
    bool setIncidentCriticality(int incidentId, CriticalityLevel criticality);
    bool assignServiceMember(int incidentId, int serviceMemberId);
    vector<User> getAvailableServiceMembers(Specialization spec);
    vector<Incident> getCriticalIncidents();
    vector<Incident> getNewIncidents();
    vector<Incident> getUnspecifiedCriticalityIncidents();
    Incident* getIncidentById(int incidentId);
};

class LocationService {
private:
    Database& db;
    AuthService& auth;
    
public:
    LocationService(Database& database, AuthService& authService);
    
    vector<Location> getAllLocations();
    Location* getLocationById(int locationId);
    vector<Event> getEventsByLocation(int locationId);
    
    bool createLocation(const string& name, const string& address);
    bool updateLocation(const Location& location);
    bool deleteLocation(int locationId);
};

class UserService {
private:
    Database& db;
    AuthService& auth;
    
public:
    UserService(Database& database, AuthService& authService);
    
    bool registerUser(const string& username, const string& password,
                      const string& fullName, AccountType type);
    vector<User> getAllUsers();
    vector<User> getPendingSpecialService();
    
    bool assignSpecialization(int userId, Specialization spec);
    bool updateUserStatus(int userId, ServiceMemberStatus status);
    bool confirmSpecialService(int userId, Specialization spec);
};

#endif 