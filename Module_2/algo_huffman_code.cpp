/* Напишите две функции для создания архива из одного файла и извлечения файла из архива.
Требования: коэффициент сжатия < 1. */
#include <iostream>
#include <sstream>
#include <vector>
#include <exception>
#include <map>
#include <queue>
#include <memory>
#include <bitset>
#include <iterator>
#include <stack>
#include <fstream>

#include "Huffman.h"

#define MINIMAL_FREQUENCY 15
#define MAX_WORDS 900
#define MAX_WORD_LEN 31
#define MAX_WORD_BITS 5
#define MIN_LETTER_IN_WORD 2
#define DEBUG
class BitWriter {
 public:
    BitWriter() : bitCount(0) {}

    void WriteBit(unsigned char bit) {
        if (bitCount == buffer.size() * 8)
            buffer.push_back(0);
        if (bit)
            buffer[bitCount/8] |= 1 << (7 - bitCount % 8);
        bitCount++;
    }

    void WriteBits(unsigned char byte, size_t bits) {
        for (int i = MAX_WORD_BITS - 1; i >= 0; i--) {
            WriteBit((byte >> i) & 1);
        }
    }

    void WriteByte(unsigned char val) {
        if (bitCount % 8 == 0) {
            buffer.push_back(val);
        } else {
            int offset = bitCount % 8;
            buffer[bitCount/8] |= val >> offset;
            buffer.push_back(val << (8 - offset));
        }
        bitCount += 8;
    }

    size_t DeleteBits(size_t number) {
        if (bitCount == buffer.size() * 8)
            return 0;
        if (number && (bitCount > number)) {
            size_t bits = bitCount % 8;
            if (!bits) {
                buffer[(bitCount-8) / 8] &= ((UINTMAX_MAX << (number)) % UINTMAX_MAX);
                bitCount -= number;
            }
            if (number > bits) {
                buffer[bitCount / 8] = 0;
                buffer[(bitCount - 8) / 8] &= ((UINTMAX_MAX << (8 - (number - bits))) % UINTMAX_MAX);
                bitCount -= number;
            } else {
                buffer[(bitCount) / 8] &= ((UINTMAX_MAX << ((8 - bits) + number)) % UINTMAX_MAX);
                bitCount -= number;
            }
            return number;
        }
        size_t temp = bitCount % number;
        bitCount -= temp;
        return temp;
    }

    const std::vector<unsigned int> &GetBuffer() const {
        return buffer;
    }

    size_t GetBitCount() const {
        return bitCount;
    }

 private:
    std::vector<unsigned int> buffer;
    size_t bitCount;
};

void visualize(const std::vector<unsigned int> &buffer) {
    for (auto &val : buffer) {
        std::cout << std::bitset<8>(val) << "|";
    }
    std::cout << std::endl;
}

void visualize2electricbugalo(const std::vector<unsigned int> &buffer) {
    for (auto &val : buffer) {
        std::cout << static_cast<char>(val) << "|";
    }
    std::cout << std::endl;
}

class MyException: public std::exception {
 public:
    explicit MyException(std::string exc) : myError(exc) {}
    std::string GetError() {
        return myError;
    }

 private:
    std::string myError;
};


struct Node {
    size_t freq;
    struct Node *left, *right;
    std::string code;
    std::vector<byte> word;
    bool operator<(const Node &rhs) const {
        return freq < rhs.freq;
    }

    bool operator>(const Node &rhs) const {
        return freq > rhs.freq;
    }
    Node():freq(0), left(nullptr), right(nullptr) {}
    Node(size_t num, byte val):freq(num), left(nullptr), right(nullptr) {
        word = std::vector<byte>();
        word.push_back(val);
    }
    Node(size_t num, std::vector<byte> val):freq(num), left(nullptr), right(nullptr), word(val) {}
    Node(Node *l, Node *r):left(l), right(r) {
        if (!l) {
            freq = 0;
        } else if (!r) {
            freq = l->freq;
        } else {
            freq = l->freq + r->freq;
        }
    }
};

bool CheckLetter(const byte letter) {
    return (letter == '\n' || letter == '\r' || letter == ' ' || letter == '.' || letter ==  ',' || letter ==  '!' || letter ==  '(' || letter ==  ')' || letter ==  '-' || letter ==  ':' || letter ==  '\"' || letter ==  ';');
}


