#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace std;

//login info
//user = user123 
//admin = admin123

struct Car {
    string model;
    string color;
    string speed;
    int dailyRate; 
    int modelCount; 
};

struct Customer {
    string customerName;
    string carModel;
    int days = 0;
    int rentalFee = 0;
    int dailyRate = 0;
    int total = 0;
};

class Rent {
private:
    vector<Customer> rentalHistory;
    vector<Car> rentedCars; 
    map<string, vector<Car>> carInventory = {
        {"Tesla", {{"Tesla S1", "Red", "300km/hr", 56, 5},
                   {"Tesla S2", "Blue", "330km/hr", 56, 3}}},
        {"Hyundai", {{"Hyundai Creta", "White", "200km/hr", 60, 4},
                     {"Hyundai Sonata", "Black", "350km/hr", 60, 2},
                     {"Hyundai Tucson", "Pink", "310km/hr", 60, 6}}},
        {"Ford", {{"Ford Focus", "Yellow", "250km/hr", 75, 3},
                  {"Ford Mustang", "Magenta", "305km/hr", 75, 4}}},
        {"BMW", {{"BMW 3 Series", "Orange", "400km/hr", 50, 5},
                 {"BMW 4 Series", "Green", "390km/hr", 50, 2},
                 {"BMW X5", "Purple", "300km/hr", 50, 4},
                 {"BMW X5 M", "Blue", "310km/hr", 50, 3},
                 {"BMW X5 Hybrid", "Black", "320km/hr", 50, 1}}},
        {"Mercedes", {{"Mercedes Gla", "Silver", "300km/hr", 40, 2},
                      {"Mercedes GLB", "White", "310km/hr", 40, 1}}}
    };
    Customer customer;
    string role;

    void clearScreen() {
#ifdef _WIN32
        system("CLS");
#else
        system("clear");
#endif
    }

    void sleepForSeconds(int seconds) {
        this_thread::sleep_for(chrono::seconds(seconds));
    }

    void welcomePage() {
        clearScreen();
        cout << "\t-----------------------------------\n";
        cout << "\tWelcome to the Car Rental System\n";
        cout << "\t-----------------------------------\n\n\n";
        cout << "\tPlease login to continue.\n";
        sleepForSeconds(2);
        clearScreen();
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return (first == string::npos || last == string::npos) ? str : str.substr(first, last - first + 1);
    }

public:
    void login() {
        string username, password;
        welcomePage(); 
        cout << "\t----------------------------\n";
        cout << "\tCar Rental System Login\n";
        cout << "\t----------------------------\n\n";
        cout << "\tEnter Username: ";
        cin >> username;
        cout << "\tEnter Password: ";
        cin >> password;

        if (username == "admin" && password == "admin123") {
            role = "admin";
            cout << "\n\tAdmin Login Successful!\n";
        }
        else if (username == "user" && password == "user123") {
            role = "user";
            cout << "\n\tUser Login Successful!\n";
        }
        else {
            cout << "\n\tInvalid credentials! Please try again.\n";
            sleepForSeconds(1);
            login();
        }
        sleepForSeconds(1);
        clearScreen();
    }

    void showUserMenu() {
        int choice;
        do {
            cout << "\n\t=========== User Menu ===========";
            cout << "\n\t1. Rent a Car";
            cout << "\n\t2. View Rental History";
            cout << "\n\t3. Return a Car";
            cout << "\n\t4. Log Out";
            cout << "\n\t=================================";
            cout << "\n\tEnter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                collectData();
                calculate();
                cout << "\n\nYour Car Rental has been successful!\n";
                showRent();
                break;
            case 2:
                viewRentalHistory();
                break;
            case 3:
                returnCar();
                break;
            case 4:
                cout << "Logging out...\n";
                sleepForSeconds(1);
                return; // Break out of the menu and go back to login
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 4);
    }

