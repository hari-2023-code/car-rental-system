#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <windows.h> 
#include <conio.h>   
#include <sstream>
#include <limits>
#define USER_FILE "users.dat"
#define CAR_FILE "cars.dat"
#define RENTAL_FILE "rentals.dat"
#define CUSTOMER_FILE "customers.dat"
#define MAINTENANCE_FILE "maintenance.dat"

using namespace std;

struct Date 
{
    int dd, mm, yy;
};

struct CustomerProfile 
{
    string customerID;
    string fullName;
    string email;
    string phone;
    string address;
};

struct Car 
{
    string carID;
    string brand;
    string model;
    float dailyRate;
    bool isAvailable;
    string category; 
    int year;
    string fuelType; 
};

struct MaintenanceRecord 
{
    string maintenanceID;
    string carID;
    Date maintenanceDate;
    string description;
    float cost;
    string status; 
};

struct Rental 
{
    string rentalID;
    string carID;
    string customerID;
    Date startDate;
    Date endDate;
    float totalCost;
    string status; 
};

struct User 
{
    string username;
    string password;
    bool isAdmin;
};

class CarRentalSystem 
{
private:
    vector<User> users;
    vector<Car> cars;
    vector<Rental> rentals;
    vector<CustomerProfile> customers;
    vector<MaintenanceRecord> maintenanceRecords;

public:

    void clearScreen() 
    {
        system("cls");
    }

    void pause(int ms) 
    {
        Sleep(ms);
    }

    void clearInputBuffer() 
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    
    bool isValidDate(int dd, int mm, int yy) 
    {
        
        if (yy < 1900 || yy > 2025) return false;
        
        if (mm < 1 || mm > 12) return false;
        
        if (dd < 1 || dd > 31) return false;

        
        if (mm == 4 || mm == 6 || mm == 9 || mm == 11) 
        {
            if (dd > 30) return false;
        }
       
        else if (mm == 2) 
        {
            bool isLeap = (yy % 4 == 0 && yy % 100 != 0) || (yy % 400 == 0);
            if (isLeap && dd > 29) return false;
            if (!isLeap && dd > 28) return false;
        }
        return true;
    }

    bool isValidCarID(const string& id) 
    {
        return id.length() >= 4 && id.substr(0, 2) == "CR";
    }

    bool isValidCustomerID(const string& id) 
    {
        return id.length() >= 4 && id.substr(0, 2) == "CU";
    }

    bool isValidRentalID(const string& id) 
    {
        
        return id.length() >= 4 && id.substr(0, 2) == "RN";
    }

    bool isValidMaintenanceID(const string& id) 
    {
        return id.length() >= 4 && id.substr(0, 2) == "MN";
    }

    bool isValidEmail(const string& email) 
    {    
        return email.find('@') != string::npos && email.find('.') != string::npos;
    }

    bool isValidPhone(const string& phone) 
    {
        if (phone.length() != 10) return false;
        return all_of(phone.begin(), phone.end(), ::isdigit);
    }

    
    void loadUsers() 
    {
        ifstream file(USER_FILE, ios::binary);
        if (!file) 
        {
            cout << "No user file found. Starting with empty user list.\n";
            return;
        }
        User user;
        while (file.read(reinterpret_cast<char*>(&user), sizeof(User))) 
        {
            users.push_back(user);
        }
        file.close();
    }

    void saveUsers() 
    {
        ofstream file(USER_FILE, ios::binary | ios::trunc);
        if (!file) 
        {
            cout << "Error: Cannot save users to file!\n";
            return;
        }
        for (const auto& user : users) 
        {
            file.write(reinterpret_cast<const char*>(&user), sizeof(User));
        }
        file.close();
    }

    void loadCars() 
    {
        ifstream file(CAR_FILE, ios::binary);
        if (!file) 
        {
            cout << "No car file found. Starting with empty car list.\n";
            return;
        }
        Car car;
        while (file.read(reinterpret_cast<char*>(&car), sizeof(Car))) 
        {
            cars.push_back(car);
        }
        file.close();
    }

    void saveCars() 
    {
        ofstream file(CAR_FILE, ios::binary | ios::trunc);
        if (!file) 
        {
            cout << "Error: Cannot save cars to file!\n";
            return;
        }
        for (const auto& car : cars) 
        {
            file.write(reinterpret_cast<const char*>(&car), sizeof(Car));
        }
        file.close();
    }

    void loadRentals() 
    {
        ifstream file(RENTAL_FILE, ios::binary);
        if (!file) 
        {
            cout << "No rental file found. Starting with empty rental list.\n";
            return;
        }
        Rental rental;
        while (file.read(reinterpret_cast<char*>(&rental), sizeof(Rental))) 
        {
            rentals.push_back(rental);
        }
        file.close();
    }