std::pair<std::map<std::vector<byte>, size_t>, std::map<std::vector<byte>, size_t>>
    SymbolFrequencyCalc(IInputStream &source, std::queue<byte> &dest) {
    std::map<std::vector<byte>, size_t> letters_map;
    std::map<std::vector<byte>, size_t> words_map;
    byte val;
    std::vector<byte> word;
    size_t symbols = 0;
    std::vector<byte> letter;
    size_t data_len = 0;
    while ( source.Read(val) ) {
        data_len++;
        if (!CheckLetter(val)) {
            word.push_back(val);
            symbols++;
        } else {
            if (symbols > MIN_LETTER_IN_WORD) {
                words_map[word]++;
            }
            word = std::vector<byte>();
            symbols = 0;
        }
        letter.push_back(val);
        letters_map[letter]++;
        dest.push(val);
        letter = std::vector<byte>();
    }
    if (symbols > 1) {
        words_map[word]++;
    }
    #ifdef DEBUG
    data_len *= 8;
    for (auto &it : words_map) {
        for (size_t i = 0; i < it.first.size(); i++) {
            std::cout << (char)it.first[i];
            //words_len += 8;
        }
        //words_len += it.first.size() * 8 * it.second;
        // std::cout << " word size " << it.first.size() << " word freq " << it.second << std::endl;
    }
    // for (auto &it : letters_map) {
    //     //letters_len += 8*it.second;
    //     // std::cout << " letter " << " letter freq " << it.second;
    // }
    std::cout << std::endl;
    #endif
    std::pair<std::map<std::vector<byte>, size_t>, std::map<std::vector<byte>, size_t>> result;
    result.first = letters_map;
    result.second = words_map;
    return result;
}

size_t UpdateFrequencies(std::pair<std::map<std::vector<byte>, size_t>, std::map<std::vector<byte>, size_t>> &pair) {
    auto compare = [](Node* lhs, Node* rhs) {
 			return (lhs->freq + (4 * lhs->word.size())) < (rhs->freq + (4 * rhs->word.size()));
            };
    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> q(compare);
    for (std::map<std::vector<byte>, size_t>::iterator it = pair.second.begin(); it != pair.second.end(); ++it) {
        // if (it->second < MINIMAL_FREQUENCY) {
        //     pair.second.erase(it->first);
        // } else {
            Node* number = new Node(it->second, it->first);
            q.push(number);
        //}
    }
    size_t words = 0;
    std::vector<byte> letter;
    while ((!q.empty()) && (words < MAX_WORDS)) {
        if (q.top()->freq > MINIMAL_FREQUENCY) {
            words++;
            for (size_t i = 0; i < q.top()->word.size(); i++) {
                letter.push_back(q.top()->word[i]);
                pair.first[letter] -= q.top()->freq;
                if (pair.first[letter] == 0) {
                    pair.first.erase(letter);
                }
                letter = std::vector<byte>();
            }
        } else {
            pair.second.erase(q.top()->word);
        }
        q.pop();
    }
    while (!q.empty()) {
        pair.second.erase(q.top()->word);
        q.pop();
    }
    return words;
}

Node* GetQueueRoot (std::pair<std::map<std::vector<byte>, size_t>, std::map<std::vector<byte>, size_t>> pair) {
    auto compare = [](Node* lhs, Node* rhs) {
                return (lhs->freq * 2 - 1) > (rhs->freq * 2 - 1);
            };
    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> q(compare);
    for (std::map<std::vector<byte>, size_t>::iterator it = pair.first.begin(); it != pair.first.end(); ++it) {
        Node* number = new Node(it->second, it->first);
        q.push(number);
    }
    for (std::map<std::vector<byte>, size_t>::iterator it = pair.second.begin(); it != pair.second.end(); ++it) {
        Node* number = new Node(it->second, it->first);
        q.push(number);
    }
    while (q.size() != 1) {
        Node* right = q.top();
        q.pop();
        Node* left = q.top();
        q.pop();
        Node* p = new Node(left, right);
        q.push(p);
    }
    return q.top();
}

