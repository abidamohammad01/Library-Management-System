#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>

using namespace std;

struct Book {
    int id;
    string title, author, category;
    bool issued;
    int issuedTo;

    Book() : id(0), issued(false), issuedTo(0) {}
};

struct Member {
    int id;
    string name, contact;

    Member() : id(0) {}
};

class Library {
private:
    vector<Book> books;
    vector<Member> members;
    string bookFile;
    string memberFile;

    static string cleanText(string text) {
        size_t i;
        for (i = 0; i < text.size(); ++i) {
            if (text[i] == '|') text[i] = '/';
        }
        return text;
    }

    static string lowerText(string text) {
        size_t i;
        for (i = 0; i < text.size(); ++i) {
            text[i] = static_cast<char>(tolower(static_cast<unsigned char>(text[i])));
        }
        return text;
    }

    int findBookIndex(int id) const {
        size_t i;
        for (i = 0; i < books.size(); ++i) {
            if (books[i].id == id) return static_cast<int>(i);
        }
        return -1;
    }

    int findMemberIndex(int id) const {
        size_t i;
        for (i = 0; i < members.size(); ++i) {
            if (members[i].id == id) return static_cast<int>(i);
        }
        return -1;
    }

    void saveBooks() const {
        ofstream out(bookFile.c_str());
        size_t i;
        if (!out) {
            cout << "Error: Could not save book records.\n";
            return;
        }
        for (i = 0; i < books.size(); ++i) {
            const Book &b = books[i];
            out << b.id << '|' << b.title << '|' << b.author << '|'
                << b.category << '|' << (b.issued ? 1 : 0) << '|'
                << b.issuedTo << '\n';
        }
    }

    void saveMembers() const {
        ofstream out(memberFile.c_str());
        size_t i;
        if (!out) {
            cout << "Error: Could not save member records.\n";
            return;
        }
        for (i = 0; i < members.size(); ++i) {
            const Member &m = members[i];
            out << m.id << '|' << m.name << '|' << m.contact << '\n';
        }
    }

