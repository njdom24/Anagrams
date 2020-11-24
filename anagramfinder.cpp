/*******************************************************************************
 * Name          : anagramfinder.cpp
 * Author        : Dominick DiMaggio
 * Pledge        : I pledge my honor that I have abided by the Stevens Honor System.
 * Date          : 11/14/19
 * Description   : Outputs the largest group(s) of anagrams in an input file
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <locale>

using namespace std;

//Precondition: Equal length
bool verify(string &s1, string &s2, int buffer[]) {

	int len = s1.length();
	for(int i = 0; i < len; ++i) {
		++buffer[(int)tolower(s1[i])];
		--buffer[(int)tolower(s2[i])];
	}

	for(int i = 0; i < len; ++i) {
		//Reset buffer if nonzero
		if(buffer[(int)s1[i]] != 0 || buffer[(int)s2[i]] != 0) {
			while(i < len) {
				buffer[(int)tolower(s1[i])] = 0;
				buffer[(int)tolower(s2[i])] = 0;
				++i;
			}

			return false;
		}
	}
	return true;
}

struct groupString {
	string *s;
	int group;

	groupString(string *str, int grp) : s(str), group(grp) {}
};

void processStrings(size_t &hashesLen, vector<string> &strVec, vector<string*> hashTable[], vector<int> &hashesOfLongest, unsigned long long &longestCount, vector<bool> &canHashVec, vector<unsigned long long> &hashVec, size_t start, size_t end) {

	int hashMod = 195023;
	if(strVec.size() > 195023)
		hashMod = strVec.size() * 1.3;

	//unsigned char alphaPrimes[] = { 5, 71, 37, 29, 2, 53, 59, 19, 11, 83, 79, 31, 43, 13, 7, 67, 97, 23, 17, 3, 41, 73, 47, 89, 61, 101 };
	//std::hash<unsigned long long> h;
	unsigned long long hash = 0;
	int buffer[256];
	for(int i = 0; i < 256; ++i)
		buffer[i] = 0;
	//unsigned long long longestCount = 0;
	//size_t hashesLen = 0;//Return this?

	for(size_t i = start; i < end; ++i) {
		/*
		for(size_t j = 0; j < strVec[i].length(); ++j) {
			char c = tolower(strVec[i][j]);
			if(c != 13 && (c < 'a' || c > 'z')) {
				prod = 0;
				j = strVec[i].length();
			}
			else {
				if(c != 13) {
					prod *= alphaPrimes[c - 'a'];
				}
			}
		}
		*/
		if(canHashVec[i]) {
			//hash = h(prod) % hashMod;//195023;
			hash = hashVec[i];
			int iters = 0;
			bool cond = true;
			vector<string*> *vec;
			while (cond) {
				vec = &hashTable[hash];
				//
				//if(vec->size() == 0 || ((*vec)[0]->length() == strVec[i].length())) {
				if(vec->size() == 0 || ((*vec)[0]->length() == strVec[i].length() && verify(*(*vec)[0], strVec[i], buffer))) {
					vec->push_back(&(strVec[i]));//MEMORY LEAK COMES FROM HERE
					//cout << "break: " << str << endl;
					cond = false;
					//break;
				}
				else {
					//++conflicts;
					hash = (hash+1) % hashMod;//195023;
					++iters;
					//cout << "Iterating: " << iters << ", " << hash << endl;
				}
			}
			//cout << endl;
			//cout << "Size: " << hashTable[hash].size() << endl;
			if(vec->size() == longestCount) {
				if(hashesOfLongest.size() <= hashesLen) {
					hashesOfLongest.push_back(hash);
					//cout << "Pushing back: " << hashesOfLongest.size() << ", " << hash << endl;
				}
				else {
					hashesOfLongest[hashesLen] = hash;
					//cout << "Appending: " << hashesOfLongest.size() << ", " << hashesLen << endl;
				}
				//cout << "Proof: " << hashesOfLongest[hashesLen] << endl;
				++hashesLen;
				
			}
			else if(vec->size() > longestCount) {
				//cout << "Resetting at " << strVec[i] << ": " << hash << endl;
				if(hashesOfLongest.size() <= hashesLen) {
					hashesOfLongest.push_back(hash);
					//cout << "Pushing back: " << hashesOfLongest.size() << ", " << hash << endl;
				}
				else {
					hashesOfLongest[0] = hash;
					//cout << "Restarting: " << hashesOfLongest.size() << ", " << hashesLen << endl;
				}

				longestCount = vec->size();
				hashesLen = 1;
			}
			//cout << "Prod is 0: " << str << endl;
		}
	}
	//cout << "Returning: " << hashesOfLongest[0] << endl;
	//return hashesLen;
}

/*
void getProducts(vector<unsigned long long> &productVec, vector<string> &strVec) {
	unsigned char alphaPrimes[] = { 5, 71, 37, 29, 2, 53, 59, 19, 11, 83, 79, 31, 43, 13, 7, 67, 97, 23, 17, 3, 41, 73, 47, 89, 61, 101 };
	for(size_t i = 0; i < strVec.size(); ++i) {
		unsigned long long prod = 1;
		for(size_t j = 0; j < strVec[i].length(); ++j) {
			char c = tolower(strVec[i][j]);
			if(c != 13 && (c < 'a' || c > 'z')) {
				prod = 1;
				j = strVec[i].length();
			}
			else {
				if(c != 13) {
					prod *= alphaPrimes[c - 'a'];
				}
			}
		}
		productVec[i] = prod;
		//cout << "Assigning productVec[" << i << "] to " << prod << endl;
	}
}
*/

