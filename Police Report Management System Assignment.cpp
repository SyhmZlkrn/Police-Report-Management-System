#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Report {
    string reportID;
    string reporterName;
    string category;
    string location;
    string date;
    string status;
};

struct Node {
    Report data;
    Node *next;
};

void printOne(const Report &r) {
    cout << "Report ID   : " << r.reportID << "\n"
         << "Reporter    : " << r.reporterName << "\n"
         << "Category    : " << r.category << "\n"
         << "Location    : " << r.location << "\n"
         << "Date        : " << r.date << "\n"
         << "Status      : " << r.status << "\n";
}

bool existsInList(Node *head, const string &id) {
    for (Node *p = head; p != NULL; p = p->next) {
        if (p->data.reportID == id)
            return true;
    }
    return false;
}

// ACTIVE LIST (QUEUE STYLE)
// enqueue at tail (back), dequeue at head (front)

void enqueueActive(Node* &head, Node* &tail, const Report &r) {
    Node *n = new Node();
    n->data = r;
    n->next = NULL;

    if (head == NULL) {
        head = tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
}

Node* searchActive(Node *head, const string &id) {
    for (Node *p = head; p != NULL; p = p->next) {
        if (p->data.reportID == id) return p;
    }
    return NULL;
}

bool deleteActive(Node* &head, Node* &tail, const string &id) {
    if (head == NULL)
        return false;

    Node *prev = NULL;
    Node *curr = head;

    while (curr != NULL && curr->data.reportID != id) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)
        return false;

    if (prev == NULL) { // delete head
        head = head->next;
        if (head == NULL) tail = NULL;
        free(curr);
        return true;
    }

    prev->next = curr->next;
    if (curr == tail) tail = prev;
    free(curr);
    return true;
}

// DEQUEUE (remove oldest = remove head)
// returns true if successfully dequeued into outReport
bool dequeueActive(Node* &head, Node* &tail, Report &outReport) {
    if (head == NULL) return false;

    Node *old = head;
    outReport = old->data;

    head = head->next;
    if (head == NULL)
        tail = NULL;

    free(old);
    return true;
}

void displayList(Node *head, const string &title) {
    cout << "\n===== " << title << " =====\n";
    if (head == NULL) {
        cout << "[Empty]\n";
        return;
    }
    int i = 1;
    for (Node *p = head; p != NULL; p = p->next) {
        cout << "\n--- Record " << i++ << " ---\n";
        printOne(p->data);
    }
}

// HISTORY LIST (SORTED)

void insertHistorySorted(Node* &hHead, const Report &r) {
    Node *n = new Node();
    n->data = r;
    n->next = NULL;

    if (hHead == NULL || r.reportID < hHead->data.reportID) {
        n->next = hHead;
        hHead = n;
        return;
    }

    Node *prev = NULL;
    Node *curr = hHead;

    while (curr != NULL && curr->data.reportID <= r.reportID) {
        prev = curr;
        curr = curr->next;
    }

    prev->next = n;
    n->next = curr;
}

vector<Node*> historyToVector(Node *hHead) {
    vector<Node*> v;
    for (Node *p = hHead; p != NULL; p = p->next)
        v.push_back(p);
    return v;
}