    void showAdminMenu() {
        int choice;
        do {
            cout << "\n\t=========== Admin Menu ===========";
            cout << "\n\t1. View All Rentals";
            cout << "\n\t2. Add Cars to Inventory";
            cout << "\n\t3. Cancel a Rental";
            cout << "\n\t4. Update Customer Info";
            cout << "\n\t5. Log Out";
            cout << "\n\t==================================";
            cout << "\n\tEnter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                viewAllRentals();
                break;
            case 2:
                addCarInventory();
                break;
            case 3:
                cancelRental();
                break;
            case 4:
                updateCustomerInfo();
                break;
            case 5:
                cout << "Logging out...\n";
                sleepForSeconds(1);
                return; 
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 5);
    }

    void start() {
        while (true) {
            login(); 
            if (role == "user") {
                showUserMenu();
            }
            else if (role == "admin") {
                showAdminMenu();
            }
        }
    }

    void collectData() {
        cout << "Enter your name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, customer.customerName);

        while (true) {
            displayAvailableCars();

            cout << "\nEnter the car model you want to rent: ";
            getline(cin, customer.carModel);
            customer.carModel = trim(customer.carModel); // Trim spaces around car model

            bool found = false;
            for (auto& carBrand : carInventory) {
                auto& models = carBrand.second;
                auto it = find_if(models.begin(), models.end(), [this](const Car& car) {
                    return car.model == customer.carModel;
                    });

                if (it != models.end() && it->modelCount > 0) {
                    customer.dailyRate = it->dailyRate; // Capture daily rate before removing car
                    rentedCars.push_back(*it); // Add to rented list
                    it->modelCount--; // Decrease the available count
                    found = true;
                    break;
                }
            }

            if (found) {
                break;
            }
            else {
                cout << "Sorry, that car model is not available. Please select another one.\n";
            }
        }

        cout << "Enter number of days to rent: ";
        cin >> customer.days;
    }

    void calculate() {
        customer.rentalFee = customer.days * customer.dailyRate;
        customer.total = customer.rentalFee;
        rentalHistory.push_back(customer);
    }

    void showRent() {
        cout << "\n\nInvoice:\n";
        cout << "Customer Name: " << customer.customerName << endl;
        cout << "Car Model: " << customer.carModel << endl;
        cout << "Days Rented: " << customer.days << endl;
        cout << "Total Rental Fee: Rs " << customer.rentalFee << endl;
        cout << "Total Rental Fee(gst included):Rs " << customer.total * 1.18 << endl;
    }

    void displayAvailableCars() {
        cout << "\nAvailable Cars for Rent:\n";
        cout << left << setw(20) << "Model"
            << setw(15) << "Color"
            << setw(15) << "Speed"
            << setw(15) << "Daily Rate"
            << setw(20) << "Available" << endl;

        cout << setfill('-') << setw(85) << "-" << setfill(' ') << endl; // Separator line

        for (const auto& carBrand : carInventory) {
            for (const auto& car : carBrand.second) {
                if (car.modelCount > 0) {
                    cout << left << setw(20) << car.model
                        << setw(15) << car.color
                        << setw(15) << car.speed
                        << setw(15) << car.dailyRate
                        << setw(20) << car.modelCount << endl;
                }
            }
        }
    }

    void viewRentalHistory() {
        cout << "\nRental History:\n";
        if (rentalHistory.empty()) {
            cout << "No rental history available.\n";
        }
        else {
            for (const auto& entry : rentalHistory) {
                cout << "Name: " << entry.customerName
                    << ", Car Model: " << entry.carModel
                    << ", Days Rented: " << entry.days
                    << ", Total Rental Fee:Rs " << entry.total * 1.18 << endl;
            }
        }
    }

    void returnCar() {
        string carModel;
        cout << "Enter the car model you want to return: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, carModel);
        carModel = trim(carModel);

        auto it = find_if(rentedCars.begin(), rentedCars.end(), [&carModel](const Car& car) {
            return car.model == carModel;
            });

        if (it != rentedCars.end()) {
            // Return the car to the inventory
            for (auto& carBrand : carInventory) {
                auto& models = carBrand.second;
                for (auto& car : models) {
                    if (car.model == carModel) {
                        car.modelCount++; // Increment the available count
                        break;
                    }
                }
            }
            rentedCars.erase(it); // Remove from rented list
            cout << "Car returned successfully!\n";
        }
        else {
            cout << "You have not rented that car.\n";
        }
    }

