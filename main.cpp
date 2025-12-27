#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <cctype>




using namespace std;

// Item structure
struct Item {
    int id;
    string name;
    string category;
    string description;
    char date[12];   // YYYY-MM-DD
    string location;
    string status; // "Lost" or "Found"
    int matched;     // 0 = No, 1 = Yes
    int claimed;     // 0 = No, 1 = Yes
    int matchedItemID;
    string personName;
    string personContact;
};






// Helper Functions 

void pause() {
    cout << "\nPress Enter to continue...";
    while (cin.get() != '\n'); // keep reading until Enter is pressed
}

string toLowerCase(const string &s) {  // pass by const reference
    string result = s;                  // make a copy
    for (char &c : result)
        c = tolower(c);                 // modify the copy
    return result;                      // return the new string
}



bool containsSubstring(const string &str, const string &substr) {
    string lowerStr = toLowerCase(str);
    string lowerSub = toLowerCase(substr);
    return lowerStr.find(lowerSub) != string::npos;
}

// Get string input (required or optional)
void getInput(string &input, const string &prompt, bool optional = false) {
    while (true) {
        cout << prompt;
        getline(cin, input);

        if (!optional && input.empty()) {
            cout << "Input cannot be empty!\n";
        } else {
            break;
        }
    }
}

void getStatus(string &status) {
    while (true) {
        cout << "Enter status (Lost/Found): ";
        getline(cin, status);

        string temp = toLowerCase(status);
        if (temp == "lost" || temp == "found") {
            temp[0] = toupper(temp[0]); // capitalize first letter
            status = temp;
            break;
        } else {
            cout << "Invalid status! Only 'Lost' or 'Found' allowed.\n";
        }
    }
}

void getValidDate(const char* prompt, char* date) {
    string temp;
    while (true) {
        getInput(temp, prompt); // get string input

        // --- Check format YYYY-MM-DD ---
        if (temp.length() != 10 || temp[4] != '-' || temp[7] != '-') {
            cout << "Invalid date format! Use YYYY-MM-DD.\n";
            continue;
        }

        // --- Extract year, month, day ---
        int year = (temp[0]-'0')*1000 + (temp[1]-'0')*100 + (temp[2]-'0')*10 + (temp[3]-'0');
        int month = (temp[5]-'0')*10 + (temp[6]-'0');
        int day = (temp[8]-'0')*10 + (temp[9]-'0');

        // --- Validate month ---
        if (month < 1 || month > 12) {
            cout << "Invalid month! Must be 01-12.\n";
            continue;
        }

        // --- Validate day ---
        int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        // Leap year check
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            daysInMonth[1] = 29;

        if (day < 1 || day > daysInMonth[month-1]) {
            cout << "Invalid day for the given month!\n";
            continue;
        }

        // --- If all checks passed, copy to char array ---
        strncpy(date, temp.c_str(), 11); // 10 chars + null terminator
        date[11] = '\0';
        break; // exit loop
    }
}










//Display & Retrieval Helpers

void displayItem(const Item& item) {
    cout << "ID: " << item.id << "\n";
    cout << "Name: " << item.name << "\n";
    cout << "Category: " << item.category << "\n";
    cout << "Description: " << item.description << "\n";
    cout << "Date: " << item.date << "\n";
    cout << "Location: " << item.location << "\n";
    cout << "Status: " << item.status << "\n";
    cout << "Matched: " << (item.matched ? "Yes" : "No") << "\n";
    cout << "Claimed: " << (item.claimed ? "Yes" : "No") << "\n";

    if (item.matchedItemID != -1)
        cout << "Matched With ID: " << item.matchedItemID << "\n";

    cout << "Person: " << item.personName << " | Contact: " << item.personContact << "\n";
    cout << "------------------------------------\n";
}

