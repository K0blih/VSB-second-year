#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <fstream>

struct Node
{
    Node* left;
    Node* right;
    int key;
    int height;

    Node(const int value) : left(nullptr), right(nullptr), key(value), height(0) {}
};

class BinaryTree
{
    private:
    Node* root;

    int height(Node* node)
    {
        if (node == nullptr) { return -1; }
        return node->height;
    }

    void updateH(Node* node)
    {
        if (node == nullptr) { return; }
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    Node* rotateRight(Node* r)
    {
        Node* c = r->left;
        Node* t2 = c->right;

        c->right = r;
        r->left = t2;

        updateH(r);
        updateH(c);

        return c;
    }

    Node* rotateLeft(Node* r)
    {
        Node* c = r->right;
        Node* t2 = c->left;

        c->left = r;
        r->right = t2;

        updateH(r);
        updateH(c);

        return c;
    }

    int balanceFactor(Node* node)
    {
        if (node == nullptr) { return 0; }
        return height(node->left) - height(node->right);
    }

    Node* balance(Node* node)
    {
        updateH(node);
        int bf = balanceFactor(node);

        if (bf > 1) {
            if (balanceFactor(node->left) < 0){
                node->left = rotateLeft(node->left);
            }

            return rotateRight(node);
        }

        if (bf < -1) {
            if (balanceFactor(node->right) > 0) {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }

        return node;
    }

    bool containsKey(Node* node, const int key)
    {
        if (node == nullptr) { return false;}
        if (key == node->key) {
            return true;
        }
        if (key < node->key) {
            return containsKey(node->left, key);
        }
        else {
            return containsKey(node->right, key);
        }
    }

    Node* findMinKeyNode(Node* node)
    {
        while (node->left != nullptr) {
            node = node->left;
        }

        return node;
    }

    Node* erase(Node* node, const int key)
    {
        if (node == nullptr) {
            return nullptr;
        }

        if (key < node->key) {
            node->left = erase(node->left, key);
        }
        else if (key > node->key) {
            node->right = erase(node->right, key);
        }
        else {
            // 1 child deletion
            if (node->left == nullptr || node->right == nullptr) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }

            //2 children case
            Node* succesor = findMinKeyNode(node->right);
            node->key = succesor->key;
            node->right = erase(node->right, succesor->key);
        }

        return balance(node);
    }

    Node* insert(Node* node, const int key)
    {
        if (node == nullptr) {
            return new Node(key);
        }
        
        if (key < node->key) {
            node->left = insert(node->left, key);
        }
        else if (key > node->key) {
            node->right = insert(node->right, key);
        }
        else {
            return node;
        }

        return balance(node);
    }

    void inorderPrint(Node* node) const
    {
        if (node == nullptr) { return; };

        inorderPrint(node->left);
        std::cout << node->key << " ";
        inorderPrint(node->right);
    }

    void preorderPrint(Node* node) const
    {
        if (node == nullptr) { return; };

        std::cout << node->key << " ";
        preorderPrint(node->left);
        preorderPrint(node->right);
    }

    void postorderPrint(Node* node) const
    {
        if (node == nullptr) { return; };

        postorderPrint(node->left);
        postorderPrint(node->right);
        std::cout << node->key << " ";
    }

    void clear(Node* node)
    {
        if ( node == nullptr ) { return; };

        clear(node->left);
        clear(node->right);
        delete node;
    }

    public:
    BinaryTree() : root(nullptr) {}
    ~BinaryTree()
    {
        clear(root);
    }

    void insert(int key)
    {
        root = insert(root, key);
    }

    void erase(int key)
    {
        root = erase(root, key);
    }

    bool containsKey(int key)
    {
        return containsKey(root, key);
    }

    void printPreorder() const{
        preorderPrint(root);
        std::cout << "\n";
    }

    void printPostorder() const
    {
        postorderPrint(root);
        std::cout << "\n";
    }

    void printInorder() const
    {
        inorderPrint(root);
        std::cout << "\n";
    }
};

// reads integers from the first line in the file
std::vector<int> readIntegersFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<int> numbersVec;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return numbersVec;
    }

    std::string line;

    // change if to while to read the whole file
    if (std::getline(file, line)) {
        std::stringstream lineAsStream(line);
        int num;
        while (lineAsStream >> num) {  // NOTE: this assumes that the input file has the data we want
            numbersVec.push_back(num);
        }
    }

    file.close();
    return numbersVec;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Not enough arguments\n";
        return 1;
    }

    std::string construction_file = argv[1];
    std::string deletion_file = argv[2];

    auto construction_data = readIntegersFromFile(construction_file);
    auto deletion_data = readIntegersFromFile(deletion_file);

    BinaryTree tree;
    for (const int item : construction_data) {
        tree.insert(item);
    }

    for (const int item : deletion_data) {
        tree.erase(item);
    }

    tree.printPostorder();
    tree.printPreorder();
    tree.printInorder();

    return 0;
}