std::map<std::vector<byte>, std::string> createCodedTable(Node &root) {
    std::stack<Node*> buf;
    buf.push(&root);
    std::map<std::vector<byte>, std::string> table;
    while (!buf.empty()) {
        Node *cur_node = buf.top();
        buf.pop();

        if (!cur_node->right && !cur_node->left) {
            (table)[cur_node->word] = cur_node->code;
            #ifdef DEBUG
                std::cout << "word ";
                for (auto it : cur_node->word) {
                    std::cout << it << " ";
                }
                // if ( cur_node->word.size() > 1 ) {
                //     std::cout << "word!!" << std::endl;
                // }
                std::cout << "code " << cur_node->code << " freq " << cur_node->freq << std::endl;
            #endif
        }
        if (cur_node->right) {
            cur_node->right->code += cur_node->code + "1";
            buf.push(cur_node->right);
        }
        if (cur_node->left) {
            cur_node->left->code += cur_node->code + "0";
            buf.push(cur_node->left);
        }
    }
    return table;
}

void BitTreeWriter(BitWriter &dst, Node *node, size_t &count, size_t zeroes) {
    if (!node->right && !node->left) {
        dst.WriteBit(1);
        #ifdef DEBUG
        std::cout << 1;
        #endif
        // Need to write letter number, we look for words up to 256
        if (node->word.size() == 1) {
            dst.WriteBit(0);
        } else {
            dst.WriteBit(1);
            dst.WriteBits(node->word.size(), MAX_WORD_BITS);
        }
        for (size_t i = 0; i < node->word.size(); i++) {
            dst.WriteByte(node->word[i]);
        }
        count--;
        return;
    }
    if (node->left) {
        BitTreeWriter(dst, node->left, count, zeroes);
    }
    if (node->right) {
        BitTreeWriter(dst, node->right, count, zeroes);
    }
    if (count) {
        dst.WriteBit(0);
        #ifdef DEBUG
        std::cout << 0;
        #endif
    }
}

void BitEncode(BitWriter &dst, std::queue<byte> &src,
                   std::map<std::vector<byte>, std::string> table) {
    byte letter;
    std::vector<byte> word;
    size_t word_len = 0;
    letter = src.front();
    src.pop();
    while (!src.empty()) {
        // TODO FIX
        // check word bounds
        while (!CheckLetter(letter) && (word_len < MAX_WORD_LEN) && !src.empty()) {
            word.push_back(letter);
            letter = src.front();
            // std::cout << " " << src.size();
            src.pop();
            word_len++;
        }

        if (word_len == 0) {
            word.push_back(letter);
            letter = src.front();
            src.pop();
            word_len++;
        }
        if (src.empty()) {
            word.push_back(letter);
            word_len++;
        }
        std::string code = (table)[word];
        std::vector<byte> tmp;
        if (word_len == 1) {  // is letter
            for (size_t i = 0; i < code.size(); i++) {
                if (code[i] == '0') {
                    dst.WriteBit(0);
                } else {
                    dst.WriteBit(1);
                }
            }
        } else {
            if (code.empty()) {
                tmp = std::vector<byte>();
                for (size_t k = 0; k < word.size(); k++) {
                    tmp.push_back(word[k]);
                    code = (table)[tmp];
                    tmp = std::vector<byte>();
                    for (size_t i = 0; i < code.size(); i++) {
                        if (code[i] == '0') {
                            dst.WriteBit(0);
                        } else {
                            dst.WriteBit(1);
                        }
                    }
                }
            } else {
                for (size_t i = 0; i < code.size(); i++) {
                    if (code[i] == '0') {
                        dst.WriteBit(0);
                    } else {
                        dst.WriteBit(1);
                    }
                }
            }
        }
        word = std::vector<byte>();
        word_len = 0;
        // std::cout << " " << src.size() << std::endl;
        if (src.empty()) {
            return;
        }
        // letter = src.front();
        // src.pop();
    }
}

std::queue<byte> Copy(IInputStream &source) {
    byte val;
    std::queue<byte> temp;
    while ( source.Read(val) ) {
        temp.push(val);
    }
    return temp;
}


