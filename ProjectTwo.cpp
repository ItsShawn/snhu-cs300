#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <limits>

using namespace std;

// Struct to hold course information
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Function prototypes
void displayMenu();
int getUserOption();
void loadData(const string& fileName, vector<Course>& courses);
bool isCourseCode(const string& token);
void sortCourses(vector<Course>& courses);
void printAllCourses(const vector<Course>& courses);
void printCourseInformation(const vector<Course>& courses);
const Course* findCourse(const vector<Course>& courses, const string& courseNumberInput);
bool equalsIgnoreCase(const string& a, const string& b);

int main() {
    vector<Course> courses;
    bool isDataLoaded = false;

    cout << "Welcome to the course planner." << "\n";

    int choice = 0;
    while (choice != 9) {
        displayMenu();
        choice = getUserOption();

        if (cin.fail()) {
            // Handle non-integer input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << choice << " is not a valid option." << "\n";
            continue;
        }

        switch (choice) {
            case 1: {
                // Load data
                cout << "Enter the file name containing course data: ";
                string fileName;
                cin >> fileName;
                loadData(fileName, courses);
                if (!courses.empty()) {
                    isDataLoaded = true;
                } else {
                    // If loading failed or file is empty, isDataLoaded remains false
                }
                break;
            }
            case 2: {
                // Print course list if data is loaded
                if (!isDataLoaded) {
                    cout << "No data loaded. Please load data first.\n";
                } else {
                    sortCourses(courses);
                    printAllCourses(courses);
                }
                break;
            }
            case 3: {
                // Print course information if data is loaded
                if (!isDataLoaded) {
                    cout << "No data loaded. Please load data first.\n";
                } else {
                    printCourseInformation(courses);
                }
                break;
            }
            case 9:
                // Exit
                cout << "Thank you for using the course planner!" << "\n";
                break;
            default:
                // Invalid menu option
                cout << choice << " is not a valid option." << "\n";
                break;
        }
    }

    return 0;
}

// Display the main menu exactly as required
void displayMenu() {
    cout << "1. Load Data Structure." << "\n";
    cout << "2. Print Course List." << "\n";
    cout << "3. Print Course." << "\n";
    cout << "9. Exit" << "\n";
    cout << "What would you like to do? ";
}

// Safely get user option
int getUserOption() {
    int option;
    cin >> option;
    return option;
}

// Check if a token resembles a course code (starts with CSCI or MATH)
bool isCourseCode(const string& token) {
    if (token.size() >= 7) {
        // Rough check: "CSCI" and "MATH" are 4 chars, followed by at least 3-digit code
        if (token.rfind("CSCI", 0) == 0 || token.rfind("MATH", 0) == 0) {
            return true;
        }
    }
    return false;
}

// Load data from file
void loadData(const string& fileName, vector<Course>& courses) {
    courses.clear();
    ifstream inFile(fileName);
    if (!inFile) {
        cout << "ERROR: Could not open file \"" << fileName << "\"\n";
        return;
    }

    string line;
    while (true) {
        if(!getline(inFile, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        vector<string> tokens;
        {
            string token;
            while (ss >> token) {
                tokens.push_back(token);
            }
        }

        if (tokens.empty()) continue; 

        Course course;
        course.courseNumber = tokens[0];

        // Determine where title ends and prerequisites begin
        vector<string> titleWords;
        size_t i = 1;
        for (; i < tokens.size(); ++i) {
            if (isCourseCode(tokens[i]) && i != 1) {
                // Found a course code after collecting some title words
                break;
            }
            titleWords.push_back(tokens[i]);
        }

        // Join title words to form the title
        {
            ostringstream oss;
            for (size_t j = 0; j < titleWords.size(); ++j) {
                if (j > 0) oss << " ";
                oss << titleWords[j];
            }
            course.courseTitle = oss.str();
        }

        // Remaining tokens are prerequisites
        for (; i < tokens.size(); ++i) {
            course.prerequisites.push_back(tokens[i]);
        }

        courses.push_back(course);
    }

    inFile.close();
    if (!courses.empty()) {
        cout << "Data loaded successfully! " << courses.size() << " courses read.\n";
    } else {
        cout << "No valid course data found in the file.\n";
    }
}

// Sort courses alphanumerically by course number
void sortCourses(vector<Course>& courses) {
    sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
    });
}

// Print all courses in sorted order
void printAllCourses(const vector<Course>& courses) {
    cout << "Here is a sample schedule:" << "\n";
    for (const auto& c : courses) {
        cout << c.courseNumber << ", " << c.courseTitle << "\n";
    }
}

// Print course information
void printCourseInformation(const vector<Course>& courses) {
    cout << "What course do you want to know about? ";
    string courseNumberInput;
    cin >> courseNumberInput;

    const Course* found = findCourse(courses, courseNumberInput);
    if (found == nullptr) {
        cout << courseNumberInput << " is not found." << "\n";
        return;
    }

    // Print the course title
    cout << found->courseNumber << ", " << found->courseTitle << "\n";

    // Print prerequisites with their titles
    if (!found->prerequisites.empty()) {
        cout << "Prerequisites:" << "\n";
        for (const auto& pre : found->prerequisites) {
            const Course* preCourse = findCourse(courses, pre);
            if (preCourse) {
                cout << preCourse->courseNumber << ", " << preCourse->courseTitle << "\n";
            } else {
                // In case a prerequisite is not found in the data (should not happen if data is consistent)
                cout << pre << " (course not found in data)\n";
            }
        }
    } else {
        cout << "No prerequisites." << "\n";
    }
}

// Find a course by its number, case-insensitive
const Course* findCourse(const vector<Course>& courses, const string& courseNumberInput) {
    for (auto& c : courses) {
        if (equalsIgnoreCase(c.courseNumber, courseNumberInput)) {
            return &c;
        }
    }
    return nullptr;
}

// Compare two strings ignoring case
bool equalsIgnoreCase(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return false;
        }
    }
    return true;
}
