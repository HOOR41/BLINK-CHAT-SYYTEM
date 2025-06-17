#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <unordered_map>
#include <queue>
#include <list>
#include <sstream>
#include <functional>
#include <algorithm>
#include <conio.h>
#include <windows.h>  
using namespace std;

// Declaration
void saveUsersToFile();  
void viewChat(const string& username);
void viewPublicChat(const string& username);
void loadUsers();
void registerUser();
bool login(string& currentUser);
void deleteAccount(string& currentUser);
void changePassword(string& currentUser);
void adminPortal();
void sendMessage(const string& sender);
void viewInbox(const string& username);
void guestMode();
void sendPublicMessage();

// Console Color Helper
void setColor(int color) 
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

size_t hashPassword(const string& password)
{
    return hash<string>{}(password);
}

// Constants 
const int CHAT_HISTORY_LIMIT = 10;
const string GUEST_USERNAME = "Guest";
const string ADMIN_USERNAME = "admin";
const size_t ADMIN_PASSWORD_HASH = hashPassword("Admin@123");

// Structures 
struct User
{
    string username;
    size_t hashedPassword;
};
struct Message 
{
    string sender;
    string receiver;    
    string content;
    time_t timestamp;
    bool viewed;
    string type;        
};

// Globals 
unordered_map<string , vector<string>> groups;
unordered_map<string, User> users;
queue<Message> offlineMessages;
list<Message> chatHistory;

// Utility Functions 
string formatTime(time_t timestamp)
{
    char buffer[26];   
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
    return string(buffer);
}
string inputHiddenPassword()
 {
    string password;
    char ch;
    while ((ch=_getch()) != '\r') //Enter Key
	{
        if (ch == '\b' && !password.empty())  //Bckspace Key
		{
            password.pop_back();
            cout << "\b \b";
        }
        else if (ch != '\b') 
		{
            password.push_back(ch);
            cout << '*';
        }
    }
    cout<<endl;
    return password;
}
bool isStrongPassword(const string & pw)
{
    if (pw.length() < 8) 
	return false;
    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
    string special = "!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?`~";
    for (char c : pw)
	{
        if (islower(static_cast<unsigned char>(c)))
	    hasLower = true;
        else if (isupper(static_cast<unsigned char>(c))) 
		hasUpper = true;
        else if (isdigit(static_cast<unsigned char>(c))) 
		hasDigit = true;
        else if (special.find(c) != string::npos) 
		hasSpecial = true;
    }
    return hasLower && hasUpper && hasDigit && hasSpecial;
}

// File Handling 
void loadUsers() 
{
    users.clear();
    ifstream infile("users.txt");
    string line;
    while (getline(infile, line))
	 {
        istringstream iss(line);
        string uname;
        size_t hashed;
        getline(iss, uname, '|');
        iss >> hashed;
        iss.ignore();
        users[uname] = { uname, hashed };
    }
    if (!users.count(ADMIN_USERNAME))
	{
        users[ADMIN_USERNAME] = { ADMIN_USERNAME, ADMIN_PASSWORD_HASH };
        saveUsersToFile();
    }
}
int randomColor()
{
    int colors[] = { 9, 10, 11, 12, 13, 14 }; 
    return colors[rand() % 6];
}
void printCentered(const string& message, int colorCode) 
{
    setColor(colorCode);
    int width = 80;
    int padding = (width - static_cast<int>(message.length())) / 2;
    for (int i = 0; i < padding; i++)
	{
     cout << " ";
    }
     cout << message << endl;
     setColor(7); 
}
void saveUsersToFile() 
{
    ofstream out("users.txt");
    for (const auto& p : users)  
	{
        const User & u = p.second;
        out << u.username << "|" << u.hashedPassword  << "\n";
    }
}
void saveMessage(const Message & msg) 
{
    ofstream out("messages.txt", ios::app);
    out << msg.sender << "|" << msg.receiver << "|" << msg.content << "|" << msg.timestamp << "|" << msg.viewed << "\n";
}
void deleteViewedMessages(const string& username) 
{
    ifstream in("messages.txt");
    ofstream out("temp.txt");
    string line;
    while (getline(in, line))
	 {
        istringstream iss(line);
        string s, r, c, ts, viewedStr;
        getline(iss, s, '|');
        getline(iss, r, '|');
        getline(iss, c, '|');
        getline(iss, ts, '|');
        getline(iss, viewedStr);
    if (!(r == username && viewedStr == "1")) 
	    {
            out << s << "|" << r << "|" << c << "|" << ts << "|" << viewedStr << "\n";
        }
    }
    in.close();
    out.close();
    remove("messages.txt");
    rename("temp.txt", "messages.txt");
}

