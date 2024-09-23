#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <Windows.h>

using namespace std;

void setTextColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

string reverseLine(const string& line) {
    string reversed = line;
    reverse(reversed.begin(), reversed.end());
    return reversed;
}

string insertAtEveryThirdChar(const string& content) {
    string modified;
    int count = 0;

    for (char ch : content) {
        modified += ch;
        if (ch != '\n') {
            count++;
        }
        if (count == 3) {
            modified += "@@";
            count = 0;
        }
    }
    return modified;
}

string reverseWordsInLine(const string& line) {
    istringstream iss(line);
    string word;
    string reversedLine;

    while (iss >> word) {
        reverse(word.begin(), word.end());
        reversedLine += word + " ";
    }

    if (!reversedLine.empty()) {
        reversedLine.pop_back();
    }

    return reversedLine;
}

string replaceCharacters(const string& content, char toReplace, char replaceWith) {
    string modified = content;
    for (char& ch : modified) {
        if (ch == toReplace) {
            ch = replaceWith;
        }
    }
    return modified;
}

string toLowerCase(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

int highlightAndCountWord(const string& content, const string& wordToFind) {
    istringstream iss(content);
    string line;
    int wordCount = 0;
    string lowerWord = toLowerCase(wordToFind);

    while (getline(iss, line)) {
        istringstream lineStream(line);
        string word;
        while (lineStream >> word) {
            string lowerWordInText = toLowerCase(word);
            if (lowerWordInText.find(lowerWord) != string::npos) {
                setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                cout << word << " ";
                wordCount++;
            }
            else {
                setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                cout << word << " ";
            }
        }
        cout << '\n';
    }
    setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return wordCount;
}

int countWordsInLine(const string& line) {
    istringstream iss(line);
    int count = 0;
    string word;
    while (iss >> word) {
        count++;
    }
    return count;
}

void sortLinesByWordCount(vector<string>& lines, bool ascending) {
    if (ascending) {
        sort(lines.begin(), lines.end(), [](const string& a, const string& b) {
            return countWordsInLine(a) < countWordsInLine(b);
            });
    }
    else {
        sort(lines.begin(), lines.end(), [](const string& a, const string& b) {
            return countWordsInLine(a) > countWordsInLine(b);
            });
    }
}

int main() {
    string filename;
    string fileContent;
    string originalContent;
    vector<string> originalLines;
    bool highlightUpperCase = false;
    bool isReversed = false;
    bool isModified = false;
    bool isWordReversed = false;
    bool isReplaced = false;
    char charToReplace, replaceWith;
    int uppercaseWordCount = 0;
    int sortState = 0;

    cout << "���� �̸��� �Է��ϼ���: ";
    cin >> filename;

    ifstream file(filename);

    if (!file) {
        cerr << "������ �� �� �����ϴ�: " << filename << '\n';
        return 1;
    }

    string line;
    while (getline(file, line)) {
        fileContent += line + "\n";
        originalLines.push_back(line);
    }

    file.close();

    originalContent = fileContent;

    cout << "���� ����:\n" << fileContent << '\n';

    vector<string> currentLines = originalLines;
    char command;

    while (true) {
        cout << "��ɾ �Է��ϼ��� : ";
        cin >> command;
        cout << '\n';

        if (command == 'c') {
            highlightUpperCase = !highlightUpperCase;
            uppercaseWordCount = 0;
            istringstream iss(fileContent);
            string line;

            while (getline(iss, line)) {
                istringstream lineStream(line);
                string word;
                while (lineStream >> word) {
                    if (highlightUpperCase && isupper(word[0])) {
                        setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                        cout << word << " ";
                        uppercaseWordCount++;
                    }
                    else {
                        setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        cout << word << " ";
                    }
                }
                cout << '\n';
            }
            setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            cout << "\n���� ���� �ܾ��� ����: " << uppercaseWordCount << "\n";
        }
        else if (command == 'd') {
            isReversed = !isReversed;
            istringstream iss(fileContent);
            string line;

            while (getline(iss, line)) {
                if (isReversed) {
                    cout << reverseLine(line) << '\n';
                }
                else {
                    cout << line << '\n';
                }
            }
        }
        else if (command == 'e') {
            isModified = !isModified;
            if (isModified) {
                fileContent = insertAtEveryThirdChar(originalContent);
                cout << fileContent << '\n';
            }
            else {
                fileContent = originalContent;
                cout << originalContent << '\n';
            }
        }
        else if (command == 'f') {
            isWordReversed = !isWordReversed;
            istringstream iss(fileContent);
            string line;

            while (getline(iss, line)) {
                if (isWordReversed) {
                    cout << reverseWordsInLine(line) << '\n';
                }
                else {
                    cout << line << '\n';
                }
            }
        }
        else if (command == 'g') {
            isReplaced = !isReplaced;

            if (isReplaced) {
                cout << "�ٲ� ���ڸ� �Է��ϼ���: ";
                cin >> charToReplace;
                cout << "���Ӱ� �Է��� ���ڸ� �Է��ϼ���: ";
                cin >> replaceWith;
                fileContent = replaceCharacters(originalContent, charToReplace, replaceWith);
            }
            else {
                fileContent = originalContent;
            }

            cout << "���� ���� ����:\n" << fileContent << '\n';
        }
        else if (command == 'h') {
            istringstream iss(fileContent);
            string line;
            int lineNum = 1;

            cout << fileContent << '\n';
            while (getline(iss, line)) {
                int wordCount = countWordsInLine(line);
                cout << "�� " << lineNum << ": �ܾ� ���� = " << wordCount << '\n';
                lineNum++;
            }
        }
        else if (command == 'r') {
            if (sortState == 0) {
                sortLinesByWordCount(currentLines, true);
                sortState = 1;
                cout << "��������:\n";
                cout << '\n';
            }
            else if (sortState == 1) {
                sortLinesByWordCount(currentLines, false);
                sortState = 2;
                cout << "��������:\n";
                cout << '\n';
            }
            else {
                currentLines = originalLines;
                sortState = 0;
                cout << "����:\n";
                cout << '\n';
            }

            for (const auto& sortedLine : currentLines) {
                cout << sortedLine << '\n';
            }
        }
        else if (command == 's') {
            string wordToFind;
            cout << "ã�� ���� �ܾ �Է��ϼ���: ";
            cin >> wordToFind;

            cout << "�Էµ� ���忡�� '" << wordToFind << "' �ܾ ã���ϴ�...\n";
            int wordCount = highlightAndCountWord(fileContent, wordToFind);
            cout << "\n�� '" << wordToFind << "' �ܾ��� ����: " << wordCount << '\n';
        }
        else if (command == 'q') {
            break;
        }
    }
    return 0;
}