#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <optional>

using namespace std;

class InMemoryDB {
private:
    unordered_map<string, int> database; // Main state of the database
    optional<unordered_map<string, int>> transaction; // Transaction state

public:
    // Get the value associated with a key
    optional<int> get(const =string& key) {
        if (transaction.has_value() && transaction->count(key)) {
            return transaction->at(key);
        }
        if (database.count(key)) {
            return database.at(key);
        }
        return nullopt;
    }

    // Put a key-value pair into the transaction
    void put(const std::string& key, int value) {
        if (!transaction.has_value()) {
            throw std::runtime_error("No transaction in progress");
        }
        (*transaction)[key] = value;
    }

    // Begin a new transaction
    void begin_transaction() {
        if (transaction.has_value()) {
            throw std::runtime_error("A transaction is already in progress");
        }
        transaction = std::unordered_map<std::string, int>();
    }

    // Commit the transaction
    void commit() {
        if (!transaction.has_value()) {
            throw std::runtime_error("No transaction in progress");
        }
        for (const auto& [key, value] : *transaction) {
            database[key] = value;
        }
        transaction.reset();
    }

    // Rollback the transaction
    void rollback() {
        if (!transaction.has_value()) {
            throw std::runtime_error("No transaction in progress");
        }
        transaction.reset();
    }
};

int main() {
    InMemoryDB inmemoryDB;

    // Should return null (std::nullopt in C++)
    std::cout << (inmemoryDB.get("A").has_value() ? std::to_string(inmemoryDB.get("A").value()) : "null") << std::endl;

    // Should throw an exception
    try {
        inmemoryDB.put("A", 5);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    // Start transaction
    inmemoryDB.begin_transaction();

    // Put values in transaction
    inmemoryDB.put("A", 5);
    std::cout << (inmemoryDB.get("A").has_value() ? std::to_string(inmemoryDB.get("A").value()) : "null") << std::endl; // Should return null

    inmemoryDB.put("A", 6);
    inmemoryDB.commit();

    std::cout << (inmemoryDB.get("A").has_value() ? std::to_string(inmemoryDB.get("A").value()) : "null") << std::endl; // Should return 6

    // Error cases for commit and rollback
    try {
        inmemoryDB.commit();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        inmemoryDB.rollback();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    // Transaction rollback example
    std::cout << (inmemoryDB.get("B").has_value() ? std::to_string(inmemoryDB.get("B").value()) : "null") << std::endl; // Should return null
    inmemoryDB.begin_transaction();
    inmemoryDB.put("B", 10);
    inmemoryDB.rollback();
    std::cout << (inmemoryDB.get("B").has_value() ? std::to_string(inmemoryDB.get("B").value()) : "null") << std::endl; // Should return null

    return 0;
}