// User System 
void registerUser()
 {
    printCentered("-------------------------------------------------------------------------------GET YOURSELF REGITER-------------------------------------------------------------------------", 11);
    cout << "ENTER USERNAME: ";string uname; 
	cin >> uname;
    loadUsers();
    if (users.count(uname)) 
	{
        setColor(4); 
		cout << "User already exists.\n"; setColor(7);
        return;
    }
    string pw;
    do 
	{
        cout << "ENTER YOUR PASSWORD: ";
        pw = inputHiddenPassword();
        if (!isStrongPassword(pw)) 
		{
            setColor(6);
            cout << "Password must be 8+ chars with upper, lower, digit & special.\n";
            setColor(7);
        }
    } 
	while (!isStrongPassword(pw));
    users[uname] = { uname, hashPassword(pw) };
    ofstream append("users.txt", ios::app);
    append << uname << "|" << users[uname].hashedPassword << "|||\n";
    setColor(2); 
	cout << " YOUR REGISTRATION IS SUCCESSFUL!\n";
	setColor(7);
}
bool login(string& currentUser) 
{
    printCentered("                                                       =============   LOGIN    ==============                                   ", 11);

    cout << "ENTER USERNAME: "; string uname; cin >> uname;
    cout << "ENTER PASSWORD: "; string pw = inputHiddenPassword();
    loadUsers();
    if (users.count(uname) && users[uname].hashedPassword == hashPassword(pw)) 
	{
        currentUser = uname;
        printCentered("                                                   ===========     LOGIN SUCCESSFULLY     ==========", 11); 
        return true;
    }
    printCentered("INVALID USERNAME OR PASSWORD", 12);
    return false;
}
void deleteAccount(string&currentUser) 
{
    loadUsers();
    users.erase(currentUser);
    saveUsersToFile();
    setColor(2);
	cout << "ACCOUNT DELETED SUCCESSFULLY.\n"; setColor(7);
    currentUser.clear();
}
void changePassword(string&currentUser) 
{
    loadUsers();
    setColor(11); 
	cout << "\n======== CHANGE PASSWORD ========\n"; 
	setColor(7);
    cout << "ENTER CURRENT PASSWORD: ";
    string oldpw = inputHiddenPassword();
    if (users[currentUser].hashedPassword != hashPassword(oldpw))
	{
        setColor(4);
		cout << "INCORRECT CURRENT PASSWORD\n"; 
		setColor(7);
        return;
    }
    string newpw;
    do 
	{
        cout << "ENTER YOUR NEW PASSWORD: ";
        newpw = inputHiddenPassword();
        if (!isStrongPassword(newpw)) 
		{
            setColor(6);
            cout << "Password must be 8+ chars with upper, lower, digit & special.\n";
            setColor(7);
        }
    } 
	while (!isStrongPassword(newpw));
    users[currentUser].hashedPassword = hashPassword(newpw);
    saveUsersToFile();
    setColor(2); 
	cout << "PASSWORD CHANGED SUCCESSFULLY!\n"; 
	setColor(7);
}
void viewUserProfile(const string&currentUser) 
{
    loadUsers();
    if (!users.count(currentUser)) 
	{
        setColor(4);
        cout << "USER PROFILE NOT FOUND\n";
        setColor(7);
        return;
    }
    User user = users[currentUser];
    setColor(11);
    cout << "\n--- USER PROFILE ---\n";
    setColor(7);
    cout << "USER NAME: " << user.username << "\n";
    cout << "HASHED PASSWORD: " << user.hashedPassword << "\n"; 
}
void adminPortal() 
{
    int choice;
    while (true) 
	{
        printCentered("---ADMIN PORTAL ---", 13);
        cout << "1. VIEW ALL USERS \n2. VIEW ALL MESSAGES\n3. DELETE USER\n4. BROADCAST MESSAGE\n5. LOGOUT\nCHOICE: ";
        cin >> choice;
        if (choice == 1) 
		{
            setColor(11); 
			cout << "\nREGISTERD USER:\n"; 
			setColor(7);
            for (auto& p : users) 
			{
                if (p.first != ADMIN_USERNAME)
                    cout << "- " << p.first << endl;
            }
        }
        else if (choice == 2) 
		{
            viewChat("ADMIN");
        }
        else if (choice == 3) 
		{
            cout << "ENTER USERNAME TO DELETE: ";
            string uname; 
			cin >> uname;
            if (users.count(uname) && uname != ADMIN_USERNAME) 
			{
                users.erase(uname);
                saveUsersToFile();
                setColor(2); cout << "USER DELETED\n"; setColor(7);
            }
            else 
			{
                setColor(4); 
				cout << "USER NOT FOUND OR PROTECTED \n"; setColor(7);
            }
        }
        else if (choice == 4) 
		{
            cin.ignore();
            setColor(11); 
			cout << "ENTER BROADCAST MESSAGE: "; 
			setColor(7);
            string msg; getline(cin, msg);
            for (auto& p : users) 
			{
                if (p.first != ADMIN_USERNAME) 
				{
                    Message m{ ADMIN_USERNAME, p.first, msg, time(nullptr), false };
                    saveMessage(m);
                }
            }
            setColor(2); 
			cout << "BROADCAST SENT\n";
			setColor(7);
        }
        else if (choice == 5) 
		{
            break;
        }
        else 
		{
            setColor(4); 
			cout << "INVALID CHOICE\n"; setColor(7);
        }
    }
}