void getHashes(vector<bool> &canHashVec, vector<unsigned long long> &hashVec, vector<string> &strVec, size_t start, size_t end) {
	int hashMod = 195023;
	if(strVec.size() > 195023)
		hashMod = strVec.size() * 1.3;
	std::hash<unsigned long long> h;

	//unsigned long long hash = 0;
	unsigned char alphaPrimes[] = { 5, 71, 37, 29, 2, 53, 59, 19, 11, 83, 79, 31, 43, 13, 7, 67, 97, 23, 17, 3, 41, 73, 47, 89, 61, 101 };
	for(size_t i = start; i < end; ++i) {
		unsigned long long prod = 1;
		for(size_t j = 0; j < strVec[i].length(); ++j) {
			char c = tolower(strVec[i][j]);
			if(c != 13 && (c < 'a' || c > 'z')) {
				prod = 1;
				canHashVec[i] = false;
				j = strVec[i].length();
			}
			else {
				if(c != 13) {
					prod *= alphaPrimes[c - 'a'];
				}
			}
		}
		if(prod != 1) {
			hashVec[i] = h(prod) % hashMod;
			canHashVec[i] = true;
		}
		
		//cout << "Assigning productVec[" << i << "] to " << prod << endl;
	}
}

int main(int argc, char *const argv[]) {
	
    if(argc != 2) {
        cerr << "Usage: ./anagramfinder <dictionary file>";
        return -1;
    }
    
    ifstream in;
    in.open(argv[1]);

    if(!in) {
        cerr << "Error: File '" << argv[1] << "' not found.";
		return -1;
    }

    vector<string> strVec;
	vector<bool> canHashVec;
	vector<unsigned long long> hashVec;
	strVec.reserve(150000);
	//sortVec.resize(100000);
	vector<vector<string*>> hashTable;
	vector<string*> *table;
	vector<string*> stackTable[195023];

    string str;
    //int inputs = 0;
	unsigned long long longestCount = 0;
	size_t hashesLen = 0;
	//vector<int> *indsOfLongest = new vector<int>();
	vector<int> hashesOfLongest;
	hashesOfLongest.reserve(6);
	
	//cout << "iter" << endl;
	while (std::getline(in, str)) {
		//cout << "Line: " << inputs << endl;
		if(str.length() > 30 || str.length() == 0) {
			//cout << "ExLen: " << str << endl;
			continue;
		}
		strVec.push_back(str);
		
		//cout << str << endl;
	}
	in.close();

	canHashVec.resize(strVec.size());
	hashVec.resize(strVec.size());
	future<void> getHashVals = async(getHashes, ref(canHashVec), ref(hashVec), ref(strVec), strVec.size()*0.25, strVec.size());

	//Allocates on the stack if under 195023 values have been stored
	if(strVec.size() > 195023) {
		hashTable.resize(strVec.size()*1.3);
		table = &hashTable[0];
	}
	else {
		table = stackTable;
	}
	
	//getProducts(productVec, strVec);
	getHashes(canHashVec, hashVec, strVec, 0, strVec.size()*0.25);
	processStrings(hashesLen, strVec, table, hashesOfLongest, longestCount, canHashVec, hashVec, 0, strVec.size()*0.25);
	unsigned long long hashVal;
	getHashVals.get();
	processStrings(hashesLen, strVec, table, hashesOfLongest, longestCount, canHashVec, hashVec, strVec.size()*0.25, strVec.size());

	//Sort within groups
	for(size_t i = 0; i < hashesLen; ++i) {
		//cout << "i: " << i << endl;
		hashVal = hashesOfLongest[i];

		sort(table[hashVal].begin(), table[hashVal].end(), [](string *a, string *b) {
			return a->compare(*b) < 0;
		});
	}
	//Sort groups themselves
	//cout << hashesOfLongest.size() << endl;
	vector<groupString> sortVec;
	sortVec.reserve(hashesLen*2);
	for(size_t i = 0; i < hashesLen; ++i) {
		hashVal = hashesOfLongest[i];
		sortVec.push_back(groupString(table[hashVal][0], i));
	}
	

	sort(sortVec.begin(), sortVec.end(), [](groupString &a, groupString &b) {
			return a.s->compare(*b.s) < 0;
		});
	
	//cout << "Conflicts: " << conflicts << '\n';
	if(longestCount > 1) {
		cout << "Max anagrams: " << longestCount << endl;
		//cout << "Iterations: " << inputs << endl;

		for(size_t i = 0; i < hashesLen; ++i) {
			hashVal = hashesOfLongest[sortVec[i].group];
			for(size_t j = 0; j < table[hashVal].size(); ++j) {
				//cout << *hashTable[hashVal][j] << ": " << hashVal << endl;
				cout << *table[hashVal][j] << endl;
			}
			cout << endl;
		}
	}
	else {
		cout << "No anagrams found.";
	}
	

	//delete hashesOfLongest;
	return 1;
}