#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>

using namespace std;

// Item structure
struct Item {
    int id;
    char name[50];
    char category[30];
    char description[100];
    char date[12];   // YYYY-MM-DD
    char location[50];
    char status[10]; // "Lost" or "Found"
    int matched;     // 0 = No, 1 = Yes
    int claimed;     // 0 = No, 1 = Yes
    int matchedItemID;
    char personName[50];
    char personContact[30];
};

// Resize dynamic array
void resizeArray(Item*& items, int& capacity) {
    int newCapacity = capacity * 2;
    Item* newItems = new Item[newCapacity]();
    for (int i = 0; i < capacity; i++)
        newItems[i] = items[i];
    delete[] items;
    items = newItems;
    capacity = newCapacity;
}

// Pause
void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Safe input for char arrays
void getInput(const char* prompt, char* input, int size, bool optional = false) {
    while (true) {
        cout << prompt;
        cin.getline(input, size);
        if (!optional && strlen(input) == 0)
            cout << "Input cannot be empty!\n";
        else
            break;
    }
}

// Validate date format YYYY-MM-DD
void getValidDate(const char* prompt, char* date) {
    bool valid;
    do {
        getInput(prompt, date, 12);
        valid = strlen(date) == 10 && date[4] == '-' && date[7] == '-';
        for (int i = 0; i < 10 && valid; i++) {
            if (i == 4 || i == 7) continue;
            if (date[i] < '0' || date[i] > '9') valid = false;
        }
        if (!valid) cout << "Invalid date format! Use YYYY-MM-DD.\n";
    } while (!valid);
}

// Save items safely to file
void saveToFile(Item* items, int itemCount, int nextID, const char* filename) {
    const char* tempFile = "temp.dat";
    ofstream out(tempFile, ios::binary);
    if (!out) {
        cout << "Error opening file for writing!\n";
        return;
    }
    out.write(reinterpret_cast<char*>(&nextID), sizeof(nextID));
    out.write(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
    out.write(reinterpret_cast<char*>(items), sizeof(Item) * itemCount);
    out.close();

    remove(filename);          // Delete old file
    rename(tempFile, filename); // Rename temp to real file
}

// Load items from file
void loadFromFile(Item* items, int& itemCount, int& nextID, const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        itemCount = 0;
        nextID = 100;
        return;
    }
    in.read(reinterpret_cast<char*>(&nextID), sizeof(nextID));
    in.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
    in.read(reinterpret_cast<char*>(items), sizeof(Item) * itemCount);
    in.close();
}

// Add Lost Item
void addLostItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    if (itemCount == capacity) resizeArray(items, capacity);

    Item newItem{};
    newItem.id = nextID++;
    getInput("Enter Item Name: ", newItem.name, 50);
    getInput("Enter Category: ", newItem.category, 30);
    getInput("Enter Description: ", newItem.description, 100);
    getValidDate("Enter Date Lost (YYYY-MM-DD): ", newItem.date);
    getInput("Enter Location Lost: ", newItem.location, 50);
    getInput("Enter Owner Name: ", newItem.personName, 50);
    getInput("Enter Owner Contact: ", newItem.personContact, 30);

    strcpy(newItem.status, "Lost");
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;
    saveToFile(items, itemCount, nextID, filename);

    cout << "\nLost item added! ID: " << newItem.id << endl;
    pause();
}

// Add Found Item
void addFoundItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    if (itemCount == capacity) resizeArray(items, capacity);

    Item newItem{};
    newItem.id = nextID++;
    getInput("Enter Item Name: ", newItem.name, 50);
    getInput("Enter Category: ", newItem.category, 30);
    getInput("Enter Description: ", newItem.description, 100);
    getValidDate("Enter Date Found (YYYY-MM-DD): ", newItem.date);
    getInput("Enter Location Found: ", newItem.location, 50);
    getInput("Enter Finder Name (Optional): ", newItem.personName, 50, true);
    getInput("Enter Finder Contact (Optional): ", newItem.personContact, 30, true);

    strcpy(newItem.status, "Found");
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;
    saveToFile(items, itemCount, nextID, filename);

    cout << "\nFound item added! ID: " << newItem.id << endl;
    pause();
}

// View items directly from file
void viewFromFile(const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cout << "No items found.\n";
        return;
    }

    int nextID = 0, itemCount = 0;
    if (!in.read(reinterpret_cast<char*>(&nextID), sizeof(nextID)) ||
        !in.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount)) || itemCount == 0) {
        cout << "No items to display.\n";
        return;
    }

    Item temp;
    cout << "\n========== ITEMS IN FILE ==========\n";
    for (int i = 0; i < itemCount; i++) {
        if (!in.read(reinterpret_cast<char*>(&temp), sizeof(Item))) break;
        cout << "ID: " << temp.id << "\n";
        cout << "Name: " << temp.name << "\n";
        cout << "Category: " << temp.category << "\n";
        cout << "Description: " << temp.description << "\n";
        cout << "Date: " << temp.date << "\n";
        cout << "Location: " << temp.location << "\n";
        cout << "Status: " << temp.status << "\n";
        cout << "Matched: " << (temp.matched ? "Yes" : "No") << "\n";
        cout << "Claimed: " << (temp.claimed ? "Yes" : "No") << "\n";
        if (temp.matchedItemID != -1)
            cout << "Matched With ID: " << temp.matchedItemID << "\n";
        cout << "Person: " << temp.personName << " | Contact: " << temp.personContact << "\n";
        cout << "------------------------------------\n";
    }
    in.close();
}

// Clear all items
void clearAllItems(Item* items, int& itemCount, int& nextID, const char* filename) {
    ofstream out(filename, ios::binary | ios::trunc);
    if (!out) {
        cout << "Error clearing file!\n";
        return;
    }
    out.close();

    itemCount = 0;
    nextID = 100;
    cout << "All items cleared successfully.\n";
}

// Main menu
// Main menu - safe input version
void mainMenu(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    int choice;
    do {
        cout << "\n====== LOST & FOUND MANAGER ======\n";
        cout << "1. Report Lost Item\n";
        cout << "2. Report Found Item\n";
        cout << "3. View Items\n";
        cout << "4. Clear All Items\n";
        cout << "5. Exit\n";
        cout << "Select an option: ";

        // Validate input
        if (!(cin >> choice)) {  // non-numeric input
            cout << "Invalid input! Please enter a number between 1 and 5.\n";
            cin.clear();  // clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            continue;     // re-display menu
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline left in buffer

        switch (choice) {
            case 1: 
                addLostItem(items, itemCount, capacity, nextID, filename); 
                break;
            case 2: 
                addFoundItem(items, itemCount, capacity, nextID, filename); 
                break;
            case 3:
                viewFromFile(filename);
                break;

            case 4: 
                clearAllItems(items, itemCount, nextID, filename); 
                break;
            case 5: 
                cout << "Exiting...\n"; 
                break;
            default: 
                cout << "Invalid choice! Please select a number between 1 and 5.\n";
        }

    } while (choice != 5);
}

// Main
int main() {
    const char* filename = "items.dat";

    int itemCount = 0, nextID = 100, capacity = 10;
    Item* items = new Item[capacity];

    loadFromFile(items, itemCount, nextID, filename);
    cout << "Items loaded: " << itemCount << ", Next ID: " << nextID << "\n";

    mainMenu(items, itemCount, capacity, nextID, filename);

    delete[] items;
    return 0;
}
