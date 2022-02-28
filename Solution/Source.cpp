#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

template<class type>
struct triple { //for storing words from Publications Data Set
	type value; //word
	int number = 0; //iteration times
	int indexKey = 0; //location of home bucket
};
triple<string>* hashBig = new triple<string>[3000000];
//2.5 million words


template<class type>
struct brothers {//for storing stopping words

	type info; //word
	int nextPlace = -1; //next elements location
};

brothers<string>* hashStop = new brothers<string>[6000];


template<class type>
struct heapStyle
{//for heap
	int number = 0; //iteration times
	type value;//word
};

heapStyle<string>* theHeap = new heapStyle<string>[16];//using minHeapify

template<class type>
class alls { //contains all functions for operations
public:

	void minHeapify(int i) { //heapfies the 16 elements but does not sort completely

		int rightChild = 2 * i + 1;
		if (rightChild > 16)return;
		int leftChild = 2 * i;
		if (theHeap[leftChild].number <= theHeap[rightChild].number) {
			if (theHeap[leftChild].number < theHeap[i].number) { //swap with parent
				heapStyle<type> d;
				d = theHeap[leftChild];  //swap
				theHeap[leftChild] = theHeap[i];
				theHeap[i] = d;
				minHeapify(leftChild);
			}

		}
		else {
			if (theHeap[rightChild].number < theHeap[i].number) {//swap with parent
				heapStyle<type> d;
				d = theHeap[rightChild];  //swap
				theHeap[rightChild] = theHeap[i];
				theHeap[i] = d;
				minHeapify(rightChild);
			}


		}




	}

	void lastHeapfy(int i, int last) { //sorts completely at the end

		int rightChild = 2 * i + 1;
		if (rightChild > last)return;
		int leftChild = 2 * i;
		if (theHeap[leftChild].number <= theHeap[rightChild].number) {
			if (theHeap[leftChild].number < theHeap[i].number) {
				heapStyle<type> d;
				d = theHeap[leftChild];
				theHeap[leftChild] = theHeap[i];
				theHeap[i] = d;
				lastHeapfy(leftChild, last);
			}

		}
		else {
			if (theHeap[rightChild].number < theHeap[i].number) {
				heapStyle<type> d;
				d = theHeap[rightChild];
				theHeap[rightChild] = theHeap[i];
				theHeap[i] = d;
				lastHeapfy(rightChild, last);
			}


		}


	}

	unsigned int hashF(string word) { //hash function for stopping words

		int h = 0;
		int x = word.length();
		for (int i = 0; i < x; i++)
		{
			h = (h * 1627) ^ ((int)word[i] * 2473);
		}

		return h;
	}


	unsigned int hashFBig(string word) { //hash function for big hash

		int h = 0;
		int x = word.length();
		for (int i = 0; i < x; i++)
		{
			h = (h * 3137) ^ ((int)word[i] * 6113);
		}

		return h;
	}


	void insert(string stopWord) { //insertion to the hashStop

		unsigned int index = hashF(stopWord);
		index = index % 6000;

		if (hashStop[index].info == "") {
			hashStop[index].info = stopWord;
		}
		else {
			int storeIndex;
			while (hashStop[index].nextPlace != -1)
			{
				index = hashStop[index].nextPlace;
			}
			storeIndex = index;

			while (hashStop[index].info != "") {
				index = (index + 7) % 6000;
			}

			hashStop[index].info = stopWord;
			hashStop[storeIndex].nextPlace = index;
		}



	}



	int InsertBig(int bigIndex, string currentStr, int currentInt) { //insertion to big hash
		//lets assume h=10
		for (int i = 0; i < 10; i++)
		{
			int searchIndex = (bigIndex + i) % 3000000;
			if (hashBig[searchIndex].value == "")
			{
				hashBig[searchIndex].value = currentStr;
				hashBig[searchIndex].number = currentInt;
				hashBig[searchIndex].indexKey = bigIndex;

				return currentInt;
			}
			else if (hashBig[searchIndex].value == currentStr) {
				hashBig[searchIndex].number += currentInt;
				return hashBig[searchIndex].number;
			}
		}

		for (int i = 10; i < 3000000; i++) {
			int searchIndex = (bigIndex + i) % 3000000;
			if (hashBig[searchIndex].value == "") {
				hashBig[searchIndex].value = currentStr;
				hashBig[searchIndex].number = currentInt;
				hashBig[searchIndex].indexKey = bigIndex;
				for (int j = 1; j < 10; j++)
				{
					int searchIndex2 = (bigIndex + j) % 3000000;
					if (10 > abs(hashBig[searchIndex2].indexKey - searchIndex)) {
						//swap
						triple<type> d = hashBig[searchIndex2];
						hashBig[searchIndex2] = hashBig[searchIndex];
						hashBig[searchIndex] = d;
						return currentInt;
					}
				}


			}
		}



		cerr << "cannot resolve collusion*****************************************************************************************************";
		return -1;
	}



	void putToHash(string currentStr, int currentInt) {  //puts the elements in relevant position of hash and if required also puts to theHeap
		//start to put hash
		unsigned int bigIndex = hashFBig(currentStr);
		bigIndex = bigIndex % 3000000;
		//3000000
		int curInt2 = InsertBig(bigIndex, currentStr, currentInt);
		bool heapfound = false;
		for (int i = 1; i < 16; i++)
		{
			if (currentStr == theHeap[i].value) {

				theHeap[i].number += currentInt;
				minHeapify(i);
				heapfound = true;
				break;
			}
		}
		if (!heapfound && (theHeap[1].number < curInt2)) {
			theHeap[1].number = curInt2;
			theHeap[1].value = currentStr;
			minHeapify(1);
		}




	}


