/////////////////////////////////////////////////////////////////////////////
//
//	File: LeftLeaningRedBlack.cpp
//
//	$Header: $
//
//
//	This version of red-black trees is based on Robert Sedgewick's new
//	algorithm, "Left Leaning Red Black Trees", based on papers published in
//	2008 and 2009.
//
//	A Left Leaning Red-Black Tree ("LLRB" for short) is a variation of the
//	standard red-black tree.  It requires that if a node has one red child,
//	that child must be the left child (hence the tree "leans to the left".
//
//	It also allows a black node to have two red children, effectively making
//	the node a 4-node.
//
//	Depending on how values are inserted into the tree, the tree may emulate
//	either a 2-3 tree or a 2-3-4 tree.  The USE_234_TREE symbol controls
//	which rule is applied.  Empirically, 2-3 trees are slightly faster for
//	insertions and look-ups, since fewer special cases are triggered due to
//	how the nodes in the tree are arranged.  Test it if it matters, but in
//	general, leave the USE_234_TREE symbol undefined for better overall
//	performance.
//
//	By enforcing the left-leaning rule, fewer special cases need to be tested
//	when performing insertions and deletions, meaning that less code needs to
//	be written to implement an LLRB.
//
//	In theory, this also makes the code faster.  However, in practice the
//	LLRB is slower than a normal red-black tree.  Due to the need to touch
//	and rearrange more child nodes to test and enforce the left-leaning
//	rule, the LLRB has to access more memory, which makes the code perform
//	slower on modern computers -- main memory is much slower than the CPU,
//	so all of those extra memory accesses cause more pipeline stalls, which
//	inhibits performance.  When running on a system where main memory runs
//	at the same speed as the CPU, LLRBs do demonstrate better performance.
//
//	However, due to limits on specific embedded processors used for this
//	testing, any performance generalization is not reliable.  If you need to
//	use balanced trees on an embedded processor (or any other processor that
//	has the CPU and memory running at similar clock speeds), the different
//	algorithms should be tested on that hardware to determine which one
//	yields the best performance.
//
/////////////////////////////////////////////////////////////////////////////


/*LLRBT methods provided by Lee Stanza*/
#include "LeftLeaningRedBlack.h"
//#include "QzCommon.h"

#ifdef USE_MALLOC_MACRO
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Define this symbol to configure the red-black tree as a 2-3-4 tree.
// If this is not defined, the tree is arranged as a 2-3 tree.
//
// In general, defining this symbol will reduce performance of all
// operations on the LLRB.
//
//#define USE_234_TREE


/////////////////////////////////////////////////////////////////////////////
//
//	constructor
//
LeftLeaningRedBlack::LeftLeaningRedBlack(void)
	: m_pRoot(nullptr)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//	destructor
//
LeftLeaningRedBlack::~LeftLeaningRedBlack(void)
{
	Free(m_pRoot);
}


