#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <regex>
#include <functional>
#include <ctime>
#include <cctype>

using namespace std;

// Constants for file names and system settings
const string USER_FILE = "users.txt";
const string CAR_FILE = "cars.txt";
const string RENTAL_FILE = "rentals.txt";
const string CUSTOMER_FILE = "customers.txt";
const string MAINTENANCE_FILE = "maintenance.txt";
const string TRANSACTION_LOG = "transaction_log.txt";
const string SETTINGS_FILE = "settings.txt";

// DateManager class for handling and validating dates
class DateManager {
private:
    static bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

public:
    struct Date {
        int dd, mm, yy;

        string toString() const {
            ostringstream oss;
            oss << setfill('0') << setw(2) << dd << "/" 
                << setw(2) << mm << "/" << yy;
            return oss.str();
        }

        bool operator<(const Date& other) const {
            if (yy != other.yy) return yy < other.yy;
            if (mm != other.mm) return mm < other.mm;
            return dd < other.dd;
        }

        bool operator==(const Date& other) const {
            return dd == other.dd && mm == other.mm && yy == other.yy;
        }

        bool operator<=(const Date& other) const {
            return *this < other || *this == other;
        }
    };

    static bool isValidDate(int dd, int mm, int yy) {
        if (yy < 1900 || yy > 2025) return false;
        if (mm < 1 || mm > 12) return false;
        if (dd < 1 || dd > 31) return false;

        const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (mm == 2 && isLeapYear(yy)) {
            return dd <= 29;
        }
        return dd <= daysInMonth[mm];
    }

    static int calculateDays(const Date& start, const Date& end) {
        if (start < end) {
            time_t t1, t2;
            struct tm tm1 = {0}, tm2 = {0};
            tm1.tm_year = start.yy - 1900;
            tm1.tm_mon = start.mm - 1;
            tm1.tm_mday = start.dd;
            tm2.tm_year = end.yy - 1900;
            tm2.tm_mon = end.mm - 1;
            tm2.tm_mday = end.dd;

            t1 = mktime(&tm1);
            t2 = mktime(&tm2);
            return static_cast<int>(difftime(t2, t1) / (60 * 60 * 24)) + 1;
        }
        return 0;
    }

    static Date getCurrentDate() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        return {timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900};
    }

    static bool isDateInFuture(const Date& date) {
        Date current = getCurrentDate();
        return date < current;
    }
};

// Settings Manager for configurable system parameters
class SettingsManager {
private:
    float latePenaltyRate;
    float loyaltyDiscountRate;
    int maxRentalDays;
    int loyaltyPointsPerRental;

public:
    SettingsManager() : latePenaltyRate(1.5f), loyaltyDiscountRate(0.1f), 
                       maxRentalDays(30), loyaltyPointsPerRental(100) {}

    void load() {
        ifstream file(SETTINGS_FILE);
        if (file) {
            string line;
            getline(file, line);
            sscanf(line.c_str(), "%f,%f,%d,%d", &latePenaltyRate, &loyaltyDiscountRate, 
                   &maxRentalDays, &loyaltyPointsPerRental);
            file.close();
        }
    }

    void save() {
        ofstream file(SETTINGS_FILE);
        if (!file) throw runtime_error("Cannot save settings to file");
        file << latePenaltyRate << "," << loyaltyDiscountRate << "," 
             << maxRentalDays << "," << loyaltyPointsPerRental << endl;
        file.close();
    }

    float getLatePenaltyRate() const { return latePenaltyRate; }
    float getLoyaltyDiscountRate() const { return loyaltyDiscountRate; }
    int getMaxRentalDays() const { return maxRentalDays; }
    int getLoyaltyPointsPerRental() const { return loyaltyPointsPerRental; }

    void updateSettings() {
        cout << "Enter new Late Penalty Rate (" << latePenaltyRate << "): ";
        float newRate;
        if (cin >> newRate && newRate >= 0) latePenaltyRate = newRate;
        clearInputBuffer();

        cout << "Enter new Loyalty Discount Rate (" << loyaltyDiscountRate << "): ";
        if (cin >> newRate && newRate >= 0 && newRate <= 1) loyaltyDiscountRate = newRate;
        clearInputBuffer();

        cout << "Enter new Max Rental Days (" << maxRentalDays << "): ";
        int newDays;
        if (cin >> newDays && newDays > 0) maxRentalDays = newDays;
        clearInputBuffer();

        cout << "Enter new Loyalty Points per Rental (" << loyaltyPointsPerRental << "): ";
        int newPoints;
        if (cin >> newPoints && newPoints >= 0) loyaltyPointsPerRental = newPoints;
        clearInputBuffer();

        save();
        cout << "Settings updated successfully!\n";
    }

private:
    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
};

// Base class for entities
class Entity {
protected:
    string id;

public:
    Entity(const string& id) : id(id) {}
    virtual ~Entity() = default;
    string getId() const { return id; }
    void setId(const string& newId) { id = newId; }
    virtual void display() const = 0;
    virtual string toString() const = 0;
};

// Customer Profile structure
struct CustomerProfile : public Entity {
    string fullName, email, phone, address;
    DateManager::Date registrationDate;
    int loyaltyPoints;

    CustomerProfile(const string& id = "", const string& name = "", 
                   const string& email = "", const string& phone = "", 
                   const string& address = "", const DateManager::Date& regDate = DateManager::getCurrentDate(),
                   int points = 0)
        : Entity(id), fullName(name), email(email), phone(phone), address(address), 
          registrationDate(regDate), loyaltyPoints(points) {}

    void display() const override {
        cout << left << setw(15) << getId() << setw(20) << fullName 
             << setw(25) << email << setw(15) << phone 
             << setw(30) << address << setw(12) << registrationDate.toString() 
             << setw(10) << loyaltyPoints << endl;
    }

    string toString() const override {
        return getId() + "," + fullName + "," + email + "," + phone + "," + address + "," + 
               registrationDate.toString() + "," + to_string(loyaltyPoints);
    }

    static CustomerProfile fromString(const string& data) {
        stringstream ss(data);
        string id, name, email, phone, address, dateStr, pointsStr;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, email, ',');
        getline(ss, phone, ',');
        getline(ss, address, ',');
        getline(ss, dateStr, ',');
        getline(ss, pointsStr);
        int dd, mm, yy;
        sscanf(dateStr.c_str(), "%d/%d/%d", &dd, &mm, &yy);
        return CustomerProfile(id, name, email, phone, address, {dd, mm, yy}, stoi(pointsStr));
    }
};

// Car structure
struct Car : public Entity {
    string brand, model, category, fuelType;
    float dailyRate;
    bool isAvailable;
    int year;
    int mileage;

    Car(const string& id = "", const string& brand = "", const string& model = "", 
        float rate = 0.0, bool available = true, const string& category = "", 
        int year = 0, const string& fuelType = "", int mileage = 0)
        : Entity(id), brand(brand), model(model), dailyRate(rate), 
          isAvailable(available), category(category), year(year), fuelType(fuelType), mileage(mileage) {}

