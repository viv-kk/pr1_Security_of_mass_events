#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
using namespace std;

enum class AccountType {
    Visitor,
    Administrator,
    SpecialService,
    PendingSpecialService
};

enum class ServiceMemberStatus {//статус сотрудника спец службы
    Free,
    Busy
};

enum class Specialization {//квалификация сотрудника спец служю
    Firefighter,
    Medic,
    Police,
    Security 
};

enum class IncidentStatus {
    New,//ожидает рассмотрения, поступил от посетителя
    InProgress,
    Completed,
    Rejected//отклонен, если не надо решать
};

enum class CriticalityLevel {//критичность
    Unspecified,//не определен, поступил от посетителя
    Low,
    Medium,
    Critical
};

struct User {
    int id;
    string username;
    string passwordHash;
    string fullName;
    AccountType accountType;
    
    vector<int> registeredEventIds;//для посетителя-мероприятия, где зарегестрирован
    
    Specialization specialization;//сотрудники спец служб: квалификация и статус
    ServiceMemberStatus status;
    
    User() : id(0), accountType(AccountType::Visitor), 
             specialization(Specialization::Security), 
             status(ServiceMemberStatus::Free) {}
};

struct Location {
    int id;
    string name;
    string address;
    int capacity;
    vector<int> eventIds;
    vector<int> incidentIds;
};

struct Event {
    int id;
    string name;
    string description;
    int locationId;
    string dateTime;
    int maxParticipants;
    int currentParticipants;
    vector<int> registeredUserIds;
    vector<int> incidentIds;
};

struct Incident {
    int id;
    string description;
    string category;
    IncidentStatus status;
    CriticalityLevel criticality;
    int locationId;
    int eventId;
    int reportedByUserId;
    int assignedToServiceMemberId;
    string createdAt;
};

struct Log {
    int id;
    int userId;
    string username;
    string action;
    string description;
    string timestamp;
};

#endif