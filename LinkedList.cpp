#include <iostream>
using namespace std;
struct Node {
    int value;
    Node* next;
    Node* prev;
};

int main()  {
    // Method A
    Node* first = new Node{100, nullptr};
    Node* second = new Node{200, nullptr};
    Node* third = new Node{300, nullptr};
    first->next = second;
    second->next = third;
    third->next = first;
    cout << first->next->next->value << "\n";
}