    void display() const override {
        cout << left << setw(10) << getId() << setw(15) << brand << setw(15) << model
             << setw(12) << fixed << setprecision(2) << dailyRate 
             << setw(10) << category << setw(12) << (isAvailable ? "Yes" : "No")
             << setw(8) << year << setw(10) << fuelType << setw(10) << mileage << endl;
    }

    string toString() const override {
        ostringstream oss;
        oss << getId() << "," << brand << "," << model << "," << fixed << setprecision(2) << dailyRate << ","
            << (isAvailable ? "Yes" : "No") << "," << category << "," << year << "," << fuelType << "," << mileage;
        return oss.str();
    }

    static Car fromString(const string& data) {
        stringstream ss(data);
        string id, brand, model, category, fuelType, availableStr;
        float rate;
        int year, mileage;
        char comma;

        getline(ss, id, ',');
        getline(ss, brand, ',');
        getline(ss, model, ',');
        ss >> rate >> comma;
        getline(ss, availableStr, ',');
        getline(ss, category, ',');
        ss >> year >> comma >> mileage >> comma;
        getline(ss, fuelType);
        bool available = (availableStr == "Yes");
        return Car(id, brand, model, rate, available, category, year, fuelType, mileage);
    }
};

// Maintenance Record structure
struct MaintenanceRecord : public Entity {
    string carID;
    DateManager::Date maintenanceDate;
    string description;
    float cost;
    string status;

    MaintenanceRecord(const string& id = "", const string& carID = "", 
                     const DateManager::Date& date = {0, 0, 0}, const string& desc = "", 
                     float cost = 0.0, const string& status = "")
        : Entity(id), carID(carID), maintenanceDate(date), description(desc), 
          cost(cost), status(status) {}

    void display() const override {
        cout << left << setw(15) << getId() << setw(10) << carID 
             << setw(15) << maintenanceDate.toString() 
             << setw(20) << description << setw(10) << fixed << setprecision(2) << cost
             << setw(12) << status << endl;
    }

    string toString() const override {
        ostringstream oss;
        oss << getId() << "," << carID << "," << maintenanceDate.toString() << ","
            << description << "," << fixed << setprecision(2) << cost << "," << status;
        return oss.str();
    }

    static MaintenanceRecord fromString(const string& data) {
        stringstream ss(data);
        string id, carID, dateStr, desc, status;
        float cost;
        char comma;

        getline(ss, id, ',');
        getline(ss, carID, ',');
        getline(ss, dateStr, ',');
        getline(ss, desc, ',');
        ss >> cost >> comma;
        getline(ss, status);
        int dd, mm, yy;
        sscanf(dateStr.c_str(), "%d/%d/%d", &dd, &mm, &yy);
        return MaintenanceRecord(id, carID, {dd, mm, yy}, desc, cost, status);
    }
};

// Rental structure
struct Rental : public Entity {
    string carID, customerID;
    DateManager::Date startDate, endDate, returnDate;
    float totalCost, latePenalty;
    string status;
    int mileageUsed;

    Rental(const string& id = "", const string& carID = "", 
           const string& customerID = "", const DateManager::Date& start = {0, 0, 0}, 
           const DateManager::Date& end = {0, 0, 0}, float cost = 0.0, 
           const string& status = "", int mileage = 0, float penalty = 0.0, 
           const DateManager::Date& retDate = {0, 0, 0})
        : Entity(id), carID(carID), customerID(customerID), startDate(start), 
          endDate(end), totalCost(cost), status(status), mileageUsed(mileage), 
          latePenalty(penalty), returnDate(retDate) {}

    void display() const override {
        cout << left << setw(10) << getId() << setw(10) << carID << setw(15) << customerID
             << setw(12) << startDate.toString() << setw(12) << endDate.toString()
             << setw(12) << (returnDate.dd == 0 ? "N/A" : returnDate.toString())
             << setw(12) << fixed << setprecision(2) << totalCost 
             << setw(10) << fixed << setprecision(2) << latePenalty
             << setw(10) << status << setw(10) << mileageUsed << endl;
    }

    string toString() const override {
        ostringstream oss;
        oss << getId() << "," << carID << "," << customerID << "," << startDate.toString() << ","
            << endDate.toString() << "," << returnDate.toString() << "," 
            << fixed << setprecision(2) << totalCost << "," << fixed << setprecision(2) << latePenalty << ","
            << status << "," << mileageUsed;
        return oss.str();
    }

    static Rental fromString(const string& data) {
        stringstream ss(data);
        string id, carID, customerID, startDateStr, endDateStr, returnDateStr, status;
        float cost, penalty;
        int mileage;
        char comma;

        getline(ss, id, ',');
        getline(ss, carID, ',');
        getline(ss, customerID, ',');
        getline(ss, startDateStr, ',');
        getline(ss, endDateStr, ',');
        getline(ss, returnDateStr, ',');
        ss >> cost >> comma >> penalty >> comma;
        getline(ss, status, ',');
        ss >> mileage;
        int sdd, smm, syy, edd, emm, eyy, rdd, rmm, ryy;
        sscanf(startDateStr.c_str(), "%d/%d/%d", &sdd, &smm, &syy);
        sscanf(endDateStr.c_str(), "%d/%d/%d", &edd, &emm, &eyy);
        sscanf(returnDateStr.c_str(), "%d/%d/%d", &rdd, &rmm, &ryy);
        return Rental(id, carID, customerID, {sdd, smm, syy}, {edd, emm, eyy}, cost, status, mileage, penalty, {rdd, rmm, ryy});
    }
};

// User structure
struct User {
    string username, password;
    bool isAdmin;
    DateManager::Date lastLogin;

    User(const string& uname = "", const string& pwd = "", bool admin = false, 
         const DateManager::Date& login = DateManager::getCurrentDate())
        : username(uname), password(pwd), isAdmin(admin), lastLogin(login) {}

    string toString() const {
        return username + "," + password + "," + (isAdmin ? "1" : "0") + "," + lastLogin.toString();
    }

    static User fromString(const string& data) {
        stringstream ss(data);
        string username, password, adminStr, dateStr;
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, adminStr, ',');
        getline(ss, dateStr);
        bool isAdmin = (adminStr == "1");
        int dd, mm, yy;
        sscanf(dateStr.c_str(), "%d/%d/%d", &dd, &mm, &yy);
        return User(username, password, isAdmin, {dd, mm, yy});
    }
};

// Transaction Logger
class TransactionLogger {
private:
    static void log(const string& message) {
        ofstream file(TRANSACTION_LOG, ios::app);
        if (file) {
            auto now = DateManager::getCurrentDate();
            file << "[" << now.toString() << "] " << message << endl;
            file.close();
        }
    }

public:
    static void logUserAction(const string& username, const string& action) {
        log("User: " + username + " | Action: " + action);
    }

    static void logSystemEvent(const string& event) {
        log("System Event: " + event);
    }
};

// Template class for managing collections
template<typename T>
class Repository {
private:
    vector<shared_ptr<T>> items;
    string filename;

public:
    Repository(const string& fname) : filename(fname) {}
    