    void saveRentals() 
    {
        ofstream file(RENTAL_FILE, ios::binary | ios::trunc);
        if (!file) 
        {
            cout << "Error: Cannot save rentals to file!\n";
            return;
        }
        for (const auto& rental : rentals) 
        {
            file.write(reinterpret_cast<const char*>(&rental), sizeof(Rental));
        }
        file.close();
    }

    void loadCustomers() 
    {
        ifstream file(CUSTOMER_FILE, ios::binary);
        if (!file) 
        {
            cout << "No customer file found. Starting with empty customer list.\n";
            return;
        }
        CustomerProfile customer;
        while (file.read(reinterpret_cast<char*>(&customer), sizeof(CustomerProfile))) 
        {
            customers.push_back(customer);
        }
        file.close();
    }

    void saveCustomers() 
    {
        ofstream file(CUSTOMER_FILE, ios::binary | ios::trunc);
        if (!file) 
        {
            cout << "Error: Cannot save customers to file!\n";
            return;
        }
        for (const auto& customer : customers) 
        {
            file.write(reinterpret_cast<const char*>(&customer), sizeof(CustomerProfile));
        }
        file.close();
    }

    void loadMaintenanceRecords() 
    {
        ifstream file(MAINTENANCE_FILE, ios::binary);
        if (!file) 
        {
            cout << "No maintenance file found. Starting with empty maintenance list.\n";
            return;
        }
        MaintenanceRecord record;
        while (file.read(reinterpret_cast<char*>(&record), sizeof(MaintenanceRecord))) 
        {
            maintenanceRecords.push_back(record);
        }
        file.close();
    }