void displayResults(Item items[], int results[], int count) {
    if (count == 0) {
        cout << "No items found matching criteria.\n";
        return;
    }

    cout << "\n========== SEARCH / FILTER RESULTS ==========\n";

    for (int i = 0; i < count; i++) {
        int idx;
        if (results != nullptr)
            idx = results[i];
        else
            idx = i;  // If results is nullptr, just display all items

        cout << "ID: " << items[idx].id << "\n";
        cout << "Name: " << items[idx].name << "\n";
        cout << "Category: " << items[idx].category << "\n";
        cout << "Description: " << items[idx].description << "\n";
        cout << "Date: " << items[idx].date << "\n";
        cout << "Location: " << items[idx].location << "\n";
        cout << "Status: " << items[idx].status << "\n";
        cout << "Matched: " << (items[idx].matched ? "Yes" : "No") << "\n";
        cout << "Claimed: " << (items[idx].claimed ? "Yes" : "No") << "\n";
        if (items[idx].matchedItemID != -1)
            cout << "Matched With ID: " << items[idx].matchedItemID << "\n";
        cout << "Person: " << items[idx].personName << " | Contact: " << items[idx].personContact << "\n";
        cout << "---------------------------------------------\n";
    }
}

Item* getItemByID(Item items[], int itemCount, int id) {
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id)
            return &items[i]; // return address of the matching item
    }
    return nullptr; // return nullptr if no item with the given ID is found
}




// Dynamic Array & Memory Management

void resizeArray(Item*& items, int& capacity) {
    int newCapacity = capacity * 2;
    Item* newItems = new Item[newCapacity]();
    for (int i = 0; i < capacity; i++)
        newItems[i] = items[i];
    delete[] items;
    items = newItems;
    capacity = newCapacity;
}







// File Operations


void saveToFile(fstream& file, Item* items, int itemCount, int nextID, const char* filename) {
    file.open(filename, ios::out | ios::binary);
    if (!file) {
        cout << "File can't be opened.\n";
        return;
    }

    // header
    file.write(reinterpret_cast<char*>(&nextID), sizeof(nextID));
    file.write(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    for (int i = 0; i < itemCount; i++) {

        // id
        file.write(reinterpret_cast<char*>(&items[i].id), sizeof(items[i].id));

        // name
        size_t len = items[i].name.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].name.c_str(), len);

        // category
        len = items[i].category.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].category.c_str(), len);

        // description
        len = items[i].description.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].description.c_str(), len);

        // date (fixed array)
        file.write(items[i].date, sizeof(items[i].date));

        // location
        len = items[i].location.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].location.c_str(), len);

        // status
        len = items[i].status.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].status.c_str(), len);

        // flags
        file.write(reinterpret_cast<char*>(&items[i].matched), sizeof(items[i].matched));
        file.write(reinterpret_cast<char*>(&items[i].claimed), sizeof(items[i].claimed));
        file.write(reinterpret_cast<char*>(&items[i].matchedItemID), sizeof(items[i].matchedItemID));

        // person name
        len = items[i].personName.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].personName.c_str(), len);

        // person contact
        len = items[i].personContact.length();
        file.write(reinterpret_cast<char*>(&len), sizeof(len));
        file.write(items[i].personContact.c_str(), len);
    }

    file.close();
}

void loadFromFile(fstream& file, Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    file.open(filename, ios::in | ios::binary);
    if (!file) {
        itemCount = 0;
        nextID = 100;
        return;
    }

    // Read header safely
    if (!file.read(reinterpret_cast<char*>(&nextID), sizeof(nextID)) ||
        !file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount))) {
        itemCount = 0;
        nextID = 100;
        file.close();
        return;
    }

    if (itemCount > capacity) {
        while (capacity < itemCount)
            capacity *= 2;

        delete[] items;
        items = new Item[capacity];
    }

    for (int i = 0; i < itemCount; i++) {

        // id
        file.read(reinterpret_cast<char*>(&items[i].id), sizeof(items[i].id));

        size_t len;
        char* buffer;

        // name
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].name.assign(buffer, len);
        delete[] buffer;

        // category
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].category.assign(buffer, len);
        delete[] buffer;

        // description
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].description.assign(buffer, len);
        delete[] buffer;

        // date
        file.read(items[i].date, sizeof(items[i].date));

        // location
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].location.assign(buffer, len);
        delete[] buffer;

        // status
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].status.assign(buffer, len);
        delete[] buffer;

        // flags
        file.read(reinterpret_cast<char*>(&items[i].matched), sizeof(items[i].matched));
        file.read(reinterpret_cast<char*>(&items[i].claimed), sizeof(items[i].claimed));
        file.read(reinterpret_cast<char*>(&items[i].matchedItemID), sizeof(items[i].matchedItemID));

        // person name
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].personName.assign(buffer, len);
        delete[] buffer;

        // person contact
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        items[i].personContact.assign(buffer, len);
        delete[] buffer;
    }

    file.close();
}


