#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <memory>

using namespace std;

// --- Enums and Constants ---

enum class Rarity {
    BLUE,
    PURPLE,
    PINK,
    RED,
    GOLD
};

string rarityToString(Rarity rarity) {
    switch (rarity) {
        case Rarity::BLUE: return "[BLUE] Costmmon";
        case Rarity::PURPLE: return "[PURPLE] Restricted";
        case Rarity::PINK: return "[PINK] Classified";
        case Rarity::RED: return "[RED] Covert";
        case Rarity::GOLD: return "[GOLD] Rare Special Item";
        default: return "Unknown";
    }
}

string getRarityColorCode(Rarity rarity) {
    // Basic ANSI colors for terminals that support it, 
    // or just textual tags as requested if colors aren't reliable.
    // User asked for textual tags like [BLUE], but we can add codes if we want.
    // Strict requirement: "Użyj kolorowych oznaczeń (np. tekstowo: [BLUE], [GOLD])"
    return rarityToString(rarity);
}

// --- Classes ---

class Item {
private:
    string name;
    Rarity rarity;
    double value;

public:
    Item(string n, Rarity r, double v) : name(n), rarity(r), value(v) {}

    string getName() const { return name; }
    Rarity getRarity() const { return rarity; }
    double getValue() const { return value; }

    string getDisplayName() const {
        return rarityToString(rarity) + " " + name + " ($" + to_string(value).substr(0, to_string(value).find(".")+3) + ")";
    }
};

class Case {
private:
    string name;
    double price;
    vector<Item> possibleItems;

public:
    Case(string n, double p) : name(n), price(p) {}

    void addItem(const Item& item) {
        possibleItems.push_back(item);
    }

    string getName() const { return name; }
    double getPrice() const { return price; }
    const vector<Item>& getItems() const { return possibleItems; }
};

class Inventory {
private:
    vector<Item> items;

public:
    void addItem(const Item& item) {
        items.push_back(item);
    }

    void showInventory() const {
        if (items.empty()) {
            cout << "Ekwipunek jest pusty." << endl;
            return;
        }

        cout << "--- TWOJ EKWIPUNEK ---" << endl;
        double totalValue = 0;
        for (const auto& item : items) {
            cout << item.getDisplayName() << endl;
            totalValue += item.getValue();
        }
        cout << "----------------------" << endl;
        cout << "Laczna wartosc: $" << fixed << setprecision(2) << totalValue << endl;
    }

    void removeItem(int index) {
        if (index >= 0 && index < items.size()) {
            items.erase(items.begin() + index);
        }
    }

    const vector<Item>& getItemsVector() const { return items; }
};

class User {
private:
    string username;
    string password;
    double balance;
    Inventory inventory;

public:
    User() : balance(0.0) {}
    User(string u, string p, double b) : username(u), password(p), balance(b) {}

    bool checkPassword(const string& p) const {
        return password == p;
    }

    string getUsername() const { return username; }
    double getBalance() const { return balance; }

    void addBalance(double amount) {
        if (amount > 0) balance += amount;
    }

    bool subtractBalance(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    void addItemToInventory(const Item& item) {
        inventory.addItem(item);
    }

    void showInventory() const {
        inventory.showInventory();
    }

    const vector<Item>& getInventoryItems() const {
        return inventory.getItemsVector();
    }

    void sellItem(int index) {
        const auto& items = inventory.getItemsVector();
        if (index >= 0 && index < items.size()) {
            double val = items[index].getValue();
            addBalance(val);
            inventory.removeItem(index);
            cout << "Sprzedano przedmiot za $" << fixed << setprecision(2) << val << endl;
        }
    }
};

class GameManager {
private:
    map<string, User> users;
    vector<Case> cases;
    User* currentUser;
    mt19937 rng;

