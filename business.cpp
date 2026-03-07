#include "business.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
using namespace std;

AuthService::AuthService(Database& database) : db(database), currentUser(nullptr) {}

bool AuthService::login(const string& username, const string& password) {
    User* user = db.getUsers()->findByUsername(username);
    
    if (user && user->passwordHash == password) {
        if (user->accountType == AccountType::PendingSpecialService) {
            cout << "Ваша учетная запись ожидает подтверждения администратором.\n";
            return false;
        }
        
        currentUser = user;
        string desc = "Пользователь " + username + " вошел в систему";
        return true;
    }
    return false;
}

void AuthService::logout() {
    if (currentUser) {
        string desc = "Пользователь " + currentUser->username + " вышел из системы";
        currentUser = nullptr;
    }
}

bool AuthService::hasRole(AccountType requiredRole) const {
    return currentUser && currentUser->accountType == requiredRole;
}

bool AuthService::isAdmin() const {
    return hasRole(AccountType::Administrator);
}

bool AuthService::isSpecialService() const {
    return hasRole(AccountType::SpecialService);
}

bool AuthService::isVisitor() const {
    return hasRole(AccountType::Visitor);
}

bool AuthService::isPendingSpecialService() const {
    return hasRole(AccountType::PendingSpecialService);
}


EventService::EventService(Database& database, AuthService& authService) 
    : db(database), auth(authService) {}

vector<Event> EventService::getAllEvents() {
    return db.getEvents()->findAll();
}

vector<Event> EventService::getEventsByLocation(int locationId) {
    return db.getEvents()->findByLocationId(locationId);
}

Event* EventService::getEventById(int eventId) {
    return db.getEvents()->findById(eventId);
}

bool EventService::registerForEvent(int eventId, int userId) {
    Event* event = db.getEvents()->findById(eventId);
    if (!event) {
        cout << "Ошибка: Мероприятие не найдено!" << endl;
        return false;
    }
    
    if (event->currentParticipants >= event->maxParticipants) {
        cout << "Ошибка: Достигнут лимит участников!" << endl;
        return false;
    }
    
    bool result = db.getEvents()->registerUserForEvent(eventId, userId);
    
    if (result) {
        User* user = db.getUsers()->findById(userId);
        if (user) {
            user->registeredEventIds.push_back(eventId);
            db.getUsers()->update(*user);
        }
        
        string desc = "Пользователь " + to_string(userId) + 
                           " зарегистрировался на мероприятие " + to_string(eventId);
    }
    
    return result;
}

bool EventService::unregisterFromEvent(int eventId, int userId) {
    bool result = db.getEvents()->unregisterUserFromEvent(eventId, userId);
    
    if (result) {
        User* user = db.getUsers()->findById(userId);
        if (user) {
            auto it = find(user->registeredEventIds.begin(), 
                                user->registeredEventIds.end(), eventId);
            if (it != user->registeredEventIds.end()) {
                user->registeredEventIds.erase(it);
                db.getUsers()->update(*user);
            }
        }
        
        string desc = "Пользователь " + to_string(userId) + 
                           " отменил регистрацию на мероприятие " + to_string(eventId);
    }
    
    return result;
}

vector<Event> EventService::getMyEvents(int userId) {
    return db.getEvents()->findEventsForUser(userId);
}

bool EventService::createEvent(const string& name, const string& description, 
                               int locationId, const string& dateTime, int maxParticipants) {
    
    Location* loc = db.getLocations()->findById(locationId);
    if (!loc) {
        cout << "Ошибка: Локация не найдена!" << endl;
        return false;
    }
    
    if (maxParticipants > loc->capacity) {
        cout << "Ошибка: Количество участников превышает вместимость локации!" << endl;
        return false;
    }
    
    Event newEvent;
    newEvent.name = name;
    newEvent.description = description;
    newEvent.locationId = locationId;
    newEvent.dateTime = dateTime;
    newEvent.maxParticipants = maxParticipants;
    newEvent.currentParticipants = 0;
    
    bool result = db.getEvents()->create(newEvent);
    
    if (result) {
        loc->eventIds.push_back(newEvent.id);
        db.getLocations()->update(*loc);
        
        string desc = "Создано мероприятие: " + name;
    }
    
    return result;
}

bool EventService::updateEvent(const Event& event) {    
    bool result = db.getEvents()->update(event);
    
    if (result) {
        string desc = "Обновлено мероприятие: " + to_string(event.id);
    }
    
    return result;
}