// Messaging
void sendMessage(const string& sender) 
{
    setColor(11); 
	cout << "\nENTER RECIPIENT: "; 
	setColor(7);
    string recv; 
	cin >> recv; 
	cin.ignore(); 
    setColor(11); 
	cout << "Message: "; 
	setColor(7);
    string msg; 
	getline(cin, msg);
    loadUsers();
    if (!users.count(recv)) 
	{
        setColor(4);
		cout << "USER NOT FOUND\n"; 
		setColor(7);
        return;
    }
    Message m{ sender, recv, msg, time(nullptr), false };
    saveMessage(m);
    setColor(2); 
	cout << "MESSGAE SENT\n"; 
	setColor(7);
}
void viewInbox(const string& username) 
{
    ifstream in("messages.txt");
    string line;
    bool found = false;
    while (getline(in, line)) 
	{
        istringstream iss(line);
        string s, r, c, ts, viewedStr;
        getline(iss, s, '|');
        getline(iss, r, '|');
        getline(iss, c, '|');
        getline(iss, ts, '|');
        getline(iss, viewedStr);  
        time_t t = stol(ts);
        bool v = (viewedStr == "1");
        if (r == username && !v) 
		{
            found = true;
            setColor(11); 
			cout << "FROM: " << s << "\n"; 
			setColor(10);
            cout << "TIME: " << formatTime(t) << "\n";
            setColor(7); 
			cout << "MESSAGE: " << c << "\n\n";
        }
    }
    if (!found)
	{
        setColor(6); 
		cout << "NO NEW MESSAGE\n"; 
		setColor(7);
    }
    in.close();
}
void viewChat(const string& username) 
{
    vector<Message> messages;
    ifstream in("messages.txt");
    if (!in.is_open()) 
	{
        setColor(4); 
		cout << "ERROR: COULD NOT OPEN messages.txt\n"; 
		setColor(7);
        return;
    }
    string line;
    while (getline(in, line)) 
	{
        stringstream ss(line);
        string sender, receiver, content, timestampStr, viewedStr;
        getline(ss, sender, '|');
        getline(ss, receiver, '|');
        getline(ss, content, '|');
        getline(ss, timestampStr, '|');
        getline(ss, viewedStr); 
        if (sender.empty() || receiver.empty()) continue; 
        Message msg;
        msg.sender = sender;
        msg.receiver = receiver;
        msg.content = content;
        msg.timestamp = stol(timestampStr);
        msg.viewed = (viewedStr == "1");
        if (msg.sender == username || msg.receiver == username) 
		{
            messages.push_back(msg);
        }
    }
    in.close(); 
    if (messages.empty()) 
	{
        setColor(6); 
		cout << "NO CHAT HISTORY FOUND\n"; 
		setColor(7);
        return;
    }
    sort(messages.begin(), messages.end(), [](const Message& a, const Message& b) 
	{
        return a.timestamp < b.timestamp;
    });
    setColor(11); 
	cout << "VIEW CHAT HISTORY\n"; 
	setColor(7);
    for (const auto& msg : messages) 
	{
        cout<< "From: " << msg.sender << "\n"
            << "To: " << msg.receiver << "\n"
            << "Time: " << formatTime(msg.timestamp) << "\n"
            << "Message: " << msg.content << "\n"
            << "Status: " << (msg.viewed ? "Read" : "Unread") << "\n\n";
    }
}
void createGroup() 
{
    setColor(11); 
	cout << "ENTER GROUP NAME: "; 
	setColor(7);
    string groupName; 
	cin >> groupName;
    if (groups.count(groupName)) 
	{
        setColor(4); 
		cout << "GROUP ALREADY EXISTS\n"; 
		setColor(7);
        return;
    }
    int n;
    setColor(11);
    cout << "HOW MANY MEMBERS TO ADD: "; 
	setColor(7);
    cin >> n;
    vector<string> members;
    string member;
    loadUsers(); 
    for (int i = 0; i < n; ++i) 
	{
        cout << "ENTER MEMBER " << i + 1 << ": ";
        cin >> member;
        if (users.count(member)) 
		{
            members.push_back(member);
        }
        else 
		{
            setColor(4); 
			cout << "USER DOES NOT EXIST. SKIPPING.\n"; 
			setColor(7);
        }
    }
    if (members.empty()) 
	{
        setColor(4); 
		cout << "NO VALID MEMBERS. GROUP NOT CREATED.\n"; 
		setColor(7);
        return;
    }
    groups[groupName] = members;
    string filename = "group_" + groupName + ".txt";
    ofstream file(filename);
    if (file) 
	{
        file << "[Group '" << groupName << "' created with " << members.size() << " member(s)]\n";
        file.close();
    }
    setColor(2); 
	cout << "GROUP CREATED SUCCESSFULLY!\n"; 
	setColor(7);
}
void sendGroupMessage(const string& sender) 
{
    setColor(11); 
	cout << "ENTER GROUP NAME: "; 
	setColor(7);
    string groupName; cin >> groupName;
    if (!groups.count(groupName)) 
	{
        setColor(4);
		cout << "GROUP DOES NOT EXIST\n"; 
		setColor(7);
        return;
    }
    cin.ignore();
    setColor(11); 
	cout << "ENTER GROUP MESSAGE: "; 
	setColor(7);
    string msg; getline(cin, msg);
    string filename = "group_" + groupName + ".txt";      // Save message to group file
    ofstream file(filename, ios::app);  
    if (!file)
	{
        setColor(4); 
		cout << "ERROR SAVING GROUP MESSAGE\n"; 
		setColor(7);
        return;
    }
    file << sender << ":" << msg << endl;
    file.close();
    setColor(2); 
	cout << "GROUP MESSAGE SENT SUCCESSFULLY.\n"; 
	setColor(7);
}
void viewGroupMessages() 
{
    string groupName;
    cout << "Enter group name to view messages: ";
    cin >> groupName;
    string filename = "group_" + groupName + ".txt";
    ifstream file(filename);
    if (!file) 
	{
        cout << "No messages found for group: " << groupName << endl;
        return;
    }
    string line;
    cout << "\n--- Messages in Group [" << groupName << "] ---\n";
    while (getline(file, line)) 
	{
        size_t delimiterPos = line.find(":");
        if (delimiterPos != string::npos)
	   {
            string sender = line.substr(0, delimiterPos);
            string message = line.substr(delimiterPos + 1);
            cout << "[" << sender << "]: " << message << endl;
        }
        else 
		{
            cout << line << endl;
        }
    }
    file.close();
    cout << "=========================================================\n";
}

