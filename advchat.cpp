#include <bits/stdc++.h>
using namespace std;

// Helper function to trim whitespace from both ends of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

// Helper function to convert string to lowercase
string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

class User {
public:
    User(const string& username) : username(username) {}
    string getUsername() const { return username; }

private:
    string username;
};

class Message {
public:
    Message(const User& sender, const User& receiver, const string& content)
        : sender(sender), receiver(receiver), content(content), 
          timestamp(chrono::system_clock::to_time_t(chrono::system_clock::now())) {}

    const User& getSender() const { return sender; }
    const User& getReceiver() const { return receiver; }
    string getContent() const { return content; }
    time_t getTimestamp() const { return timestamp; }

private:
    User sender;
    User receiver;
    string content;
    time_t timestamp;
};

class ChatRoom {
public:
    void addUser(const User& user) {
        users.push_back(user);
    }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
        messageStack.push(msg);
    }

    void displayMessages() const {
        for (const auto& msg : messages) {
            time_t timestamp = msg.getTimestamp();
            string timeString = ctime(&timestamp);
            timeString.erase(timeString.length() - 1);
            
            cout << "[" << timeString << "] "
                 << msg.getSender().getUsername() << " to " 
                 << msg.getReceiver().getUsername() << ": " 
                 << msg.getContent() << endl;
        }
    }

    void undoLastMessage() {
        if (!messageStack.empty()) {
            messageStack.pop();
            messages.pop_back();
            cout << "Last message undone.\n";
        } else {
            cout << "No messages to undo.\n";
        }
    }

    void saveToCSV(const string& filename) const {
        ofstream file(filename);
        if (file.is_open()) {
            file << "Timestamp,Sender,Receiver,Content\n";
            for (const auto& msg : messages) {
                file << msg.getTimestamp() << "," 
                     << msg.getSender().getUsername() << "," 
                     << msg.getReceiver().getUsername() << "," 
                     << msg.getContent() << "\n";
            }
            file.close();
            cout << "Chat history saved to " << filename << "\n";
        } else {
            cout << "Unable to open file for writing.\n";
        }
    }

    void loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            messages.clear();
            string line;
            getline(file, line); // Skip header
            while (getline(file, line)) {
                istringstream iss(line);
                string timestamp, senderName, receiverName, content;
                getline(iss, timestamp, ',');
                getline(iss, senderName, ',');
                getline(iss, receiverName, ',');
                getline(iss, content);

                // Find or create users
                User* sender = findOrCreateUser(senderName);
                User* receiver = findOrCreateUser(receiverName);

                if (sender && receiver) {
                    messages.emplace_back(*sender, *receiver, content);
                }
            }
            file.close();
            cout << "Chat history loaded from " << filename << "\n";
        } else {
            cout << "Unable to open file for reading.\n";
        }
    }

    Message* linearSearch(const string& keyword) {
        for (auto& msg : messages) {
            if (msg.getContent().find(keyword) != string::npos) {
                return &msg;
            }
        }
        return nullptr;
    }

    void quickSort() {
        if (!messages.empty()) {
            quickSortHelper(0, messages.size() - 1);
        }
    }

    size_t getMessageCount() const {
        return messages.size();
    }

    const User* findUser(const string& username) const {
        auto lowerUsername = toLower(trim(username));
        auto it = find_if(users.begin(), users.end(),
                          [&lowerUsername](const User& user) {
                              return toLower(trim(user.getUsername())) == lowerUsername;
                          });
        return it != users.end() ? &(*it) : nullptr;
    }

private:
    vector<User> users;
    vector<Message> messages;
    stack<Message> messageStack;

    User* findOrCreateUser(const string& username) {
        auto it = find_if(users.begin(), users.end(),
                          [&username](const User& user) { return user.getUsername() == username; });
        if (it != users.end()) {
            return &(*it);
        } else {
            users.emplace_back(username);
            return &users.back();
        }
    }

    void swapMessages(Message& a, Message& b) {
        Message temp = move(a);
        a = move(b);
        b = move(temp);
    }

    void quickSortHelper(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSortHelper(low, pi - 1);
            quickSortHelper(pi + 1, high);
        }
    }

    int partition(int low, int high) {
        time_t pivot = messages[high].getTimestamp();
        int i = low - 1;

        for (int j = low; j <= high - 1; j++) {
            if (messages[j].getTimestamp() < pivot) {
                i++;
                swapMessages(messages[i], messages[j]);
            }
        }
        swapMessages(messages[i + 1], messages[high]);
        return (i + 1);
    }
};

int main() {
    ChatRoom chatRoom;
    string currentUser;

    while (true) {
        cout << "\n1. Add user\n2. Switch user\n3. Send message\n4. Display messages"
             << "\n5. Undo last message\n6. Save chat to CSV\n7. Load chat from CSV"
             << "\n8. Search message\n9. Sort messages\n10. Exit\nChoice: ";

        int choice;
        if (!(cin >> choice)) {
            // Clear the error flag and discard invalid input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                string username;
                cout << "Enter new username: ";
                getline(cin, username);
                username = trim(username);  // Trim input
                chatRoom.addUser(User(username));
                cout << "User added: " << username << "\n";
                break;
            }
            case 2: {
                cout << "Enter username to switch to: ";
                getline(cin, currentUser);
                currentUser = trim(currentUser);  // Trim input
                if (chatRoom.findUser(currentUser)) {
                    cout << "Switched to user: " << currentUser << "\n";
                } else {
                    cout << "User not found. Please add the user first.\n";
                    currentUser.clear();
                }
                break;
            }
            case 3: {
                if (currentUser.empty()) {
                    cout << "Please switch to a user first.\n";
                    break;
                }
                string receiverName, content;
                cout << "Enter receiver's username: ";
                getline(cin, receiverName);
                receiverName = trim(receiverName);  // Trim input
                cout << "Enter your message: ";
                getline(cin, content);
                
                const User* sender = chatRoom.findUser(currentUser);
                const User* receiver = chatRoom.findUser(receiverName);
                
                if (sender && receiver) {
                    chatRoom.addMessage(Message(*sender, *receiver, content));
                    cout << "Message sent.\n";
                } else {
                    cout << "Sender or receiver not found.\n";
                }
                break;
            }
            case 4:
                chatRoom.displayMessages();
                break;
            case 5:
                chatRoom.undoLastMessage();
                break;
            case 6:
                chatRoom.saveToCSV("chat_history.csv");
                break;
            case 7:
                chatRoom.loadFromCSV("chat_history.csv");
                break;
            case 8: {
                string keyword;
                cout << "Enter search keyword: ";
                getline(cin, keyword);
                Message* found = chatRoom.linearSearch(keyword);
                if (found) {
                    cout << "Message found: " << found->getSender().getUsername() 
                         << " to " << found->getReceiver().getUsername()
                         << ": " << found->getContent() << "\n";
                } else {
                    cout << "Message not found.\n";
                }
                break;
            }
            case 9:
                chatRoom.quickSort();
                cout << "Messages sorted by timestamp.\n";
                break;
            case 10:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
