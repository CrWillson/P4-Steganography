#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include "lodepng.h"

using namespace std;

// convert the string to binary and add its length (in binary) to the front
string tobinary(string input) {
	string binaryStr = bitset<32>(input.length() * 8).to_string();
	for (unsigned i = 0; i < input.length(); i++) {
		binaryStr += bitset<8>(input.at(i)).to_string();
	}
	return binaryStr;
}

// encode the text into the given image vector one bit at a time
void encode_text(vector<unsigned char>& image, const char* text) {
	for (int i = 0; i < strlen(text); i++) {
		if ((int)image.at(i) % 2 == 0) {
			if (text[i] == '0') { continue; }
			else if (text[i] == '1') { image.at(i)++; }
		}
		else {
			if (text[i] == '0') { image.at(i)--; }
			else if (text[i] == '1') { continue; }
		}
	}
}

// get the last bit from a pixel
char get_bit(unsigned char pixel) {
	if ((int)pixel % 2 == 0) { return '0'; }
	else { return '1'; }
}

// decode the text out of a given image and store it in 'text'
void decode_text(vector<unsigned char> image, string& text) {
	// get the length of the message
	string textLen = "";
	for (int i = 0; i < 32; i++) {
		textLen += get_bit(image.at(i));
	}
	unsigned long len = bitset<32>(textLen).to_ulong();

	// get the message itself
	for (int i = 32; i < len + 32; i += 8) {
		string currChar = "";
		for (int j = 0; j < 8; j++) {
			currChar += get_bit(image.at(i + j));
		}
		text += (char)bitset<8>(currChar).to_ulong();
	}
}

int main(int argc, char** argv) {
	// usage check
	if (argc < 2) {
		cout << "Usage: steg <-e/-d>" << endl;
		return 1;
	}

	string mode(argv[1]);

	// ENCODE MODE
	if (mode == "-e") {
		// usage check
		if (argc < 4 || argc > 5) {
			cout << "Usage: steg -e <original image name> <modified image name>" 
				 << " [input ASCII text file name]\n";
			return 1;
		}

		// get file names
		char* imageFile = argv[2];
		char* destFile = argv[3];
		char* textFile = argv[4];
		string text;

		// get the input from a file if specified or from cin
		if (argc == 5) {
			ifstream inFile(textFile);

			stringstream strStream;
			strStream << inFile.rdbuf();
			text = strStream.str();
			inFile.close();
			cout << "Encoding text to " << destFile << " from " << textFile << "." << endl;
		}
		else {
			cout << "Input text to encode: " << endl;
			
			int c = 0;
			while (c != EOF) {
				c = getchar();
				if (c != -1) {
					text += (char)c;
				}
			}
			cout << "\nEncoding text to " << destFile << "." << endl;
		}

		// convert the input text to its binary values
		text = tobinary(text);

		// decode the input image into a vector of pixel values
		vector<unsigned char> image;
		unsigned width, height;

		unsigned decError = lodepng::decode(image, width, height, imageFile);
		if (decError) cout << "decoder error " << decError << ": " 
						   << lodepng_error_text(decError) << endl;

		// check to make sure there is space for the whole string
		if (text.length() > image.size()) {
			cout << "Image to small to fit text." << endl;
			return 1;
		}

		// encode the text into the image
		encode_text(image, text.c_str());

		// re-encode the new image data into a new png file
		unsigned encError = lodepng::encode(destFile, image, width, height);
		if (encError) cout << "encoder error " << encError << ": " 
						   << lodepng_error_text(encError) << endl;
	}

	// DECODE MODE
	else if (mode == "-d") {
		// usage check
		if (argc < 3 || argc > 4) {
			cout << "Usage: steg -d <modified image name> [output ASCII text file name]\n";
			return 1;
		}

		// decode the secret image into a vector of pixel values
		string text = "";
		char* imageFile = argv[2];
		vector<unsigned char> image;
		unsigned width, height;

		unsigned decError = lodepng::decode(image, width, height, imageFile);
		if (decError) cout << "decoder error " << decError << ": " 
						   << lodepng_error_text(decError) << endl;

		// read the hidden message out of the image
		decode_text(image, text);

		// save the message to the specified file
		if (argc == 4) {
			char* textFile = argv[3];
			cout << "Saved text from " << imageFile << " to " << textFile << "." << endl;

			ofstream outFile(textFile);
			outFile << text;
			outFile.close();
		}
		// print the message to the console
		if (argc == 3) {
			cout << "Printing text from " << imageFile << ":\n" << endl;
			cout << text;
			cout << endl;
		}
	}

	// usage check
	else {
		cout << "Usage: steg <-e/-d>" << endl;
		return 1;
	}

	return 0;
}