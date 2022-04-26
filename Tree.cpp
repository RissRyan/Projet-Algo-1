#include <iostream>
#include <string>

class Tree
{

private:
	std::string letter = "";
	int frequency = 0;
	Tree* leftChild = nullptr;
	Tree* rightChild = nullptr;
	Tree* ancestor = nullptr;

public:
	Tree(std::string pLetter, int pFrequency) : letter(pLetter), frequency(pFrequency)
	{
	}

	void setChild(Tree* child, char leftOrRight)
	{
		if (leftOrRight == 'l')
		{
			leftChild = child;
		}
		else
		{
			rightChild = child;
		}
	}

	void afficher()
	{
		std::cout << this << " La lettre : " << letter << " a pour frequence " << frequency << ". L : " << leftChild << " R :"<< rightChild << std::endl;
	}

	int getFrequency()
	{
		return frequency;
	}

	void setAncestor(Tree* mother)
	{
		ancestor = mother;
	}

	Tree* getAncestor()
	{
		return ancestor;
	}
	
	std::string getLetter()
	{
		return letter;
	}

	std::pair<Tree*, Tree*> getChildren()
	{
		return std::make_pair(leftChild, rightChild);
	}

};