    void viewAllRentals() {
        cout << "\nAll Rentals:\n";
        if (rentalHistory.empty()) {
            cout << "No rentals available.\n";
        }
        else {
            for (const auto& entry : rentalHistory) {
                cout << "Name: " << entry.customerName
                    << ", Car Model: " << entry.carModel
                    << ", Days Rented: " << entry.days
                    << ", Total Rental Fee: Rs " << entry.total * 1.18 << endl;
            }
        }
    }

    void addCarInventory() {
        string brand, model, color, speed;
        int dailyRate, modelCount;

        cout << "Enter the car brand (e.g., Tesla, Hyundai): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, brand);

        cout << "Enter the car model: ";
        getline(cin, model);

        cout << "Enter the car color: ";
        getline(cin, color);

        cout << "Enter the car speed: ";
        getline(cin, speed);

        cout << "Enter the daily rental rate: Rs";
        cin >> dailyRate;

        cout << "Enter the number of available models: ";
        cin >> modelCount;

        Car newCar = { model, color, speed, dailyRate, modelCount };
        carInventory[brand].push_back(newCar);

        cout << "Car added successfully to the inventory.\n";
    }

    void cancelRental() {
        string customerName;
        cout << "Enter the name of the customer whose rental you want to cancel: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, customerName);

        auto it = remove_if(rentalHistory.begin(), rentalHistory.end(),
            [&customerName](const Customer& customer) {
                return customer.customerName == customerName;
            });

        if (it != rentalHistory.end()) {
            // If found, restore the car count to inventory
            for (auto& customer : rentalHistory) {
                if (customer.customerName == customerName) {
                    for (auto& carBrand : carInventory) {
                        auto& models = carBrand.second;
                        for (auto& car : models) {
                            if (car.model == customer.carModel) {
                                car.modelCount++; // Increment the available count
                                break;
                            }
                        }
                    }
                }
            }

            rentalHistory.erase(it, rentalHistory.end()); // Remove the canceled rentals
            cout << "Rental for " << customerName << " canceled successfully.\n";
        }
        else {
            cout << "No rental found for customer: " << customerName << endl;
        }
    }

    void updateCustomerInfo() {
        string customerName;
        cout << "Enter the name of the customer whose info you want to update: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, customerName);

        auto it = find_if(rentalHistory.begin(), rentalHistory.end(),
            [&customerName](const Customer& customer) {
                return customer.customerName == customerName;
            });

        if (it != rentalHistory.end()) {
            cout << "Enter new name (leave empty to keep current): ";
            string newName;
            getline(cin, newName);
            if (!newName.empty()) {
                it->customerName = newName;
            }

            cout << "Enter new car model (leave empty to keep current): ";
            string newCarModel;
            getline(cin, newCarModel);
            if (!newCarModel.empty()) {
                // Update car model in rental history
                for (auto& carBrand : carInventory) {
                    auto& models = carBrand.second;
                    for (auto& car : models) {
                        if (car.model == it->carModel) {
                            car.modelCount++; // Increment the available count for the old model
                            break;
                        }
                    }
                }

                it->carModel = newCarModel; // Update to the new car model

                // Check for availability of the new car model
                bool found = false;
                for (auto& carBrand : carInventory) {
                    auto& models = carBrand.second;
                    auto carIt = find_if(models.begin(), models.end(),
                        [&newCarModel](const Car& car) {
                            return car.model == newCarModel;
                        });

                    if (carIt != models.end() && carIt->modelCount > 0) {
                        carIt->modelCount--; // Decrease the count for the new model
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cout << "Sorry, the new car model is not available. Please try again.\n";
                    it->carModel = it->carModel; // Revert to old car model
                }
            }

            cout << "Customer info updated successfully.\n";
        }
        else {
            cout << "No customer found with the name: " << customerName << endl;
        }
    }
};

int main() {
    Rent rentalSystem;
    rentalSystem.start();
    return 0;
}