    void saveMaintenanceRecords() 
    {
        ofstream file(MAINTENANCE_FILE, ios::binary | ios::trunc);
        if (!file) 
        {
            cout << "Error: Cannot save maintenance records to file!\n";
            return;
        }
        for (const auto& record : maintenanceRecords) 
        {
            file.write(reinterpret_cast<const char*>(&record), sizeof(MaintenanceRecord));
        }
        file.close();
    }

    
    void inputPassword(string& password) 
    {
        password.clear();
        char ch;
        while (true) 
        {
            ch = _getch();
            if (ch == '\r' || ch == '\n') 
            {
                cout << endl;
                break;
            } 
            else if (ch == '\b' && !password.empty()) 
            {
                password.pop_back();
                cout << "\b \b";
            }
             else if (password.length() < 24) 
            {
                password.push_back(ch);
                cout << '*';
            }
        }
    }

    
    void mainMenu() 
    {
        loadUsers();
        loadCars();
        loadRentals();
        loadCustomers();
        loadMaintenanceRecords();
        int choice;
        while (true) 
        {
            clearScreen();
            cout << "\n\n\t\t\t\t\t======================================\n";
            cout << "\t\t\t\t\t\tCAR RENTAL SYSTEM\n";
            cout << "\t\t\t\t\t======================================\n\n";
            cout << "1) Login\n2) Register\n3) Exit\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) 
            {
                cout << "Invalid input! Please enter a number.\n";
                clearInputBuffer();
                pause(2000);
                continue;
            }
            clearInputBuffer();
            switch (choice) 
            {
                case 1: login(); break;
                case 2: registerUser(); break;
                case 3:
                    cout << "Exiting Car Rental System...\n";
                    pause(1000);
                    exit(0);
                default:
                    cout << "Invalid choice! Please try again.\n";
                    pause(2000);
            }
        }
    }

    
    void login() 
    {
        string username, password;
        cout << "Enter username: ";
        getline(cin, username);
        cout << "Enter password: ";
        inputPassword(password);

        bool found = false;
        bool isAdmin = false;
        for (const auto& user : users) 
        {
            if (user.username == username && user.password == password) 
            {
                found = true;
                isAdmin = user.isAdmin;
                break;
            }
        }

        if (found) 
        {
            cout << "LOGIN SUCCESSFUL!\n";
            cout << "Press any key to continue!\n";
            _getch();
            clearScreen();
            if (isAdmin) adminMenu();
            else customerMenu();
        } else 
        {
            cout << "Invalid Credentials! Please try again.\n";
            pause(2000);
            clearScreen();
            mainMenu();
        }
    }

    
    void registerUser() 
    {
        User user;
        cout << "Enter username: ";
        getline(cin, user.username);
        
        for (const auto& u : users) 
        {
            if (u.username == user.username) 
            {
                cout << "User already exists! Please choose a different username.\n";
                pause(2000);
                clearScreen();
                registerUser();
                return;
            }
        }

        cout << "Enter password: ";
        inputPassword(user.password);
        cout << "Is this an admin account? (y/n): ";
        char adminChoice;
        cin >> adminChoice;
        clearInputBuffer();
        user.isAdmin = (tolower(adminChoice) == 'y');

        users.push_back(user);
        saveUsers();
        cout << "User registered successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
        clearScreen();
        mainMenu();
    }

    
    void adminMenu() 
    {
        int choice;
        while (true) 
        {
            clearScreen();
            cout << "\n\n\t\t\t\t\t======================================\n";
            cout << "\t\t\t\t\t\tADMIN MENU\n";
            cout << "\t\t\t\t\t======================================\n\n";
            cout << "1) Add Car\n2) View Cars\n3) Update Car\n4) Delete Car\n";
            cout << "5) View Rentals\n6) Monthly Report\n7) Manage Customers\n";
            cout << "8) Manage Maintenance\n9) Export Rental History\n10) Logout\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) 
            {
                cout << "Invalid input! Please enter a number.\n";
                clearInputBuffer();
                pause(2000);
                continue;
            }
            clearInputBuffer();
            switch (choice) 
            {
                case 1: addCar(); break;
                case 2: viewCars(); break;
                case 3: updateCar(); break;
                case 4: deleteCar(); break;
                case 5: viewRentals(); break;
                case 6: monthlyReport(); break;
                case 7: manageCustomers(); break;
                case 8: manageMaintenance(); break;
                case 9: exportRentalHistory(); break;
                case 10: mainMenu(); break;
                default:
                    cout << "Invalid choice! Please try again.\n";
                    pause(2000);
            }
        }
    }

    
    void customerMenu() 
    {
        int choice;
        while (true) 
        {
            clearScreen();
            cout << "\n\n\t\t\t\t\t======================================\n";
            cout << "\t\t\t\t\t\tCUSTOMER MENU\n";
            cout << "\t\t\t\t\t======================================\n\n";
            cout << "1) View Available Cars\n2) Rent Car\n3) View My Rentals\n";
            cout << "4) Cancel Rental\n5) Update Profile\n6) Logout\n";
            cout << "Enter choice: ";
            if (!(cin >> choice)) 
            {
                cout << "Invalid input! Please enter a number.\n";
                clearInputBuffer();
                pause(2000);
                continue;
            }
            clearInputBuffer();
            switch (choice) 
            {
                case 1: viewAvailableCars(); break;
                case 2: rentCar(); break;
                case 3: viewMyRentals(); break;
                case 4: cancelRental(); break;
                case 5: updateCustomerProfile(); break;
                case 6: mainMenu(); break;
                default:
                    cout << "Invalid choice! Please try again.\n";
                    pause(2000);
            }
        }
    }

  
    void addCar() 
    {
        Car car;
        cout << "Enter Car ID (start with CR): ";
        getline(cin, car.carID);
        if (!isValidCarID(car.carID)) 
        {
            cout << "Invalid Car ID! Must start with 'CR' and be at least 4 characters.\n";
            pause(2000);
            return;
        }
        for (const auto& c : cars) 
        {
            if (c.carID == car.carID) 
            {
                cout << "Car ID already exists!\n";
                pause(2000);
                return;
            }
        }

        cout << "Enter Brand: ";
        getline(cin, car.brand);
        cout << "Enter Model: ";
        getline(cin, car.model);
        cout << "Enter Daily Rate: ";
        while (!(cin >> car.dailyRate) || car.dailyRate < 0) 
        {
            cout << "Invalid rate! Enter a positive number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        cout << "Enter Category (Economy/Standard/Luxury): ";
        getline(cin, car.category);
        cout << "Enter Year: ";
        while (!(cin >> car.year) || car.year < 1900 || car.year > 2025) 
        {
            cout << "Invalid year! Enter a year between 1900 and 2025: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        cout << "Enter Fuel Type (Petrol/Diesel/Electric): ";
        getline(cin, car.fuelType);
        car.isAvailable = true;

        cars.push_back(car);
        saveCars();
        cout << "Car added successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }

    
    void viewCars() 
    {
        clearScreen();
        cout << "========== Car Inventory ==========\n";
        if (cars.empty()) 
        {
            cout << "No cars available.\n";
        } else 
        {
            cout << left << setw(10) << "Car ID" << setw(15) << "Brand" << setw(15) << "Model"
                 << setw(12) << "Daily Rate" << setw(10) << "Category" << setw(12) << "Available"
                 << setw(8) << "Year" << setw(10) << "Fuel Type" << endl;
            cout << string(92, '-') << endl;
            for (const auto& car : cars) 
            {
                cout << left << setw(10) << car.carID << setw(15) << car.brand << setw(15) << car.model
                     << setw(12) << fixed << setprecision(2) << car.dailyRate << setw(10) << car.category
                     << setw(12) << (car.isAvailable ? "Yes" : "No") << setw(8) << car.year
                     << setw(10) << car.fuelType << endl;
            }
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }

    
    void updateCar() 
    {
        string carID;
        cout << "Enter Car ID to update: ";
        getline(cin, carID);
        if (!isValidCarID(carID)) 
        {
            cout << "Invalid Car ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == carID; });
        if (it == cars.end()) 
        {
            cout << "Car not found!\n";
            pause(2000);
            return;
        }

        Car& car = *it;
        string input;
        cout << "Enter new Brand (" << car.brand << "): ";
        getline(cin, input);
        if (!input.empty()) car.brand = input;

        cout << "Enter new Model (" << car.model << "): ";
        getline(cin, input);
        if (!input.empty()) car.model = input;

        cout << "Enter new Daily Rate (" << car.dailyRate << "): ";
        float rate;
        if (cin >> rate && rate >= 0) 
        {
            car.dailyRate = rate;
        }
        clearInputBuffer();

        cout << "Enter new Category (" << car.category << "): ";
        getline(cin, input);
        if (!input.empty()) car.category = input;

        cout << "Enter new Year (" << car.year << "): ";
        int year;
        if (cin >> year && year >= 1900 && year <= 2025) 
        {
            car.year = year;
        }
        clearInputBuffer();

        cout << "Enter new Fuel Type (" << car.fuelType << "): ";
        getline(cin, input);
        if (!input.empty()) car.fuelType = input;

        saveCars();
        cout << "Car updated successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }

    void deleteCar() 
    {
        string carID;
        cout << "Enter Car ID to delete: ";
        getline(cin, carID);
        if (!isValidCarID(carID)) 
        {
            cout << "Invalid Car ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == carID; });
        if (it == cars.end()) 
        {
            cout << "Car not found!\n";
            pause(2000);
            return;
        }

        cars.erase(it);
        saveCars();
        cout << "Car deleted successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }

    
    void viewAvailableCars() 
    {
        clearScreen();
        cout << "========== Available Cars ==========\n";
        bool found = false;
        cout << left << setw(10) << "Car ID" << setw(15) << "Brand" << setw(15) << "Model"
             << setw(12) << "Daily Rate" << setw(10) << "Category" << setw(8) << "Year"
             << setw(10) << "Fuel Type" << endl;
        cout << string(80, '-') << endl;
        for (const auto& car : cars) {
            if (car.isAvailable) 
            {
                cout << left << setw(10) << car.carID << setw(15) << car.brand << setw(15) << car.model
                     << setw(12) << fixed << setprecision(2) << car.dailyRate << setw(10) << car.category
                     << setw(8) << car.year << setw(10) << car.fuelType << endl;
                found = true;
            }
        }
        if (!found) 
        {
            cout << "No cars available for rent.\n";
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }
    void rentCar() 
    {
        Rental rental;
        cout << "Enter Rental ID (start with RN): ";
        getline(cin, rental.rentalID);
        if (!isValidRentalID(rental.rentalID)) 
        {
            cout << "Invalid Rental ID! Must start with 'RN' and be at least 4 characters.\n";
            pause(2000);
            return;
        }
        for (const auto& r : rentals) 
        {
            if (r.rentalID == rental.rentalID) 
            {
                cout << "Rental ID already exists!\n";
                pause(2000);
                return;
            }
        }

        cout << "Enter Customer ID (start with CU): ";
        getline(cin, rental.customerID);
        if (!isValidCustomerID(rental.customerID)) 
        {
            cout << "Invalid Customer ID! Must start with 'CU' and be at least 4 characters.\n";
            pause(2000);
            return;
        }
        auto custIt = find_if(customers.begin(), customers.end(), [&](const CustomerProfile& c) { return c.customerID == rental.customerID; });
        if (custIt == customers.end()) 
        {
            cout << "Customer not found! Please create a customer profile first.\n pilihan kembali";
            pause(2000);
            return;
        }

        cout << "Enter Car ID (start with CR): ";
        getline(cin, rental.carID);
        auto it = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == rental.carID && c.isAvailable; });
        if (it == cars.end()) 
        {
            cout << "Car not found or unavailable!\n";
            pause(2000);
            return;
        }

        cout << "Enter Start Date (dd mm yyyy): ";
        while (!(cin >> rental.startDate.dd >> rental.startDate.mm >> rental.startDate.yy) ||
               !isValidDate(rental.startDate.dd, rental.startDate.mm, rental.startDate.yy)) 
        {
            cout << "Invalid start date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        cout << "Enter End Date (dd mm yyyy): ";
        while (!(cin >> rental.endDate.dd >> rental.endDate.mm >> rental.endDate.yy) ||
               !isValidDate(rental.endDate.dd, rental.endDate.mm, rental.endDate.yy))        
        {
            cout << "Invalid end date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();
        int days = (rental.endDate.dd - rental.startDate.dd) + 1;
        if (days <= 0) 
        {
            cout << "Invalid rental period! End date must be after start date.\n";
            pause(2000);
            return;
        }

        rental.totalCost = it->dailyRate * days;
        rental.status = "Active";
        it->isAvailable = false;

        rentals.push_back(rental);
        saveRentals();
        saveCars();
        cout << "Car rented successfully! Total Cost: $" << fixed << setprecision(2) << rental.totalCost << endl;
        cout << "Press any key to continue!\n";
        _getch();
    }
    void viewMyRentals() 
    {
        string customerID;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, customerID);
        if (!isValidCustomerID(customerID)) 
        {
            cout << "Invalid Customer ID!\n";
            pause(2000);
            return;
        }

        clearScreen();
        cout << "========== Your Rentals ==========\n";
        bool found = false;
        cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" << setw(15) << "Customer ID"
             << setw(12) << "Start Date" << setw(12) << "End Date" << setw(12) << "Total Cost"
             << setw(10) << "Status" << endl;
        cout << string(81, '-') << endl;
        for (const auto& rental : rentals) {
            if (rental.customerID == customerID) {
                cout << left << setw(10) << rental.rentalID << setw(10) << rental.carID << setw(15) << rental.customerID
                     << setw(12) << (to_string(rental.startDate.dd) + "/" + to_string(rental.startDate.mm) + "/" + to_string(rental.startDate.yy))
                     << setw(12) << (to_string(rental.endDate.dd) + "/" + to_string(rental.endDate.mm) + "/" + to_string(rental.endDate.yy))
                     << setw(12) << fixed << setprecision(2) << rental.totalCost << setw(10) << rental.status << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "No rentals found.\n";
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }
    void cancelRental() {
        string rentalID;
        cout << "Enter Rental ID to cancel: ";
        getline(cin, rentalID);
        if (!isValidRentalID(rentalID)) {
            cout << "Invalid Rental ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(rentals.begin(), rentals.end(), [&](const Rental& r) { return r.rentalID == rentalID && r.status == "Active"; });
        if (it == rentals.end()) {
            cout << "Rental not found or already cancelled/completed!\n";
            pause(2000);
            return;
        }

        it->status = "Cancelled";
        auto carIt = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == it->carID; });
        if (carIt != cars.end()) {
            carIt->isAvailable = true;
        }

        saveRentals();
        saveCars();
        cout << "Rental cancelled successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void manageCustomers() {
        int choice;
        clearScreen();
        cout << "\n\n\t\t\t\t\t======================================\n";
        cout << "\t\t\t\t\t\tCUSTOMER MANAGEMENT\n";
        cout << "\t\t\t\t\t======================================\n\n";
        cout << "1) Add Customer\n2) View Customers\n3) Update Customer\n4) Delete Customer\n5) Back\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            clearInputBuffer();
            pause(2000);
            return;
        }
        clearInputBuffer();
        switch (choice) {
            case 1: addCustomer(); break;
            case 2: viewCustomers(); break;
            case 3: updateCustomerProfile(true); break;
            case 4: deleteCustomer(); break;
            case 5: return;
            default:
                cout << "Invalid choice!\n";
                pause(2000);
        }
    }
    void addCustomer() {
        CustomerProfile customer;
        cout << "Enter Customer ID (start with CU): ";
        getline(cin, customer.customerID);
        if (!isValidCustomerID(customer.customerID)) {
            cout << "Invalid Customer ID!\n";
            pause(2000);
            return;
        }
        for (const auto& c : customers) {
            if (c.customerID == customer.customerID) {
                cout << "Customer ID already exists!\n";
                pause(2000);
                return;
            }
        }

        cout << "Enter Full Name: ";
        getline(cin, customer.fullName);
        cout << "Enter Email: ";
        getline(cin, customer.email);
        if (!isValidEmail(customer.email)) {
            cout << "Invalid email format!\n";
            pause(2000);
            return;
        }
        cout << "Enter Phone (10 digits): ";
        getline(cin, customer.phone);
        if (!isValidPhone(customer.phone)) {
            cout << "Invalid phone number! Must be 10 digits.\n";
            pause(2000);
            return;
        }
        cout << "Enter Address: ";
        getline(cin, customer.address);

        customers.push_back(customer);
        saveCustomers();
        cout << "Customer added successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void viewCustomers() {
        clearScreen();
        cout << "========== Customer List ==========\n";
        if (customers.empty()) {
            cout << "No customers available.\n";
        } else {
            cout << left << setw(15) << "Customer ID" << setw(20) << "Full Name" << setw(25) << "Email"
                 << setw(15) << "Phone" << setw(30) << "Address" << endl;
            cout << string(105, '-') << endl;
            for (const auto& customer : customers) {
                cout << left << setw(15) << customer.customerID << setw(20) << customer.fullName
                     << setw(25) << customer.email << setw(15) << customer.phone << setw(30) << customer.address << endl;
            }
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }
    void updateCustomerProfile(bool isAdmin = false) {
        string customerID;
        cout << "Enter Customer ID to update: ";
        getline(cin, customerID);
        if (!isValidCustomerID(customerID)) {
            cout << "Invalid Customer ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(customers.begin(), customers.end(), [&](const CustomerProfile& c) { return c.customerID == customerID; });
        if (it == customers.end()) {
            cout << "Customer not found!\n";
            pause(2000);
            return;
        }

        CustomerProfile& customer = *it;
        string input;
        cout << "Enter new Full Name (" << customer.fullName << "): ";
        getline(cin, input);
        if (!input.empty()) customer.fullName = input;

        cout << "Enter new Email (" << customer.email << "): ";
        getline(cin, input);
        if (!input.empty() && isValidEmail(input)) {
            customer.email = input;
        } else if (!input.empty()) {
            cout << "Invalid email format! Keeping original email.\n";
        }

        cout << "Enter new Phone (" << customer.phone << "): ";
        getline(cin, input);
        if (!input.empty() && isValidPhone(input)) {
            customer.phone = input;
        } else if (!input.empty()) {
            cout << "Invalid phone number! Keeping original phone.\n";
        }

        cout << "Enter new Address (" << customer.address << "): ";
        getline(cin, input);
        if (!input.empty()) customer.address = input;

        saveCustomers();
        cout << "Customer profile updated successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void deleteCustomer() {
        string customerID;
        cout << "Enter Customer ID to delete: ";
        getline(cin, customerID);
        if (!isValidCustomerID(customerID)) {
            cout << "Invalid Customer ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(customers.begin(), customers.end(), [&](const CustomerProfile& c) { return c.customerID == customerID; });
        if (it == customers.end()) {
            cout << "Customer not found!\n";
            pause(2000);
            return;
        }

        customers.erase(it);
        saveCustomers();
        cout << "Customer deleted successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void manageMaintenance() {
        int choice;
        clearScreen();
        cout << "\n\n\t\t\t\t\t======================================\n";
        cout << "\t\t\t\t\t\tMAINTENANCE MANAGEMENT\n";
        cout << "\t\t\t\t\t======================================\n\n";
        cout << "1) Schedule Maintenance\n2) View Maintenance Records\n3) Update Maintenance\n";
        cout << "4) Delete Maintenance\n5) Back\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            clearInputBuffer();
            pause(2000);
            return;
        }
        clearInputBuffer();
        switch (choice) {
            case 1: scheduleMaintenance(); break;
            case 2: viewMaintenanceRecords(); break;
            case 3: updateMaintenance(); break;
            case 4: deleteMaintenance(); break;
            case 5: return;
            default:
                cout << "Invalid choice!\n";
                pause(2000);
        }
    }
    void scheduleMaintenance() {
        MaintenanceRecord record;
        cout << "Enter Maintenance ID (start with MN): ";
        getline(cin, record.maintenanceID);
        if (!isValidMaintenanceID(record.maintenanceID)) {
            cout << "Invalid Maintenance ID!\n";
            pause(2000);
            return;
        }
        for (const auto& r : maintenanceRecords) {
            if (r.maintenanceID == record.maintenanceID) {
                cout << "Maintenance ID already exists!\n";
                pause(2000);
                return;
            }
        }

        cout << "Enter Car ID: ";
        getline(cin, record.carID);
        if (!isValidCarID(record.carID)) {
            cout << "Invalid Car ID!\n";
            pause(2000);
            return;
        }
        auto it = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == record.carID; });
        if (it == cars.end()) {
            cout << "Car not found!\n";
            pause(2000);
            return;
        }

        cout << "Enter Maintenance Date (dd mm yyyy): ";
        while (!(cin >> record.maintenanceDate.dd >> record.maintenanceDate.mm >> record.maintenanceDate.yy) ||
               !isValidDate(record.maintenanceDate.dd, record.maintenanceDate.mm, record.maintenanceDate.yy)) {
            cout << "Invalid date! Enter again (dd mm yyyy): ";
            clearInputBuffer();
        }
        clearInputBuffer();

        cout << "Enter Description: ";
        getline(cin, record.description);
        cout << "Enter Cost: ";
        while (!(cin >> record.cost) || record.cost < 0) {
            cout << "Invalid cost! Enter a positive number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
        record.status = "Scheduled";

        maintenanceRecords.push_back(record);
        saveMaintenanceRecords();
        cout << "Maintenance scheduled successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void viewMaintenanceRecords() {
        clearScreen();
        cout << "========== Maintenance Records ==========\n";
        if (maintenanceRecords.empty()) {
            cout << "No maintenance records available.\n";
        } else {
            cout << left << setw(15) << "Maintenance ID" << setw(10) << "Car ID" << setw(15) << "Date"
                 << setw(20) << "Description" << setw(10) << "Cost" << setw(12) << "Status" << endl;
            cout << string(82, '-') << endl;
            for (const auto& record : maintenanceRecords) {
                cout << left << setw(15) << record.maintenanceID << setw(10) << record.carID
                     << setw(15) << (to_string(record.maintenanceDate.dd) + "/" + to_string(record.maintenanceDate.mm) + "/" + to_string(record.maintenanceDate.yy))
                     << setw(20) << record.description << setw(10) << fixed << setprecision(2) << record.cost
                     << setw(12) << record.status << endl;
            }
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }
    void updateMaintenance() {
        string maintenanceID;
        cout << "Enter Maintenance ID to update: ";
        getline(cin, maintenanceID);
        if (!isValidMaintenanceID(maintenanceID)) {
            cout << "Invalid Maintenance ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(maintenanceRecords.begin(), maintenanceRecords.end(), [&](const MaintenanceRecord& r) { return r.maintenanceID == maintenanceID; });
        if (it == maintenanceRecords.end()) {
            cout << "Maintenance record not found!\n";
            pause(2000);
            return;
        }

        MaintenanceRecord& record = *it;
        string input;
        cout << "Enter new Car ID (" << record.carID << "): ";
        getline(cin, input);
        if (!input.empty() && isValidCarID(input)) {
            auto carIt = find_if(cars.begin(), cars.end(), [&](const Car& c) { return c.carID == input; });
            if (carIt != cars.end()) {
                record.carID = input;
            } else {
                cout << "Car not found! Keeping original Car ID.\n";
            }
        }

        cout << "Enter new Maintenance Date (" << record.maintenanceDate.dd << "/" << record.maintenanceDate.mm << "/" << record.maintenanceDate.yy << "): ";
        int dd, mm, yy;
        if (cin >> dd >> mm >> yy && isValidDate(dd, mm, yy)) {
            record.maintenanceDate = {dd, mm, yy};
        }
        clearInputBuffer();

        cout << "Enter new Description (" << record.description << "): ";
        getline(cin, input);
        if (!input.empty()) record.description = input;

        cout << "Enter new Cost (" << record.cost << "): ";
        float cost;
        if (cin >> cost && cost >= 0) {
            record.cost = cost;
        }
        clearInputBuffer();

        cout << "Enter new Status (Scheduled/Completed): ";
        getline(cin, input);
        if (input == "Scheduled" || input == "Completed") {
            record.status = input;
        }

        saveMaintenanceRecords();
        cout << "Maintenance record updated successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void deleteMaintenance() {
        string maintenanceID;
        cout << "Enter Maintenance ID to delete: ";
        getline(cin, maintenanceID);
        if (!isValidMaintenanceID(maintenanceID)) {
            cout << "Invalid Maintenance ID!\n";
            pause(2000);
            return;
        }

        auto it = find_if(maintenanceRecords.begin(), maintenanceRecords.end(), [&](const MaintenanceRecord& r) { return r.maintenanceID == maintenanceID; });
        if (it == maintenanceRecords.end()) {
            cout << "Maintenance record not found!\n";
            pause(2000);
            return;
        }

        maintenanceRecords.erase(it);
        saveMaintenanceRecords();
        cout << "Maintenance record deleted successfully!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void exportRentalHistory() {
        ofstream file("rental_history.txt");
        if (!file) {
            cout << "Error: Cannot create rental history file!\n";
            pause(2000);
            return;
        }

        file << "========== Rental History Report ==========\n\n";
        file << left << setw(10) << "Rental ID" << setw(10) << "Car ID" << setw(15) << "Customer ID"
             << setw(12) << "Start Date" << setw(12) << "End Date" << setw(12) << "Total Cost"
             << setw(10) << "Status" << endl;
        file << string(81, '-') << endl;

        float totalRevenue = 0.0;
        int activeRentals = 0, completedRentals = 0, cancelledRentals = 0;
        for (const auto& rental : rentals) {
            file << left << setw(10) << rental.rentalID << setw(10) << rental.carID << setw(15) << rental.customerID
                 << setw(12) << (to_string(rental.startDate.dd) + "/" + to_string(rental.startDate.mm) + "/" + to_string(rental.startDate.yy))
                 << setw(12) << (to_string(rental.endDate.dd) + "/" + to_string(rental.endDate.mm) + "/" + to_string(rental.endDate.yy))
                 << setw(12) << fixed << setprecision(2) << rental.totalCost << setw(10) << rental.status << endl;
            if (rental.status == "Active") activeRentals++;
            else if (rental.status == "Completed") completedRentals++;
            else if (rental.status == "Cancelled") cancelledRentals++;
            if (rental.status != "Cancelled") totalRevenue += rental.totalCost;
        }

        file << "\n========== Summary ==========\n";
        file << "Total Rentals: " << rentals.size() << endl;
        file << "Active Rentals: " << activeRentals << endl;
        file << "Completed Rentals: " << completedRentals << endl;
        file << "Cancelled Rentals: " << cancelledRentals << endl;
        file << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
        file << "=============================\n";
        file.close();

        cout << "Rental history exported to rental_history.txt!\n";
        cout << "Press any key to continue!\n";
        _getch();
    }
    void monthlyReport() {
        clearScreen();
        cout << "========== Monthly Rental Report ==========\n";
        float totalRevenue = 0.0;
        int activeRentals = 0, completedRentals = 0, cancelledRentals = 0;

        cout << "\n========== All Rentals ==========\n";
        if (rentals.empty()) {
            cout << "No rentals found.\n";
        } else {
            cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" << setw(15) << "Customer ID"
                 << setw(12) << "Start Date" << setw(12) << "End Date" << setw(12) << "Total Cost"
                 << setw(10) << "Status" << endl;
            cout << string(81, '-') << endl;
            for (const auto& rental : rentals) {
                cout << left << setw(10) << rental.rentalID << setw(10) << rental.carID << setw(15) << rental.customerID
                     << setw(12) << (to_string(rental.startDate.dd) + "/" + to_string(rental.startDate.mm) + "/" + to_string(rental.startDate.yy))
                     << setw(12) << (to_string(rental.endDate.dd) + "/" + to_string(rental.endDate.mm) + "/" + to_string(rental.endDate.yy))
                     << setw(12) << fixed << setprecision(2) << rental.totalCost << setw(10) << rental.status << endl;
                if (rental.status == "Active") activeRentals++;
                else if (rental.status == "Completed") completedRentals++;
                else if (rental.status == "Cancelled") cancelledRentals++;
                if (rental.status != "Cancelled") totalRevenue += rental.totalCost;
            }
        }

        cout << "\n========== Maintenance Summary ==========\n";
        float totalMaintenanceCost = 0.0;
        int scheduledMaintenance = 0, completedMaintenance = 0;
        if (!maintenanceRecords.empty()) {
            cout << left << setw(15) << "Maintenance ID" << setw(10) << "Car ID" << setw(15) << "Date"
                 << setw(20) << "Description" << setw(10) << "Cost" << setw(12) << "Status" << endl;
            cout << string(82, '-') << endl;
            for (const auto& record : maintenanceRecords) {
                cout << left << setw(15) << record.maintenanceID << setw(10) << record.carID
                     << setw(15) << (to_string(record.maintenanceDate.dd) + "/" + to_string(record.maintenanceDate.mm) + "/" + to_string(record.maintenanceDate.yy))
                     << setw(20) << record.description << setw(10) << fixed << setprecision(2) << record.cost
                     << setw(12) << record.status << endl;
                totalMaintenanceCost += record.cost;
                if (record.status == "Scheduled") scheduledMaintenance++;
                else if (record.status == "Completed") completedMaintenance++;
            }
        } else {
            cout << "No maintenance records found.\n";
        }

        cout << "\n========== Summary ==========\n";
        cout << "Total Rentals: " << rentals.size() << endl;
        cout << "Active Rentals: " << activeRentals << endl;
        cout << "Completed Rentals: " << completedRentals << endl;
        cout << "Cancelled Rentals: " << cancelledRentals << endl;
        cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
        cout << "Total Maintenance Records: " << maintenanceRecords.size() << endl;
        cout << "Scheduled Maintenance: " << scheduledMaintenance << endl;
        cout << "Completed Maintenance: " << completedMaintenance << endl;
        cout << "Total Maintenance Cost: $" << fixed << setprecision(2) << totalMaintenanceCost << endl;
        cout << "Net Revenue: $" << fixed << setprecision(2) << (totalRevenue - totalMaintenanceCost) << endl;
        cout << "=============================\n";
        cout << "Press any key to continue!\n";
        _getch();
    }

    // View all rentals (Admin)
    void viewRentals() {
        clearScreen();
        cout << "========== All Rentals ==========\n";
        if (rentals.empty()) {
            cout << "No rentals found.\n";
        } else {
            cout << left << setw(10) << "Rental ID" << setw(10) << "Car ID" << setw(15) << "Customer ID"
                 << setw(12) << "Start Date" << setw(12) << "End Date" << setw(12) << "Total Cost"
                 << setw(10) << "Status" << endl;
            cout << string(81, '-') << endl;
            for (const auto& rental : rentals) {
                cout << left << setw(10) << rental.rentalID << setw(10) << rental.carID << setw(15) << rental.customerID
                     << setw(12) << (to_string(rental.startDate.dd) + "/" + to_string(rental.startDate.mm) + "/" + to_string(rental.startDate.yy))
                     << setw(12) << (to_string(rental.endDate.dd) + "/" + to_string(rental.endDate.mm) + "/" + to_string(rental.endDate.yy))
                     << setw(12) << fixed << setprecision(2) << rental.totalCost << setw(10) << rental.status << endl;
            }
        }
        cout << "\nPress any key to continue!\n";
        _getch();
    }
};

int main() {
    CarRentalSystem system;
    system.mainMenu();
    return 0;
}