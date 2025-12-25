#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
#include <cctype>




using namespace std;

// Item structure
struct Item {
    int id;
    char name[51];
    char category[51];
    char description[201];
    char date[12];   // YYYY-MM-DD
    char location[61];
    char status[10]; // "Lost" or "Found"
    int matched;     // 0 = No, 1 = Yes
    int claimed;     // 0 = No, 1 = Yes
    int matchedItemID;
    char personName[51];
    char personContact[51];
};






// Helper Functions 

void pause() {
    cout << "\nPress Enter to continue...";
    while (cin.get() != '\n'); // keep reading until Enter is pressed
}

void toLowerCase(const char* src, char* dest) {
    int i = 0;
    while (src[i]) {
        dest[i] = tolower(src[i]);
        i++;
    }
    dest[i] = '\0';
}

bool containsSubstring(const char* str, const char* substr) {
    char lowerStr[200];
    char lowerSub[200];

    toLowerCase(str, lowerStr);
    toLowerCase(substr, lowerSub);

    return strstr(lowerStr, lowerSub) != nullptr;
}

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


void getValidDate(const char* prompt, char* date) {
    while (true) {
        getInput(prompt, date, 12);

        // --- Check format YYYY-MM-DD ---
        if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
            cout << "Invalid date format! Use YYYY-MM-DD.\n";
            continue;
        }

        bool formatValid = true;
        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue; // skip dashes
            if (!isdigit(date[i])) {
                formatValid = false;
                break;
            }
        }

        if (!formatValid) {
            cout << "Invalid date format! Use YYYY-MM-DD.\n";
            continue;
        }

        // --- Check valid calendar date ---
        int year = (date[0]-'0')*1000 + (date[1]-'0')*100 + (date[2]-'0')*10 + (date[3]-'0');
        int month = (date[5]-'0')*10 + (date[6]-'0');
        int day = (date[8]-'0')*10 + (date[9]-'0');

        if (month < 1 || month > 12) {
            cout << "Invalid month! Must be 01-12.\n";
            continue;
        }

        int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        // Leap year check
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            daysInMonth[1] = 29;

        if (day < 1 || day > daysInMonth[month-1]) {
            cout << "Invalid day for the given month!\n";
            continue;
        }

        // --- If all checks passed ---
        break;
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

void displayResults(Item* items, int* results, int count) {
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

void loadFromFile(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        itemCount = 0;
        nextID = 100;
        return;
    }

    in.read(reinterpret_cast<char*>(&nextID), sizeof(nextID));
    in.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    if (itemCount > capacity) {
        while (capacity < itemCount)
            capacity *= 2;

        delete[] items;
        items = new Item[capacity];
    }

    in.read(reinterpret_cast<char*>(items), sizeof(Item) * itemCount);
    in.close();
}

void viewFromFile(const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cout << "No items found.\n";
        return;
    }

    int nextID = 0, itemCount = 0;
    if (!in.read(reinterpret_cast<char*>(&nextID), sizeof(nextID)) ||
        !in.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount)) ||
        itemCount == 0) {
        cout << "No items to display.\n";
        in.close();
        return;
    }

    Item temp;
    cout << "\n========== ITEMS IN FILE ==========\n";
    for (int i = 0; i < itemCount; i++) {
        if (!in.read(reinterpret_cast<char*>(&temp), sizeof(Item)))
            break;

        displayItem(temp);
    }

    in.close();
}

void clearAllItems(Item* items, int& itemCount, int& nextID, const char* filename) {
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

int searchByName(Item* items, int itemCount, const char* name, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].name, name)) {
            results[count++] = i;
        }
    }
    return count; // number of matches
}

int searchByCategory(Item* items, int itemCount, const char* category, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].category, category)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByDescription(Item* items, int itemCount, const char* description, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].description, description)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByLocation(Item* items, int itemCount, const char* location, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (containsSubstring(items[i].location, location)) {
            results[count++] = i;
        }
    }
    return count;
}

int searchByStatus(Item* items, int itemCount, const char* status, int* results) {
    int count = 0;
    char lowerStatus[10];
    toLowerCase(status, lowerStatus);

    for (int i = 0; i < itemCount; i++) {
        char itemStatus[10];
        toLowerCase(items[i].status, itemStatus);

        if (strcmp(itemStatus, lowerStatus) == 0) {
            results[count++] = i;
        }
    }
    return count;
}

int filterByMatched(Item* items, int itemCount, int matchedValue, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].matched == matchedValue) {
            results[count++] = i;
        }
    }
    return count;
}

int filterByClaimed(Item* items, int itemCount, int claimedValue, int* results) {
    int count = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].claimed == claimedValue) {
            results[count++] = i;
        }
    }
    return count;
}









// Filter/search menu