void viewFromFile(const char* filename, fstream& file) {
    int itemCount = 0;
    int capacity = 10;  // initial capacity
    int nextID = 100;

    Item* items = new Item[capacity];

    loadFromFile(file, items, itemCount, capacity, nextID, filename);

    if (itemCount == 0) {
        cout << "No items to display.\n";
        delete[] items;
        return;
    }

    cout << "\n========== ITEMS IN FILE ==========\n";
    for (int i = 0; i < itemCount; i++) {
        displayItem(items[i]);
    }

    delete[] items;
}

void clearAllItems(Item items[], int& itemCount, int& nextID, const char* filename) {
    char confirm;
    cout << "Are you sure you want to delete ALL items? (Y/N): ";
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear input buffer

    if (confirm == 'Y' || confirm == 'y') {
        ofstream out(filename, ios::binary | ios::trunc);
        if (!out) {
            cout << "Error clearing file!\n";
            return;
        }
        out.close();

        itemCount = 0;
        nextID = 100;
        cout << "All items cleared successfully.\n";
    } else {
        cout << "Operation cancelled. No items were deleted.\n";
    }
}










//Search & Filter Functions


int searchByName(Item items[], int itemCount, const string& name, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].name, name)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByCategory(Item items[], int itemCount, const string& category, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].category, category)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByDescription(Item items[], int itemCount, const string& description, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].description, description)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByLocation(Item items[], int itemCount, const string& location, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].location, location)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByStatus(Item items[], int itemCount, const string& status, int results[]) {
    int count = 0;
    string lowerStatus = toLowerCase(status);

    for (int i = 0; i < itemCount; i++) {
        if (toLowerCase(items[i].status) == lowerStatus) {
            results[count++] = i;
        }
    }
    return count;
}

int filterByMatched(Item items[], int itemCount, int matchedValue, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].matched == matchedValue) {
            results[count++] = i;
        }
    }
    return count;
}

int filterByClaimed(Item items[], int itemCount, int claimedValue, int results[]) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].claimed == claimedValue) {
            results[count++] = i;
        }
    }
    return count;
}









// Filter/search menu