//Guest Mode 
void viewPublicChat(const string& username) 
{
    ifstream in("messages.txt");
    string line;
    vector<Message> messages;
    while (getline(in, line)) 
	{
        istringstream iss(line);
        string s, r, c, ts, viewedStr;
        getline(iss, s, '|');
        getline(iss, r, '|');
        getline(iss, c, '|');
        getline(iss, ts, '|');
        getline(iss, viewedStr);  
        time_t t = stol(ts);
        bool v = (viewedStr == "1");
        if (r == "ALL") 
		{
            messages.push_back({ s, r, c, t, v });
        }
    }
    in.close();
    if (messages.empty()) 
	{
        setColor(6); 
		cout << "NO PUBLIC MESSAGE FOUND\n"; 
		setColor(7);
        return;
    }
    sort(messages.begin(), messages.end(), [](const Message& a, const Message& b)
	 {
        return a.timestamp < b.timestamp;
    });
     setColor(11);
	 cout << "\n--- PUBLIC CHAT ---\n"; 
	 setColor(7);
    for (const auto& msg : messages) 
	{
        setColor(randomColor());
        setColor(randomColor());
        cout << "From: " << msg.sender << "\n" << "Time: " << formatTime(msg.timestamp) << "\n" << "Message: " << msg.content << "\n";
        cout << "Status: " << (msg.viewed ? "Read" : "Unread") << "\n\n";

    }
}
void exportChatToFile(const string& username) 
{
    ofstream out(username + "_chat_history.txt");
    ifstream in("messages.txt");
    string line;
    while (getline(in, line)) 
	{
        istringstream iss(line);
        string s, r, c, ts, viewedStr;
        getline(iss, s, '|');
        getline(iss, r, '|');
        getline(iss, c, '|');
        getline(iss, ts, '|');
        getline(iss, viewedStr);
        time_t t = stol(ts);
        bool v = (viewedStr == "1");
        if (s == username || r == username) 
		{
            out << "From: " << s << "\n"
                << "To: " << r << "\n"
                << "Time: " << formatTime(t) << "\n"
                << "Message: " << c << "\n"
                << "Status: " << (v ? "Read" : "Unread") << "\n\n";
        }
    }
    in.close();
    out.close();
    setColor(2);
	cout << "CHAT HISTORY EXPORTED TO " << username + "_chat_history.txt\n"; 
	setColor(7);
}