    void add(const shared_ptr<T>& item) {
        items.push_back(item);
        save();
    }

    void remove(const string& id) {
        items.erase(remove_if(items.begin(), items.end(),
            [&](const shared_ptr<T>& item) { return item->getId() == id; }), items.end());
        save();
    }

    shared_ptr<T> find(const string& id) const {
        auto it = find_if(items.begin(), items.end(),
            [&](const shared_ptr<T>& item) { return item->getId() == id; });
        return (it != items.end()) ? *it : nullptr;
    }

    const vector<shared_ptr<T>>& getAll() const { return items; }

    void load() {
        ifstream file(filename);
        if (!file) return;

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                items.push_back(make_shared<T>(T::fromString(line)));
            }
        }
        file.close();
    }

    void save() {
        ofstream file(filename);
        if (!file) throw runtime_error("Cannot save to file: " + filename);
        
        for (const auto& item : items) {
            file << item->toString() << endl;
        }
        file.close();
    }
};

class CarRentalSystem {
private:
    Repository<User> userRepo;
    Repository<Car> carRepo;
    Repository<Rental> rentalRepo;
    Repository<CustomerProfile> customerRepo;
    Repository<MaintenanceRecord> maintenanceRepo;
    SettingsManager settings;
    map<string, function<void()>> adminMenuOptions;
    map<string, function<void()>> customerMenuOptions;
    string currentUser;

public:
    CarRentalSystem()
        : userRepo(USER_FILE), carRepo(CAR_FILE), rentalRepo(RENTAL_FILE),
          customerRepo(CUSTOMER_FILE), maintenanceRepo(MAINTENANCE_FILE) {
        initializeDefaultData();
        initializeMenuOptions();
        settings.load();
        TransactionLogger::logSystemEvent("System initialized");
    }

    void initializeDefaultData() {
        if (userRepo.getAll().empty()) {
            userRepo.add(make_shared<User>("admin", "admin123", true));
            TransactionLogger::logSystemEvent("Default admin user created");
        }
    }

    void initializeMenuOptions() {
        adminMenuOptions = {
            {"1", [this]() { addCar(); }},
            {"2", [this]() { viewCars(); }},
            {"3", [this]() { updateCar(); }},
            {"4", [this]() { deleteCar(); }},
            {"5", [this]() { viewRentals(); }},
            {"6", [this]() { monthlyReport(); }},
            {"7", [this]() { manageCustomers(); }},
            {"8", [this]() { manageMaintenance(); }},
            {"9", [this]() { extendRental(); }},
            {"10", [this]() { returnCar(); }},
            {"11", [this]() { viewCarMaintenanceHistory(); }},
            {"12", [this]() { updateSystemSettings(); }},
            {"13", [this]() { generateCustomerLoyaltyReport(); }},
            {"14", [this]() { mainMenu(); }}
        };

        customerMenuOptions = {
            {"1", [this]() { viewAvailableCars(); }},
            {"2", [this]() { rentCar(); }},
            {"3", [this]() { viewMyRentals(); }},
            {"4", [this]() { cancelRental(); }},
            {"5", [this]() { updateCustomerProfile(); }},
            {"6", [this]() { viewRentalHistory(); }},
            {"7", [this]() { viewLoyaltyPoints(); }},
            {"8", [this]() { mainMenu(); }}
        };
    }

    void clearScreen() {
        cout << string(50, '\n');
    }

    void pause() {
        cout << "Press Enter to continue...";
        cin.get();
    }

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string sanitizeInput(const string& input) {
        string sanitized = input;
        sanitized.erase(remove_if(sanitized.begin(), sanitized.end(),
            [](char c) { return !isprint(c); }), sanitized.end());
        return sanitized;
    }

    bool isValidID(const string& id, const string& prefix) const {
        return id.length() >= 4 && id.substr(0, 2) == prefix;
    }

    bool isValidEmail(const string& email) const {
        const regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return regex_match(email, pattern);
    }

    bool isValidPhone(const string& phone) const {
        return phone.length() == 10 && all_of(phone.begin(), phone.end(), ::isdigit);
    }

    void inputPassword(string& password) {
        password.clear();
        char ch;
        cout << "Enter password: ";
        while (cin.get(ch) && ch != '\n') {
            if (ch == '\b' && !password.empty()) {
                password.pop_back();
                cout << "\b \b";
            } else if (password.length() < 24) {
                password.push_back(ch);
                cout << '*';
            }
        }
        cout << endl;
    }

    void mainMenu() {
        userRepo.load();
        carRepo.load();
        rentalRepo.load();
        customerRepo.load();
        maintenanceRepo.load();

        while (true) {
            clearScreen();
            cout << "\n\t\t=== Car Rental System ===\n\n";
            cout << "1) Login\n2) Register\n3) Exit\n";
            cout << "Enter choice: ";
            
            string choice;
            getline(cin, choice);
            choice = sanitizeInput(choice);
            try {
                if (choice == "1") login();
                else if (choice == "2") registerUser();
                else if (choice == "3") {
                    TransactionLogger::logSystemEvent("System shutdown");
                    cout << "Exiting system...\n";
                    pause();
                    return;
                } else {
                    throw invalid_argument("Invalid choice!");
                }
            } catch (const invalid_argument& e) {
                cout << e.what() << " Try again.\n";
                pause();
            }
        }
    }

    void login() {
        string username, password;
        cout << "Enter username: ";
        getline(cin, username);
        username = sanitizeInput(username);
        inputPassword(password);

        auto users = userRepo.getAll();
        auto user = find_if(users.begin(), users.end(),
            [&](const shared_ptr<User>& u) { return u->username == username && u->password == password; });

        if (user != users.end()) {
            currentUser = username;
            (*user)->lastLogin = DateManager::getCurrentDate();
            userRepo.save();
            TransactionLogger::logUserAction(username, "Logged in");
            cout << "Login successful! Last login: " << (*user)->lastLogin.toString() << "\n";
            pause();
            clearScreen();
            (*user)->isAdmin ? adminMenu() : customerMenu();
        } else {
            cout << "Invalid credentials!\n";
            pause();
            clearScreen();
            mainMenu();
        }
    }

    void registerUser() {
        User user;
        cout << "Enter username: ";
        getline(cin, user.username);
        user.username = sanitizeInput(user.username);
        
        auto users = userRepo.getAll();
        if (any_of(users.begin(), users.end(),
            [&](const shared_ptr<User>& u) { return u->username == user.username; })) {
            cout << "Username already exists!\n";
            pause();
            clearScreen();
            registerUser();
            return;
        }

        inputPassword(user.password);
        cout << "Is this an admin account? (y/n): ";
        char adminChoice;
        cin >> adminChoice;
        clearInputBuffer();
        user.isAdmin = (tolower(adminChoice) == 'y');
        user.lastLogin = DateManager::getCurrentDate();

        userRepo.add(make_shared<User>(user));
        TransactionLogger::logUserAction(user.username, "Registered new user");
        cout << "User registered successfully!\n";
        pause();
        clearScreen();
        mainMenu();
    }