void filterSearchMenu(Item items[], int itemCount) {
    int choice;
    string input;

    int* results = new int[itemCount]; // dynamic array for search results

    do {
        cout << "\n--- Filter / Search Items ---\n";
        cout << "1. By Name\n2. By Category\n3. By Description\n4. By Location\n";
        cout << "5. By Status\n6. By Matched / Unmatched\n7. By Claimed / Unclaimed\n";
        cout << "8. Back to Main Menu\n";
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int count = 0;

        switch (choice) {
            case 1:
                getInput(input, "Enter name: ");
                count = searchByName(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 2:
                getInput(input, "Enter category: ");
                count = searchByCategory(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 3:
                getInput(input, "Enter description: ");
                count = searchByDescription(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 4:
                getInput(input, "Enter location: ");
                count = searchByLocation(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 5:
                getInput(input, "Enter status (Lost/Found): ");
                count = searchByStatus(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 6: {
                int m;
                cout << "1. Matched\n2. Unmatched\nSelect: ";
                if (!(cin >> m)) {
                    cout << "Invalid input! Please enter 1 or 2.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                count = filterByMatched(items, itemCount, m == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }

            case 7: {
                int c;
                cout << "1. Claimed\n2. Unclaimed\nSelect: ";
                if (!(cin >> c)) {
                    cout << "Invalid input! Please enter 1 or 2.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                count = filterByClaimed(items, itemCount, c == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }

            case 8:
                delete[] results;
                return;

            default:
                cout << "Invalid choice! Please select 1-8.\n";
        }

    } while (true);
}






//Matching System

int* findPotentialMatches(Item items[], int itemCount, const Item& newItem, int& matchCount) {
    int* matchIndices = new int[itemCount];

    for (int i = 0; i < itemCount; i++) {
        // Skip already matched items or same status
        if (items[i].matched == 1 || items[i].status == newItem.status)
            continue;

        // Use case-insensitive matching for strings
        bool match =
            containsSubstring(items[i].name, newItem.name) ||
            containsSubstring(newItem.name, items[i].name) ||
            containsSubstring(items[i].category, newItem.category) ||
            containsSubstring(newItem.category, items[i].category) ||
            containsSubstring(items[i].description, newItem.description) ||
            containsSubstring(newItem.description, items[i].description) ||
            containsSubstring(items[i].location, newItem.location) ||
            containsSubstring(newItem.location, items[i].location);

        if (match) {
            matchIndices[matchCount++] = i;
        }
    }

    if (matchCount == 0) {
        delete[] matchIndices;
        return nullptr;
    }

    return matchIndices;
}





void markAsMatched(Item& item1, Item& item2) {
    item1.matched = 1;
    item2.matched = 1;
    item1.matchedItemID = item2.id;
    item2.matchedItemID = item1.id;

    cout << "Items matched successfully!\n";
    cout << "Item " << item1.id << " matched with Item " << item2.id << "\n";
}

bool markMatchByID(Item items[], int itemCount, Item& newItem, int matchID) {

    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == matchID) {
            markAsMatched(newItem, items[i]);
            return true;
        }
    }
    return false; // ID not found
}

void displayMatches(Item items[], int matchIndices[], int matchCount) {
    if (matchCount == 0 || matchIndices == nullptr) {
        cout << "No potential matches found.\n";
        return;
    }

    for (int i = 0; i < matchCount; i++) {
        cout << "\n--- Potential Match " << i + 1 << " ---\n";
        displayItem(items[matchIndices[i]]);
    }
}








//Add Item Operations

void searchForMatches(Item*& items, int itemCount, Item& newItem, int& nextID, const char* filename,fstream& file) {
    char searchChoice;
    while (true) {
        cout << "Do you want to search for matching items now? (Y/N): ";
        cin >> searchChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        searchChoice = tolower(searchChoice);
        if (searchChoice == 'y' || searchChoice == 'n') break;
        cout << "Invalid input. Please enter Y or N.\n";
    }

    if (searchChoice == 'y') {
        int matchCount = 0;
        int* matchIndices = findPotentialMatches(items, itemCount - 1, newItem, matchCount);

        displayMatches(items, matchIndices, matchCount);

        if (matchCount > 0) {
            int choice;
            while (true) {
                cout << "Enter the ID of the item to mark as matched (0 to skip): ";
                if (!(cin >> choice)) {
                    cout << "Invalid input. Enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            bool valid = false;
            for (int i = 0; i < matchCount; i++) {
                if (items[matchIndices[i]].id == choice) {
                    valid = true;
                    break;
                }
            }

            if (choice > 0 && valid) {
                if (markMatchByID(items, itemCount, newItem, choice)) {
                    saveToFile(file, items, itemCount, nextID, filename);
                    cout << "Items marked as matched successfully!\n";
                }
            } else if (choice != 0) {
                cout << "Invalid match ID.\n";
            }
        }

        delete[] matchIndices;
    }
}





void addLostItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename,fstream &file) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

    Item newItem{};
    newItem.id = nextID++;

    getInput(newItem.name, "Enter Item Name: ");
    getInput(newItem.category, "Enter Category: ");
    getInput(newItem.description, "Enter Description: ");
    getValidDate("Enter Date Found (YYYY-MM-DD): ", newItem.date);
    getInput(newItem.location, "Enter Location Found: ");
    getInput(newItem.personName, "Enter owner Name : ", true);
    getInput(newItem.personContact, "Enter owner Contact : ", true);


    newItem.status = "Lost";// keep char[] for status if needed
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;
    saveToFile(file, items, itemCount, nextID, filename);

    cout << "\nLost item added! ID: " << newItem.id << "\n";

    // Call the new match search function
    searchForMatches(items, itemCount, items[itemCount - 1], nextID, filename,file);
    pause();
}

void addFoundItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename,fstream &file) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

    Item newItem{};
    newItem.id = nextID++;

    getInput(newItem.name, "Enter Item Name: ");
    getInput(newItem.category, "Enter Category: ");
    getInput(newItem.description, "Enter Description: ");
    getValidDate("Enter Date Found (YYYY-MM-DD): ", newItem.date);
    getInput(newItem.location, "Enter Location Found: ");
    getInput(newItem.personName, "Enter Finder Name (Optional): ", true);
    getInput(newItem.personContact, "Enter Finder Contact (Optional): ", true);


    newItem.status = "Found";
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;

    saveToFile(file, items, itemCount, nextID, filename);

    cout << "\nFound item added! ID: " << newItem.id << "\n";

    // Reuse the search function
    searchForMatches(items, itemCount, items[itemCount - 1], nextID, filename,file);

    pause();
}









//Update Operations

void updateItemMenu(Item* item) {
    int choice;

    do {
        cout << "\n--- Update Menu for Item ID: " << item->id << " ---\n";
        cout << "1. Name\n";
        cout << "2. Category\n";
        cout << "3. Description\n";
        cout << "4. Date\n";
        cout << "5. Location\n";
        cout << "6. Person Name\n";
        cout << "7. Person Contact\n";
        cout << "8. Update All Fields\n";
        cout << "9. Return to Main Menu\n";
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                getInput(item->name, "New Name: ");
                break;
            case 2:
                getInput(item->category, "New Category: ");
                break;
            case 3:
                getInput(item->description, "New Description: ");
                break;
            case 4:
                getValidDate("New Date (YYYY-MM-DD): ", item->date);
                break;
            case 5:
                getInput(item->location, "New Location: ");
                break;
            case 6:
                getInput(item->personName, "New Person Name (optional): ", true);
                break;
            case 7:
                getInput(item->personContact, "New Person Contact (optional): ", true);
                break;
            case 8: // Update all fields
                getInput(item->name, "New Name: ");
                getInput(item->category, "New Category: ");
                getInput(item->description, "New Description: ");
                getValidDate("New Date (YYYY-MM-DD): ", item->date);
                getInput(item->location, "New Location: ");
                getInput(item->personName, "New Person Name (optional): ", true);
                getInput(item->personContact, "New Person Contact (optional): ", true);
                break;
            case 9:
                return; // exit menu
            default:
                cout << "Invalid option. Please select 1-9.\n";
        }

    } while (true);
}

void updateItem(Item items[], int itemCount, const char* filename, int nextID,fstream &file) {
    if (itemCount == 0) {
        cout << "No items available to update.\n";
        return;
    }

    int id;
    cout << "Enter the ID of the item to update: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Item* item = nullptr;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            item = &items[i];
            break;
        }
    }

    if (!item) {
        cout << "Item with ID " << id << " not found.\n";
        return;
    }

    displayItem(*item); // Show current details
    updateItemMenu(item); // Let user update fields
    saveToFile(file, items, itemCount, nextID, filename);
    cout << "Item updated successfully!\n";
}








//Delete & Claim Operations

void deleteItem(Item*& items, int& itemCount, int& nextID, const char* filename,fstream& file) {
    if (itemCount == 0) {
        cout << "No items available to delete.\n";
        return;
    }

    int id;
    cout << "Enter the ID of the item to delete: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int index = -1;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        cout << "Item with ID " << id << " not found.\n";
        return;
    }

    // Confirm deletion
    char confirm;
    cout << "Are you sure you want to delete this item? (Y/N): ";
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (confirm != 'Y' && confirm != 'y') {
        cout << "Deletion cancelled.\n";
        return;
    }

    // Shift items down to fill the gap
    for (int i = index; i < itemCount - 1; i++) {
        items[i] = items[i + 1];
    }
    itemCount--;

    // Save updated array to file
    saveToFile(file, items, itemCount, nextID, filename);


    cout << "Item deleted successfully!\n";
}

void markAsClaimed(Item items[], int itemCount, const char* filename, int nextID,fstream& file) {
    int id;
    cout << "Enter the ID of the item to mark as claimed: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Item* item = nullptr;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            item = &items[i];
            break;
        }
    }

    if (!item) {
        cout << "Item not found.\n";
        return;
    }

    if (item->matched == 0) {
        cout << "Item cannot be claimed because it is not matched yet.\n";
        return;
    }

    if (item->claimed == 1) {
        cout << "Item is already claimed.\n";
        return;
    }

    item->claimed = 1;

    // Optionally mark the matched item as claimed too
    if (item->matchedItemID != -1) {
        Item* matchedItem = getItemByID(items, itemCount, item->matchedItemID);
        if (matchedItem) matchedItem->claimed = 1;
    }

    saveToFile(file, items, itemCount, nextID, filename);
    cout << "Item marked as claimed successfully.\n";
}








