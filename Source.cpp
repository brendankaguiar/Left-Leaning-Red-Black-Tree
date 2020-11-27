#include "LeftLeaningRedBlack.h"
#define SIZE 10 //Tree Size

/*Main Declarations*/
void loadArr(VoidRef_t[]);
void displayArr(VoidRef_t[]);
void loadLLRBTree(VoidRef_t[], LeftLeaningRedBlack&);

int main()
{
	cout << "\nThis program, which executes once, shows basic functionality of the Left Leaning Red Black Tree.\n";
	LeftLeaningRedBlack LLRBT;
	VoidRef_t treeArr[SIZE];
	/*Load Features*/
	loadArr(treeArr); //Loads array with 10 random integers
	displayArr(treeArr);
	loadLLRBTree(treeArr,LLRBT); //Loads tree with 10 array
	cout << "Deleting 4th node :" << treeArr[3].Key << endl;
	LLRBT.Delete(treeArr[3].Key); //Deletes 4th value in Tree
	cout << "Traversal for proof of deletion...\n";
	LLRBT.Traverse();
	cout << "Freeing memory...\n";
	LLRBT.FreeAll();
	cout << "Memory freed...\n";
	cout << "Ending Program...\n";
	return 0;
}

/*Main Functions*/
void loadArr(VoidRef_t target[])
{
	std::cout << "Loading Array" << std::endl;
	for (int i = 0; i < SIZE; i++)
		target[i].Key = rand() % 200 + 1;     // range 1 to 200
	std::cout << "Array loaded successfully." << std::endl;
} // end loadArr

void displayArr(VoidRef_t target[]) 
{
	cout << "Values :";
	for (int i = 0; i < SIZE; i++)
		cout << target[i].Key << "\t";
	cout << endl;
}


void loadLLRBTree(VoidRef_t target[], LeftLeaningRedBlack& tree)
{
	std::cout << "Loading Tree" << std::endl;
	for (int i = 0; i < SIZE; i++)
		tree.Insert(target[i]);
	// range 1 to 200
	std::cout << "Tree loaded successfully." << std::endl;
}