    void adminMenu() {
        while (true) {
            clearScreen();
            cout << "\n\t\t=== Admin Menu ===\n\n";
            cout << "1) Add Car\n2) View Cars\n3) Update Car\n4) Delete Car\n";
            cout << "5) View Rentals\n6) Monthly Report\n7) Manage Customers\n";
            cout << "8) Manage Maintenance\n9) Extend Rental\n10) Return Car\n";
            cout << "11) View Car Maintenance History\n12) Update System Settings\n";
            cout << "13) Customer Loyalty Report\n14) Logout\n";
            cout << "Enter choice: ";

            string choice;
            getline(cin, choice);
            choice = sanitizeInput(choice);
            try {
                auto it = adminMenuOptions.find(choice);
                if (it != adminMenuOptions.end()) {
                    it->second();
                    TransactionLogger::logUserAction(currentUser, "Admin menu option " + choice);
                } else {
                    throw invalid_argument("Invalid choice!");
                }
            } catch (const invalid_argument& e) {
                cout << e.what() << " Try again.\n";
                pause();
            }
        }
    }

    void customerMenu() {
        while (true) {
            clearScreen();
            cout << "\n\t\t=== Customer Menu ===\n\n";
            cout << "1) View Available Cars\n2) Rent Car\n3) View My Rentals\n";
            cout << "4) Cancel Rental\n5) Update Profile\n6) View Rental History\n";
            cout << "7) View Loyalty Points\n8) Logout\n";
            cout << "Enter choice: ";

            string choice;
            getline(cin, choice);
            choice = sanitizeInput(choice);
            try {
                auto it = customerMenuOptions.find(choice);
                if (it != customerMenuOptions.end()) {
                    it->second();
                    TransactionLogger::logUserAction(currentUser, "Customer menu option " + choice);
                } else {
                    throw invalid_argument("Invalid choice!");
                }
            } catch (const invalid_argument& e) {
                cout << e.what() << " Try again.\n";
                pause();
            }
        }
    }

    void addCar() {
        Car car;
        string id;
        cout << "Enter Car ID (start with CR): ";
        getline(cin, id);
        id = sanitizeInput(id);
        if (!isValidID(id, "CR")) {
            cout << "Invalid Car ID! Must start with 'CR' and be at least 4 characters.\n";
            pause();
            return;
        }
        if (carRepo.find(id)) {
            cout << "Car ID already exists!\n";
            pause();
            return;
        }
        car.setId(id);

        cout << "Enter Brand: ";
        getline(cin, car.brand);
        car.brand = sanitizeInput(car.brand);
        cout << "Enter Model: ";
        getline(cin, car.model);
        car.model = sanitizeInput(car.model);
        cout << "Enter Daily Rate: ";
        while (!(cin >> car.dailyRate) || car.dailyRate < 0) {
            cout << "Invalid rate! Enter a positive number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        cout << "Enter Category (Economy/Standard/Luxury): ";
        getline(cin, car.category);
        car.category = sanitizeInput(car.category);
        cout << "Enter Year: ";
        while (!(cin >> car.year) || car.year < 1900 || car.year > 2025) {
            cout << "Invalid year! Enter a year between 1900 and 2025: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        cout << "Enter Fuel Type (Petrol/Diesel/Electric): ";
        getline(cin, car.fuelType);
        car.fuelType = sanitizeInput(car.fuelType);
        cout << "Enter Initial Mileage: ";
        while (!(cin >> car.mileage) || car.mileage < 0) {
            cout << "Invalid mileage! Enter a non-negative number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        car.isAvailable = true;

        carRepo.add(make_shared<Car>(car));
        TransactionLogger::logUserAction(currentUser, "Added car " + car.getId());
        cout << "Car added successfully!\n";
        pause();
    }

    void viewCars() {
        clearScreen();
        cout << "=== Car Inventory ===\n\n";
        auto cars = carRepo.getAll();
        if (cars.empty()) {
            cout << "No cars available.\n";
        } else {
            cout << left << setw(10) << "Car ID" << setw(15) << "Brand" << setw(15) << "Model"
                 << setw(12) << "Daily Rate" << setw(10) << "Category" << setw(12) << "Available"
                 << setw(8) << "Year" << setw(10) << "Fuel Type" << setw(10) << "Mileage" << endl;
            cout << string(102, '-') << endl;
            for (const auto& car : cars) {
                car->display();
            }
        }
        pause();
    }

    void updateCar() {
        string carID;
        cout << "Enter Car ID to update: ";
        getline(cin, carID);
        carID = sanitizeInput(carID);
        if (!isValidID(carID, "CR")) {
            cout << "Invalid Car ID!\n";
            pause();
            return;
        }

        auto car = carRepo.find(carID);
        if (!car) {
            cout << "Car not found!\n";
            pause();
            return;
        }

        string input;
        cout << "Enter new Brand (" << car->brand << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) car->brand = input;

        cout << "Enter new Model (" << car->model << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) car->model = input;

        cout << "Enter new Daily Rate (" << car->dailyRate << "): ";
        float rate;
        if (cin >> rate && rate >= 0) {
            car->dailyRate = rate;
        }
        clearInputBuffer();

        cout << "Enter new Category (" << car->category << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) car->category = input;

        cout << "Enter new Year (" << car->year << "): ";
        int year;
        if (cin >> year && year >= 1900 && year <= 2025) {
            car->year = year;
        }
        clearInputBuffer();

        cout << "Enter new Fuel Type (" << car->fuelType << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) car->fuelType = input;

        cout << "Enter new Mileage (" << car->mileage << "): ";
        int mileage;
        if (cin >> mileage && mileage >= 0) {
            car->mileage = mileage;
        }
        clearInputBuffer();