void filterSearchMenu(Item* items, int itemCount) {
    int choice;
    char input[100];

    int* results = new int[itemCount]; // dynamic

    do {
        cout << "\n--- Filter / Search Items ---\n";
        cout << "1. By Name\n2. By Category\n3. By Description\n4. By Location\n";
        cout << "5. By Status\n6. By Matched / Unmatched\n7. By Claimed / Unclaimed\n";
        cout << "8. Back to Main Menu\n";
        cout << "Select an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int count = 0;

        switch (choice) {
            case 1:
                getInput("Enter name: ", input, 50);
                count = searchByName(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 2:
                getInput("Enter category: ", input, 30);
                count = searchByCategory(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 3:
                getInput("Enter description: ", input, 100);
                count = searchByDescription(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 4:
                getInput("Enter location: ", input, 50);
                count = searchByLocation(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 5:
                getInput("Enter status (Lost/Found): ", input, 10);
                count = searchByStatus(items, itemCount, input, results);
                displayResults(items, results, count);
                break;

            case 6: {
                int m;
                cout << "1. Matched\n2. Unmatched\nSelect: ";
                cin >> m;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                count = filterByMatched(items, itemCount, m == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }

            case 7: {
                int c;
                cout << "1. Claimed\n2. Unclaimed\nSelect: ";
                cin >> c;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                count = filterByClaimed(items, itemCount, c == 1 ? 1 : 0, results);
                displayResults(items, results, count);
                break;
            }

            case 8:
                delete[] results;
                return;

            default:
                cout << "Invalid choice!\n";
        }

    } while (true);
}








//Matching System

int* findPotentialMatches(Item* items, int itemCount, const Item& newItem, int& matchCount) {
    matchCount = 0;
    int* matchIndices = new int[itemCount];

    for (int i = 0; i < itemCount; i++) {
        // Skip already matched items or same status
        if (items[i].matched == 1 || strcmp(items[i].status, newItem.status) == 0)
            continue;

        // Use case-insensitive matching
        bool match =
            containsSubstring(items[i].name, newItem.name) ||
            containsSubstring(newItem.name, items[i].name) ||
            containsSubstring(items[i].category, newItem.category) ||
            containsSubstring(newItem.category, items[i].category) ||
            containsSubstring(items[i].description, newItem.description) ||
            containsSubstring(newItem.description, items[i].description) ||
            containsSubstring(items[i].location, newItem.location) ||
            containsSubstring(newItem.location, items[i].location) ||
            strcmp(items[i].date, newItem.date) == 0;

        if (match)
            matchIndices[matchCount++] = i;
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

bool markMatchByID(Item* items, int itemCount, Item& newItem, int matchID) {

    for (int i = 0; i < itemCount; i++) {
        if (items[i].id == matchID) {
            markAsMatched(newItem, items[i]);
            return true;
        }
    }
    return false; // ID not found
}

void displayMatches(Item* items, int* matchIndices, int matchCount) {
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

void addLostItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

    Item newItem{};
    newItem.id = nextID++;

    getInput("Enter Item Name (max 50 chars): ", newItem.name, 51);
    getInput("Enter Category (max 50 chars): ", newItem.category, 51);
    getInput("Enter Description (max 200 chars): ", newItem.description, 201);
    getValidDate("Enter Date Lost (YYYY-MM-DD): ", newItem.date);
    getInput("Enter Location Lost (max 60 chars): ", newItem.location, 61);
    getInput("Enter Owner Name (max 50 chars): ", newItem.personName, 51);
    getInput("Enter Owner Contact (max 50 chars): ", newItem.personContact, 51);


    strcpy(newItem.status, "Lost");
    newItem.matched = 0;
    newItem.claimed = 0;
    newItem.matchedItemID = -1;

    items[itemCount++] = newItem;
    saveToFile(items, itemCount, nextID, filename);

    cout << "\nLost item added! ID: " << newItem.id << "\n";

    char searchChoice;
    cout << "Do you want to search for matching Found items now? (Y/N): ";
    cin >> searchChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (searchChoice == 'Y' || searchChoice == 'y') {
        int matchCount = 0;
        Item& storedItem = items[itemCount - 1];

        int* matchIndices = findPotentialMatches(items, itemCount - 1, storedItem, matchCount);

        displayMatches(items, matchIndices, matchCount);

        if (matchCount > 0) {
            int choice;
            cout << "Enter the ID of the item to mark as matched (0 to skip): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            bool valid = false;
            for (int i = 0; i < matchCount; i++) {
                if (items[matchIndices[i]].id == choice) {
                    valid = true;
                    break;
                }
            }

            if (choice > 0 && valid) {
                if (markMatchByID(items, itemCount, storedItem, choice)) {
                    saveToFile(items, itemCount, nextID, filename);
                    cout << "Items marked as matched successfully!\n";
                }
            } else if (choice != 0) {
                cout << "Invalid match ID.\n";
            }
        }

        if (matchIndices)
            delete[] matchIndices;
    }

    pause();
}

void addFoundItem(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
    if (itemCount == capacity)
        resizeArray(items, capacity);

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

    cout << "\nFound item added! ID: " << newItem.id << "\n";

    char searchChoice;
    cout << "Do you want to search for matching Lost items now? (Y/N): ";
    cin >> searchChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (searchChoice == 'Y' || searchChoice == 'y') {
        int matchCount = 0;
        Item& storedItem = items[itemCount - 1];

        int* matchIndices = findPotentialMatches(items, itemCount - 1, storedItem, matchCount);

        displayMatches(items, matchIndices, matchCount);

        if (matchCount > 0) {
            int choice;
            cout << "Enter the ID of the item to mark as matched (0 to skip): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            bool valid = false;
            for (int i = 0; i < matchCount; i++) {
                if (items[matchIndices[i]].id == choice) {
                    valid = true;
                    break;
                }
            }

            if (choice > 0 && valid) {
                if (markMatchByID(items, itemCount, storedItem, choice)) {
                    saveToFile(items, itemCount, nextID, filename);
                    cout << "Items marked as matched successfully!\n";
                }
            } else if (choice != 0) {
                cout << "Invalid match ID.\n";
            }
        }

        if (matchIndices)
            delete[] matchIndices;
    }

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
            cout << "Invalid input!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: getInput("New Name: ", item->name, 50); break;
            case 2: getInput("New Category: ", item->category, 30); break;
            case 3: getInput("New Description: ", item->description, 100); break;
            case 4: getValidDate("New Date (YYYY-MM-DD): ", item->date); break;
            case 5: getInput("New Location: ", item->location, 50); break;
            case 6: getInput("New Person Name: ", item->personName, 50, true); break;
            case 7: getInput("New Person Contact: ", item->personContact, 30, true); break;
            case 8:
                getInput("New Name: ", item->name, 50);
                getInput("New Category: ", item->category, 30);
                getInput("New Description: ", item->description, 100);
                getValidDate("New Date (YYYY-MM-DD): ", item->date);
                getInput("New Location: ", item->location, 50);
                getInput("New Person Name: ", item->personName, 50, true);
                getInput("New Person Contact: ", item->personContact, 30, true);
                break;
            case 9:
                return; // exit menu
            default:
                cout << "Invalid option.\n";
        }

    } while (true);
}

void updateItem(Item* items, int itemCount, const char* filename, int nextID) {
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
    saveToFile(items, itemCount, nextID, filename);
    cout << "Item updated successfully!\n";
}








//Delete & Claim Operations

void deleteItem(Item*& items, int& itemCount, int& nextID, const char* filename) {
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
    saveToFile(items, itemCount, nextID, filename);

    cout << "Item deleted successfully!\n";
}

void markAsClaimed(Item* items, int itemCount, const char* filename, int nextID) {
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

    saveToFile(items, itemCount, nextID, filename);
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
            if ((ascending && strcmp(items[j].name, items[j + 1].name) > 0) ||
                (!ascending && strcmp(items[j].name, items[j + 1].name) < 0)) {
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
            if ((lostFirst && strcmp(items[j].status, items[j + 1].status) > 0) ||
                (!lostFirst && strcmp(items[j].status, items[j + 1].status) < 0)) {
                swapItems(items[j], items[j + 1]);
            }
        }
    }
}

void sortMenu(Item* items, int itemCount, const char* filename, int nextID) {
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

        saveToFile(items, itemCount, nextID, filename);
        cout << "Items sorted successfully!\n";
        displayResults(items, nullptr, itemCount); // display all items

    } while (true);
}







//Main Menu Controller
void mainMenu(Item*& items, int& itemCount, int& capacity, int& nextID, const char* filename) {
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
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number between 1 and 10.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
                updateItem(items, itemCount, filename, nextID);
                break;
            case 5:
                filterSearchMenu(items, itemCount);
                break;
            case 6:
                deleteItem(items, itemCount, nextID, filename); // fixed argument order
                break;
            case 7:
                markAsClaimed(items, itemCount, filename, nextID);
                break;
            case 8:
                sortMenu(items, itemCount, filename, nextID);
                break;
            case 9:
                clearAllItems(items, itemCount, nextID, filename);
                break;
            case 10:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Please select a number between 1 and 10.\n";
        }

    } while (choice != 10);
}








int main() {
    const char* filename = "items.dat";

    int itemCount = 0, nextID = 100, capacity = 10;
    Item* items = new Item[capacity];

    loadFromFile(items, itemCount, capacity, nextID, filename);

    cout << "Items loaded: " << itemCount << ", Next ID: " << nextID << "\n";

    mainMenu(items, itemCount, capacity, nextID, filename);

    delete[] items;
    return 0;
}
