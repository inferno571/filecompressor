#include <bits/stdc++.h>
#include <windows.h>

using namespace std;

struct HuffmanNode {
    char data;
    unsigned freq;
    HuffmanNode *left, *right;
    HuffmanNode(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

void deleteTree(HuffmanNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

class BitWriter {
    ofstream &out;
    char buffer;
    int bitCount;
public:
    BitWriter(ofstream &out) : out(out), buffer(0), bitCount(0) {}
    void writeBit(int bit) {
        buffer = (buffer << 1) | (bit & 1);
        if (++bitCount == 8) flush();
    }
    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            out.put(buffer);
            buffer = 0;
            bitCount = 0;
        }
    }
};

class BitReader {
    ifstream &in;
    char buffer;
    int bitCount;
public:
    BitReader(ifstream &in) : in(in), buffer(0), bitCount(0) {}
    int readBit() {
        if (bitCount == 0) {
            if (!in.get(buffer)) return EOF;
            bitCount = 8;
        }
        return (buffer >> (--bitCount)) & 1;
    }
};

void serializeTree(HuffmanNode* root, BitWriter &writer) {
    if (!root->left && !root->right) {
        writer.writeBit(1);
        for (int i = 7; i >= 0; --i)
            writer.writeBit((root->data >> i) & 1);
    } else {
        writer.writeBit(0);
        serializeTree(root->left, writer);
        serializeTree(root->right, writer);
    }
}

HuffmanNode* deserializeTree(BitReader &reader) {
    int bit = reader.readBit();
    if (bit == EOF) return nullptr;
    if (bit == 1) {
        char data = 0;
        for (int i = 0; i < 8; ++i) {
            bit = reader.readBit();
            if (bit == EOF) return nullptr;
            data = (data << 1) | bit;
        }
        return new HuffmanNode(data, 0);
    }
    HuffmanNode* left = deserializeTree(reader);
    HuffmanNode* right = deserializeTree(reader);
    HuffmanNode* node = new HuffmanNode('\0', 0);
    node->left = left;
    node->right = right;
    return node;
}

void compress(const string &inputFilename, const string &outputFilename) {
    ifstream inFile(inputFilename, ios::binary | ios::ate);
    if (!inFile) {
        cerr << "Cannot open input file!\n";
        return;
    }
    streamsize size = inFile.tellg();
    inFile.seekg(0, ios::beg);
    if (size == 0) {
        ofstream outFile(outputFilename, ios::binary);
        return;
    }

    vector<char> buffer(size);
    inFile.read(buffer.data(), size);
    inFile.close();

    unordered_map<char, unsigned> freqMap;
    for (char c : buffer) freqMap[c]++;

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto pair : freqMap)
        pq.push(new HuffmanNode(pair.first, pair.second));

    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        auto internal = new HuffmanNode('\0', left->freq + right->freq);
        internal->left = left;
        internal->right = right;
        pq.push(internal);
    }

    HuffmanNode* root = pq.top();
    unordered_map<char, string> huffmanCode;
    if (root->left || root->right) {
        function<void(HuffmanNode*, string)> generateCodes = [&](HuffmanNode* node, string code) {
            if (!node) return;
            if (!node->left && !node->right) huffmanCode[node->data] = code;
            generateCodes(node->left, code + "0");
            generateCodes(node->right, code + "1");
        };
        generateCodes(root, "");
    } else {
        huffmanCode[root->data] = "0";
    }

    ofstream outFile(outputFilename, ios::binary);
    BitWriter writer(outFile);
    serializeTree(root, writer);
    writer.flush();

    bool singleNode = !root->left && !root->right;
    if (singleNode) {
        uint32_t frequency = root->freq;
        outFile.write(reinterpret_cast<const char*>(&frequency), sizeof(frequency));
    } else {
        uint32_t totalBits = 0;
        for (char c : buffer) totalBits += huffmanCode[c].size();
        outFile.write(reinterpret_cast<const char*>(&totalBits), sizeof(totalBits));
        for (char c : buffer) {
            for (char bit : huffmanCode[c]) {
                writer.writeBit(bit == '1' ? 1 : 0);
            }
        }
        writer.flush();
    }
    deleteTree(root);
}

void decompress(const string &inputFilename, const string &outputFilename) {
    ifstream inFile(inputFilename, ios::binary);
    if (!inFile) {
        cerr << "Cannot open input file!\n";
        return;
    }
    BitReader reader(inFile);
    HuffmanNode* root = deserializeTree(reader);
    if (!root) return;

    ofstream outFile(outputFilename, ios::binary);
    if (!outFile) {
        deleteTree(root);
        return;
    }

    bool singleNode = !root->left && !root->right;
    uint32_t dataSize;
    inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
    if (singleNode) {
        for (uint32_t i = 0; i < dataSize; ++i)
            outFile.put(root->data);
    } else {
        HuffmanNode* current = root;
        uint32_t bitsRead = 0;
        while (bitsRead < dataSize) {
            int bit = reader.readBit();
            if (bit == EOF) break;
            current = (bit == 0) ? current->left : current->right;
            if (!current->left && !current->right) {
                outFile.put(current->data);
                current = root;
                bitsRead++;
            }
        }
    }
    deleteTree(root);
}

void showProgressBar() {
    cout << "Progress: [";
    for (int i = 0; i <= 30; ++i) {
        cout << "#";
        cout.flush();
        Sleep(40);
    }
    cout << "] Done!\n";
}

void displayBanner() {
    cout << "\033[1;32m";
    cout << "+---------------------------------------------------------------+\n";
    cout << "____ _ _    ____    ____ ____ _  _ ___  ____ ____ ____ ____ ____ ____\n";
    cout << "|___ | |    |___    |    |  | |\\/| |__] |__/ |___ [__  [__  |  | |__/ \n";
    cout << "|    | |___ |___    |___ |__| |  | |    |  \\ |___ ___] ___] |__| |  \\ \n";
    cout << "+---------------------------------------------------------------+\n";
    cout << "\033[0m";
}

int main() {
    string inputFile, outputFile;
    char choice;

    while (true) {
        system("cls");
        displayBanner();
        cout << "[1] Compress File\n";
        cout << "[2] Decompress File\n";
        cout << "[3] Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == '3') {
            cout << "Exiting program...\n";
            break;
        }

        cout << "Enter input file name: ";
        getline(cin, inputFile);
        cout << "Enter output file name: ";
        getline(cin, outputFile);

        if (choice == '1') {
            showProgressBar();
            compress(inputFile, outputFile);
            cout << "File compressed successfully!\n";
        }
        else if (choice == '2') {
            showProgressBar();
            decompress(inputFile, outputFile);
            cout << "File decompressed successfully!\n";
        }
        else {
            cout << "Invalid choice! Please try again.\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}