	bool checkOut(int check, string currentStr) { //looks whether word is in hashStop or not
		if (hashStop[check].info == currentStr) {
			return false;
		}
		else {
			while (hashStop[check].nextPlace != -1) {
				check = hashStop[check].nextPlace;
				if (hashStop[check].info == currentStr) return false;
			}
			return true;
		}
	}



	bool alphaChecker(string& line, int& curLoc) { //looks whether another word used after non-alphabetic char
		curLoc++;

		while ((line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) {
			if (isalpha(line[curLoc])) {
				return false;
			}
			curLoc++;
		}
		return true;

	}


	void generalHashPutter(string& currentStr, int& curLoc, string& line, int& currentInt) { //starter function to put hash

		while (!isalpha(currentStr.back()))//delete if any arbitrary chars left at the end
			currentStr.pop_back();
		unsigned int check = hashF(currentStr);
		check = check % 6000;
		if (checkOut(check, currentStr))
		{
			putToHash(currentStr, currentInt);
		}


	}




	bool dream3(string& line, string& str3, int& curLoc) { //third word determiner until arbitrary chars

		while ((isalpha(line[curLoc]) || line[curLoc] == '\'') && (line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2])))
		{
			str3 += tolower(line[curLoc]);
			curLoc++;
		}
		if ((line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) {
			return alphaChecker(line, curLoc);
		}
		return 1;

	}



	bool dream2(string& line, string& str2, string& str3, int& curLoc) {  //second word determiner until second "-"
		while ((isalpha(line[curLoc]) || line[curLoc] == '\'') && (line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2])))
		{
			str2 += tolower(line[curLoc]);
			curLoc++;
		}
		if ((line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) {
			if (line[curLoc] == '-') {
				if (isalpha(line[curLoc + 1])) {
					curLoc++; return dream3(line, str3, curLoc);
				}
				else {
					return alphaChecker(line, curLoc);
				}
			}
			else {
				return alphaChecker(line, curLoc);
			}
		}
		return 1;
	}



	bool dream1(string& line, string& str1, string& str2, string& str3, int& curLoc) { //first word determiner until first "-"
		while ((isalpha(line[curLoc]) || line[curLoc] == '\'') && (line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2])))
		{
			str1 += tolower(line[curLoc]);
			curLoc++;
		}
		if ((line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) {
			if (line[curLoc] == '-') {
				if (isalpha(line[curLoc + 1])) {
					curLoc++; return dream2(line, str2, str3, curLoc);
				}
				else {
					return alphaChecker(line, curLoc);
				}
			}
			else {
				return alphaChecker(line, curLoc);
			}
		}
		return 1;
	}

	~alls() {
		delete[] theHeap;
		delete[] hashBig;
		delete[]hashStop;
	}


};






int main() {
	clock_t start, stop;
	start = clock();
	ifstream file(R"(C:\Users\myusu\Downloads\stopwords.txt)", ifstream::in);
	string stopWord;
	alls<string> Y;
	while (file) { //read stopping words and insert
		std::getline(file, stopWord);
		Y.insert(stopWord);
	}
	file.close();



	ifstream otherFile(R"(C:\Users\myusu\Downloads\PublicationsDataSet.txt)", ifstream::in);
	string line;

	while (otherFile) {//different lines
		std::getline(otherFile, line);
		int cur = line.find("unigramCount", 482); //start to search from 482 because the nearest unigramCount starts from that point
		//+14 
		int curLoc = cur + 15; //pass words of unigramCount and "{"
		while (line[curLoc] != '}') //loops when every value number pairs read          
		{
			curLoc++; //pass "   - - - 
			string str1;
			string str2;
			string str3;
			while (!isalpha(line[curLoc]) && (line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) // go until find a-z
				curLoc++;
			bool r = Y.dream1(line, str1, str2, str3, curLoc); //if a-z found then go to dream

			if (!r) str1 = str2 = str3 = ""; //if the condition is not satisfied to be a word
			//word determination operatons finished

			while ((line[curLoc] != '\"' || line[curLoc + 1] != ':' || !isdigit(line[curLoc + 2]))) curLoc++;

			curLoc += 2;//pass " :
			//now collect numbers
			int num = 0;

			while (line[curLoc] != ',' && line[curLoc] != '}') { //take all digits
				num *= 10;
				num += line[curLoc] - '0';
				curLoc++;
			}
			//if being word is satisfied, then go to generalHashPutter
			if (str1 != "") Y.generalHashPutter(str1, curLoc, line, num);
			if (str2 != "") Y.generalHashPutter(str2, curLoc, line, num);
			if (str3 != "") Y.generalHashPutter(str3, curLoc, line, num);

			while (line[curLoc] != ',' && line[curLoc] != '}') { //if there is any space left until , and }
				curLoc++;
			}
			curLoc++;//pass , or }



		}




	}
	otherFile.close();




	int last = 15;
	heapStyle<string> sw; //sorts the heap at the end
	for (int i = 1; i < 16; i++) {
		sw = theHeap[1];
		theHeap[1] = theHeap[last];
		theHeap[last] = sw;
		last--;
		Y.lastHeapfy(1, last);
	}




	stop = clock();
	double totalTime = (stop - start) / (double)CLOCKS_PER_SEC;

	for (int i = 1; i < 11; i++)
		std::cout << theHeap[i].value << "\t" << theHeap[i].number << endl;

	std::cout << "Total time is: " << totalTime << endl;



	return 0;
}
