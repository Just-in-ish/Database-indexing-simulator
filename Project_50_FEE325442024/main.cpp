// BTree Indexing Simulator in C++ with Qt for Visualization and GUI Insertion
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsTextItem>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <vector>
#include <algorithm>
#include <iostream>

const int MAX_KEYS = 4;

class BTreeNode {
public:
    bool leaf;
    std::vector<int> keys;
    std::vector<BTreeNode*> children;

    BTreeNode(bool isLeaf) : leaf(isLeaf) {}
};

class BTree {
public:
    BTreeNode* root;
    int t;

    BTree(int degree) : t(degree) {
        root = new BTreeNode(true);
    }

    void insert(int k);
    void insertNonFull(BTreeNode* node, int k);
    void splitChild(BTreeNode* parent, int i);
    void drawTree(QGraphicsScene* scene, BTreeNode* node, int x, int y, int dx);
};

void BTree::insert(int k) {
    if (root->keys.size() == MAX_KEYS) {
        BTreeNode* s = new BTreeNode(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    insertNonFull(root, k);
}

void BTree::insertNonFull(BTreeNode* node, int k) {
    int i = node->keys.size() - 1;
    if (node->leaf) {
        node->keys.push_back(0);
        while (i >= 0 && k < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            --i;
        }
        node->keys[i + 1] = k;
    } else {
        while (i >= 0 && k < node->keys[i])
            --i;
        ++i;
        if (node->children[i]->keys.size() == MAX_KEYS) {
            splitChild(node, i);
            if (k > node->keys[i]) ++i;
        }
        insertNonFull(node->children[i], k);
    }
}

void BTree::splitChild(BTreeNode* parent, int i) {
    BTreeNode* y = parent->children[i];
    BTreeNode* z = new BTreeNode(y->leaf);
    z->keys.assign(y->keys.begin() + 3, y->keys.end());
    y->keys.resize(2);
    if (!y->leaf) {
        z->children.assign(y->children.begin() + 3, y->children.end());
        y->children.resize(3);
    }
    parent->children.insert(parent->children.begin() + i + 1, z);
    parent->keys.insert(parent->keys.begin() + i, y->keys[2]);
}

void BTree::drawTree(QGraphicsScene* scene, BTreeNode* node, int x, int y, int dx) {
    if (!node) return;

    QString nodeLabel;
    for (int key : node->keys) {
        nodeLabel += QString::number(key) + " ";
    }

    int width = 50 + 20 * node->keys.size();
    QGraphicsRectItem* rect = scene->addRect(x, y, width, 30, QPen(Qt::darkBlue), QBrush(Qt::cyan));
    QGraphicsTextItem* text = scene->addText(nodeLabel);
    text->setFont(QFont("Arial", 10, QFont::Bold));
    text->setDefaultTextColor(Qt::black);
    text->setPos(x + 5, y + 5);

    int childX = x - dx * (node->children.size() / 2);
    for (size_t i = 0; i < node->children.size(); ++i) {
        scene->addLine(x + width / 2, y + 30, childX + 30, y + 80, QPen(Qt::darkGray));
        drawTree(scene, node->children[i], childX, y + 80, dx / 2);
        childX += dx;
    }
}

class BTreeApp : public QWidget {
    Q_OBJECT

public:
    BTreeApp(QWidget* parent = nullptr) : QWidget(parent), tree(3) {
        QVBoxLayout* layout = new QVBoxLayout(this);

        QLabel* title = new QLabel("<h2 style='color:#2E8B57;'>B-Tree Indexing Simulator</h2>", this);
        title->setAlignment(Qt::AlignCenter);

        input = new QLineEdit(this);
        input->setPlaceholderText("Enter number to insert");
        input->setStyleSheet("padding: 5px; font-size: 14px;");

        QPushButton* insertButton = new QPushButton("Insert", this);
        insertButton->setStyleSheet("background-color: #2E8B57; color: white; padding: 8px; font-weight: bold; border-radius: 4px;");

        view = new QGraphicsView(this);
        view->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc;");
        scene = new QGraphicsScene(this);
        view->setScene(scene);

        layout->addWidget(title);
        layout->addWidget(input);
        layout->addWidget(insertButton);
        layout->addWidget(view);
        setLayout(layout);

        connect(insertButton, &QPushButton::clicked, this, &BTreeApp::handleInsert);
        resize(900, 700);
        updateTree();
    }

private slots:
    void handleInsert() {
        bool ok;
        int value = input->text().toInt(&ok);
        if (ok) {
            tree.insert(value);
            updateTree();
        } else {
            QMessageBox::critical(this, "Invalid Input", "Invalid input, enter numerical data");
        }
        input->clear();
    }

private:
    QLineEdit* input;
    QGraphicsView* view;
    QGraphicsScene* scene;
    BTree tree;

    void updateTree() {
        scene->clear();
        tree.drawTree(scene, tree.root, 400, 20, 120);
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMessageBox::information(nullptr, "B-Tree Indexing Simulator",
                             "Enter numerical data between 6 to 10 numbers to simulate the B-tree data indexing.\nClick OK to proceed.");

    BTreeApp window;
    window.setWindowTitle("B-Tree Indexing Simulator");
    window.show();
    return app.exec();
}
