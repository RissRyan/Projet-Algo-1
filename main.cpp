#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <bitset>
#include <filesystem >

#include "Tree.cpp"

static std::vector<std::pair<char, int>> alphabet;
static std::vector<std::pair<char, std::string>> codes;
static std::vector<Tree> trees;
static std::vector<Tree*> huffman;

static std::ifstream file;
static std::ofstream fileW;

static std::string const textPath1 = "Q:/GitHub/Projet-Algo-1/files/textesimple.txt";
static std::string const textPath2 = "Q:/GitHub/Projet-Algo-1/files/extraitalice.txt";
static std::string const textPath3 = "Q:/GitHub/Projet-Algo-1/files/alice.txt";

static std::string const codePath1 = "Q:/GitHub/Projet-Algo-1/files/huffmaned1.txt";
static std::string const codePath2 = "Q:/GitHub/Projet-Algo-1/files/huffmaned2.txt";
static std::string const codePath3 = "Q:/GitHub/Projet-Algo-1/files/huffmaned3.txt";


bool cmp(std::pair<char, int>& a, std::pair<char, int>& b)
{
    if (a.second == b.second)
    {
        return a.first < b.first;
    }
    else
    {
        return a.second < b.second;
    }
}

void fillListAlphabet(std:: string textPath)
{
    std::string lettre;

    file.open(textPath);

    if (file.is_open())
    {
        while (file)
        {
            lettre = file.get();
            bool isFind = false;

            // Si la lettre est déjà present dans notre tableau on incrémente la fréquence

            for (auto& it : alphabet) {

                if (it.first == lettre[0])
                {
                    isFind = true;
                    it.second += 1;
                }
            }

            // Sinon on l'ajoute
            
            if(!isFind)
            {
                alphabet.push_back(std::make_pair(lettre[0], 1));
            }
        }

        // On trie le tableau selon les fréquences

        std::sort(alphabet.begin(), alphabet.end(), cmp);

        std::cout << "Operation successfully performed\n";
        file.close();
    }
    else
    {
        std::cout << "Error opening file";
    }
}

Tree* generateTree()
{
    if (huffman.size() == 1)
    {
        // Retourne la racine
        return huffman[0];
    }
    else
    {
        // Booléen

        bool inserted = false;

        // Prend les deux arbres de fréqunces minimales
        Tree* t1 = huffman[0];
        Tree* t2 = huffman[1];


        // Crée l'arbre parent et le push

        Tree* node = new Tree("IN", t1->getFrequency() + t2->getFrequency());

        node->setChild(t1, 'l');
        node->setChild(t2, 'r');

        t1->setAncestor(node);
        t2->setAncestor(node);

        trees.push_back(*node);


        for (auto it = huffman.begin(); it != huffman.end(); ++it)
        {
            if (node->getFrequency() <= (*it)->getFrequency())
            {
                huffman.insert(it, node);
                inserted = true;
                break;
            }
        }

        if (!inserted)
        {
            huffman.push_back(node);
        }

        // Supprime les elements minimaux

        huffman.erase(huffman.begin());
        huffman.erase(huffman.begin());

        generateTree();
    }
}

std::string codeLetter(char letter)
{
    Tree* tree = nullptr;
    std::string code;

    for (auto it = trees.begin(); it != trees.end(); ++it)
    {
        if ((*it).getLetter() != "IN" && ((*it).getLetter())[0] == letter)
        {
            tree = &(*it);
            break;
        }
    }

    while (tree->getAncestor() != nullptr)
    {
        Tree* theAncestor = tree->getAncestor();

        if (theAncestor->getChildren().first == tree)
        {
            code += "0";
        }

        if (theAncestor->getChildren().second == tree)
        {
            code += "1";
        }

        tree = theAncestor;
    }

    std::reverse(code.begin(), code.end());

    return code;
}

void codeAllLetters()
{
    for (auto& it : alphabet)
    {
        codes.push_back(std::make_pair(it.first, codeLetter(it.first)));
    }
}

void generateCode(Tree* src, std::string textPath, std::string codePath)
{
    fileW.open(codePath, std::ofstream::out | std::ofstream::binary);
    file.open(textPath);

    std::string lettre;
    std::string code;
    std::string waitingBits = "";

    if (file.is_open())
    {
        while (file)
        {
            lettre = file.get();
            

            for (auto it = codes.begin(); it != codes.end(); ++it)
            {
                if ((*it).first == lettre[0])
                {
                    code = (*it).second;
                    break;
                }
            }

            if (code.size() + waitingBits.size() > 8)
            {
                waitingBits += code;
                std::bitset<8> myBits(waitingBits.substr(0, 8));
                waitingBits.erase(0, 8);
                unsigned long i = myBits.to_ulong();
                unsigned char c = static_cast<unsigned char>(i);
                fileW << c;

            }
            else if (code.size() + waitingBits.size() < 8)
            {
                waitingBits += code;
            }
            else
            {
                std::bitset<8> myBits(waitingBits + code);
                waitingBits = "";
                unsigned long i = myBits.to_ulong();
                unsigned char c = static_cast<unsigned char>(i);
                fileW << c;
            }

        }

        file.close();
    }
    else
    {
        std::cout << "Error opening file";
    }

    fileW << "a";

    fileW.close();
}

void compressionRatio(std::string original, std::string huffmaned)
{
    float res = 1.0f -  (float)std::filesystem::file_size(huffmaned) / (float)std::filesystem::file_size(original);
    std::cout << "Le taux de compression pour " << original << " est de " << res << " %.\n";
}

void avgSize()
{
    float res = 0.0f;

    for (auto it = codes.begin(); it != codes.end(); ++it)
    {
        res += (float)(*it).second.size();
    }

    std::cout << "Le nombre de bits moyen pour coder une lettre est : " << res / (float)codes.size();
}

void codageHuffman(std::string textPath, std::string codePath)
{

    fillListAlphabet(textPath);

    for (auto& it : alphabet)
    {
        std::string lettre(1, it.first);
        trees.emplace_back(lettre, it.second);
    }

    for (auto& it : trees)
    {
        //it.afficher();
        huffman.push_back(&it);
    }

    Tree* root = generateTree();

    codeAllLetters();

    generateCode(root, textPath, codePath);

    compressionRatio(textPath, codePath);

    avgSize();

    alphabet.clear();

    for (auto& it : trees)
    {
        it.~Tree();
    }

    trees.clear();

    huffman.clear();

    codes.clear();
}

int main()
{

    trees.reserve(1000);

    codageHuffman(textPath1, codePath1);
    codageHuffman(textPath2, codePath2);
    codageHuffman(textPath3, codePath3);

    return 0;
}