        carRepo.save();
        TransactionLogger::logUserAction(currentUser, "Updated car " + carID);
        cout << "Car updated successfully!\n";
        pause();
    }

    void deleteCar() {
        string carID;
        cout << "Enter Car ID to delete: ";
        getline(cin, carID);
        carID = sanitizeInput(carID);
        if (!isValidID(carID, "CR")) {
            cout << "Invalid Car ID!\n";
            pause();
            return;
        }

        if (!carRepo.find(carID)) {
            cout << "Car not found!\n";
            pause();
            return;
        }

        carRepo.remove(carID);
        TransactionLogger::logUserAction(currentUser, "Deleted car " + carID);
        cout << "Car deleted successfully!\n";
        pause();
    }

    void viewAvailableCars() {
        clearScreen();
        cout << "=== Available Cars ===\n\n";
        auto cars = carRepo.getAll();
        bool found = false;
        cout << left << setw(10) << "Car ID" << setw(15) << "Brand" << setw(15) << "Model"
             << setw(12) << "Daily Rate" << setw(10) << "Category" << setw(8) << "Year"
             << setw(10) << "Fuel Type" << setw(10) << "Mileage" << endl;
        cout << string(90, '-') << endl;
        for (const auto& car : cars) {
            if (car->isAvailable) {
                car->display();
                found = true;
            }
        }
        if (!found) {
            cout << "No cars available for rent.\n";
        }
        pause();
    }

    void rentCar() {
        Rental rental;
        string id;
        cout << "Enter Rental ID (start with RN): ";
        getline(cin, id);
        id = sanitizeInput(id);
        if (!isValidID(id, "RN")) {
            cout << "Invalid Rental ID! Must start with 'RN' and be at least 4 characters.\n";
            pause();
            return;
        }
        if (rentalRepo.find(id)) {
            cout << "Rental ID already exists!\n";
            pause();
            return;
        }
        rental.setId(id);

        cout << "Enter Customer ID (start with CU): ";
        getline(cin, rental.customerID);
        rental.customerID = sanitizeInput(rental.customerID);
        if (!isValidID(rental.customerID, "CU")) {
            cout << "Invalid Customer ID! Must start with 'CU' and be at least 4 characters.\n";
            pause();
            return;
        }
        auto customer = customerRepo.find(rental.customerID);
        if (!customer) {
            cout << "Customer not found! Please create a customer profile first.\n";
            pause();
            return;
        }

        cout << "Enter Car ID (start with CR): ";
        getline(cin, rental.carID);
        rental.carID = sanitizeInput(rental.carID);
        auto car = carRepo.find(rental.carID);
        if (!car || !car->isAvailable) {
            cout << "Car not found or unavailable!\n";
            pause();
            return;
        }

        cout << "Enter Start Date (dd mm yyyy): ";
        while (!(cin >> rental.startDate.dd >> rental.startDate.mm >> rental.startDate.yy) ||
               !DateManager::isValidDate(rental.startDate.dd, rental.startDate.mm, rental.startDate.yy)) {
            cout << "Invalid start date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        cout << "Enter End Date (dd mm yyyy): ";
        while (!(cin >> rental.endDate.dd >> rental.endDate.mm >> rental.endDate.yy) ||
               !DateManager::isValidDate(rental.endDate.dd, rental.endDate.mm, rental.endDate.yy)) {
            cout << "Invalid end date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        int days = DateManager::calculateDays(rental.startDate, rental.endDate);
        if (days <= 0) {
            cout << "Invalid rental period! End date must be after start date.\n";
            pause();
            return;
        }
        if (days > settings.getMaxRentalDays()) {
            cout << "Rental period exceeds maximum allowed days (" << settings.getMaxRentalDays() << ")!\n";
            pause();
            return;
        }

        cout << "Enter Estimated Mileage Used: ";
        while (!(cin >> rental.mileageUsed) || rental.mileageUsed < 0) {
            cout << "Invalid mileage! Enter a non-negative number: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        float discount = customer->loyaltyPoints >= 1000 ? settings.getLoyaltyDiscountRate() : 0.0f;
        rental.totalCost = car->dailyRate * days * (1.0f - discount);
        rental.status = "Active";
        car->isAvailable = false;
        car->mileage += rental.mileageUsed;
        customer->loyaltyPoints += settings.getLoyaltyPointsPerRental();

        rentalRepo.add(make_shared<Rental>(rental));
        carRepo.save();
        customerRepo.save();
        TransactionLogger::logUserAction(currentUser, "Rented car " + rental.carID + " for rental " + rental.getId());
        cout << "Car rented successfully! Total Cost: $" << fixed << setprecision(2) << rental.totalCost;
        if (discount > 0) {
            cout << " (Loyalty Discount Applied: " << (discount * 100) << "%)";
        }
        cout << "\nRental Duration: " << days << " days\n";
        cout << "Loyalty Points Earned: " << settings.getLoyaltyPointsPerRental() << endl;
        pause();
    }

    void viewMyRentals() {
        string customerID;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, customerID);
        customerID = sanitizeInput(customerID);
        if (!isValidID(customerID, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }

        clearScreen();
        cout << "=== Your Rentals ===\n\n";
        auto rentals = rentalRepo.getAll();
        bool found = false;
        cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" << setw(15) << "Customer ID"
             << setw(12) << "Start Date" << setw(12) << "End Date" << setw(12) << "Return Date"
             << setw(12) << "Total Cost" << setw(10) << "Penalty" << setw(10) << "Status" 
             << setw(10) << "Mileage" << endl;
        cout << string(101, '-') << endl;
        for (const auto& rental : rentals) {
            if (rental->customerID == customerID) {
                rental->display();
                found = true;
            }
        }
        if (!found) {
            cout << "No rentals found.\n";
        }
        pause();
    }

    void cancelRental() {
        string rentalID;
        cout << "Enter Rental ID to cancel: ";
        getline(cin, rentalID);
        rentalID = sanitizeInput(rentalID);
        if (!isValidID(rentalID, "RN")) {
            cout << "Invalid Rental ID!\n";
            pause();
            return;
        }

        auto rental = rentalRepo.find(rentalID);
        if (!rental || rental->status != "Active") {
            cout << "Rental not found or already cancelled/completed!\n";
            pause();
            return;
        }

        rental->status = "Cancelled";
        auto car = carRepo.find(rental->carID);
        if (car) {
            car->isAvailable = true;
        }

        rentalRepo.save();
        carRepo.save();
        TransactionLogger::logUserAction(currentUser, "Cancelled rental " + rentalID);
        cout << "Rental cancelled successfully!\n";
        pause();
    }

    void extendRental() {
        string rentalID;
        cout << "Enter Rental ID to extend: ";
        getline(cin, rentalID);
        rentalID = sanitizeInput(rentalID);
        if (!isValidID(rentalID, "RN")) {
            cout << "Invalid Rental ID!\n";
            pause();
            return;
        }

        auto rental = rentalRepo.find(rentalID);
        if (!rental || rental->status != "Active") {
            cout << "Rental not found or not active!\n";
            pause();
            return;
        }

        cout << "Current End Date: " << rental->endDate.toString() << endl;
        cout << "Enter New End Date (dd mm yyyy): ";
        DateManager::Date newEndDate;
        while (!(cin >> newEndDate.dd >> newEndDate.mm >> newEndDate.yy) ||
               !DateManager::isValidDate(newEndDate.dd, newEndDate.mm, newEndDate.yy)) {
            cout << "Invalid end date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        int additionalDays = DateManager::calculateDays(rental->endDate, newEndDate);
        if (additionalDays <= 0) {
            cout << "New end date must be after current end date!\n";
            pause();
            return;
        }

        int totalDays = DateManager::calculateDays(rental->startDate, newEndDate);
        if (totalDays > settings.getMaxRentalDays()) {
            cout << "Extended rental period exceeds maximum allowed days (" << settings.getMaxRentalDays() << ")!\n";
            pause();
            return;
        }

        auto car = carRepo.find(rental->carID);
        if (!car) {
            cout << "Car not found!\n";
            pause();
            return;
        }

        auto customer = customerRepo.find(rental->customerID);
        float discount = customer && customer->loyaltyPoints >= 1000 ? settings.getLoyaltyDiscountRate() : 0.0f;
        float additionalCost = car->dailyRate * additionalDays * (1.0f - discount);
        rental->totalCost += additionalCost;
        rental->endDate = newEndDate;

        rentalRepo.save();
        TransactionLogger::logUserAction(currentUser, "Extended rental " + rentalID + " by " + to_string(additionalDays) + " days");
        cout << "Rental extended successfully! Additional Cost: $" << fixed << setprecision(2) << additionalCost << endl;
        pause();
    }

    void returnCar() {
        string rentalID;
        cout << "Enter Rental ID to return: ";
        getline(cin, rentalID);
        rentalID = sanitizeInput(rentalID);
        if (!isValidID(rentalID, "RN")) {
            cout << "Invalid Rental ID!\n";
            pause();
            return;
        }

        auto rental = rentalRepo.find(rentalID);
        if (!rental || rental->status != "Active") {
            cout << "Rental not found or not active!\n";
            pause();
            return;
        }

        auto car = carRepo.find(rental->carID);
        if (!car) {
            cout << "Car not found!\n";
            pause();
            return;
        }

        cout << "Enter Return Date (dd mm yyyy): ";
        DateManager::Date returnDate;
        while (!(cin >> returnDate.dd >> returnDate.mm >> returnDate.yy) ||
               !DateManager::isValidDate(returnDate.dd, returnDate.mm, returnDate.yy)) {
            cout << "Invalid return date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        if (returnDate < rental->endDate) {
            cout << "Return date cannot be before end date!\n";
            pause();
            return;
        }

        int lateDays = DateManager::calculateDays(rental->endDate, returnDate);
        if (lateDays > 0) {
            rental->latePenalty = car->dailyRate * lateDays * settings.getLatePenaltyRate();
            TransactionLogger::logUserAction(currentUser, "Applied late penalty of $" + 
                                            to_string(rental->latePenalty) + " for rental " + rentalID);
        }

        cout << "Enter Actual Mileage Used: ";
        int actualMileage;
        while (!(cin >> actualMileage) || actualMileage < 0) {
            cout << "Invalid mileage! Enter a non-negative number: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        car->mileage += (actualMileage - rental->mileageUsed);
        rental->mileageUsed = actualMileage;
        rental->returnDate = returnDate;
        rental->status = "Completed";
        car->isAvailable = true;

        rentalRepo.save();
        carRepo.save();
        TransactionLogger::logUserAction(currentUser, "Returned car for rental " + rentalID);
        cout << "Car returned successfully! ";
        if (rental->latePenalty > 0) {
            cout << "Late Penalty: $" << fixed << setprecision(2) << rental->latePenalty;
        }
        cout << endl;
        pause();
    }

    void manageCustomers() {
        while (true) {
            clearScreen();
            cout << "\n\t\t=== Customer Management ===\n\n";
            cout << "1) Add Customer\n2) View Customers\n3) Update Customer\n";
            cout << "4) Delete Customer\n5) Back\n";
            cout << "Enter choice: ";

            string choice;
            getline(cin, choice);
            choice = sanitizeInput(choice);
            try {
                if (choice == "1") addCustomer();
                else if (choice == "2") viewCustomers();
                else if (choice == "3") updateCustomerProfile(true);
                else if (choice == "4") deleteCustomer();
                else if (choice == "5") return;
                else throw invalid_argument("Invalid choice!");
            } catch (const invalid_argument& e) {
                cout << e.what() << " Try again.\n";
                pause();
            }
        }
    }

    void addCustomer() {
        CustomerProfile customer;
        string id;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, id);
        id = sanitizeInput(id);
        if (!isValidID(id, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }
        if (customerRepo.find(id)) {
            cout << "Customer ID already exists!\n";
            pause();
            return;
        }
        customer.setId(id);

        cout << "Enter Full Name: ";
        getline(cin, customer.fullName);
        customer.fullName = sanitizeInput(customer.fullName);
        cout << "Enter Email: ";
        getline(cin, customer.email);
        customer.email = sanitizeInput(customer.email);
        if (!isValidEmail(customer.email)) {
            cout << "Invalid email format!\n";
            pause();
            return;
        }
        cout << "Enter Phone (10 digits): ";
        getline(cin, customer.phone);
        customer.phone = sanitizeInput(customer.phone);
        if (!isValidPhone(customer.phone)) {
            cout << "Invalid phone number! Must be 10 digits.\n";
            pause();
            return;
        }
        cout << "Enter Address: ";
        getline(cin, customer.address);
        customer.address = sanitizeInput(customer.address);

        customerRepo.add(make_shared<CustomerProfile>(customer));
        TransactionLogger::logUserAction(currentUser, "Added customer " + customer.getId());
        cout << "Customer added successfully!\n";
        pause();
    }

    void viewCustomers() {
        clearScreen();
        cout << "=== Customer List ===\n\n";
        auto customers = customerRepo.getAll();
        if (customers.empty()) {
            cout << "No customers available.\n";
        } else {
            cout << left << setw(15) << "Customer ID" << setw(20) << "Full Name" 
                 << setw(25) << "Email" << setw(15) << "Phone" 
                 << setw(30) << "Address" << setw(12) << "Reg. Date" 
                 << setw(10) << "Loyalty" << endl;
            cout << string(127, '-') << endl;
            for (const auto& customer : customers) {
                customer->display();
            }
        }
        pause();
    }

    void updateCustomerProfile(bool isAdmin = false) {
        string customerID;
        cout << "Enter Customer ID to update: ";
        getline(cin, customerID);
        customerID = sanitizeInput(customerID);
        if (!isValidID(customerID, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }

        auto customer = customerRepo.find(customerID);
        if (!customer) {
            cout << "Customer not found!\n";
            pause();
            return;
        }

        string input;
        cout << "Enter new Full Name (" << customer->fullName << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) customer->fullName = input;

        cout << "Enter new Email (" << customer->email << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty() && isValidEmail(input)) {
            customer->email = input;
        } else if (!input.empty()) {
            cout << "Invalid email format! Keeping original.\n";
        }

        cout << "Enter new Phone (" << customer->phone << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty() && isValidPhone(input)) {
            customer->phone = input;
        } else if (!input.empty()) {
            cout << "Invalid phone number! Keeping original.\n";
        }

        cout << "Enter new Address (" << customer->address << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) customer->address = input;

        if (isAdmin) {
            cout << "Enter new Loyalty Points (" << customer->loyaltyPoints << "): ";
            int points;
            if (cin >> points && points >= 0) {
                customer->loyaltyPoints = points;
            }
            clearInputBuffer();
        }

        customerRepo.save();
        TransactionLogger::logUserAction(currentUser, "Updated customer " + customerID);
        cout << "Customer profile updated successfully!\n";
        pause();
    }

    void deleteCustomer() {
        string customerID;
        cout << "Enter Customer ID to delete: ";
        getline(cin, customerID);
        customerID = sanitizeInput(customerID);
        if (!isValidID(customerID, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }

        if (!customerRepo.find(customerID)) {
            cout << "Customer not found!\n";
            pause();
            return;
        }

        customerRepo.remove(customerID);
        TransactionLogger::logUserAction(currentUser, "Deleted customer " + customerID);
        cout << "Customer deleted successfully!\n";
        pause();
    }

    void manageMaintenance() {
        while (true) {
            clearScreen();
            cout << "\n\t\t=== Maintenance Management ===\n\n";
            cout << "1) Schedule Maintenance\n2) View Maintenance Records\n";
            cout << "3) Update Maintenance\n4) Delete Maintenance\n5) Back\n";
            cout << "Enter choice: ";

            string choice;
            getline(cin, choice);
            choice = sanitizeInput(choice);
            try {
                if (choice == "1") scheduleMaintenance();
                else if (choice == "2") viewMaintenanceRecords();
                else if (choice == "3") updateMaintenance();
                else if (choice == "4") deleteMaintenance();
                else if (choice == "5") return;
                else throw invalid_argument("Invalid choice!");
            } catch (const invalid_argument& e) {
                cout << e.what() << " Try again.\n";
                pause();
            }
        }
    }

    void scheduleMaintenance() {
        MaintenanceRecord record;
        string id;
        cout << "Enter Maintenance ID (start with MN): ";
        getline(cin, id);
        id = sanitizeInput(id);
        if (!isValidID(id, "MN")) {
            cout << "Invalid Maintenance ID!\n";
            pause();
            return;
        }
        if (maintenanceRepo.find(id)) {
            cout << "Maintenance ID already exists!\n";
            pause();
            return;
        }
        record.setId(id);

        cout << "Enter Car ID: ";
        getline(cin, record.carID);
        record.carID = sanitizeInput(record.carID);
        if (!isValidID(record.carID, "CR") || !carRepo.find(record.carID)) {
            cout << "Invalid or non-existent Car ID!\n";
            pause();
            return;
        }

        cout << "Enter Maintenance Date (dd mm yyyy): ";
        while (!(cin >> record.maintenanceDate.dd >> record.maintenanceDate.mm >> record.maintenanceDate.yy) ||
               !DateManager::isValidDate(record.maintenanceDate.dd, record.maintenanceDate.mm, record.maintenanceDate.yy)) {
            cout << "Invalid date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        cout << "Enter Description: ";
        getline(cin, record.description);
        record.description = sanitizeInput(record.description);
        cout << "Enter Cost: ";
        while (!(cin >> record.cost) || record.cost < 0) {
            cout << "Invalid cost! Enter a positive number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        record.status = "Scheduled";

        maintenanceRepo.add(make_shared<MaintenanceRecord>(record));
        TransactionLogger::logUserAction(currentUser, "Scheduled maintenance " + record.getId());
        cout << "Maintenance scheduled successfully!\n";
        pause();
    }

    void viewMaintenanceRecords() {
        clearScreen();
        cout << "=== Maintenance Records ===\n\n";
        auto records = maintenanceRepo.getAll();
        if (records.empty()) {
            cout << "No maintenance records available.\n";
        } else {
            cout << left << setw(15) << "Maintenance ID" << setw(10) << "Car ID" 
                 << setw(15) << "Date" << setw(20) << "Description" 
                 << setw(10) << "Cost" << setw(12) << "Status" << endl;
            cout << string(82, '-') << endl;
            for (const auto& record : records) {
                record->display();
            }
        }
        pause();
    }

    void updateMaintenance() {
        string maintenanceID;
        cout << "Enter Maintenance ID to update: ";
        getline(cin, maintenanceID);
        maintenanceID = sanitizeInput(maintenanceID);
        if (!isValidID(maintenanceID, "MN")) {
            cout << "Invalid Maintenance ID!\n";
            pause();
            return;
        }

        auto record = maintenanceRepo.find(maintenanceID);
        if (!record) {
            cout << "Maintenance record not found!\n";
            pause();
            return;
        }

        string input;
        cout << "Enter new Car ID (" << record->carID << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty() && isValidID(input, "CR") && carRepo.find(input)) {
            record->carID = input;
        } else if (!input.empty()) {
            cout << "Invalid or non-existent Car ID! Keeping original.\n";
        }

        cout << "Enter new Maintenance Date (" << record->maintenanceDate.toString() << "): ";
        int dd, mm, yy;
        if (cin >> dd >> mm >> yy && DateManager::isValidDate(dd, mm, yy)) {
            record->maintenanceDate = {dd, mm, yy};
        } else {
            cout << "Invalid date! Keeping original.\n";
            clearInputBuffer();
        }
        clearInputBuffer();

        cout << "Enter new Description (" << record->description << "): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (!input.empty()) record->description = input;

        cout << "Enter new Cost (" << record->cost << "): ";
        float cost;
        if (cin >> cost && cost >= 0) {
            record->cost = cost;
        }
        clearInputBuffer();

        cout << "Enter new Status (Scheduled/Completed): ";
        getline(cin, input);
        input = sanitizeInput(input);
        if (input == "Scheduled" || input == "Completed") {
            record->status = input;
        }

        maintenanceRepo.save();
        TransactionLogger::logUserAction(currentUser, "Updated maintenance " + maintenanceID);
        cout << "Maintenance record updated successfully!\n";
        pause();
    }

    void deleteMaintenance() {
        string maintenanceID;
        cout << "Enter Maintenance ID to delete: ";
        getline(cin, maintenanceID);
        maintenanceID = sanitizeInput(maintenanceID);
        if (!isValidID(maintenanceID, "MN")) {
            cout << "Invalid Maintenance ID!\n";
            pause();
            return;
        }

        if (!maintenanceRepo.find(maintenanceID)) {
            cout << "Maintenance record not found!\n";
            pause();
            return;
        }

        maintenanceRepo.remove(maintenanceID);
        TransactionLogger::logUserAction(currentUser, "Deleted maintenance " + maintenanceID);
        cout << "Maintenance record deleted successfully!\n";
        pause();
    }

    void viewCarMaintenanceHistory() {
        string carID;
        cout << "Enter Car ID to view maintenance history: ";
        getline(cin, carID);
        carID = sanitizeInput(carID);
        if (!isValidID(carID, "CR")) {
            cout << "Invalid Car ID!\n";
            pause();
            return;
        }

        if (!carRepo.find(carID)) {
            cout << "Car not found!\n";
            pause();
            return;
        }

        clearScreen();
        cout << "=== Maintenance History for Car " << carID << " ===\n\n";
        auto records = maintenanceRepo.getAll();
        bool found = false;
        cout << left << setw(15) << "Maintenance ID" << setw(10) << "Car ID" 
             << setw(15) << "Date" << setw(20) << "Description" 
             << setw(10) << "Cost" << setw(12) << "Status" << endl;
        cout << string(82, '-') << endl;
        for (const auto& record : records) {
            if (record->carID == carID) {
                record->display();
                found = true;
            }
        }
        if (!found) {
            cout << "No maintenance records found for this car.\n";
        }
        TransactionLogger::logUserAction(currentUser, "Viewed maintenance history for car " + carID);
        pause();
    }

    void updateSystemSettings() {
        settings.updateSettings();
        TransactionLogger::logUserAction(currentUser, "Updated system settings");
        pause();
    }

    void viewRentalHistory() {
        string customerID;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, customerID);
        customerID = sanitizeInput(customerID);
        if (!isValidID(customerID, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }

        clearScreen();
        cout << "=== Rental History ===\n\n";
        auto rentals = rentalRepo.getAll();
        bool found = false;
        cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" 
             << setw(15) << "Customer ID" << setw(12) << "Start Date" 
             << setw(12) << "End Date" << setw(12) << "Return Date"
             << setw(12) << "Total Cost" << setw(10) << "Penalty" 
             << setw(10) << "Status" << setw(10) << "Mileage" << endl;
        cout << string(101, '-') << endl;
        for (const auto& rental : rentals) {
            if (rental->customerID == customerID) {
                rental->display();
                found = true;
            }
        }
        if (!found) {
            cout << "No rental history found.\n";
        }
        TransactionLogger::logUserAction(currentUser, "Viewed rental history for customer " + customerID);
        pause();
    }

    void viewLoyaltyPoints() {
        string customerID;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, customerID);
        customerID = sanitizeInput(customerID);
        if (!isValidID(customerID, "CU")) {
            cout << "Invalid Customer ID!\n";
            pause();
            return;
        }

        auto customer = customerRepo.find(customerID);
        if (!customer) {
            cout << "Customer not found!\n";
            pause();
            return;
        }

        clearScreen();
        cout << "=== Loyalty Points for " << customer->fullName << " ===\n\n";
        cout << "Customer ID: " << customer->getId() << endl;
        cout << "Loyalty Points: " << customer->loyaltyPoints << endl;
        cout << "Discount Eligibility: " << (customer->loyaltyPoints >= 1000 ? "Yes (" + 
             to_string(settings.getLoyaltyDiscountRate() * 100) + "%)" : "No (Need 1000 points)") << endl;
        TransactionLogger::logUserAction(currentUser, "Viewed loyalty points for customer " + customerID);
        pause();
    }

    void generateCustomerLoyaltyReport() {
        clearScreen();
        cout << "=== Customer Loyalty Report ===\n\n";
        auto customers = customerRepo.getAll();
        if (customers.empty()) {
            cout << "No customers available.\n";
        } else {
            cout << left << setw(15) << "Customer ID" << setw(20) << "Full Name" 
                 << setw(25) << "Email" << setw(15) << "Phone" 
                 << setw(12) << "Loyalty Points" << setw(20) << "Discount Eligible" << endl;
            cout << string(107, '-') << endl;
            for (const auto& customer : customers) {
                cout << left << setw(15) << customer->getId() << setw(20) << customer->fullName 
                     << setw(25) << customer->email << setw(15) << customer->phone 
                     << setw(12) << customer->loyaltyPoints 
                     << setw(20) << (customer->loyaltyPoints >= 1000 ? "Yes" : "No") << endl;
            }
        }
        TransactionLogger::logUserAction(currentUser, "Generated customer loyalty report");
        pause();
    }

    void monthlyReport() {
        clearScreen();
        cout << "=== Monthly Rental Report ===\n\n";
        float totalRevenue = 0.0, totalPenalties = 0.0;
        int activeRentals = 0, completedRentals = 0, cancelledRentals = 0;

        cout << "=== All Rentals ===\n";
        auto rentals = rentalRepo.getAll();
        if (rentals.empty()) {
            cout << "No rentals found.\n";
        } else {
            cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" 
                 << setw(15) << "Customer ID" << setw(12) << "Start Date" 
                 << setw(12) << "End Date" << setw(12) << "Return Date"
                 << setw(12) << "Total Cost" << setw(10) << "Penalty" 
                 << setw(10) << "Status" << setw(10) << "Mileage" << endl;
            cout << string(101, '-') << endl;
            for (const auto& rental : rentals) {
                rental->display();
                if (rental->status == "Active") activeRentals++;
                else if (rental->status == "Completed") completedRentals++;
                else if (rental->status == "Cancelled") cancelledRentals++;
                if (rental->status != "Cancelled") {
                    totalRevenue += rental->totalCost;
                    totalPenalties += rental->latePenalty;
                }
            }
        }

        cout << "\n=== Maintenance Summary ===\n";
        float totalMaintenanceCost = 0.0;
        int scheduledMaintenance = 0, completedMaintenance = 0;
        auto records = maintenanceRepo.getAll();
        if (records.empty()) {
            cout << "No maintenance records found.\n";
        } else {
            cout << left << setw(15) << "Maintenance ID" << setw(10) << "Car ID" 
                 << setw(15) << "Date" << setw(20) << "Description" 
                 << setw(10) << "Cost" << setw(12) << "Status" << endl;
            cout << string(82, '-') << endl;
            for (const auto& record : records) {
                record->display();
                totalMaintenanceCost += record->cost;
                if (record->status == "Scheduled") scheduledMaintenance++;
                else if (record->status == "Completed") completedMaintenance++;
            }
        }

        cout << "\n=== Summary ===\n";
        cout << "Total Rentals: " << rentals.size() << endl;
        cout << "Active Rentals: " << activeRentals << endl;
        cout << "Completed Rentals: " << completedRentals << endl;
        cout << "Cancelled Rentals: " << cancelledRentals << endl;
        cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
        cout << "Total Late Penalties: $" << fixed << setprecision(2) << totalPenalties << endl;
        cout << "Total Maintenance Records: " << records.size() << endl;
        cout << "Scheduled Maintenance: " << scheduledMaintenance << endl;
        cout << "Completed Maintenance: " << completedMaintenance << endl;
        cout << "Total Maintenance Cost: $" << fixed << setprecision(2) << totalMaintenanceCost << endl;
        cout << "Net Revenue: $" << fixed << setprecision(2) << (totalRevenue + totalPenalties - totalMaintenanceCost) << endl;
        TransactionLogger::logUserAction(currentUser, "Generated monthly report");
        pause();
    }

    void viewRentals() {
        clearScreen();
        cout << "=== All Rentals ===\n\n";
        auto rentals = rentalRepo.getAll();
        if (rentals.empty()) {
            cout << "No rentals found.\n";
        } else {
            cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" 
                 << setw(15) << "Customer ID" << setw(12) << "Start Date" 
                 << setw(12) << "End Date" << setw(12) << "Return Date"
                 << setw(12) << "Total Cost" << setw(10) << "Penalty" 
                 << setw(10) << "Status" << setw(10) << "Mileage" << endl;
            cout << string(101, '-') << endl;
            for (const auto& rental : rentals) {
                rental->display();
            }
        }
        pause();
    }
};

int main() {
    try {
        CarRentalSystem system;
        TransactionLogger::logSystemEvent("System started");
        system.mainMenu();
    } catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        TransactionLogger::logSystemEvent("Error: " + string(e.what()));
        return 1;
    }
    return 0;
}