    void loadBooks() {
        ifstream in(bookFile.c_str());
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields;
            while (getline(ss, field, '|')) fields.push_back(field);

            if (fields.size() == 6) {
                Book b;
                stringstream idStream(fields[0]);
                stringstream issuedStream(fields[4]);
                stringstream memberStream(fields[5]);

                if ((idStream >> b.id) && (issuedStream >> b.issued) &&
                    (memberStream >> b.issuedTo)) {
                    b.title = fields[1];
                    b.author = fields[2];
                    b.category = fields[3];
                    b.issued = (fields[4] == "1");
                    if (findBookIndex(b.id) == -1) books.push_back(b);
                }
            }
        }
    }

    void loadMembers() {
        ifstream in(memberFile.c_str());
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields;
            while (getline(ss, field, '|')) fields.push_back(field);

            if (fields.size() == 3) {
                Member m;
                stringstream idStream(fields[0]);
                if (idStream >> m.id) {
                    m.name = fields[1];
                    m.contact = fields[2];
                    if (findMemberIndex(m.id) == -1) members.push_back(m);
                }
            }
        }
    }

    static string readText(const string &prompt) {
        string value;
        cout << prompt;
        getline(cin, value);
        return cleanText(value);
    }

    static int readInt(const string &prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

public:
    Library() : bookFile("books.txt"), memberFile("members.txt") {
        loadBooks();
        loadMembers();
    }

    void addBook() {
        Book b;
        b.id = readInt("Enter book ID: ");

        if (b.id <= 0 || findBookIndex(b.id) != -1) {
            cout << "Invalid or duplicate book ID.\n";
            return;
        }

        b.title = readText("Enter book title: ");
        b.author = readText("Enter author name: ");
        b.category = readText("Enter category: ");

        if (b.title.empty() || b.author.empty()) {
            cout << "Title and author cannot be empty.\n";
            return;
        }

        b.issued = false;
        b.issuedTo = 0;
        books.push_back(b);
        saveBooks();
        cout << "Book added successfully.\n";
    }

    void displayBooks() const {
        size_t i;
        if (books.empty()) {
            cout << "No books found.\n";
            return;
        }

        cout << "\n========== BOOK LIST ==========\n";
        for (i = 0; i < books.size(); ++i) {
            const Book &b = books[i];
            cout << "Book ID: " << b.id << "\n"
                 << "Title: " << b.title << "\n"
                 << "Author: " << b.author << "\n"
                 << "Category: " << b.category << "\n"
                 << "Status: " << (b.issued ? "Issued" : "Available") << "\n";
            if (b.issued) cout << "Issued to member ID: " << b.issuedTo << "\n";
            cout << "-------------------------------\n";
        }
    }

    void searchBook() const {
        string key = readText("Enter book ID, title or author: ");
        string lowerKey = lowerText(key);
        bool found = false;
        size_t i;

        for (i = 0; i < books.size(); ++i) {
            const Book &b = books[i];
            stringstream idStream;
            idStream << b.id;
            if (key == idStream.str() ||
                lowerText(b.title).find(lowerKey) != string::npos ||
                lowerText(b.author).find(lowerKey) != string::npos) {
                cout << "ID: " << b.id << " | Title: " << b.title
                     << " | Author: " << b.author
                     << " | Category: " << b.category
                     << " | Status: " << (b.issued ? "Issued" : "Available")
                     << "\n";
                found = true;
            }
        }

        if (!found) cout << "No matching book found.\n";
    }

    void addMember() {
        Member m;
        m.id = readInt("Enter member ID: ");

        if (m.id <= 0 || findMemberIndex(m.id) != -1) {
            cout << "Invalid or duplicate member ID.\n";
            return;
        }

        m.name = readText("Enter member name: ");
        m.contact = readText("Enter contact details: ");

        if (m.name.empty()) {
            cout << "Member name cannot be empty.\n";
            return;
        }

        members.push_back(m);
        saveMembers();
        cout << "Member registered successfully.\n";
    }

    void displayMembers() const {
        size_t i;
        if (members.empty()) {
            cout << "No members registered.\n";
            return;
        }

        cout << "\n========== MEMBER LIST ==========\n";
        for (i = 0; i < members.size(); ++i) {
            cout << "Member ID: " << members[i].id
                 << " | Name: " << members[i].name
                 << " | Contact: " << members[i].contact << "\n";
        }
    }

    void searchMember() const {
        int id = readInt("Enter member ID: ");
        int index = findMemberIndex(id);

        if (index == -1) {
            cout << "Member not found.\n";
        } else {
            const Member &m = members[index];
            cout << "Member ID: " << m.id << "\nName: " << m.name
                 << "\nContact: " << m.contact << "\n";
        }
    }

    void issueBook() {
        int bookId = readInt("Enter book ID to issue: ");
        int memberId = readInt("Enter member ID: ");
        int bi = findBookIndex(bookId);
        int mi = findMemberIndex(memberId);

        if (bi == -1) {
            cout << "Book not found.\n";
            return;
        }
        if (mi == -1) {
            cout << "Member not found. Register the member first.\n";
            return;
        }
        if (books[bi].issued) {
            cout << "This book is already issued.\n";
            return;
        }

        books[bi].issued = true;
        books[bi].issuedTo = memberId;
        saveBooks();
        cout << "Book issued successfully to " << members[mi].name << ".\n";
    }

    void returnBook() {
        int bookId = readInt("Enter book ID to return: ");
        int bi = findBookIndex(bookId);

        if (bi == -1) {
            cout << "Book not found.\n";
            return;
        }
        if (!books[bi].issued) {
            cout << "This book is not currently issued.\n";
            return;
        }

        books[bi].issued = false;
        books[bi].issuedTo = 0;
        saveBooks();
        cout << "Book returned successfully.\n";
    }

    void checkAvailability() const {
        int bookId = readInt("Enter book ID: ");
        int bi = findBookIndex(bookId);

        if (bi == -1) {
            cout << "Book not found.\n";
        } else {
            cout << "Book: " << books[bi].title << "\nStatus: "
                 << (books[bi].issued ? "Currently issued" : "Available") << "\n";
        }
    }

    void run() {
        int choice;
        do {
            cout << "\n====================================\n";
            cout << "      LIBRARY MANAGEMENT SYSTEM\n";
            cout << "====================================\n";
            cout << "1. Add Book\n";
            cout << "2. Display All Books\n";
            cout << "3. Search Book\n";
            cout << "4. Register Member\n";
            cout << "5. Display Members\n";
            cout << "6. Search Member\n";
            cout << "7. Issue Book\n";
            cout << "8. Return Book\n";
            cout << "9. Check Book Availability\n";
            cout << "0. Exit\n";

            choice = readInt("Enter your choice: ");

            switch (choice) {
                case 1: addBook(); break;
                case 2: displayBooks(); break;
                case 3: searchBook(); break;
                case 4: addMember(); break;
                case 5: displayMembers(); break;
                case 6: searchMember(); break;
                case 7: issueBook(); break;
                case 8: returnBook(); break;
                case 9: checkAvailability(); break;
                case 0: cout << "Thank you for using the Library Management System.\n"; break;
                default: cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    Library library;
    library.run();
    return 0;
}
