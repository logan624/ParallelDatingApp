#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

// Constants
const int NUM_PROFILES = 20;

// Profile structure
struct Profile {
    int id;
    string name;
    int politicalView;
    int childrenPreference;
    int monogamous;
    int seriousDating;

    // Constructor to initialize a profile
    Profile(string n, int pv, int cp, int m, int sd)
        : name(n), politicalView(pv), childrenPreference(cp), monogamous(m), seriousDating(sd) {}

    // Default Constructor for a profile
    Profile()
        : id(0), name(""), politicalView(0), childrenPreference(0), monogamous(0), seriousDating(0) {}
};

// Enumeration types to describe users' dating preferences
enum {
    LIBERAL = 1, MODERATE, CONSERVATIVE
} PoliticalView;

enum {
    KIDS = 1, NO_KIDS, UNDECIDED
} ChildrenPreference;

enum {
    MONOGAMOUS = 1, POLYAMOROUS
} Monogamous;

enum {
    SERIOUS = 1, CASUAL
} SeriousDating;

// Pair structure for map-reduce
struct Pair {
    int key;
    string val;

    Pair(int k, const string &v) : key(k), val(v) {}
};

// Map-reduce class
class MR {
private:
    vector<Profile> profiles;
    vector<Pair> pairs, results;

    void Map(const Profile &user, const vector<Profile> &potentialMatches);
    void do_sort(vector<Pair> &vec);
    int Reduce(int key, const vector<Pair> &pairs, int index, string &values);

public:
    MR(const vector<Profile> &p) : profiles(p) {}
    const vector<Pair> &run(const Profile &user, int minCompatibilityScore);
};

// Global Variables

// Array of user profiles
std::vector<Profile> potential_matches;

// Function Prototypes
void getUserData(Profile & p);
std::vector<std::string> readNamesFromFile(const std::string& filename);
std::vector<Profile> generateProfiles(int numProfiles);
int calculateCompatibility(const Profile &user, const Profile &other);
bool compare(const Pair &p1, const Pair &p2);

int main() {
    // User's profile
    string name;
    int politicalView, childrenPreference, monogamous, seriousDating;

    Profile user(name, politicalView, childrenPreference, monogamous, seriousDating);

    getUserData(user);

    // Generate profiles for the user to match with
    potential_matches = generateProfiles(NUM_PROFILES);

    for(const auto &match : potential_matches)
    {
        cout << match.name << std::endl;
    }

    // Map-reduce processing
    MR map_reduce(potential_matches);
    vector<Pair> results = map_reduce.run(user, 3); // Assuming a minimum compatibility score of 3

    // Display results
    cout << "\nPotential matches:\n";
    for (const auto &result : results) {
        cout << result.val << " - Compatibility Score: " << result.key << endl;
    }

    return 0;
}

void getUserData(Profile & p)
{
    cout << "Enter your name: ";
    cin >> p.name;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Enter your political view (1 for liberal, 2 for moderate, 3 for conservative): ";
    cin >> p.politicalView;

    cout << "Enter your preference regarding children (1 for yes, 2 for no, 3 for undecided): ";
    cin >> p.childrenPreference;

    cout << "Enter your view on monogamy (1 for monogamous, 2 for polyamorous): ";
    cin >> p.monogamous;

    cout << "Enter your preference for serious dating (1 for yes, 2 for no): ";
    cin >> p.seriousDating;
}

// Function to read names from a file into a vector
std::vector<std::string> readNamesFromFile(const std::string& filename) {
    std::vector<std::string> names;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string name;
        while (std::getline(file, name)) {
            names.push_back(name);
        }
        file.close();
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    return names;
}

// Function to generate a specified number of profiles
std::vector<Profile> generateProfiles(int numProfiles) {
    std::vector<Profile> profiles;

    // Seed for randomization based on current time
    std::srand(static_cast<unsigned int>(std::time(0)));

    // Reading names from a file
    std::vector<std::string> commonNames = readNamesFromFile("first_names.txt");

    if (commonNames.empty()) {
        std::cerr << "No names found in the file." << std::endl;
        return profiles;
    }

    for (int i = 0; i < numProfiles; ++i) {
        Profile newProfile;

        // Assigning unique ID
        newProfile.id = i + 1;

        // Randomly selecting a name from the common names list
        int randomIndex = rand() % commonNames.size();
        newProfile.name = commonNames[randomIndex];

        // Assigning random values based on constraints
        newProfile.politicalView = rand() % 3 + 1;
        newProfile.childrenPreference = rand() % 3 + 1;
        newProfile.monogamous = rand() % 2 + 1;
        newProfile.seriousDating = rand() % 2 + 1;

        // Adding the new profile to the vector
        profiles.push_back(newProfile);
    }

    return profiles;
}

// Function to calculate compatibility score
int calculateCompatibility(const Profile &user, const Profile &other) {
    int score = 0;
    score += (user.politicalView == other.politicalView);
    score += (user.childrenPreference == other.childrenPreference);
    score += (user.monogamous == other.monogamous);
    score += (user.seriousDating == other.seriousDating);
    return score;
}

void MR::Map(const Profile &user, const vector<Profile> &potentialMatches) {
    for (const Profile &match : potentialMatches) {
        int score = calculateCompatibility(user, match);
        pairs.push_back(Pair(score, match.name));
    }
}

bool compare(const Pair &p1, const Pair &p2) {
    return p1.key > p2.key;
}

void MR::do_sort(vector<Pair> &vec) {
    sort(vec.begin(), vec.end(), compare);
}

int MR::Reduce(int key, const vector<Pair> &pairs, int index, string &values) {
    while (index < pairs.size() && pairs[index].key == key)
        values += pairs[index++].val + " ";
    return index;
}

const vector<Pair> &MR::run(const Profile &user, int minCompatibilityScore) {
    Map(user, profiles);
    do_sort(pairs);

    int next = 0;
    int bestMatchScore = -1; // Variable to store the score of the best match
    string bestMatchValues;

    while (next < pairs.size()) {
        string values;
        values = "";
        int key = pairs[next].key;

        // Check if it's the best match so far
        if (bestMatchScore == -1 || key > bestMatchScore) {
            bestMatchScore = key;
            bestMatchValues = pairs[next].val; // Assign the name associated with the best match
        }

        next++;
    }

    // If no match is found, use the best match found
    if (bestMatchScore != -1) {
        Pair bestMatch(bestMatchScore, bestMatchValues);
        results.push_back(bestMatch);
    } else {
        results.push_back(Pair(-1, "No suitable match found."));
    }

    return results;
}
