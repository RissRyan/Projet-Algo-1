#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

#include "Tree.cpp"

static std::vector<std::pair<char, int>> alphabet;
static std::vector<Tree> trees;
static std::vector<Tree*> huffman;

static std::ifstream file;

static std::string const textPath = "C:/Dev/Mini projet 1/files/extraitalice.txt";


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

void fillListAlphabet()
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

int main()
{

    trees.reserve(1000);

    fillListAlphabet();

    for (auto& it : alphabet)
    {
        std::string lettre(1, it.first);
        trees.emplace_back(lettre, it.second);
    }

    for (auto& it : trees)
    {
        it.afficher();
        huffman.push_back(&it);
    }

    Tree* root = generateTree();

    return 0;
}