void Encode(IInputStream& original, IOutputStream& compressed) {
    //  IOutputStream& copy;
    //  auto originalCopy = std::make_unique<std::queue<byte>>();
    //  std::queue<byte> copy = Copy(original);
    std::queue<byte> copy;
    //  1. Один раз проходимся по входным данным, считая частотности букв алфавита.
    //  Так как пройтись придется дважды, надо запомнить весь ввод.
    std::map<byte, size_t> LetterFreqMap;
    std::map<std::vector<byte>, size_t> WordFreqMap;
    std::pair<std::map<std::vector<byte>, size_t>, std::map<std::vector<byte>, size_t>> Frequency;
    Frequency = SymbolFrequencyCalc(original, copy);
    // std::map<byte, size_t> freqMap = SymbolFrequencyCalc(original, copy);
    std::queue<byte> copy_original = copy;

    std::map<std::vector<byte>, std::string> codedTable;
    UpdateFrequencies(Frequency);
    size_t count = Frequency.first.size() + Frequency.second.size();
    Node* root = GetQueueRoot(Frequency);
    Node temp = *root;
    // get coded table
    codedTable = createCodedTable(temp);
    BitWriter bw;
    // write # of symbols in table
    bw.WriteByte(count >> 8);
    bw.WriteByte((count & 255));
    // write encoding tree
    size_t number = count;
        #ifdef DEBUG
        std::cout << bw.GetBitCount() << std::endl;
        #endif
    BitTreeWriter(bw, root, count, number);
        #ifdef DEBUG
        std::cout  << std::endl;
        //std::cout << "tree 2 words =" << words << std::endl;
        // std::cout << zero << std::endl;
        std::cout << bw.GetBitCount() << std::endl;
        #endif
    // bw.DeleteBits(zero);
    // encode input
    BitEncode(bw, copy, codedTable);
        #ifdef DEBUG
        std::cout << bw.GetBitCount() << std::endl;
        #endif
    size_t total_len = bw.GetBitCount();
    size_t remainder = total_len % 8;  // 8 - total_len % 8
    if (remainder <=5 && remainder > 0) {
        while (remainder != 5) {
            bw.WriteBit(0);
            remainder++;
        }
        bw.WriteBit((total_len % 8 >> 2) & 1);
        bw.WriteBit((total_len % 8 >> 1) & 1);
        bw.WriteBit((total_len % 8 >> 0) & 1);
    } else {
        if (total_len % 8 == 6) {
            bw.WriteBit(0);
            bw.WriteBit(0);
            bw.WriteByte((128));
        }
        if (total_len % 8 == 7) {
            bw.WriteBit(0);
            bw.WriteByte((0b11000000));
        }
        if (total_len % 8 == 0) {
            bw.WriteByte((0b11100000));
        }
          // 11111 000 | остаток (три бита метка - 0)
    }
    // while (remainder != 0 && remainder != 8) {
    //     bw.WriteBit(0);
    //     remainder--;
    // }
    // bw.WriteByte(total_len % 8);
    std::vector<unsigned int> coded = bw.GetBuffer();
    #ifdef DEBUG
    // visualize(coded);
    std::cout << bw.GetBitCount() << std::endl;
    std::cout << copy_original.size() * 8 << std::endl;
    #endif

    // testing if no compression is worth it
    if ( (copy_original.size() * 8) >= bw.GetBitCount() ) {
        for (auto &val : coded) {
            compressed.Write(val);
        }
        //codedTable.clear();
        //compressed.Write(0);
    } else {
        compressed.Write(0);
        compressed.Write(0);
        while (!(copy_original.empty())) {
            compressed.Write(copy_original.front());
            copy_original.pop();
        }
    }
}

template <typename T>
bool node_less(const T &l, const T &r) {
    if (l.freq == r.freq)
        return l.freq < r.freq;
    return l.freq < r.freq;
}


void BitDecode(std::shared_ptr<BitWriter> &dst, std::vector<byte> &src, size_t last) {
    while (!src.empty()) {
        // byte tmp = src.front();
        // src.pop();
        std::string code;
        for (size_t i = 0; i < code.size(); i++) {
            if (code[i] == '0') {
                dst->WriteBit(0);
            } else {
                dst->WriteBit(1);
            }
        }
    }
}

