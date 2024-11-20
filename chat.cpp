#include<bits/stdc++.h>
using namespace std;

class Message {
public:
    Message(const string& sender, const string& content)
        : sender(sender), content(content), 
          timestamp(chrono::system_clock::to_time_t(chrono::system_clock::now())) {}

    string getSender() const { return sender; }
    string getContent() const { return content; }
    time_t getTimestamp() const { return timestamp; }

private:
    string sender;
    string content;
    time_t timestamp;
};

class ChatRoom {
public:
    void addMessage(const Message& msg) {
        messages.push_back(msg);
        messageStack.push(msg);
    }

   
    void displayMessages() const {
    for (const auto& msg : messages) {
        time_t timestamp = msg.getTimestamp();
        string timeString = ctime(&timestamp);
        // Remove the newline character that ctime() adds
        timeString.erase(timeString.length() - 1);
        
        cout << "[" << timeString << "] "
             << msg.getSender() << ": " << msg.getContent() << endl;
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
            file << "Timestamp,Sender,Content\n";
            for (const auto& msg : messages) {
                file << msg.getTimestamp() << "," 
                     << msg.getSender() << "," 
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
                string timestamp, sender, content;
                getline(iss, timestamp, ',');
                getline(iss, sender, ',');
                getline(iss, content);
                messages.emplace_back(sender, content);
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

private:
    vector<Message> messages;
    stack<Message> messageStack;

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
                swap(messages[i], messages[j]);
            }
        }
        swap(messages[i + 1], messages[high]);
        return (i + 1);
    }
};
int main(){
       ChatRoom chatRoom;
    string username;

    cout << "Enter your username: ";
    getline(cin, username);

    while (true) {
        cout << "\n1. Send message\n2. Display messages\n3. Undo last message"
             << "\n4. Save chat to CSV\n5. Load chat from CSV\n6. Search message"
             << "\n7. Sort messages\n8. Exit\nChoice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                string content;
                cout << "Enter your message: ";
                getline(cin, content);
                chatRoom.addMessage(Message(username, content));
                break;
            }
            case 2:
                chatRoom.displayMessages();
                break;
            case 3:
                chatRoom.undoLastMessage();
                break;
            case 4:
                chatRoom.saveToCSV("chat_history.csv");
                break;
            case 5:
                chatRoom.loadFromCSV("chat_history.csv");
                break;
            case 6: {
                string keyword;
                cout << "Enter search keyword: ";
                getline(cin, keyword);
                Message* found = chatRoom.linearSearch(keyword);
                if (found) {
                    cout << "Message found: " << found->getSender() 
                         << ": " << found->getContent() << "\n";
                } else {
                    cout << "Message not found.\n";
                }
                break;
            }
            case 7:
                chatRoom.quickSort();
                cout << "Messages sorted by timestamp.\n";
                break;
            case 8:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }


return 0;
}