bool EventService::deleteEvent(int eventId) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может удалять мероприятия!" << endl;
        return false;
    }
    
    bool result = db.getEvents()->remove(eventId);
    
    if (result) {
        string desc = "Удалено мероприятие: " + to_string(eventId);
    }
    
    return result;
}

IncidentService::IncidentService(Database& database, AuthService& authService) 
    : db(database), auth(authService) {}

void IncidentService::notifySpecialServices(const Incident& incident) {
    cout << "\n=== УВЕДОМЛЕНИЕ СПЕЦСЛУЖБ ===" << endl;
    cout << "Критический инцидент: " << incident.description << endl;
    cout << "Категория: " << incident.category << endl;
    cout << "Местоположение ID: " << incident.locationId << endl;
    
    if (incident.category == "Пожар") {
        cout << "Оповещаются пожарные службы!" << endl;
        auto firefighters = db.getUsers()->findFreeServiceMembers(Specialization::Firefighter);
        for (const auto& f : firefighters) {
            cout << "  - " << f.fullName << " (свободен)" << endl;
        }
    } else if (incident.category == "Медицина" || incident.category == "Травма") {
        cout << "Оповещаются медицинские службы!" << endl;
        auto medics = db.getUsers()->findFreeServiceMembers(Specialization::Medic);
        for (const auto& m : medics) {
            cout << "  - " << m.fullName << " (свободен)" << endl;
        }
    } else if (incident.category == "Терроризм" || incident.category == "Драка") {
        cout << "Оповещаются службы безопасности и полиция!" << endl;
        auto security = db.getUsers()->findFreeServiceMembers(Specialization::Security);
        for (const auto& s : security) {
            cout << "  - " << s.fullName << " (свободен)" << endl;
        }
    }
}

bool IncidentService::reportIncident(const string& description, const string& category,
                                    int locationId, int eventId, int userId) {
    if (!auth.isAuthenticated()) {
        cout << "Ошибка: Необходимо авторизоваться!" << endl;
        return false;
    }
    
    Location* loc = db.getLocations()->findById(locationId);
    if (!loc) {
        cout << "Ошибка: Локация не найдена!" << endl;
        return false;
    }
    
    Incident newInc;
    newInc.description = description;
    newInc.category = category;
    newInc.locationId = locationId;
    newInc.eventId = eventId;
    newInc.reportedByUserId = userId;
    newInc.status = IncidentStatus::New;
    newInc.criticality = CriticalityLevel::Unspecified;
    
    bool result = db.getIncidents()->create(newInc);
    
    if (result) {
        db.getLocations()->addIncidentToLocation(locationId, newInc.id);
        
        string desc = "Сообщено об инциденте: " + category + " в локации " + to_string(locationId);
        
        cout << "\nИнцидент зарегистрирован и передан на рассмотрение.\n";
        cout << "Статус инцидента будет определен администратором или сотрудниками спецслужб.\n";
    }
    
    return result;
}

bool IncidentService::createIncident(const string& description, const string& category,
                                    int locationId, int eventId, CriticalityLevel criticality) {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав для создания инцидента!" << endl;
        return false;
    }
    
    Location* loc = db.getLocations()->findById(locationId);
    if (!loc) {
        cout << "Ошибка: Локация не найдена!" << endl;
        return false;
    }
    
    Incident newInc;
    newInc.description = description;
    newInc.category = category;
    newInc.locationId = locationId;
    newInc.eventId = eventId;
    newInc.reportedByUserId = auth.getCurrentUser()->id;
    newInc.criticality = criticality;
    newInc.status = IncidentStatus::New;
    
    bool result = db.getIncidents()->create(newInc);
    
    if (result) {
        db.getLocations()->addIncidentToLocation(locationId, newInc.id);
        
        string desc = "Создан инцидент: " + category + " в локации " + to_string(locationId);
        
        if (newInc.criticality == CriticalityLevel::Critical) {
            notifySpecialServices(newInc);
        }
    }
    
    return result;
}

bool IncidentService::updateIncident(const Incident& incident) {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав для изменения инцидента!" << endl;
        return false;
    }
    
    bool result = db.getIncidents()->update(incident);
    
    if (result) {
        string desc = "Изменен инцидент: " + to_string(incident.id);
    }
    
    return result;
}