Node* DecodeTree (std::vector<byte> &input, size_t &byte_num, size_t &bit_num, size_t &bit_count, std::bitset<8> &bits_current) {
    size_t symbols = 0;
    size_t byte_n = byte_num;
    size_t bit_n = bit_num;
    size_t  bit_c = bit_count;
    std::bitset<8> bits = bits_current;
    size_t count = input[0] << 8 | input[1];
    // получаем stack дерева
    std::stack<Node*> new_tree;
    std::vector<byte> word;
    while (symbols < count) {
        std::bitset<8> letter;
        if (bits[bit_n] == 1) {  // leaf
            if (!bit_n) {
                bit_n = 8;
                byte_n++;
                bits = input[byte_n];
            }
            bit_n--;
            bit_c++;
            if (bits[bit_n] == 1) {  // word length in next byte
                if (!bit_n) {
                    bit_n = 8;
                    byte_n++;
                    bits = input[byte_n];
                }
                bit_n--;
                bit_c++;
                letter = std::bitset<8>();
                for (int i = MAX_WORD_BITS - 1; i >= 0; i--) {  // get the length
                    letter[i] = bits[bit_n];
                    if (!bit_n) {
                        bit_n = 8;
                        byte_n++;
                        bits = input[byte_n];
                    }
                    bit_n--;
                    bit_c++;
                }
                // read the vector
                for (size_t k = letter.to_ulong(); k > 0; k--) {
                    for (int i = 7; i >= 0; i--) {
                        letter[i] = bits[bit_n];
                        if (!bit_n) {
                            bit_n = 8;
                            byte_n++;
                            bits = input[byte_n];
                        }
                        bit_n--;
                        bit_c++;
                    }
                    word.push_back(static_cast<byte>(letter.to_ulong()));
                    letter = std::bitset<8>();
                }
                Node* leaf = new Node(symbols, word);
                new_tree.push(leaf);
                word = std::vector<byte>();
                symbols++;
            } else {  // letter in next byte
                if (!bit_n) {
                    bit_n = 8;
                    byte_n++;
                    bits = input[byte_n];
                }
                bit_n--;
                bit_c++;
                for (int i = 7; i >= 0; i--) {
                    letter[i] = bits[bit_n];
                    if (!bit_n) {
                        bit_n = 8;
                        byte_n++;
                        bits = input[byte_n];
                    }
                    bit_n--;
                    bit_c++;
                }
                Node* leaf = new Node(symbols, static_cast<byte>(letter.to_ulong()));
                new_tree.push(leaf);
                letter = std::bitset<8>();
                symbols++;
            }
        } else {  // not leaf
            Node* new_r = nullptr;
            if (!new_tree.empty()) {
                new_r = new_tree.top();
                new_tree.pop();
            }
            Node* new_l = nullptr;
            if (!new_tree.empty()) {
                new_l = new_tree.top();
                new_tree.pop();
            }
            Node* p = new Node(new_l, new_r);
            new_tree.push(p);
            if (!bit_n) {
                bit_n = 8;
                byte_n++;
                bits = input[byte_n];
            }
            bit_n--;
            bit_c++;
        }
    }
    size_t deleted = 0;
    while (new_tree.size() != 1) {
        // bit_n--;
        // bit_c++;
        Node* new_r = new_tree.top();
        new_tree.pop();
        Node* new_l = new_tree.top();
        new_tree.pop();
        Node* p = new Node(new_l, new_r);
        new_tree.push(p);
        deleted++;
    }
    #ifdef DEBUG
    std::cout << "deleted " <<deleted << std::endl;
    #endif
    byte_num = byte_n;
    bit_num = bit_n;
    bit_count = bit_c;
    bits_current = bits;
    Node* root = new_tree.top();
    new_tree.pop();
    return root;
}

