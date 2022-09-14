#pragma once

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

		// changes curr to point towards next inorder node
        iterator operator++() {
			if(curr -> isThreaded)
				curr = curr -> right;
			else {
				curr = curr -> right;
				NODE* leading = curr;
				while(leading != nullptr) {
					curr = leading;
					leading = leading -> left;
				}
			}
            return iterator(curr);
        }
    };

 public:
    mymap() {
		root = nullptr;
		size = 0;
    }


	// Milestone #1 Begin

		// Inserts or updates the key/value pair inside the mymap
	    void put(keyType key, valueType value) {
			NODE* cur = root;
			NODE* prev = nullptr;

			while(cur != nullptr) {
				prev = cur;
				if(cur -> key == key) {
					cur -> value = value;
					return;
				} else if(cur -> key < key) {
					if(cur -> isThreaded)
						cur = nullptr;
					else
						cur = cur -> right;
				  }
			  	  else if(cur -> key > key)
			  		cur = cur -> left;
			}
			NODE* newNode = new NODE{key, value, nullptr, nullptr, 0, 0, true};

			if(prev == nullptr)
				root = newNode;
			else if(newNode -> key > prev -> key) {
				newNode -> right = prev -> right;
				prev -> right = newNode;
				prev -> isThreaded = false;
			}
			else if(newNode -> key < prev -> key){
				prev -> left = newNode;
				newNode -> right = prev;
			}

			size++;
		}

		// returns the number key/value pairs inside the mymap
		int Size() { return size; }

		// returns true or false depending on if the passed in key is inside mymap
	    bool contains(keyType key) {
			NODE* cur = root;

			while(cur != nullptr) {
				if(cur -> key == key)
					return true;
				else if(key < cur -> key)
					cur = cur -> left;
				else if(cur -> key < key) {
					if(cur -> isThreaded)
						cur = nullptr;
					else
						cur = cur -> right;
					}
			}
	        return false;
	    }

		// returns the value which corresponds to the passed in key
	    valueType get(keyType key) {
			NODE* cur = root;

			while(cur != nullptr) {
				if(cur -> key == key)
					return cur -> value;
				else if(key < cur -> key)
					cur = cur -> left;
				else if(cur -> key < key) {
					if(cur -> isThreaded)
						cur = nullptr;
					else
						cur = cur -> right;
					}
			}

			return valueType();
	    }

		// Recursive helper function for toString();
		void inorder(NODE* cur, stringstream& ss) {
			if(cur == nullptr)
				return;

			inorder(cur -> left, ss);

			ss << "key: " << cur -> key << " value: " << cur -> value << "\n";

			if(!(cur -> isThreaded))
				inorder(cur -> right, ss);
		}

		// returns a string corresponding to the key/values in the mymap (inorder)
		string toString() {
			stringstream ss;
			inorder(root, ss);

			return ss.str();
		}

		// returns value of the passed in key, if not found, then new key/value is inserted
		valueType operator[](keyType key) {
	       if(!contains(key))
	           put(key, get(key));

	       return get(key);
	   }

	// Milestone #1 End


	// Milestone #3 Begin

	// returns itterator to first inorder node
	iterator begin() {
		NODE* cur = root;
		NODE* prev = nullptr;

		while(cur != nullptr) {
			prev = cur;
			cur = cur -> left;
		}

		return iterator(prev);
	}

	iterator end() {
		return iterator(nullptr);
	}

	// Milestone #3 End


	// Milestone #4 Begin

	// helper function for clear();
	void destroy(NODE* cur) {
		if(cur == nullptr)
			return;

		destroy(cur -> left);
		if(!(cur -> isThreaded))
			destroy(cur -> right);

		delete cur;
	}

	// helper function for the destructor which destroys the mymap
	void clear() {

		destroy(root);
		size = 0;
		root = nullptr;
	}

	// destructor
	~mymap() {
		clear();
	}

	// helper function for the copy constructor and copy operator
	void copy(NODE* otherRoot) {
		if(otherRoot == nullptr)
			return;

		put(otherRoot -> key, otherRoot -> value);

		copy(otherRoot -> left);
		if(!(otherRoot -> isThreaded))
			copy(otherRoot -> right);
	}

	// constucts a new mymap which is a copy of the “other” mymap
    mymap(const mymap& other) {
		size = 0;
		root = nullptr;

		copy(other.root);
		size = other.size;
    }

	// deletes the current map and then constucts a new mymap which is a copy of the “other” mymap
    mymap& operator=(const mymap& other) {
		if(this == &other)
			return *this;

		clear();
		copy(other.root);

        return *this;
    }

	// Milestone #4 End


	// Milestone #5 Begin

	// helper function for toVector();
	void inorder(NODE* cur, vector<pair<keyType, valueType>>& solution) {
		if(cur == nullptr)
			return;

		inorder(cur -> left, solution);

		pair<keyType, valueType> p;
		p.first = cur -> key;
		p.second = cur -> value;
		solution.push_back(p);

		if(!(cur -> isThreaded))
			inorder(cur -> right, solution);
	}

	// returns a vector containing pairs of key/values in the mymap (inorder)
    vector<pair<keyType, valueType>> toVector() {
		vector<pair<keyType, valueType>> solution;
		inorder(root, solution);

        return solution;
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {


        // TODO: write this function.


        return {};  // TODO: Update this return.
    }

	// Milestone #5 End
};