    void initCases() {
        // 1. Chroma Case
        Case chroma("Chroma Case", 2.50);
        chroma.addItem(Item("Glock-18 | Catacombs", Rarity::BLUE, 0.10));
        chroma.addItem(Item("M249 | System Lock", Rarity::BLUE, 0.10));
        chroma.addItem(Item("MP9 | Deadly Poison", Rarity::BLUE, 0.12));
        chroma.addItem(Item("SCAR-20 | Grotto", Rarity::BLUE, 0.15));
        chroma.addItem(Item("XM1014 | Quicksilver", Rarity::BLUE, 0.15));
        
        chroma.addItem(Item("Dual Berettas | Urban Shock", Rarity::PURPLE, 0.50));
        chroma.addItem(Item("Desert Eagle | Naga", Rarity::PURPLE, 0.80));
        chroma.addItem(Item("MAC-10 | Malachite", Rarity::PURPLE, 1.20));
        
        chroma.addItem(Item("AK-47 | Cartel", Rarity::PINK, 8.50));
        chroma.addItem(Item("M4A4 | Dragon King", Rarity::PINK, 12.00));
        
        chroma.addItem(Item("AWP | Man-o'-war", Rarity::RED, 35.00));
        chroma.addItem(Item("Galil AR | Chatterbox", Rarity::RED, 20.00));
        
        chroma.addItem(Item("Karambit | Doppler", Rarity::GOLD, 450.00));
        cases.push_back(chroma);

        // 2. Gamma Case
        Case gamma("Gamma Case", 3.00);
        gamma.addItem(Item("Five-SeveN | Violent Daimyo", Rarity::BLUE, 0.15));
        gamma.addItem(Item("MAC-10 | Carnivore", Rarity::BLUE, 0.15));
        gamma.addItem(Item("Nova | Exo", Rarity::BLUE, 0.15));
        
        gamma.addItem(Item("P90 | Chopper", Rarity::PURPLE, 0.60));
        gamma.addItem(Item("AUG | Aristocrat", Rarity::PURPLE, 0.75));
        
        gamma.addItem(Item("M4A1-S | Mecha Industries", Rarity::PINK, 15.00));
        gamma.addItem(Item("Desolate Space", Rarity::PINK, 18.00));
        
        gamma.addItem(Item("M4A4 | Desolate Space", Rarity::PINK, 15.00)); // Duplicate name fix
        gamma.addItem(Item("Glock-18 | Wasteland Rebel", Rarity::RED, 40.00));
        
        gamma.addItem(Item("M9 Bayonet | Lore", Rarity::GOLD, 1200.00));
        cases.push_back(gamma);

        // 3. Prisma Case
        Case prisma("Prisma Case", 1.80);
        prisma.addItem(Item("Famas | Crypsis", Rarity::BLUE, 0.08));
        prisma.addItem(Item("AK-47 | Uncharted", Rarity::BLUE, 0.50));
        prisma.addItem(Item("P250 | Verdigris", Rarity::BLUE, 0.10));
        
        prisma.addItem(Item("MP7 | Mischief", Rarity::PURPLE, 0.40));
        prisma.addItem(Item("Galil AR | Akoben", Rarity::PURPLE, 0.45));
        
        prisma.addItem(Item("XM1014 | Incinegator", Rarity::PINK, 4.00));
        prisma.addItem(Item("R8 Revolver | Skull Crusher", Rarity::PINK, 3.50));
        
        prisma.addItem(Item("M4A4 | The Emperor", Rarity::RED, 80.00));
        prisma.addItem(Item("Five-SeveN | Angry Mob", Rarity::RED, 15.00));
        
        prisma.addItem(Item("Ursus Knife | Marble Fade", Rarity::GOLD, 300.00));
        cases.push_back(prisma);

        // 4. Dreams & Nightmares
        Case dreams("Dreams & Nightmares", 0.90);
        dreams.addItem(Item("Five-SeveN | Scrawl", Rarity::BLUE, 0.05));
        dreams.addItem(Item("MAC-10 | Ensnared", Rarity::BLUE, 0.05));
        dreams.addItem(Item("MAG-7 | Foresight", Rarity::BLUE, 0.05));
        
        dreams.addItem(Item("PP-Bizon | Space Cat", Rarity::PURPLE, 0.30));
        dreams.addItem(Item("G3SG1 | Dream Glade", Rarity::PURPLE, 0.30));
        
        dreams.addItem(Item("M4A1-S | Night Terror", Rarity::PINK, 2.50));
        dreams.addItem(Item("Savage", Rarity::PINK, 2.50)); // Fix
        
        dreams.addItem(Item("AK-47 | Nightwish", Rarity::RED, 60.00));
        dreams.addItem(Item("MP9 | Starlight Protector", Rarity::RED, 25.00));
        
        dreams.addItem(Item("Butterfly Knife | Gamma Doppler", Rarity::GOLD, 1500.00));
        cases.push_back(dreams);
    }