/////////////////////////////////////////////////////////////////////////////
//
//	FreeAll()
//
void LeftLeaningRedBlack::FreeAll(void)
{
	Free(m_pRoot);

	m_pRoot = nullptr;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Free()
//
void LeftLeaningRedBlack::Free(LLTB_t* pNode)
{
	if (nullptr != pNode) {
		if (nullptr != pNode->pLeft) {
			Free(pNode->pLeft);
		}
		if (nullptr != pNode->pRight) {
			Free(pNode->pRight);
		}

		pNode = nullptr;
		delete pNode;

	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	NewNode()
//
//	Note that a new node defaults to being red.
//
LLTB_t* LeftLeaningRedBlack::NewNode(void)
{
	LLTB_t* pNew = new LLTB_t;

	pNew->Ref.Key = 0;
	pNew->IsRed = true;
	pNew->pLeft = nullptr;
	pNew->pRight = nullptr;

	return pNew;
}


/////////////////////////////////////////////////////////////////////////////
//
//	LookUp()
//
//	Since a red-black tree is a binary tree, look-up operations are done
//	using iterative traversal.
//
//	This assumes that all keys stored in the tree have been assigned non-nullptr
//	values.
//
//	If the key is not in the tree, this will return nullptr.
//
void* LeftLeaningRedBlack::LookUp(uint32_t key)
{
	LLTB_t* pNode = m_pRoot;

	while (nullptr != pNode) {
		if (key < pNode->Ref.Key) {
			pNode = pNode->pLeft;
		}
		else {
			pNode = pNode->pRight;
		}
	}

	return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
//
//	IsRed()
//
static inline bool IsRed(LLTB_t* pNode)
{
	return ((nullptr != pNode) && pNode->IsRed);
}


/////////////////////////////////////////////////////////////////////////////
//
//	RotateLeft()
//
//	Rotate the subtree to the left:
//
//	   4            6
//	  / \          /
//	 2   6  -->   4
//	             /
//	            2
//
//	Node '6' will take on the color '4' used to have, while '4' becomes a
//	red node.
//
static LLTB_t* RotateLeft(LLTB_t* pNode)
{
	LLTB_t* pTemp = pNode->pRight;
	pNode->pRight = pTemp->pLeft;
	pTemp->pLeft = pNode;
	pTemp->IsRed = pNode->IsRed;
	pNode->IsRed = true;

	return pTemp;
}


/////////////////////////////////////////////////////////////////////////////
//
//	RotateRight()
//
//	Rotate the subtree to the right:
//
//	   4        2
//	  / \        \
//	 2   6  -->   4
//	               \
//	                6
//
//	Node '2' will take on the color '4' used to have, while '4' becomes a
//	red node.
//
static LLTB_t* RotateRight(LLTB_t* pNode)
{
	LLTB_t* pTemp = pNode->pLeft;
	pNode->pLeft = pTemp->pRight;
	pTemp->pRight = pNode;
	pTemp->IsRed = pNode->IsRed;
	pNode->IsRed = true;

	return pTemp;
}


/////////////////////////////////////////////////////////////////////////////
//
//	ColorFlip()
//
//	Flips the color of the node and both of its children from black to red
//	(or red to black).
//
//	Any time that a color flip is applied, it may cause pNode to end up
//	with an invalid color relative to its parent node.  Therefore some kind
//	of fix-up operation will be required after this function is called.
//
static void ColorFlip(LLTB_t* pNode)
{
	pNode->IsRed = !pNode->IsRed;

	if (nullptr != pNode->pLeft) {
		pNode->pLeft->IsRed = !pNode->pLeft->IsRed;
	}

	if (nullptr != pNode->pRight) {
		pNode->pRight->IsRed = !pNode->pRight->IsRed;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	Insert()
//
bool LeftLeaningRedBlack::Insert(VoidRef_t ref)
{
	m_pRoot = InsertRec(m_pRoot, ref);

	// The root node of a red-black tree must be black.
	m_pRoot->IsRed = false;
	if (/*m_pRoot->pLeft == nullptr && m_pRoot->pRight == nullptr ||*/ m_pRoot->Ref.Key == ref.Key) //if is root or empty
	{
		cout << "No Parent. ";
		cout << "Newest :" <<m_pRoot->Ref.Key;
		m_pRoot->IsRed ? cout << " (Red) \n" : cout << " (Black) \n"; //ternary color check
	}
	else
	{
		LLTB_t* parent = FindParent(m_pRoot, ref);//shallow copy?
		cout << "Parent :";
		cout << parent->Ref.Key;
		parent->IsRed ? cout << "(Red). " : cout << "(Black). ";
		cout << "Newest :";
		if (parent->pLeft->Ref.Key == ref.Key)
		{
			cout << parent->pLeft->Ref.Key;
			parent->pLeft->IsRed ? cout << " (Red)\n" : cout << " (Black)\n";
		}
		else
		{
			cout << parent->pRight->Ref.Key;
			parent->pRight->IsRed ? cout << " (Red)\n" : cout << " (Black)\n";
		}
	}
  	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//	InsertRec()
//
//	Note that during rebalancing, a rotation may change the value assigned
//	to pNode.  This will change which node is the root of the subtree that
//	is being modified.  The caller will need to use the returned pointer
//	to replace whichever value was passed into InsertRec.
//
LLTB_t* LeftLeaningRedBlack::InsertRec(LLTB_t* pNode, VoidRef_t ref)
{
	// Special case for inserting a leaf.  Just return the pointer;
	// the caller will insert the new node into the parent node.
	if (nullptr == pNode) {
		pNode = NewNode();
		pNode->Ref = ref;
		return pNode;
	}

	// Check to see if the value is already in the tree.  If so, we
	// simply replace the value of the key, since duplicate keys are
	// not allowed.
	if (ref.Key == pNode->Ref.Key) {
		pNode->Ref = ref;
	}

	// Otherwise recurse left or right depending on key value.
	//
	// Note: pLeft or pRight may be a nullptr pointer before recursing.
	// This indicates that pNode is a leaf (or only has one child),
	// so the new node will be inserted using the return value.
	//
	// The other reason for pass-by-value, followed by an assignment,
	// is that the recursive call may perform a rotation, so the
	// pointer that gets passed in may end up not being the root of
	// the subtree once the recursion returns.
	//
	else {
		if (ref.Key < pNode->Ref.Key) {
			pNode->pLeft = InsertRec(pNode->pLeft, ref);
		}
		else {
			pNode->pRight = InsertRec(pNode->pRight, ref);
		}
	}

	// If necessary, apply a rotation to get the correct representation
	// in the parent node as we're backing out of the recursion.  This
	// places the tree in a state where the parent can safely apply a
	// rotation to restore the required black/red balance of the tree.

	// Fix a right-leaning red node: this will assure that a 3-node is
	// the left child.
	if (IsRed(pNode->pRight) && (false == IsRed(pNode->pLeft))) {
		pNode = RotateLeft(pNode);
	}

	// Fix two reds in a row: this will rebalance a 4-node.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);
	}

	// If we perform the color flip here, the tree is assembled as a
	// mapping of a 2-3 tree.
#if !defined(USE_234_TREE)
	// This color flip will effectively split 4-nodes on the way back
	// out of the tree.  By doing this here, there will be no 4-nodes
	// left in the tree after the insertion is complete.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pRight)) {
		ColorFlip(pNode);
	}
#endif
	// Return the new root of the subtree that was just updated,
	// since rotations may have changed the value of this pointer.
	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	MoveRedLeft()
//
//	This code assumes that either pNode or pNode->pLeft are red.
//
static LLTB_t* MoveRedLeft(LLTB_t* pNode)
{
	// If both children are black, we turn these three nodes into a
	// 4-node by applying a color flip.
	ColorFlip(pNode);

	// But we may end up with a case where pRight has a red child.
	// Apply a pair of rotations and a color flip to make pNode a
	// red node, both of its children become black nodes, and pLeft
	// becomes a 3-node.
	if ((nullptr != pNode->pRight) && IsRed(pNode->pRight->pLeft)) {
		pNode->pRight = RotateRight(pNode->pRight);
		pNode = RotateLeft(pNode);

		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	MoveRedRight()
//
//	This code assumes that either pNode or pNode->Right is a red node.
//
static LLTB_t* MoveRedRight(LLTB_t* pNode)
{
	// Applying a color flip may turn pNode into a 4-node,
	// with both of its children being red.
	ColorFlip(pNode);

	// However, this may cause a situation where both of pNode's
	// children are red, along with pNode->pLeft->pLeft.  Applying a
	// rotation and a color flip will fix this special case, since
	// it makes pNode red and pNode's children black.
	if ((nullptr != pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);

		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FindMin()
//
//	Find the node under pNode that contains the smallest key value.
//
static LLTB_t* FindMin(LLTB_t* pNode)
{
	while (nullptr != pNode->pLeft) {
		pNode = pNode->pLeft;
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	FixUp()
//
//	Apply fix-up logic during deletion.  Applying color flips and rotations
//	will preserve perfect black-link balance.  This will also fix any
//	right-leaning red nodes and eliminate 4-nodes when backing out of
//	recursive calls.
//
static LLTB_t* FixUp(LLTB_t* pNode)
{
	// Fix right-leaning red nodes.
	if (IsRed(pNode->pRight)) {
		pNode = RotateLeft(pNode);
	}

	// Detect if there is a 4-node that traverses down the left.
	// This is fixed by a right rotation, making both of the red
	// nodes the children of pNode.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pLeft->pLeft)) {
		pNode = RotateRight(pNode);
	}

	// Split 4-nodes.
	if (IsRed(pNode->pLeft) && IsRed(pNode->pRight)) {
		ColorFlip(pNode);
	}

	return pNode;
}


/////////////////////////////////////////////////////////////////////////////
//
//	Delete()
//
void LeftLeaningRedBlack::Delete(const uint32_t key)
{
	if (nullptr != m_pRoot) {
		m_pRoot = DeleteRec(m_pRoot, key);

		// Assuming we have not deleted the last node from the tree, we
		// need to force the root to be a black node to conform with the
		// the rules of a red-black tree.
		if (nullptr != m_pRoot) {
			m_pRoot->IsRed = false;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteRec()
//
LLTB_t* LeftLeaningRedBlack::DeleteRec(LLTB_t* pNode, const uint32_t key)
{
	if (key < pNode->Ref.Key) {
		if (nullptr != pNode->pLeft) {
			// If pNode and pNode->pLeft are black, we may need to
			// move pRight to become the left child if a deletion
			// would produce a red node.
			if ((false == IsRed(pNode->pLeft)) && (false == IsRed(pNode->pLeft->pLeft))) {
				pNode = MoveRedLeft(pNode);
			}

			pNode->pLeft = DeleteRec(pNode->pLeft, key);
		}
	}
	else {
		// If the left child is red, apply a rotation so we make
		// the right child red.
		if (IsRed(pNode->pLeft)) {
			pNode = RotateRight(pNode);
		}

		// Special case for deletion of a leaf node.
		// The arrangement logic of LLRBs assures that in this case,
		// pNode cannot have a left child.
		if ((key == pNode->Ref.Key) && (nullptr == pNode->pRight)) {
			Free(pNode);
			return nullptr;
		}

		// If we get here, we need to traverse down the right node.
		// However, if there is no right node, then the target key is
		// not in the tree, so we can break out of the recursion.
		if (nullptr != pNode->pRight) {
			if ((false == IsRed(pNode->pRight)) && (false == IsRed(pNode->pRight->pLeft))) {
				pNode = MoveRedRight(pNode);
			}

			// Deletion of an internal node: We cannot delete this node
			// from the tree, so we have to find the node containing
			// the smallest key value that is larger than the key we're
			// deleting.  This other key will replace the value we're
			// deleting, then we can delete the node that previously
			// held the key/value pair we just moved.
			if (key == pNode->Ref.Key) {
				pNode->Ref = FindMin(pNode->pRight)->Ref;
				pNode->pRight = DeleteMin(pNode->pRight);
			}
			else {
				pNode->pRight = DeleteRec(pNode->pRight, key);
			}
		}
	}

	// Fix right-leaning red nodes and eliminate 4-nodes on the way up.
	// Need to avoid allowing search operations to terminate on 4-nodes,
	// or searching may not locate intended key.
	return FixUp(pNode);
}


/////////////////////////////////////////////////////////////////////////////
//
//	DeleteMin()
//
//	Delete the bottom node on the left spine while maintaining balance.
//	To do so, we maintain the invariant that the current node or its left
//	child is red.
//
//	The only reason this is a member function instead of a static function
//	like all of the other support functions is because it needs to call
//	Free().
//
LLTB_t* LeftLeaningRedBlack::DeleteMin(LLTB_t* pNode)
{
	// If this node has no children, we're done.
	// Due to the arrangement of an LLRB tree, the node cannot have a
	// right child.
	if (NULL == pNode->pLeft) {
		Free(pNode);
		return NULL;
	}

	// If these nodes are black, we need to rearrange this subtree to
	// force the left child to be red.
	if ((false == IsRed(pNode->pLeft)) && (false == IsRed(pNode->pLeft->pLeft))) {
		pNode = MoveRedLeft(pNode);
	}

	// Continue recursing to locate the node to delete.
	pNode->pLeft = DeleteMin(pNode->pLeft);

	// Fix right-leaning red nodes and eliminate 4-nodes on the way up.
	// Need to avoid allowing search operations to terminate on 4-nodes,
	// or searching may not locate intended key.
	return FixUp(pNode);
}

/////////////////////////////////////////////////////////////////////////////
//
//	Traverse()
//
//	Perform an in-order traversal of the tree, printing out all of the key
//	values in the tree.  This should display all keys in their sorted order.
//
void LeftLeaningRedBlack::Traverse(void)
{
	if (nullptr != m_pRoot) {
		uint32_t prev = 0;
		TraverseRec(m_pRoot, prev);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//	TraverseRec()
//
void LeftLeaningRedBlack::TraverseRec(LLTB_t* pNode, uint32_t& prev)
{
	if (!(nullptr != pNode))
	{
		cout << "\nOnly root to traverse.\n";
	}
	if (nullptr != pNode->pLeft) {
		TraverseRec(pNode->pLeft, prev);
	}
	if (!(prev < pNode->Ref.Key))
	{
		cout << "\nRight child should not be greater than parent.\n";
	}
	prev = pNode->Ref.Key;
	if (pNode->IsRed)
		cout << "(Red) ";
	else
		cout << "(Black) ";
	cout << pNode->Ref.Key << endl;

	if (nullptr != pNode->pRight) {
		TraverseRec(pNode->pRight, prev);
	}
}

/*Project Functions*/
uint32_t LeftLeaningRedBlack::Max(uint32_t& left, uint32_t& right)
{//Written by Brendan Aguiar
	return (left > right) ? left : right; //ternary statement returns max value
}//end Max

uint32_t LeftLeaningRedBlack::Min(uint32_t& left, uint32_t& right)
{//Written by Brendan Aguiar
	return (left < right) ? left : right; //ternary statement returns min value
}//end Min

LLTB_t* LeftLeaningRedBlack::FindParent(LLTB_t* tempPtr, VoidRef_t ref)
{//Written by Brendan Aguiar
	if ((tempPtr->pLeft->Ref.Key == ref.Key) || (tempPtr->pRight->Ref.Key == ref.Key)) // Check children
		return tempPtr;//return if child matches Key
	else if (tempPtr->Ref.Key > ref.Key) //Search left subtree
		return FindParent(tempPtr->pLeft, ref);
	else //Search right subtree
		return FindParent(tempPtr->pRight, ref);
}//end Find Parent
