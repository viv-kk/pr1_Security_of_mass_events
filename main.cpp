#include "business.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <string>

using namespace std;

void clearScreen() {
    cout << "\n\n";
}

void printHeader() {
    cout << "\n";
    cout << "Система безопасности внешних и внутренних массовых мероприятий\n";
}

void printUserInfo(AuthService& auth) {
    if (auth.isAuthenticated()) {
        User* user = auth.getCurrentUser();
        cout << "Вход как: " << user->fullName;
        switch (user->accountType) {
            case AccountType::Visitor: cout << " (Посетитель)"; break;
            case AccountType::SpecialService: cout << " (Сотрудник спец службы)"; break;
            default: cout << ""; break;
        }
        cout << "\n";
    } else {
        cout << "Пользователь не авторизован\n";
    }
}

void showUnauthorizedMenu(AuthService& auth, UserService& userService) {
    int choice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "1. Войти в систему\n";
        cout << "2. Зарегистрироваться как посетитель\n";
        cout << "3. Зарегистрироваться как сотрудник спецслужб\n";
        cout << "0. Выход\n";
        cout << "Выбор: ";
        cin >> choice;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                string username, password;
                cout << "Логин: ";
                getline(cin, username);
                cout << "Пароль: ";
                getline(cin, password);
                
                if (auth.login(username, password)) {
                    cout << "\nВход выполнен успешно!\n";
                    return;
                } else {
                    cout << "\nОшибка входа! Неверный логин или пароль.\n";
                }
                
                break;
            }
            case 2: {
                string username, password, fullName;
                cout << "Регистрация нового посетителя\n";
                cout << "Логин: ";
                getline(cin, username);
                cout << "Пароль: ";
                getline(cin, password);
                cout << "ФИО: ";
                getline(cin, fullName);
                
                if (userService.registerUser(username, password, fullName, AccountType::Visitor)) {
                    cout << "\nРегистрация успешна! Теперь вы можете войти.\n";
                } else {
                    cout << "\nОшибка регистрации!\n";
                }
                
                break;
            }
            case 3: {
                string username, password, fullName;
                cout << "Регистрация сотрудника спецслужб\n";
                cout << "Ваша регистрация будет ожидать подтверждения администратором.\n\n";
                cout << "Логин: ";
                getline(cin, username);
                cout << "Пароль: ";
                getline(cin, password);
                cout << "ФИО: ";
                getline(cin, fullName);
                
                if (userService.registerUser(username, password, fullName, AccountType::PendingSpecialService)) {
                    cout << "\nРегистрация успешна! Ожидайте подтверждения администратором.\n";
                } else {
                    cout << "\nОшибка регистрации!\n";
                }
                
                break;
            }
            case 0:
                cout << "Программа завершена.\n";
                exit(0);
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showVisitorMenu(AuthService& auth, EventService& eventService, IncidentService& incidentService) {
    int choice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "1. Просмотреть все мероприятия\n";
        cout << "2. Мои мероприятия\n";
        cout << "3. Зарегистрироваться на мероприятие\n";
        cout << "4. Отменить регистрацию\n";
        cout << "5. Сообщить об инциденте\n";
        cout << "6. Просмотреть инциденты\n";
        cout << "0. Выйти из системы\n";
        cout << "Выбор: ";
        cin >> choice;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                auto events = eventService.getAllEvents();
                cout << "\n=== Все мероприятия ===\n";
                if (events.empty()) {
                    cout << "Мероприятий не найдено.\n";
                } else {
                    for (const auto& ev : events) {
                        cout << "ID: " << ev.id << " | " << ev.name 
                                  << " | Дата: " << ev.dateTime
                                  << " | Участники: " << ev.currentParticipants << "/" << ev.maxParticipants << "\n";
                    }
                }
                
                break;
            }
            case 2: {
                auto events = eventService.getMyEvents(auth.getCurrentUser()->id);
                cout << "\n=== Мои мероприятия ===\n";
                if (events.empty()) {
                    cout << "Вы не зарегистрированы ни на одно мероприятие.\n";
                } else {
                    for (const auto& ev : events) {
                        cout << "ID: " << ev.id << " | " << ev.name 
                                  << " | Дата: " << ev.dateTime << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                int eventId;
                cout << "Введите ID мероприятия: ";
                cin >> eventId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (eventService.registerForEvent(eventId, auth.getCurrentUser()->id)) {
                    cout << "\nВы успешно зарегистрированы!\n";
                } else {
                    cout << "\nОшибка регистрации!\n";
                }
                
                break;
            }
            case 4: {
                int eventId;
                cout << "Введите ID мероприятия: ";
                cin >> eventId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (eventService.unregisterFromEvent(eventId, auth.getCurrentUser()->id)) {
                    cout << "\nРегистрация отменена!\n";
                } else {
                    cout << "\nОшибка отмены регистрации!\n";
                }
                
                break;
            }
            case 5: {
                string description, category;
                int locationId, eventId;
                
                cout << "Сообщение об инциденте\n";
                cout << "Категория (Пожар/Медицина/Драка/Терроризм/Другое): ";
                getline(cin, category);
                cout << "Описание: ";
                getline(cin, description);
                cout << "ID локации: ";
                cin >> locationId;
                cout << "ID мероприятия (0, если не связано): ";
                cin >> eventId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (incidentService.reportIncident(description, category, locationId, 
                                                  eventId, auth.getCurrentUser()->id)) {
                    cout << "\nИнцидент зарегистрирован и передан на рассмотрение!\n";
                } else {
                    cout << "\nОшибка регистрации инцидента!\n";
                }
                
                break;
            }
            case 6: {
                auto incidents = incidentService.getAllIncidents();
                cout << "\n=== Все инциденты ===\n";
                if (incidents.empty()) {
                    cout << "Инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | Статус: " << (inc.status == IncidentStatus::New ? "Новый" :
                                                       inc.status == IncidentStatus::InProgress ? "В процессе" :
                                                       inc.status == IncidentStatus::Completed ? "Завершен" : "Отклонен")
                                  << " | Крит.: " << (inc.criticality == CriticalityLevel::Critical ? "КРИТИЧНЫЙ" : 
                                                     inc.criticality == CriticalityLevel::Medium ? "Средний" :
                                                     inc.criticality == CriticalityLevel::Low ? "Низкий" : "Не указана")
                                  << "\n";
                    }
                }
                
                break;
            }
            case 0:
                auth.logout();
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showSpecialServiceMenu(AuthService& auth, IncidentService& incidentService, LocationService& locationService) {
    int choice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "1. Просмотреть все инциденты\n";
        cout << "2. Просмотреть новые инциденты (от посетителей)\n";
        cout << "3. Просмотреть критические инциденты\n";
        cout << "4. Создать инцидент\n";
        cout << "5. Изменить инцидент\n";
        cout << "6. Удалить инцидент\n";
        cout << "7. Изменить статус инцидента\n";
        cout << "8. Установить критичность инцидента\n";
        cout << "0. Выйти из системы\n";
        cout << "Выбор: ";
        cin >> choice;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                auto incidents = incidentService.getAllIncidents();
                cout << "\n=== Все инциденты ===\n";
                if (incidents.empty()) {
                    cout << "Инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | Статус: " << (inc.status == IncidentStatus::New ? "Новый" :
                                                       inc.status == IncidentStatus::InProgress ? "В процессе" :
                                                       inc.status == IncidentStatus::Completed ? "Завершен" : "Отклонен")
                                  << " | Крит.: " << (inc.criticality == CriticalityLevel::Critical ? "КРИТИЧНЫЙ" : 
                                                     inc.criticality == CriticalityLevel::Medium ? "Средний" :
                                                     inc.criticality == CriticalityLevel::Low ? "Низкий" : "Не указана")
                                  << "\n";
                    }
                }
                
                break;
            }
            case 2: {
                auto incidents = incidentService.getNewIncidents();
                cout << "\n=== Новые инциденты (от посетителей) ===\n";
                if (incidents.empty()) {
                    cout << "Новых инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | " << inc.description
                                  << " | От пользователя: " << inc.reportedByUserId << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                auto incidents = incidentService.getCriticalIncidents();
                cout << "\n=== КРИТИЧЕСКИЕ ИНЦИДЕНТЫ ===\n";
                if (incidents.empty()) {
                    cout << "Критических инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | " << inc.description << "\n";
                    }
                }
                
                break;
            }
            case 4: {
                string description, category;
                int locationId, eventId;
                int criticalityChoice;
                
                cout << "Создание инцидента\n";
                cout << "Категория: ";
                getline(cin, category);
                cout << "Описание: ";
                getline(cin, description);
                cout << "ID локации: ";
                cin >> locationId;
                cout << "ID мероприятия (0, если не связано): ";
                cin >> eventId;
                cout << "Уровень критичности (0-Не указана, 1-Низкий, 2-Средний, 3-Критический): ";
                cin >> criticalityChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                CriticalityLevel criticality;
                if (criticalityChoice == 0) criticality = CriticalityLevel::Unspecified;
                else if (criticalityChoice == 1) criticality = CriticalityLevel::Low;
                else if (criticalityChoice == 2) criticality = CriticalityLevel::Medium;
                else if (criticalityChoice == 3) criticality = CriticalityLevel::Critical;
                else {
                    cout << "\nНеверный выбор критичности!\n";
                    break;
                }
                
                if (incidentService.createIncident(description, category, locationId, eventId, criticality)) {
                    cout << "\nИнцидент создан!\n";
                } else {
                    cout << "\nОшибка создания инцидента!\n";
                }
                
                break;
            }
            case 5: {
                int incidentId;
                cout << "Введите ID инцидента для изменения: ";
                cin >> incidentId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Incident* incident = incidentService.getIncidentById(incidentId);
                if (!incident) {
                    cout << "\nИнцидент не найден!\n";
                    break;
                }
                
                string description, category;
                int locationId, eventId;
                int criticalityChoice;
                
                cout << "Изменение инцидента ID: " << incidentId << "\n";
                cout << "Текущая категория (" << incident->category << "). Новая категория: ";
                getline(cin, category);
                if (category.empty()) category = incident->category;
                
                cout << "Текущее описание (" << incident->description << "). Новое описание: ";
                getline(cin, description);
                if (description.empty()) description = incident->description;
                
                cout << "Текущая локация ID (" << incident->locationId << "). Новая локация ID: ";
                cin >> locationId;
                if (locationId == 0) locationId = incident->locationId;
                
                cout << "Текущее мероприятие ID (" << incident->eventId << "). Новое мероприятие ID: ";
                cin >> eventId;
                
                cout << "Текущий уровень критичности (" 
                          << (incident->criticality == CriticalityLevel::Unspecified ? "Не указана" :
                              incident->criticality == CriticalityLevel::Low ? "Низкий" :
                              incident->criticality == CriticalityLevel::Medium ? "Средний" : "Критический")
                          << "). Новый уровень (0-Не указана, 1-Низкий, 2-Средний, 3-Критический, -1-оставить текущий): ";
                cin >> criticalityChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Incident updatedInc = *incident;
                updatedInc.category = category;
                updatedInc.description = description;
                updatedInc.locationId = locationId;
                updatedInc.eventId = eventId;
                
                if (criticalityChoice >= 0) {
                    if (criticalityChoice == 0) updatedInc.criticality = CriticalityLevel::Unspecified;
                    else if (criticalityChoice == 1) updatedInc.criticality = CriticalityLevel::Low;
                    else if (criticalityChoice == 2) updatedInc.criticality = CriticalityLevel::Medium;
                    else if (criticalityChoice == 3) updatedInc.criticality = CriticalityLevel::Critical;
                }
                
                if (incidentService.updateIncident(updatedInc)) {
                    cout << "\nИнцидент обновлен!\n";
                } else {
                    cout << "\nОшибка обновления инцидента!\n";
                }
                
                break;
            }
            case 6: {
                int incidentId;
                cout << "Введите ID инцидента для удаления: ";
                cin >> incidentId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Вы уверены? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (incidentService.deleteIncident(incidentId)) {
                        cout << "\nИнцидент удален!\n";
                    } else {
                        cout << "\nОшибка удаления инцидента!\n";
                    }
                } else {
                    cout << "\nУдаление отменено.\n";
                }
                
                break;
            }
            case 7: {
                int incidentId;
                cout << "Введите ID инцидента: ";
                cin >> incidentId;
                
                cout << "Выберите новый статус:\n";
                cout << "1. В процессе\n";
                cout << "2. Завершен\n";
                cout << "3. Отклонен\n";
                int statusChoice;
                cin >> statusChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                IncidentStatus status;
                if (statusChoice == 1) status = IncidentStatus::InProgress;
                else if (statusChoice == 2) status = IncidentStatus::Completed;
                else if (statusChoice == 3) status = IncidentStatus::Rejected;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (incidentService.updateIncidentStatus(incidentId, status, auth.getCurrentUser()->id)) {
                    cout << "\nСтатус обновлен!\n";
                } else {
                    cout << "\nОшибка обновления!\n";
                }
                
                break;
            }
            case 8: {
                int incidentId;
                cout << "Введите ID инцидента: ";
                cin >> incidentId;
                
                cout << "Выберите уровень критичности:\n";
                cout << "0. Не указана\n";
                cout << "1. Низкий\n";
                cout << "2. Средний\n";
                cout << "3. Критический\n";
                int critChoice;
                cin >> critChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                CriticalityLevel criticality;
                if (critChoice == 0) criticality = CriticalityLevel::Unspecified;
                else if (critChoice == 1) criticality = CriticalityLevel::Low;
                else if (critChoice == 2) criticality = CriticalityLevel::Medium;
                else if (critChoice == 3) criticality = CriticalityLevel::Critical;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (incidentService.setIncidentCriticality(incidentId, criticality)) {
                    cout << "\nКритичность установлена!\n";
                } else {
                    cout << "\nОшибка установки критичности!\n";
                }
                
                break;
            }
            case 0:
                auth.logout();
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showEventManagementMenu(AuthService& auth, EventService& eventService, LocationService& locationService) {
    int subChoice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "--- Управление мероприятиями ---\n";
        cout << "1. Создать мероприятие\n";
        cout << "2. Просмотреть все мероприятия\n";
        cout << "3. Изменить мероприятие\n";
        cout << "4. Удалить мероприятие\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (subChoice) {
            case 1: {
                string name, desc, dateTime;
                int locationId, maxParticipants;
                
                cout << "Название: ";
                getline(cin, name);
                cout << "Описание: ";
                getline(cin, desc);
                cout << "ID локации: ";
                cin >> locationId;
                cout << "Дата и время (ГГГГ-ММ-ДД ЧЧ:ММ): ";
                cin >> dateTime;
                cout << "Макс. участников: ";
                cin >> maxParticipants;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (eventService.createEvent(name, desc, locationId, dateTime, maxParticipants)) {
                    cout << "\nМероприятие создано!\n";
                } else {
                    cout << "\nОшибка создания!\n";
                }
                
                break;
            }
            case 2: {
                auto events = eventService.getAllEvents();
                cout << "\n=== Все мероприятия ===\n";
                if (events.empty()) {
                    cout << "Мероприятий не найдено.\n";
                } else {
                    for (const auto& ev : events) {
                        cout << "ID: " << ev.id << " | " << ev.name 
                                  << " | Дата: " << ev.dateTime
                                  << " | Локация ID: " << ev.locationId
                                  << " | Участники: " << ev.currentParticipants << "/" << ev.maxParticipants << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                int eventId;
                cout << "Введите ID мероприятия для изменения: ";
                cin >> eventId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Event* event = eventService.getEventById(eventId);
                if (!event) {
                    cout << "\nМероприятие не найдено!\n";
                    break;
                }
                
                string name, desc, dateTime;
                int locationId, maxParticipants;
                
                cout << "Изменение мероприятия ID: " << eventId << "\n";
                cout << "Текущее название (" << event->name << "). Новое название: ";
                getline(cin, name);
                if (name.empty()) name = event->name;
                
                cout << "Текущее описание (" << event->description << "). Новое описание: ";
                getline(cin, desc);
                if (desc.empty()) desc = event->description;
                
                cout << "Текущая локация ID (" << event->locationId << "). Новая локация ID: ";
                cin >> locationId;
                if (locationId == 0) locationId = event->locationId;
                
                cout << "Текущая дата (" << event->dateTime << "). Новая дата (ГГГГ-ММ-ДД ЧЧ:ММ): ";
                cin >> dateTime;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (dateTime.empty()) dateTime = event->dateTime;
                
                cout << "Текущий максимум участников (" << event->maxParticipants << "). Новый максимум: ";
                cin >> maxParticipants;
                if (maxParticipants == 0) maxParticipants = event->maxParticipants;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Event updatedEvent = *event;
                updatedEvent.name = name;
                updatedEvent.description = desc;
                updatedEvent.locationId = locationId;
                updatedEvent.dateTime = dateTime;
                updatedEvent.maxParticipants = maxParticipants;
                
                if (eventService.updateEvent(updatedEvent)) {
                    cout << "\nМероприятие обновлено!\n";
                } else {
                    cout << "\nОшибка обновления!\n";
                }
                
                break;
            }
            case 4: {
                int eventId;
                cout << "ID мероприятия для удаления: ";
                cin >> eventId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Вы уверены? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (eventService.deleteEvent(eventId)) {
                        cout << "\nМероприятие удалено!\n";
                    } else {
                        cout << "\nОшибка удаления!\n";
                    }
                } else {
                    cout << "\nУдаление отменено.\n";
                }
                
                break;
            }
            case 0:
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showLocationManagementMenu(AuthService& auth, LocationService& locationService, EventService& eventService) {
    int subChoice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "--- Управление локациями ---\n";
        cout << "1. Создать локацию\n";
        cout << "2. Просмотреть все локации\n";
        cout << "3. Просмотреть мероприятия локации\n";
        cout << "4. Изменить локацию\n";
        cout << "5. Удалить локацию\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (subChoice) {
            case 1: {
                string name, address;
                
                cout << "Название: ";
                getline(cin, name);
                cout << "Адрес: ";
                getline(cin, address);
                
                if (locationService.createLocation(name, address)) {
                    cout << "\nЛокация создана!\n";
                } else {
                    cout << "\nОшибка создания!\n";
                }
                
                break;
            }
            case 2: {
                auto locations = locationService.getAllLocations();
                cout << "\n=== Все локации ===\n";
                if (locations.empty()) {
                    cout << "Локаций не найдено.\n";
                } else {
                    for (const auto& loc : locations) {
                        cout << "ID: " << loc.id << " | " << loc.name 
                                  << " | Адрес: " << loc.address
                                  << " | Вместимость: " << loc.capacity << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                int locationId;
                cout << "Введите ID локации: ";
                cin >> locationId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                auto events = locationService.getEventsByLocation(locationId);
                cout << "\n=== Мероприятия в локации ID " << locationId << " ===\n";
                if (events.empty()) {
                    cout << "В этой локации нет мероприятий.\n";
                } else {
                    for (const auto& ev : events) {
                        cout << "ID: " << ev.id << " | " << ev.name 
                                  << " | Дата: " << ev.dateTime
                                  << " | Участники: " << ev.currentParticipants << "/" << ev.maxParticipants << "\n";
                    }
                }
                
                break;
            }
            case 4: {
                int locationId;
                cout << "Введите ID локации для изменения: ";
                cin >> locationId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Location* location = locationService.getLocationById(locationId);
                if (!location) {
                    cout << "\nЛокация не найдена!\n";
                    break;
                }
                
                string name, address;
                int capacity;
                
                cout << "Изменение локации ID: " << locationId << "\n";
                cout << "Текущее название (" << location->name << "). Новое название: ";
                getline(cin, name);
                if (name.empty()) name = location->name;
                
                cout << "Текущий адрес (" << location->address << "). Новый адрес: ";
                getline(cin, address);
                if (address.empty()) address = location->address;
                
                cout << "Текущая вместимость (" << location->capacity << "). Новая вместимость: ";
                cin >> capacity;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Location updatedLoc = *location;
                updatedLoc.name = name;
                updatedLoc.address = address;
                updatedLoc.capacity = capacity;
                
                if (locationService.updateLocation(updatedLoc)) {
                    cout << "\nЛокация обновлена!\n";
                } else {
                    cout << "\nОшибка обновления!\n";
                }
                
                break;
            }
            case 5: {
                int locationId;
                cout << "ID локации для удаления: ";
                cin >> locationId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Вы уверены? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (locationService.deleteLocation(locationId)) {
                        cout << "\nЛокация удалена!\n";
                    } else {
                        cout << "\nОшибка удаления!\n";
                    }
                } else {
                    cout << "\nУдаление отменено.\n";
                }
                
                break;
            }
            case 0:
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showIncidentManagementMenu(AuthService& auth, IncidentService& incidentService, LocationService& locationService) {
    int subChoice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "--- Управление инцидентами ---\n";
        cout << "1. Просмотреть все инциденты\n";
        cout << "2. Просмотреть новые инциденты (от посетителей)\n";
        cout << "3. Просмотреть инциденты с неуказанной критичностью\n";
        cout << "4. Просмотреть критические инциденты\n";
        cout << "5. Создать инцидент\n";
        cout << "6. Изменить инцидент\n";
        cout << "7. Удалить инцидент\n";
        cout << "8. Назначить сотрудника на инцидент\n";
        cout << "9. Изменить статус инцидента\n";
        cout << "10. Установить критичность инцидента\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (subChoice) {
            case 1: {
                auto incidents = incidentService.getAllIncidents();
                cout << "\n=== Все инциденты ===\n";
                if (incidents.empty()) {
                    cout << "Инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | Статус: " << (inc.status == IncidentStatus::New ? "Новый" :
                                                       inc.status == IncidentStatus::InProgress ? "В процессе" :
                                                       inc.status == IncidentStatus::Completed ? "Завершен" : "Отклонен")
                                  << " | Крит.: " << (inc.criticality == CriticalityLevel::Critical ? "КРИТИЧНЫЙ" : 
                                                     inc.criticality == CriticalityLevel::Medium ? "Средний" :
                                                     inc.criticality == CriticalityLevel::Low ? "Низкий" : "Не указана")
                                  << " | От: " << inc.reportedByUserId << "\n";
                    }
                }
                
                break;
            }
            case 2: {
                auto incidents = incidentService.getNewIncidents();
                cout << "\n=== Новые инциденты (от посетителей) ===\n";
                if (incidents.empty()) {
                    cout << "Новых инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | " << inc.description
                                  << " | От пользователя: " << inc.reportedByUserId << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                auto incidents = incidentService.getUnspecifiedCriticalityIncidents();
                cout << "\n=== Инциденты с неуказанной критичностью ===\n";
                if (incidents.empty()) {
                    cout << "Инцидентов с неуказанной критичностью не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | " << inc.description
                                  << " | Статус: " << (inc.status == IncidentStatus::New ? "Новый" :
                                                       inc.status == IncidentStatus::InProgress ? "В процессе" :
                                                       inc.status == IncidentStatus::Completed ? "Завершен" : "Отклонен")
                                  << "\n";
                    }
                }
                
                break;
            }
            case 4: {
                auto incidents = incidentService.getCriticalIncidents();
                cout << "\n=== КРИТИЧЕСКИЕ ИНЦИДЕНТЫ ===\n";
                if (incidents.empty()) {
                    cout << "Критических инцидентов не найдено.\n";
                } else {
                    for (const auto& inc : incidents) {
                        cout << "ID: " << inc.id << " | " << inc.category 
                                  << " | " << inc.description << "\n";
                    }
                }
                
                break;
            }
            case 5: {
                string description, category;
                int locationId, eventId;
                int criticalityChoice;
                
                cout << "Создание инцидента\n";
                cout << "Категория: ";
                getline(cin, category);
                cout << "Описание: ";
                getline(cin, description);
                cout << "ID локации: ";
                cin >> locationId;
                cout << "ID мероприятия (0, если не связано): ";
                cin >> eventId;
                cout << "Уровень критичности (0-Не указана, 1-Низкий, 2-Средний, 3-Критический): ";
                cin >> criticalityChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                CriticalityLevel criticality;
                if (criticalityChoice == 0) criticality = CriticalityLevel::Unspecified;
                else if (criticalityChoice == 1) criticality = CriticalityLevel::Low;
                else if (criticalityChoice == 2) criticality = CriticalityLevel::Medium;
                else if (criticalityChoice == 3) criticality = CriticalityLevel::Critical;
                else {
                    cout << "\nНеверный выбор критичности!\n";
                    break;
                }
                
                if (incidentService.createIncident(description, category, locationId, eventId, criticality)) {
                    cout << "\nИнцидент создан!\n";
                } else {
                    cout << "\nОшибка создания инцидента!\n";
                }
                
                break;
            }
            case 6: {
                int incidentId;
                cout << "Введите ID инцидента для изменения: ";
                cin >> incidentId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Incident* incident = incidentService.getIncidentById(incidentId);
                if (!incident) {
                    cout << "\nИнцидент не найден!\n";
                    break;
                }
                
                string description, category;
                int locationId, eventId;
                int criticalityChoice;
                
                cout << "Изменение инцидента ID: " << incidentId << "\n";
                cout << "Текущая категория (" << incident->category << "). Новая категория: ";
                getline(cin, category);
                if (category.empty()) category = incident->category;
                
                cout << "Текущее описание (" << incident->description << "). Новое описание: ";
                getline(cin, description);
                if (description.empty()) description = incident->description;
                
                cout << "Текущая локация ID (" << incident->locationId << "). Новая локация ID: ";
                cin >> locationId;
                if (locationId == 0) locationId = incident->locationId;
                
                cout << "Текущее мероприятие ID (" << incident->eventId << "). Новое мероприятие ID: ";
                cin >> eventId;
                
                cout << "Текущий уровень критичности (" 
                          << (incident->criticality == CriticalityLevel::Unspecified ? "Не указана" :
                              incident->criticality == CriticalityLevel::Low ? "Низкий" :
                              incident->criticality == CriticalityLevel::Medium ? "Средний" : "Критический")
                          << "). Новый уровень (0-Не указана, 1-Низкий, 2-Средний, 3-Критический, -1-оставить текущий): ";
                cin >> criticalityChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Incident updatedInc = *incident;
                updatedInc.category = category;
                updatedInc.description = description;
                updatedInc.locationId = locationId;
                updatedInc.eventId = eventId;
                
                if (criticalityChoice >= 0) {
                    if (criticalityChoice == 0) updatedInc.criticality = CriticalityLevel::Unspecified;
                    else if (criticalityChoice == 1) updatedInc.criticality = CriticalityLevel::Low;
                    else if (criticalityChoice == 2) updatedInc.criticality = CriticalityLevel::Medium;
                    else if (criticalityChoice == 3) updatedInc.criticality = CriticalityLevel::Critical;
                }
                
                if (incidentService.updateIncident(updatedInc)) {
                    cout << "\nИнцидент обновлен!\n";
                } else {
                    cout << "\nОшибка обновления инцидента!\n";
                }
                
                break;
            }
            case 7: {
                int incidentId;
                cout << "Введите ID инцидента для удаления: ";
                cin >> incidentId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Вы уверены? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (incidentService.deleteIncident(incidentId)) {
                        cout << "\nИнцидент удален!\n";
                    } else {
                        cout << "\nОшибка удаления инцидента!\n";
                    }
                } else {
                    cout << "\nУдаление отменено.\n";
                }
                
                break;
            }
            case 8: {
                int incidentId, serviceMemberId;
                cout << "ID инцидента: ";
                cin >> incidentId;
                
                cout << "\nДоступные сотрудники:\n";
                auto firefighters = incidentService.getAvailableServiceMembers(Specialization::Firefighter);
                if (!firefighters.empty()) {
                    cout << "Пожарные:\n";
                    for (const auto& f : firefighters) {
                        cout << "  ID: " << f.id << " | " << f.fullName << "\n";
                    }
                }
                
                auto medics = incidentService.getAvailableServiceMembers(Specialization::Medic);
                if (!medics.empty()) {
                    cout << "Медики:\n";
                    for (const auto& m : medics) {
                        cout << "  ID: " << m.id << " | " << m.fullName << "\n";
                    }
                }
                
                auto security = incidentService.getAvailableServiceMembers(Specialization::Security);
                if (!security.empty()) {
                    cout << "Охрана:\n";
                    for (const auto& s : security) {
                        cout << "  ID: " << s.id << " | " << s.fullName << "\n";
                    }
                }
                
                cout << "\nID сотрудника: ";
                cin >> serviceMemberId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (incidentService.assignServiceMember(incidentId, serviceMemberId)) {
                    cout << "\nСотрудник назначен!\n";
                } else {
                    cout << "\nОшибка назначения!\n";
                }
                
                break;
            }
            case 9: {
                int incidentId;
                cout << "Введите ID инцидента: ";
                cin >> incidentId;
                
                cout << "Выберите новый статус:\n";
                cout << "1. В процессе\n";
                cout << "2. Завершен\n";
                cout << "3. Отклонен\n";
                int statusChoice;
                cin >> statusChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                IncidentStatus status;
                if (statusChoice == 1) status = IncidentStatus::InProgress;
                else if (statusChoice == 2) status = IncidentStatus::Completed;
                else if (statusChoice == 3) status = IncidentStatus::Rejected;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (incidentService.updateIncidentStatus(incidentId, status, auth.getCurrentUser()->id)) {
                    cout << "\nСтатус обновлен!\n";
                } else {
                    cout << "\nОшибка обновления!\n";
                }
                
                break;
            }
            case 10: {
                int incidentId;
                cout << "Введите ID инцидента: ";
                cin >> incidentId;
                
                cout << "Выберите уровень критичности:\n";
                cout << "0. Не указана\n";
                cout << "1. Низкий\n";
                cout << "2. Средний\n";
                cout << "3. Критический\n";
                int critChoice;
                cin >> critChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                CriticalityLevel criticality;
                if (critChoice == 0) criticality = CriticalityLevel::Unspecified;
                else if (critChoice == 1) criticality = CriticalityLevel::Low;
                else if (critChoice == 2) criticality = CriticalityLevel::Medium;
                else if (critChoice == 3) criticality = CriticalityLevel::Critical;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (incidentService.setIncidentCriticality(incidentId, criticality)) {
                    cout << "\nКритичность установлена!\n";
                } else {
                    cout << "\nОшибка установки критичности!\n";
                }
                
                break;
            }
            case 0:
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showUserManagementMenu(AuthService& auth, UserService& userService) {
    int subChoice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "--- Управление пользователями ---\n";
        cout << "1. Просмотреть всех пользователей\n";
        cout << "2. Просмотреть ожидающих подтверждения сотрудников\n";
        cout << "3. Подтвердить сотрудника спецслужб\n";
        cout << "4. Назначить специализацию сотруднику\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        cin >> subChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (subChoice) {
            case 1: {
                auto users = userService.getAllUsers();
                cout << "\n=== Все пользователи ===\n";
                if (users.empty()) {
                    cout << "Пользователей не найдено.\n";
                } else {
                    for (const auto& u : users) {
                        cout << "ID: " << u.id << " | " << u.username 
                                  << " | " << u.fullName << " | Тип: ";
                        switch (u.accountType) {
                            case AccountType::Administrator: cout << "Админ"; break;
                            case AccountType::Visitor: cout << "Посетитель"; break;
                            case AccountType::SpecialService: cout << "Спецслужба"; break;
                            case AccountType::PendingSpecialService: cout << "Ожидает подтверждения"; break;
                        }
                        if (u.accountType == AccountType::SpecialService) {
                            cout << " | Спец.: ";
                            switch (u.specialization) {
                                case Specialization::Firefighter: cout << "Пожарный"; break;
                                case Specialization::Medic: cout << "Медик"; break;
                                case Specialization::Police: cout << "Полиция"; break;
                                case Specialization::Security: cout << "Охрана"; break;
                            }
                            cout << " | Статус: " << (u.status == ServiceMemberStatus::Free ? "Свободен" : "Занят");
                        }
                        cout << "\n";
                    }
                }
                
                break;
            }
            case 2: {
                auto pending = userService.getPendingSpecialService();
                cout << "\n=== Ожидающие подтверждения сотрудники ===\n";
                if (pending.empty()) {
                    cout << "Нет пользователей, ожидающих подтверждения.\n";
                } else {
                    for (const auto& u : pending) {
                        cout << "ID: " << u.id << " | " << u.username 
                                  << " | " << u.fullName << "\n";
                    }
                }
                
                break;
            }
            case 3: {
                auto pending = userService.getPendingSpecialService();
                if (pending.empty()) {
                    cout << "\nНет пользователей, ожидающих подтверждения.\n";
                    break;
                }
                
                cout << "\nОжидающие подтверждения сотрудники:\n";
                for (const auto& u : pending) {
                    cout << "ID: " << u.id << " | " << u.username << " | " << u.fullName << "\n";
                }
                
                int userId;
                cout << "\nВведите ID сотрудника для подтверждения: ";
                cin >> userId;
                
                cout << "Выберите специализацию:\n";
                cout << "1. Пожарный\n";
                cout << "2. Медик\n";
                cout << "3. Охрана\n";
                int specChoice;
                cin >> specChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Specialization spec;
                if (specChoice == 1) spec = Specialization::Firefighter;
                else if (specChoice == 2) spec = Specialization::Medic;
                else if (specChoice == 3) spec = Specialization::Security;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (userService.confirmSpecialService(userId, spec)) {
                    cout << "\nСотрудник подтвержден и получил специализацию!\n";
                } else {
                    cout << "\nОшибка подтверждения!\n";
                }
                
                break;
            }
            case 4: {
                int userId;
                cout << "ID сотрудника: ";
                cin >> userId;
                
                cout << "Выберите специализацию:\n";
                cout << "1. Пожарный\n";
                cout << "2. Медик\n";
                cout << "3. Охрана\n";
                int specChoice;
                cin >> specChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                Specialization spec;
                if (specChoice == 1) spec = Specialization::Firefighter;
                else if (specChoice == 2) spec = Specialization::Medic;
                else if (specChoice == 3) spec = Specialization::Security;
                else {
                    cout << "\nНеверный выбор!\n";
                    break;
                }
                
                if (userService.assignSpecialization(userId, spec)) {
                    cout << "\nСпециализация назначена!\n";
                } else {
                    cout << "\nОшибка назначения!\n";
                }
                
                break;
            }
            case 0:
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

void showAdminMenu(AuthService& auth, EventService& eventService, IncidentService& incidentService,
                  LocationService& locationService, UserService& userService) {
    int choice;
    
    do {
        printHeader();
        printUserInfo(auth);
        cout << "1. Управление мероприятиями\n";
        cout << "2. Управление локациями\n";
        cout << "3. Управление инцидентами\n";
        cout << "4. Управление пользователями\n";
        cout << "0. Выйти из системы\n";
        cout << "Выбор: ";
        cin >> choice;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                showEventManagementMenu(auth, eventService, locationService);
                break;
            case 2:
                showLocationManagementMenu(auth, locationService, eventService);
                break;
            case 3:
                showIncidentManagementMenu(auth, incidentService, locationService);
                break;
            case 4:
                showUserManagementMenu(auth, userService);
                break;
            case 0:
                auth.logout();
                return;
            default:
                cout << "\nНеверный выбор!\n";
                
        }
    } while (true);
}

int main() {
    Database db;
    AuthService auth(db);
    UserService userService(db, auth);
    EventService eventService(db, auth);
    IncidentService incidentService(db, auth);
    LocationService locationService(db, auth);

    cout << "Тестовые аккаунты:\n";
    cout << "логин / пароль (админ)\n";
    cout << "admin / admin (админ)\n";
    cout << "fireman / fire (сотрудник-пожарник)\n";
    cout << "medic / medic (сотрудник-медик)\n";
    cout << "visitor / pass (посетитель)\n";
    
    while (true) {
        if (!auth.isAuthenticated()) {
            showUnauthorizedMenu(auth, userService);
        } else {
            User* user = auth.getCurrentUser();
            switch (user->accountType) {
                case AccountType::Visitor:
                    showVisitorMenu(auth, eventService, incidentService);
                    break;
                case AccountType::SpecialService:
                    showSpecialServiceMenu(auth, incidentService, locationService);
                    break;
                case AccountType::Administrator:
                    showAdminMenu(auth, eventService, incidentService, locationService, userService);
                    break;
                default:
                    cout << "Неизвестный тип пользователя. Выход из системы.\n";
                    auth.logout();
            }
        }
    }
    
    return 0;
}