bool IncidentService::deleteIncident(int incidentId) {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав для удаления инцидента!" << endl;
        return false;
    }
    
    bool result = db.getIncidents()->remove(incidentId);
    
    if (result) {
        string desc = "Удален инцидент: " + to_string(incidentId);
    }
    
    return result;
}

vector<Incident> IncidentService::getAllIncidents() {
    return db.getIncidents()->findAll();
}

vector<Incident> IncidentService::getIncidentsByLocation(int locationId) {
    return db.getIncidents()->findByLocationId(locationId);
}

Incident* IncidentService::getIncidentById(int incidentId) {
    return db.getIncidents()->findById(incidentId);
}

vector<Incident> IncidentService::getNewIncidents() {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав!" << endl;
        return vector<Incident>();
    }
    return db.getIncidents()->findByStatus(IncidentStatus::New);
}

vector<Incident> IncidentService::getUnspecifiedCriticalityIncidents() {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав!" << endl;
        return vector<Incident>();
    }
    
    vector<Incident> result;
    auto incidents = db.getIncidents()->findAll();
    for (const auto& inc : incidents) {
        if (inc.criticality == CriticalityLevel::Unspecified) {
            result.push_back(inc);
        }
    }
    return result;
}

bool IncidentService::updateIncidentStatus(int incidentId, IncidentStatus status, int resolverId) {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав для изменения статуса инцидента!" << endl;
        return false;
    }
    
    Incident* incident = db.getIncidents()->findById(incidentId);
    if (!incident) {
        cout << "Ошибка: Инцидент не найден!" << endl;
        return false;
    }
    
    bool result = db.getIncidents()->updateStatus(incidentId, status, resolverId);
    
    if (result) {
        if (status == IncidentStatus::Completed || status == IncidentStatus::Rejected) {
            if (incident->assignedToServiceMemberId != 0) {
                db.getUsers()->updateServiceMemberStatus(incident->assignedToServiceMemberId, ServiceMemberStatus::Free);
            }
        }
        
        string statusStr;
        switch (status) {
            case IncidentStatus::New: statusStr = "Новый"; break;
            case IncidentStatus::InProgress: statusStr = "В процессе"; break;
            case IncidentStatus::Completed: statusStr = "Завершен"; break;
            case IncidentStatus::Rejected: statusStr = "Отклонен"; break;
        }
        
        string desc = "Изменен статус инцидента " + to_string(incidentId) + " на " + statusStr;
    }
    
    return result;
}

bool IncidentService::setIncidentCriticality(int incidentId, CriticalityLevel criticality) {
    if (!auth.isAdmin() && !auth.isSpecialService()) {
        cout << "Ошибка: Недостаточно прав для установки критичности инцидента!" << endl;
        return false;
    }
    
    Incident* incident = db.getIncidents()->findById(incidentId);
    if (!incident) {
        cout << "Ошибка: Инцидент не найден!" << endl;
        return false;
    }
    
    incident->criticality = criticality;
    bool result = db.getIncidents()->update(*incident);
    
    if (result) {
        string critStr;
        switch (criticality) {
            case CriticalityLevel::Unspecified: critStr = "Не указана"; break;
            case CriticalityLevel::Low: critStr = "Низкая"; break;
            case CriticalityLevel::Medium: critStr = "Средняя"; break;
            case CriticalityLevel::Critical: critStr = "Критическая"; break;
        }
        
        string desc = "Установлена критичность инцидента " + to_string(incidentId) + ": " + critStr;
        
        if (criticality == CriticalityLevel::Critical) {
            notifySpecialServices(*incident);
        }
    }
    
    return result;
}

bool IncidentService::assignServiceMember(int incidentId, int serviceMemberId) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может назначать сотрудников!" << endl;
        return false;
    }
    
    User* member = db.getUsers()->findById(serviceMemberId);
    if (!member || member->status != ServiceMemberStatus::Free) {
        cout << "Ошибка: Сотрудник занят или не найден!" << endl;
        return false;
    }
    
    bool result = db.getIncidents()->assignServiceMember(incidentId, serviceMemberId);
    
    if (result) {
        db.getUsers()->updateServiceMemberStatus(serviceMemberId, ServiceMemberStatus::Busy);
        
        string desc = "Назначен сотрудник " + to_string(serviceMemberId) + 
                           " на инцидент " + to_string(incidentId);
    }
    
    return result;
}

