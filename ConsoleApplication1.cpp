#include <iostream>

using namespace std;

class Node {
private:
    int _data;
    Node* _next;
    Node* _prev;
public:
    Node() : _data(0), _next(nullptr), _prev(nullptr) {}
    Node(int value) : _data(value), _next(nullptr), _prev(nullptr) {}
    int data() {
        return _data;
    }
    Node* next() {
        return _next;
    }
    Node* prev() {
        return _prev;
    }
    void setData(int value) {
        _data = value;
    }
    void setNext(Node* node) {
        _next = node;
    }
    void setPrev(Node* node) {
        _prev = node;
    }
};

class LinkedList {
private:
    Node* _head;
    Node* _tail;
public:
    Node* head() {
        return _head;
    }
    Node* tail() {
        return _tail;
    }
    void setHead(Node* node) {
        _head = node;
    }
    void setTail(Node* node) {
        _tail = node;
    }
    void push_back(int value) {
        Node* newNode = new Node(value);
        if (_head == nullptr) {
            _head = newNode;
            _tail = newNode;
        }
        else {
            _tail->setNext(newNode);
            newNode->setPrev(_tail);
            _tail = newNode;
        }
    }
    void push_back(LinkedList& other) {
        Node* current = other._head;
        while (current != nullptr) {
            this->push_back(current->data());
            current = current->next();
        }
    }
    void pop_back() {
        if (_tail == nullptr) return;
        if (_head == _tail) {
            delete _head;
            _head = _tail = nullptr;
        }
        else {
            Node* new_tail = _tail->prev();
            delete _tail;
            _tail = new_tail;
            _tail->setNext(nullptr);
        }
    }
    void pop_head() {
        if (_head == nullptr) return;
        if (_head == _tail) {
            delete _head;
            _head = _tail = nullptr;
        }
        else {
            Node* new_head = _head->next();
            delete _head;
            _head = new_head;
            _head->setPrev(nullptr);
        }
    }
    void push_head(int value) {
        Node* newNode = new Node(value);
        if (_head == nullptr) {
            _head = newNode;
            _tail = newNode;
        }
        else {
            _head->setPrev(newNode);
            newNode->setNext(_head);
            _head = newNode;
        }
    }
    void push_head(LinkedList& other) {
        Node* current = other._tail;
        while (current != nullptr) {
            this->push_head(current->data());
            current = current->prev();
        }
    }
    void delete_node(int value) {
        Node* current = _head;
        while (current != nullptr) {
            if (current->data() == value) {
                if (current->prev() != nullptr) {
                    current->prev()->setNext(current->next());
                }
                else {
                    _head = current->next();
                }
                if (current->next() != nullptr) {
                    current->next()->setPrev(current->prev());
                }
                else {
                    _tail = current->prev();
                }
                Node* toDelete = current;
                current = current->next();
                delete toDelete;
            }
            else {
                current = current->next();
            }
        }
    }
    LinkedList() : _head(nullptr), _tail(nullptr) {}
    LinkedList(LinkedList& other) {
        _head = _tail = nullptr;
        Node* current = other._head;
        while (current != nullptr) {
            this->push_back(current->data());
            current = current->next();
        }
    }
    LinkedList(int count) {
        if (count <= 0) {
            _head = _tail = nullptr;
            return;
        }
        _head = new Node(1);
        Node* current = _head;
        for (int i = 2; i <= count; i++) {
            Node* newNode = new Node(i);
            current->setNext(newNode);
            newNode->setPrev(current);
            current = newNode;
        }
        _tail = current;
    }
    ~LinkedList() {
        Node* current = _head;
        while (current != nullptr) {
            Node* nextNode = current->next();
            delete current;
            current = nextNode;
        }
    }
    int operator[](int index) const {
        Node* current = _head;
        for (int i = 0; i < index; i++) {
            current = current->next();
        }
        return current->data();
    }
    void set(int index, int value) {
        Node* current = _head;
        for (int i = 0; i < index; i++) {
            current = current->next();
        }
        current->setData(value);
    }
    void Print() {
        cout << "List: ";
        Node* current = _head;
        while (current != nullptr) {
            cout << current->data() << " ";
            current = current->next();
        }
        cout << endl;
    }
};

void delete_n(LinkedList& list, int n) {
    int step = list[n - 1];
    Node* current = list.head();
    Node* new_current = nullptr;
    while (current != nullptr) {
        for (int i = 1; i < step; i++) {
            current = current->next();
            if (current == nullptr) return;
        }
        if (current->prev() != nullptr) {
            current->prev()->setNext(current->next());
        }
        else {
            list.setHead(current->next());
        }
        if (current->next() != nullptr) {
            current->next()->setPrev(current->prev());
        }
        else {
            list.setTail(current->prev());
        }
        new_current = current->next();
        delete current;
        current = new_current;
    }
}

int main() {
    LinkedList list(170);
    delete_n(list, 2);
    for (int i = 2; i < 32; i++) {
        int count = 0;
        Node* current = list.head();
        while (current != nullptr) {
            count++;
            current = current->next();
        }
        if (count < i) {
            cout << "Invalid index!" << endl;
            return 0;
        }
        delete_n(list, i);
    }
    list.Print();
    return 0;
}