void Decode(IInputStream& compressed, IOutputStream& original) {
    std::vector<byte> input;
    size_t bytes_read = 0;
    byte key;
    while (compressed.Read(key)) {
        input.push_back(key);
        bytes_read++;
    }
    // was testing if non-compression is a problem
    if ((input[0] == 0) && (input[1] == 0)) {
        input.erase(input.begin());
        input.erase(input.begin());
        for (auto &val : input) {
            original.Write(static_cast<char>(val));
        }
        return;
    }

    size_t byte_n = 2;  // incremented cos 2 byte size
    size_t bit_n = 7;
    size_t bit_c = 16;
    std::bitset<8> bits = input[byte_n];
    // reconstruct tree, get root
    Node* new_root = DecodeTree(input, byte_n, bit_n, bit_c, bits);
    Node* cur_node = new_root;
    BitWriter answer;
    bool byte_less = 0;
    size_t last_byte_bits = input[input.size()-1];
    if ((last_byte_bits & 7) != 0) { // check last 3 bits
        last_byte_bits &= 7;
        byte_less = 1;
    } else {
        switch (last_byte_bits) {
            case 0b10000000: last_byte_bits = 6; break;
            case 0b11000000: last_byte_bits = 7; break;
            case 0b11100000: last_byte_bits = 8; break;
        }
    }
    size_t bits_max;
    if (byte_less) {
        bits_max  = ((input.size())*8 - (8 - last_byte_bits));
    } else {
        bits_max  = ((input.size())*8 - 8 - (8 - last_byte_bits));
    }
    size_t bits_read = bit_c;
    size_t letter_read = 0;
    #ifdef DEBUG
    std::cout << "bits read " <<  bits_read << std::endl;
    std::cout << "bits max " <<  bits_max << std::endl;
    std::cout << "last_byte_bits " <<  last_byte_bits << std::endl;
    #endif
    //decode TODO: move to function
    while (bits_read < bits_max) {
        while ((cur_node->left != nullptr) && (cur_node->right != nullptr) && (bits_read < bits_max)) {
            if (bits[bit_n] == 0) {
                cur_node = cur_node->left;
                #ifdef DEBUG
                std::cout << "0";
                #endif
            } else {
                cur_node = cur_node->right;
                #ifdef DEBUG
                std::cout << "1";
                #endif
            }
            if (!bit_n) {
                bit_n = 8;
                byte_n++;
                bits = input[byte_n];
            }
            bit_n--;
            bits_read++;
        }
        //TODO FIX
        // for (auto it : cur_node->word) {
        //     answer.WriteByte(it);
        // }
                #ifdef DEBUG
                std::cout << std::endl;
                #endif
        for (size_t i = 0; i < cur_node->word.size(); i++) {
            answer.WriteByte(cur_node->word[i]);
        }
        // answer.WriteByte(cur_node->data);
        letter_read++;
        cur_node = new_root;
    }
    input.clear();
    std::vector<unsigned int> ans = answer.GetBuffer();
    // visualize function
    #ifdef DEBUG
    visualize2electricbugalo(ans);
    std::cout << ans.size() * 8 << std::endl;
    std::cout << "bits read " <<  bits_read << std::endl;
    std::cout << "letters read " << letter_read << std::endl;
    #endif
    for (auto &val : ans) {
         original.Write(static_cast<char>(val));
    }
}


#ifdef DEBUG
class inputs: public IInputStream, public IOutputStream {
 public:
    inputs(std::string input = "example") {
        for (byte byte : input) {
            buffer.push(byte);
        }
    }

    bool Read(byte& value) {
        if (buffer.empty()) {
            return false;
        }

        value = buffer.front();
        buffer.pop();
        return true;
    }

    void Write(byte value) {
        buffer.push(value);
    }

    void Clear() {
        std::queue<byte> empty;
        std::swap(buffer, empty);
    }

    void Print() {
        for (size_t i = 0; i < buffer.size(); ++i) {
            std::cout << buffer.front();
            buffer.push(buffer.front());
            buffer.pop();
        }
        std::cout << std::endl;
    }

    void PrintBytes() {
        for (size_t i = 0; i < buffer.size(); ++i) {
            std::cout << std::bitset<8>(buffer.front()) << "|";
            buffer.push(buffer.front());
            buffer.pop();
        }
        std::cout << std::endl;
    }

    std::string ToString() {
        std::string result;
        for (size_t i = 0; i < buffer.size(); ++i) {
            result += buffer.front();
            buffer.push(buffer.front());
            buffer.pop();
        }
        return result;
    }

 private:
    size_t pos;
    std::queue<byte> buffer;
};

void load_file(std::string& s, std::istream& is) {
    s.erase();
    if (is.bad()) return;
    //
    // attempt to grow string buffer to match file size,
    // this doesn't always work...
    s.reserve(is.rdbuf()->in_avail());
    char c;
    while (is.get(c)) {
      // use logarithmic growth stategy, in case
      // in_avail (above) returned zero:
      if ( s.capacity() == s.size() )
         s.reserve(s.capacity() * 3);
      s.append(1, c);
    }
}


int main(int argc, const char * argv[]) {
    // inputs InputFake, OutputFake;
    // OutputFake.flush();
    // Encode(InputFake, OutputFake);
    // InputFake.flush();
    // Decode(OutputFake, InputFake);
    // // void Encode(IInputStream& original, IOutputStream& compressed) {
    // // void Decode(IInputStream& compressed, IOutputStream& original)
    std::ifstream input("data.txt", std::fstream::in);
    std::string inputString;
    load_file(inputString, input);
    input.close();

    inputs first(inputString), second;
    second.Clear();
    Encode(first, second);
    Decode(second, first);

    std::ofstream file("output.txt", std::fstream::out);
    std::string my_string = first.ToString();
    file << my_string;
    file.close();

    first.Print();
    return 0;
}

#endif