void sendPublicMessage() 
{
    setColor(11); 
	cout << "\nENTER PUBLUC MESSAGE: "; 
	setColor(7);
    cin.ignore(); 
	string msg; 
	getline(cin, msg);
    Message m{ GUEST_USERNAME, "ALL", msg, time(nullptr), false };
    saveMessage(m);
    setColor(2); 
	cout << "PUBLIC MESSAGE SENT!\n"; 
	setColor(7);
}

void guestMode() 
{
    int opt;
    while (true) 
	{
        setColor(11); 
		cout << "\n--- GUEST MODE ---\n"; 
		setColor(7);
        cout << "1. VIEW PUBLIC CHAT\n2.SEND PUBLIC MESSAGE\n3. BACK TO MAIN MENU\nC: "; 
		cin >> opt;
        switch (opt) 
		{
        case 1: viewPublicChat(GUEST_USERNAME); 
		break;
        case 2: sendPublicMessage(); 
		break;
        case 3: return;
        default: 
		setColor(4); 
		cout << "INVALID OPTION\n"; 
		setColor(7);
        }
    }
}

// Main 
int main() 
{
    int CHOICE;
    string currentUser;
    while (true) 
	{
        setColor(14); 
        cout << R"(
                                                      ******************************************************************
                                                      *                                                                *
                                                      *    ********************************************************    *
                                                      *    *                                                      *    *
                                                      *    *      W E L C O M E   T O   B L I N K   C H A T       *    *
                                                      *    *                                                      *    *
                                                      *    ********************************************************    *
                                                      *                                                                *
                                                      ******************************************************************

)";
        setColor(9); 
        cout << " \n";
        cout << "\n";
        cout << "\n";
        setColor(11); 
		cout << "\n=========================================================================BlINK CHAT MENU================================================================================\n"; 
		setColor(7);
        cout << "\n";
        cout << "\n";
        cout << "1. REGISTER YOUR ACCOUNT HERE \n 2. CLICK FOR LOGIN PAGE \n 3. GUEST MODE \n 4. EXIT\n CHOICE: ";
        cin >> CHOICE;
        if (CHOICE == 1) 
		{
            cout << "\n";
            registerUser();
        }
        else if (CHOICE == 2) 
		{
            if (login(currentUser)) 
			{
                if (currentUser == ADMIN_USERNAME) 
				{
                    adminPortal();
                    currentUser.clear();
                }
                else 
				{
                    int opt;
                    while (!currentUser.empty()) 
					{
                        setColor(11); 
						cout << "\nLOGGED IN AS: " << currentUser << "\n"; 
						setColor(7);
                    cout << "1.VIEW INBOX\n"
                            "2.SEND MESSAGE\n"
                            "3.VIEW CHAT HISTORY\n"
                            "4.CHANGE PASSWORD\n"
                            "5.DELETE ACCOUNT\n"
                            "6.LOGOUT\n"
                            "7.EXPORT CHAT\n"
                            "8.CREATE GROUP\n"
                            "9.SEND GROUP MESSAGE\n"
                            "10.VIEW GROUP MESSAGES\n"
                            "11.VIEW PROFILE\nCHOICE: ";
                    cin >> opt;
                        switch (opt) 
						{
                        case 1: viewInbox(currentUser); 
						break;
                        case 2: sendMessage(currentUser); 
						break;
                        case 3: viewChat(currentUser);  
						break;
                        case 4: changePassword(currentUser); 
						break;
                        case 5: deleteAccount(currentUser); 
						break;
                        case 6: currentUser.clear(); 
						break;
                        case 7: exportChatToFile(currentUser); 
						break;
                        case 8: createGroup(); 
						break;
                        case 9: sendGroupMessage(currentUser); 
						break;
                        case 10: viewGroupMessages(); 
						break;
                        case 11: viewUserProfile(currentUser); 
						break;
                        default: 
						setColor(4); 
						cout << "INVALID OPTION\n"; 
						setColor(7);
                        }
                    }
                }
            }
        }
        else if (CHOICE == 3) 
		{
            guestMode();
        }
        else if (CHOICE == 4) 
		{
            setColor(2); 
			cout << "TAKE CARE!!!!!!!!!!!!! HOPE YOU ENJOYED OUR BLINK CHAT SYSTEM !!!!!!!!!!!!!!!!!!!!!1\n"; 
			setColor(7);
            break;
        }
        else
		{
            setColor(4); 
			cout << "INVALID OPTION.\n"; 
			setColor(7);
        } 
		}
    }