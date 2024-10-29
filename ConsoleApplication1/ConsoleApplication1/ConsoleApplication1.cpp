#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
typedef long long ll;

// Thresholds for leading digit and underflow
const long long leadingDigitThreshold = pow(10, 9);
const long long underflowThreshold = pow(10, 8);

// Function to convert string to long long
ll stringToLongLong(const string& inputString) {
    ll result = 0;
    for (char ch : inputString) {
        result = result * 10 + (ch - '0');
    }
    return result;
}

// Function for encoding
void encode() {
    string inputMessage;
    vector<int> frequencyTable(256, 1); // Initialize frequency table with 1
    ll low = 0, high = 9999999999;

    // Read input from file
    ifstream inputFile("input.txt");
    string temp;
    while (getline(inputFile, temp)) {
        if (!inputMessage.empty()) {
            inputMessage += '\n';
        }
        inputMessage += temp;
    }
    inputMessage += "#"; // End marker

    ofstream outputFile("output.bin", ios::binary);
    string encodedMessage;

    // Arithmetic encoding
    for (char character : inputMessage) {
        int characterPosition = static_cast<int>(character);
        ll intervalRange = (high - low) + 1;
        ll cumulativeFrequency = 0;

        // Calculate cumulative frequency
        for (int k = 0; k < characterPosition; k++) {
            cumulativeFrequency += frequencyTable[k];
        }

        // Update high and low
        high = low + intervalRange * (cumulativeFrequency + frequencyTable[characterPosition]) / (frequencyTable.size() + 1) - 1;
        low = low + intervalRange * cumulativeFrequency / (frequencyTable.size() + 1);

        frequencyTable[characterPosition]++;

        // Handle leading digit
        while ((low / leadingDigitThreshold) == (high / leadingDigitThreshold)) {
            encodedMessage += char(low / leadingDigitThreshold + '0');
            low = (low % leadingDigitThreshold) * 10;
            high = (high % leadingDigitThreshold) * 10 + 9;
        }

        // Handle underflow
        while (high - low < underflowThreshold) {
            low = (low / leadingDigitThreshold) * leadingDigitThreshold;
            high = (high / leadingDigitThreshold) * leadingDigitThreshold + leadingDigitThreshold - 1;
        }
    }

    // Final code
    ll finalCode = (low + high) / 2;
    string finalCodeString = to_string(finalCode);
    while (finalCodeString.length() < 10) {
        finalCodeString += "0";
    }
    encodedMessage += finalCodeString;

    // Write encoded message to file
    outputFile.write(encodedMessage.c_str(), encodedMessage.size());
    cout << "Encoding of the file was successful!" << endl;
}

// Function for decoding
void decode() {
    string decodedMessage;
    string encodedMessage;

    vector<int> frequencyTable(256, 1); // Initialize frequency table with 1
    ll low = 0, high = 9999999999;

    // Read encoded message from file
    ifstream encodedFile("output.bin", ios::binary);
    encodedMessage.assign((istreambuf_iterator<char>(encodedFile)), istreambuf_iterator<char>());

    ll currentCode = stringToLongLong(encodedMessage.substr(encodedMessage.length() - 10));
    int currentPosition = encodedMessage.length() - 10;

    // Decoding process
    while (true) {
        ll intervalRange = (high - low) + 1;
        ll cumulativeFrequency = 0;
        int currentCharacter = -1;

        // Find the character corresponding to the current code
        for (int k = 0; k < frequencyTable.size(); k++) {
            ll topHalf = low + intervalRange * (cumulativeFrequency + frequencyTable[k]) / (frequencyTable.size() + 1) - 1;
            ll bottomHalf = low + intervalRange * cumulativeFrequency / (frequencyTable.size() + 1);

            cumulativeFrequency += frequencyTable[k];

            if (currentCode >= bottomHalf && currentCode <= topHalf) {
                currentCharacter = k;
                low = bottomHalf;
                high = topHalf;
                break;
            }
        }

        if (currentCharacter == '#') break; // End of message
        decodedMessage += char(currentCharacter);
        frequencyTable[currentCharacter]++;
        // Handle leading digit
        while ((low / leadingDigitThreshold) == (high / leadingDigitThreshold)) {
            encodedMessage += char(low / leadingDigitThreshold + '0');
            low = (low % leadingDigitThreshold) * 10;
            high = (high % leadingDigitThreshold) * 10 + 9;

            // Update current code
            if (currentPosition < encodedMessage.size()) {
                currentCode = (currentCode % leadingDigitThreshold) * 10 + (encodedMessage[currentPosition] - '0');
                currentPosition++;
            }
        }

        // Handle underflow
        while (high - low < underflowThreshold) {
            ll temp = currentCode / leadingDigitThreshold;
            currentCode = temp * leadingDigitThreshold + (currentCode % underflowThreshold) * 10;

            low = (low / leadingDigitThreshold) * leadingDigitThreshold;
            high = (high / leadingDigitThreshold) * leadingDigitThreshold + leadingDigitThreshold - 1;
        }
    }

    // Write the decoded message to a file
    ofstream decodedFile("decodedText.txt");
    decodedFile << decodedMessage;

    cout << "The decoding of the file was successful!" << endl;
}

int main() {
    encode(); // Call the encoding function
    decode(); // Call the decoding function

    return 0;
}
