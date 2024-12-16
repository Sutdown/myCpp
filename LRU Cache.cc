#include <unordered_map>
using namespace std;

class Node
{
public:
  Node *prev, *next;
  int key, value;

  Node(int k = 0, int v = 0) : key(k), value(v) {};
};

class LRUCache
{
public:
  LRUCache(int capacity) : capacity(capacity), dummy(new Node())
  {
    dummy->prev = dummy->next = dummy;
  }

  int get(int key)
  {
    Node *n = find_node(key);
    if (n == nullptr)
      return -1;
    else
      return n->value;
  }

  void put(int key, int value)
  {
    auto n = find_node(key);
    if (n)
    {
      n->value = value;
      return;
    }

    Node *newNode = new Node(key, value);
    m[key] = newNode;
    put_front(newNode);
    if (m.size() > capacity)
    {
      auto n = dummy->prev;
      m.erase(n->key);
      erase_node(n);
      delete n;
    }
  }

private:
  Node *find_node(int key)
  {
    auto n = m.find(key);
    if (n == m.end())
    {
      return nullptr;
    }
    erase_node(n->second);
    put_front(n->second);
    return n->second;
  }

  void erase_node(Node *n)
  {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  void put_front(Node *n)
  {
    n->prev = dummy;
    n->next = dummy->next;
    n->prev->next = n;
    n->next->prev = n;
  }

private:
  Node *dummy;
  unordered_map<int, Node *> m;
  int capacity;
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */

int main(){
  return 0;
}