    Item rollItem(const Case& c) {
        // Weights:
        // Blue: 79.92%
        // Purple: 15.98%
        // Pink: 3.2%
        // Red: 0.64%
        // Gold: 0.26%
        
        // Simplified weights for demonstration but keeping order of magnitude:
        // Blue: 80%
        // Purple: 15%
        // Pink: 3%
        // Red: 1.5%
        // Gold: 0.5%
        
        uniform_int_distribution<> dist(1, 10000);
        int roll = dist(rng);
        
        Rarity targetRarity;
        if (roll <= 8000) targetRarity = Rarity::BLUE;
        else if (roll <= 9500) targetRarity = Rarity::PURPLE;
        else if (roll <= 9800) targetRarity = Rarity::PINK;
        else if (roll <= 9950) targetRarity = Rarity::RED;
        else targetRarity = Rarity::GOLD;

        // Filter items from case by rarity
        vector<Item> pool;
        const auto& allItems = c.getItems();
        for (const auto& item : allItems) {
            if (item.getRarity() == targetRarity) {
                pool.push_back(item);
            }
        }

        // If pool is empty (e.g. case doesn't have that rarity), fallback to Blue
        if (pool.empty()) {
             for (const auto& item : allItems) {
                if (item.getRarity() == Rarity::BLUE) {
                    pool.push_back(item);
                }
            }
        }
        
        // If still empty (bad case definition), just pick random
        if (pool.empty()) pool = allItems;

        uniform_int_distribution<> indexDist(0, pool.size() - 1);
        return pool[indexDist(rng)];
    }

    void animateOpening(const Item& wonItem, const Case& c) {
        cout << "\nOtwieranie skrzyni " << c.getName() << "..." << endl;
        
        // Fake rolling items
        vector<Item> animationPool = c.getItems();
        uniform_int_distribution<> dist(0, animationPool.size() - 1);
        
        int rolls = 20;
        int delay = 50;

        for (int i = 0; i < rolls; ++i) {
            Item randomItem = animationPool[dist(rng)];
            cout << "\r> " << randomItem.getName() << "   " << flush;
            this_thread::sleep_for(chrono::milliseconds(delay));
            delay += 15; // Slow down effect
        }
        
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "\r                                       " << flush; // Clear line
        cout << "\rWYGRANO: " << wonItem.getDisplayName() << "!" << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

public:
    GameManager() : currentUser(nullptr) {
        random_device rd;
        rng = mt19937(rd());
        initCases();
    }

    void run() {
        while (true) {
            if (!currentUser) {
                authMenu();
            } else {
                mainMenu();
            }
        }
    }

    void authMenu() {
        cout << "\n=== CS2 CASE SIMULATOR ===" << endl;
        cout << "1. Zaloguj sie" << endl;
        cout << "2. Zarejestruj sie" << endl;
        cout << "3. Wyjscie" << endl;
        cout << "Wybierz opcje: ";
        
        int choice;
        cin >> choice;
        
        string u, p;
        if (choice == 1) {
            cout << "Login: "; cin >> u;
            cout << "Haslo: "; cin >> p;
            if (users.find(u) != users.end() && users[u].checkPassword(p)) {
                currentUser = &users[u];
                cout << "Zalogowano pomyslnie!" << endl;
            } else {
                cout << "Bledny login lub haslo." << endl;
            }
        } else if (choice == 2) {
            cout << "Nowy Login: "; cin >> u;
            cout << "Nowe Haslo: "; cin >> p;
            if (users.find(u) == users.end()) {
                users[u] = User(u, p, 100.00);
                cout << "Konto utworzone! Mozesz sie zalogowac." << endl;
            } else {
                cout << "Uzytkownik o takim loginie juz istnieje." << endl;
            }
        } else if (choice == 3) {
            exit(0);
        }
    }

    void mainMenu() {
        cout << "\n=== MENU GLOWNE ===" << endl;
        cout << "Zalogowany jako: " << currentUser->getUsername() 
             << " | Saldo: $" << fixed << setprecision(2) << currentUser->getBalance() << endl;
        cout << "1. Otworz Skrzynki" << endl;
        cout << "2. Ekwipunek (Sprzedaz)" << endl;
        cout << "3. Doladuj Portfel" << endl;
        cout << "4. Wyloguj" << endl;
        cout << "Wybierz opcje: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: openCaseMenu(); break;
            case 2: inventoryMenu(); break;
            case 3: addFundsMenu(); break;
            case 4: currentUser = nullptr; break;
            default: cout << "Nieznana opcja." << endl;
        }
    }