//Sorting System

void swapItems(Item& a, Item& b) {
    Item temp = a;
    a = b;
    b = temp;
}

void sortByID(Item* items, int itemCount, bool ascending = true) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && items[j].id > items[j + 1].id) ||
                (!ascending && items[j].id < items[j + 1].id)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByName(Item* items, int itemCount, bool ascending = true) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && items[j].name > items[j + 1].name) ||
                (!ascending && items[j].name < items[j + 1].name)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}


void sortByDate(Item* items, int itemCount, bool ascending = true) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if ((ascending && strcmp(items[j].date, items[j + 1].date) > 0) ||
                (!ascending && strcmp(items[j].date, items[j + 1].date) < 0)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortByStatus(Item* items, int itemCount, bool lostFirst = true) {
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            // Assuming "Lost" < "Found" in alphabetical order
            if ((lostFirst && items[j].status > items[j + 1].status) ||
                (!lostFirst && items[j].status < items[j + 1].status)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}


void sortMenu(Item items[], int itemCount, const char* filename, int nextID,fstream& file) {
    int choice;
    int order;
    do {
        cout << "\n--- Sort Items ---\n";
        cout << "1. By ID\n";
        cout << "2. By Name\n";
        cout << "3. By Date\n";
        cout << "4. By Status\n";
        cout << "5. Back to Main Menu\n";
        cout << "Select an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice >= 1 && choice <= 3) {
            cout << "1. Ascending\n2. Descending\nSelect order: ";
            cin >> order;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1:
                sortByID(items, itemCount, order == 1);
                break;
            case 2:
                sortByName(items, itemCount, order == 1);
                break;
            case 3:
                sortByDate(items, itemCount, order == 1);
                break;
            case 4:
                cout << "1. Lost First\n2. Found First\nSelect: ";
                cin >> order;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                sortByStatus(items, itemCount, order == 1);
                break;
            case 5:
                return;
            default:
                cout << "Invalid choice!\n";
                continue;
        }

        saveToFile(file, items, itemCount, nextID, filename);
        cout << "Items sorted successfully!\n";
        displayResults(items, nullptr, itemCount); // display all items

    } while (true);
}







//Main Menu Controller
void mainMenu(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename,fstream & file) {
    int choice;

    do {
        cout << "\n====== LOST & FOUND MANAGER ======\n";
        cout << "1. Report Lost Item\n";
        cout << "2. Report Found Item\n";
        cout << "3. View Items\n";
        cout << "4. Update Item\n";
        cout << "5. Filter / Search Items\n";
        cout << "6. Delete Item\n";
        cout << "7. Mark Item as Claimed\n";
        cout << "8. Sort Items\n";
        cout << "9. Clear All Items\n";
        cout << "10. Exit\n";
        cout << "Select an option (1-10): ";

        cin >> choice;

        // Validate input
        if (cin.fail() || choice < 1 || choice > 10) {
            cout << "Invalid input! Please enter a number between 1 and 10.\n";
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            continue; // Ask again
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Remove leftover newline

        switch (choice) {
            case 1:
                addLostItem(items, itemCount, capacity, nextID, filename,file);
                break;
            case 2:
                addFoundItem(items, itemCount, capacity, nextID, filename,file);
                break;
            case 3:
                viewFromFile(filename,file);
                break;
            case 4:
                updateItem(items, itemCount, filename, nextID,file);
                break;
            case 5:
                filterSearchMenu(items, itemCount);
                break;
            case 6:
                deleteItem(items, itemCount, nextID, filename,file);
                break;
            case 7:
                markAsClaimed(items, itemCount, filename, nextID,file);
                break;
            case 8:
                sortMenu(items, itemCount, filename, nextID,file);
                break;
            case 9:
                clearAllItems(items, itemCount, nextID, filename);
                break;
            case 10:
                cout << "Exiting...\n";
                break;
        }

    } while (choice != 10);
}






int main() {
    fstream file;
    const char* filename = "items.bin";

    int itemCount = 0, nextID = 100, capacity = 10;
    Item* items = new Item[capacity];

    // Correct call
    loadFromFile(file, items, itemCount, capacity, nextID, filename);

    cout << "Items loaded: " << itemCount << ", Next ID: " << nextID << "\n";

    mainMenu(items, itemCount, capacity, nextID, filename, file);

    delete[] items;
    return 0;
}