vector<User> IncidentService::getAvailableServiceMembers(Specialization spec) {
    return db.getUsers()->findFreeServiceMembers(spec);
}

vector<Incident> IncidentService::getCriticalIncidents() {
    return db.getIncidents()->findCriticalIncidents();
}


LocationService::LocationService(Database& database, AuthService& authService) 
    : db(database), auth(authService) {}

vector<Location> LocationService::getAllLocations() {
    return db.getLocations()->findAll();
}

Location* LocationService::getLocationById(int locationId) {
    return db.getLocations()->findById(locationId);
}

vector<Event> LocationService::getEventsByLocation(int locationId) {
    return db.getEvents()->findByLocationId(locationId);
}

bool LocationService::createLocation(const string& name, const string& address) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может создавать локации!" << endl;
        return false;
    }
    
    Location newLoc;
    newLoc.name = name;
    newLoc.address = address;
    newLoc.capacity = 0;
    
    bool result = db.getLocations()->create(newLoc);
    
    if (result) {
        string desc = "Создана локация: " + name;
    }
    
    return result;
}

bool LocationService::updateLocation(const Location& location) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может обновлять локации!" << endl;
        return false;
    }
    
    bool result = db.getLocations()->update(location);
    
    if (result) {
        string desc = "Обновлена локация: " + to_string(location.id);
    }
    
    return result;
}

bool LocationService::deleteLocation(int locationId) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может удалять локации!" << endl;
        return false;
    }
    
    auto events = db.getEvents()->findByLocationId(locationId);
    if (!events.empty()) {
        cout << "Ошибка: Нельзя удалить локацию, в которой есть мероприятия!" << endl;
        return false;
    }
    
    bool result = db.getLocations()->remove(locationId);
    
    if (result) {
        string desc = "Удалена локация: " + to_string(locationId);
    }
    
    return result;
}


UserService::UserService(Database& database, AuthService& authService) 
    : db(database), auth(authService) {}

bool UserService::registerUser(const string& username, const string& password,
                              const string& fullName, AccountType type) {
    if (db.getUsers()->findByUsername(username) != nullptr) {
        cout << "Ошибка: Пользователь с таким логином уже существует!" << endl;
        return false;
    }
    
    User newUser;
    newUser.username = username;
    newUser.passwordHash = password;
    newUser.fullName = fullName;
    newUser.accountType = type;
    
    if (type == AccountType::SpecialService) {
        newUser.status = ServiceMemberStatus::Free;
    }
    
    bool result = db.getUsers()->create(newUser);
    
    if (result) {
        string desc = "Зарегистрирован новый пользователь: " + username;
    }
    
    return result;
}

vector<User> UserService::getAllUsers() {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может просматривать всех пользователей!" << endl;
        return vector<User>();
    }
    return db.getUsers()->findAll();
}

vector<User> UserService::getPendingSpecialService() {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может просматривать ожидающих подтверждения!\n";
        return vector<User>();
    }
    return db.getUsers()->findPendingSpecialService();
}

bool UserService::assignSpecialization(int userId, Specialization spec) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может назначать специализацию!" << endl;
        return false;
    }
    
    User* user = db.getUsers()->findById(userId);
    if (!user || user->accountType != AccountType::SpecialService) {
        cout << "Ошибка: Пользователь не найден или не является сотрудником спецслужб!" << endl;
        return false;
    }
    
    user->specialization = spec;
    bool result = db.getUsers()->update(*user);
    
    if (result) {
        string desc = "Назначена специализация пользователю " + to_string(userId);
    }
    
    return result;
}

bool UserService::updateUserStatus(int userId, ServiceMemberStatus status) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может изменять статус!" << endl;
        return false;
    }
    
    bool result = db.getUsers()->updateServiceMemberStatus(userId, status);
    
    if (result) {
        string desc = "Изменен статус пользователя " + to_string(userId);
    }
    
    return result;
}

bool UserService::confirmSpecialService(int userId, Specialization spec) {
    if (!auth.isAdmin()) {
        cout << "Ошибка: Только администратор может подтверждать сотрудников!\n";
        return false;
    }
    
    bool result = db.getUsers()->confirmSpecialService(userId, spec);
    
    if (result) {
        string desc = "Подтвержден сотрудник спецслужб: " + to_string(userId);
    }
    
    return result;
}