    void inventoryMenu() {
        while(true) {
            cout << "\n--- EKWIPUNEK ---" << endl;
            const auto& items = currentUser->getInventoryItems();
            
            if (items.empty()) {
                cout << "Ekwipunek jest pusty." << endl;
                return;
            }

            double totalValue = 0;
            for (size_t i = 0; i < items.size(); ++i) {
                cout << i + 1 << ". " << items[i].getDisplayName() << endl;
                totalValue += items[i].getValue();
            }
            cout << "----------------------" << endl;
            cout << "Laczna wartosc: $" << fixed << setprecision(2) << totalValue << endl;
            
            cout << "\nWpisz numer przedmiotu, aby go sprzedac." << endl;
            cout << "Wpisz 0, aby wrocic." << endl;
            cout << "Wybierz: ";

            int choice;
            cin >> choice;

            if (choice == 0) return;

            if (choice > 0 && choice <= items.size()) {
                currentUser->sellItem(choice - 1);
            } else {
                cout << "Nieprawidlowy numer." << endl;
            }
        }
    }

    void addFundsMenu() {
        cout << "\n--- DOLADOWANIE ---" << endl;
        cout << "Podaj kwote do wplaty: ";
        double amount;
        cin >> amount;
        
        if (amount <= 0) {
            cout << "Kwota musi byc dodatnia!" << endl;
            return;
        }

        cout << "Podaj numer karty (fikcyjnie): ";
        string dummy;
        cin >> dummy;

        cout << "Przetwarzanie..." << endl;
        this_thread::sleep_for(chrono::seconds(1));
        
        currentUser->addBalance(amount);
        cout << "Dodano $" << amount << " do portfela." << endl;
    }

    void openCaseMenu() {
        cout << "\n--- SKLEP SKRZYN ---" << endl;
        for (size_t i = 0; i < cases.size(); ++i) {
            cout << i + 1 << ". " << cases[i].getName() << " - $" << cases[i].getPrice() << endl;
        }
        cout << "0. Powrot" << endl;
        cout << "Wybierz skrzynie: ";
        
        int choice;
        cin >> choice;

        if (choice == 0 || choice > cases.size()) return;

        Case& selectedCase = cases[choice - 1];
        
        if (currentUser->getBalance() < selectedCase.getPrice()) {
            cout << "Niewystarczajace srodki! Doladuj portfel." << endl;
            return;
        }

        char confirm;
        cout << "Kupic " << selectedCase.getName() << " za $" << selectedCase.getPrice() << "? (t/n): ";
        cin >> confirm;

        if (confirm == 't' || confirm == 'T') {
            if (currentUser->subtractBalance(selectedCase.getPrice())) {
                Item wonItem = rollItem(selectedCase);
                animateOpening(wonItem, selectedCase);
                currentUser->addItemToInventory(wonItem);
            }
        }
    }
};

int main() {
    GameManager game;
    game.run();
    return 0;
}