Node* binarySearchHistory(Node *hHead, const string &id) {
    vector<Node*> v = historyToVector(hHead);
    int lo = 0, hi = (int)v.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (v[mid]->data.reportID == id) return v[mid];
        else if (v[mid]->data.reportID < id) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

// PROCESS ACTIVE to HISTORY

bool processReportByID(Node* &aHead, Node* &aTail, Node* &hHead, const string &id) {
    Node *target = searchActive(aHead, id);
    if (target == NULL)
        return false;

    Report closed = target->data;
    closed.status = "CLOSED";

    if (!deleteActive(aHead, aTail, id))
        return false;

    insertHistorySorted(hHead, closed);
    return true;
}

// NEW: Dequeue oldest + ask user to process or not
void dequeueOldestAndAskProcess(Node* &aHead, Node* &aTail, Node* &hHead) {
    if (aHead == NULL) {
        cout << "[Empty] No active reports to dequeue.\n";
        return;
    }

    // oldest is at head
    cout << "\n[OLDEST ACTIVE REPORT]\n";
    printOne(aHead->data);

    char ans;
    cout << "\nProcess this report and move to HISTORY? (Y/N): ";
    cin >> ans;

    if (ans == 'Y' || ans == 'y') {
        Report r;
        bool ok = dequeueActive(aHead, aTail, r);
        if (ok) {
            r.status = "CLOSED";
            insertHistorySorted(hHead, r);
            cout << "[OK] Oldest report dequeued and moved to HISTORY.\n";
        } else {
            cout << "[Error] Failed to dequeue.\n";
        }
    } else {
        cout << "[Info] Report not processed. It stays in ACTIVE list.\n";
    }
}

//INPUT

Report inputReport(Node *aHead, Node *hHead) {
    Report r;

    cout << "Enter Report ID (unique): ";
    cin >> ws;
    getline(cin, r.reportID);

    while (existsInList(aHead, r.reportID) || existsInList(hHead, r.reportID)) {
        cout << "[Error] Report ID already exists. Enter another ID: ";
        getline(cin, r.reportID);
    }

    cout << "Enter Reporter Name: ";
    getline(cin, r.reporterName);

    cout << "Enter Category (e.g. Theft/Assault/Accident): ";
    getline(cin, r.category);

    cout << "Enter Location: ";
    getline(cin, r.location);

    cout << "Enter Date (e.g. 2025-12-21): ";
    getline(cin, r.date);

    r.status = "ACTIVE";
    return r;
}

void updateReportFields(Report &r, bool allowStatus) {
    int choice;
    do {
        cout << "\nUpdate Menu for Report ID: " << r.reportID << "\n";
        cout << "1. Reporter Name\n";
        cout << "2. Category\n";
        cout << "3. Location\n";
        cout << "4. Date\n";
        if (allowStatus) cout << "5. Status\n";
        cout << "0. Back\n";
        cout << "Enter choice: ";
        cin >> choice;

        cin >> ws;
        if (choice == 1) {
            cout << "New Reporter Name: ";
            getline(cin, r.reporterName);
        } else if (choice == 2) {
            cout << "New Category: ";
            getline(cin, r.category);
        } else if (choice == 3) {
            cout << "New Location: ";
            getline(cin, r.location);
        } else if (choice == 4) {
            cout << "New Date: ";
            getline(cin, r.date);
        } else if (allowStatus && choice == 5) {
            cout << "New Status (ACTIVE/CLOSED): ";
            getline(cin, r.status);
        }
    } while (choice != 0);
}

//MENUS

int mainMenu() {
    int c;
    cout << "\n===== POLICE REPORT MANAGEMENT SYSTEM =====\n";
    cout << "1. Report List (ACTIVE - Queue Style)\n";
    cout << "2. History List (CLOSED - Sorted)\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
    cin >> c;
    return c;
}

int reportMenu() {
    int c;
    cout << "\n--- REPORT LIST MENU (ACTIVE) ---\n";
    cout << "1. Add Report (ENQUEUE)\n";
    cout << "2. Update Report\n";
    cout << "3. Search Report\n";
    cout << "4. Delete Report\n";
    cout << "5. Process Report by ID (Move to History)\n";
    cout << "6. Display All Active Reports\n";
    cout << "7. Dequeue Oldest Report (View + Optional Process)\n"; // NEW
    cout << "0. Back\n";
    cout << "Enter choice: ";
    cin >> c;
    return c;
}

int historyMenu() {
    int c;
    cout << "\n--- HISTORY LIST MENU (CLOSED) ---\n";
    cout << "1. Display All History Reports (Sorted)\n";
    cout << "2. Search History Report (Binary Search)\n";
    cout << "3. Update History Report\n";
    cout << "0. Back\n";
    cout << "Enter choice: ";
    cin >> c;
    return c;
}

// CLEANUP

void clearList(Node* &head) {
    while (head != NULL) {
        Node *t = head;
        head = head->next;
        delete t;
    }
}

// MAIN

int main() {
    Node *activeHead = NULL, *activeTail = NULL;
    Node *historyHead = NULL;

    int mainChoice;
    do {
        mainChoice = mainMenu();

        if (mainChoice == 1) {
            int c;
            do {
                c = reportMenu();

                if (c == 1) {
                    Report r = inputReport(activeHead, historyHead);
                    enqueueActive(activeHead, activeTail, r);
                    cout << "[OK] Report added to ACTIVE list (FIFO queue).\n";

                } else if (c == 2) {
                    string id;
                    cout << "Enter Report ID to update: ";
                    cin >> ws; getline(cin, id);
                    Node *p = searchActive(activeHead, id);
                    if (p == NULL) cout << "[Not Found]\n";
                    else updateReportFields(p->data, true);

                } else if (c == 3) {
                    string id;
                    cout << "Enter Report ID to search: ";
                    cin >> ws; getline(cin, id);
                    Node *p = searchActive(activeHead, id);
                    if (p == NULL) cout << "[Not Found]\n";
                    else {
                        cout << "\n[FOUND - ACTIVE REPORT]\n";
                        printOne(p->data);
                    }

                } else if (c == 4) {
                    string id;
                    cout << "Enter Report ID to delete: ";
                    cin >> ws; getline(cin, id);
                    if (deleteActive(activeHead, activeTail, id))
                        cout << "[OK] Deleted from ACTIVE list.\n";
                    else
                        cout << "[Not Found]\n";

                } else if (c == 5) {
                    string id;
                    cout << "Enter Report ID to process (move to history): ";
                    cin >> ws; getline(cin, id);
                    if (processReportByID(activeHead, activeTail, historyHead, id))
                        cout << "[OK] Report processed and moved to HISTORY list.\n";
                    else
                        cout << "[Not Found]\n";

                } else if (c == 6) {
                    displayList(activeHead, "ACTIVE REPORT LIST (FIFO)");

                } else if (c == 7) {
                    dequeueOldestAndAskProcess(activeHead, activeTail, historyHead);
                }

            } while (c != 0);

        } else if (mainChoice == 2) {
            int c;
            do {
                c = historyMenu();

                if (c == 1) {
                    displayList(historyHead, "HISTORY REPORT LIST (SORTED by ReportID)");

                } else if (c == 2) {
                    string id;
                    cout << "Enter Report ID to search (binary search): ";
                    cin >> ws; getline(cin, id);
                    Node *p = binarySearchHistory(historyHead, id);
                    if (p == NULL) cout << "[Not Found]\n";
                    else {
                        cout << "\n[FOUND - HISTORY REPORT]\n";
                        printOne(p->data);
                    }

                } else if (c == 3) {
                    string id;
                    cout << "Enter Report ID to update in history: ";
                    cin >> ws; getline(cin, id);
                    Node *p = binarySearchHistory(historyHead, id);
                    if (p == NULL) cout << "[Not Found]\n";
                    else updateReportFields(p->data, true);
                }

            } while (c != 0);
        }

    } while (mainChoice != 0);

    clearList(activeHead);
    clearList(historyHead);

    cout << "\nEnd of program\n";
    return 0;
}
