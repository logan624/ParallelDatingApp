// Program: Matchmaker (Serial)
// Authors: Hitomi Ichwan and Logan Gregg
// Description:
//       Dating app simulation using the Map-Reduction algorithm from the
//       Drug Design Exemplar

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <limits>
#include <cmath> // Include cmath for the floor function

using namespace std;

// Constants
const int NUM_PROFILES = 200;

// Profile structure
struct Profile {
    string name;
    double politicalView;
    double politicalImportance; // Importance factor for political view
    double childrenPreference;
    double childrenImportance; // Importance factor for children preference
    double monogamous;
    double monogamousImportance; // Importance factor for monogamous preference
    double seriousDating;
    double seriousDatingImportance; // Importance factor for serious dating preference

    Profile(string n, double pv, double pImp, double cp, double cImp, double m, double mImp, double sd, double sdImp)
        : name(n), politicalView(pv), politicalImportance(pImp), childrenPreference(cp), childrenImportance(cImp),
          monogamous(m), monogamousImportance(mImp), seriousDating(sd), seriousDatingImportance(sdImp) {}

    Profile()
        : name(""), politicalView(0.0), politicalImportance(0.0), childrenPreference(0.0), childrenImportance(0.0),
          monogamous(0.0), monogamousImportance(0.0), seriousDating(0.0), seriousDatingImportance(0.0) {}
};

// Documentation: Enumeration types to describe possible values for users' dating preferences
    // enum PoliticalViewType {
    //     LIBERAL = 1, MODERATE, CONSERVATIVE
    // };
    // enum ChildrenPreferenceType {
    //     KIDS = 1, NO_KIDS, UNDECIDED
    // };
    // enum MonogamousType {
    //     MONOGAMOUS = 1, POLYAMOROUS
    // };
    // enum SeriousDatingType {
    //     SERIOUS = 1, CASUAL
    // };

// Pair structure for map-reduce
struct Pair {
    double key;
    string val;
    double politicalView;
    double politicalViewImportance;
    double childrenPreference;
    double childrenPreferenceImportance;
    double monogamous;
    double monogamousImportance;
    double seriousDating;
    double seriousDatingImportance;

    Pair(double k, const string &v, double pv, double pvi, double cp, double cpi, double m, double mi, double sd, double sdi)
        : key(k), val(v), politicalView(pv), politicalViewImportance(pvi), childrenPreference(cp), childrenPreferenceImportance(cpi),
          monogamous(m), monogamousImportance(mi), seriousDating(sd), seriousDatingImportance(sdi) {}
};

class MR {
private:
    vector<Profile> profiles;
    vector<Pair> pairs, results;

    // Declare pairs as a member variable here
    void Map(const Profile &user, const vector<Profile> &potentialMatches);
    void do_sort(vector<Pair> &vec);
    int Reduce(double key, const vector<Pair> &pairs, int index, string &values);

public:
    MR(const vector<Profile> &p) : profiles(p) {}
    const vector<Pair> &run(const Profile &user);
};
// Global Variables

// Array of user profiles
std::vector<Profile> potential_matches;

// Function Prototypes
void getUserData(Profile &p);
std::vector<std::string> readNamesFromFile(const std::string &filename);
std::vector<Profile> generateProfiles(int numProfiles);
double calculateCompatibility(const Profile &user, const Profile &other);
bool compare(const Pair &p1, const Pair &p2);

int main() {
    // User's profile
    string name = "Logan";
    double politicalView = 1, childrenPreference = 1, monogamous = 1, seriousDating = 1;

    Profile user(name, politicalView, 0.0, childrenPreference, 0.0, monogamous, 0.0, seriousDating, 0.0);

    // Uncomment to manually enter profile data if desired
    // getUserData(user);

    // Generate profiles for the user to match with
    potential_matches = generateProfiles(NUM_PROFILES);

    cout << "\nPotential Matches:\n";
    cout << "------------------------------\n";
    for (const auto &match : potential_matches) {
        cout << match.name << std::endl;
    }

    // Map-reduce processing
    MR map_reduce(potential_matches);
    vector<Pair> results = map_reduce.run(user);

    // Display potential matches
    cout << "\nBest Matches:\n";
    cout << "------------------------------\n";

    if (results.empty()) {
        cout << "No potential matches found.\n";
    } else {
        for (const auto &result : results) {
            cout << result.val << "\n";
        }
    }
    return 0;
}

void getUserData(Profile &p) {
    cout << "Enter your name: ";
    cin >> p.name;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Enter your political view (1 for liberal, 2 for moderate, 3 for conservative): ";
    cin >> p.politicalView;

    cout << "On a scale from 0 to 10, how important is your political view? ";
    double politicalImportance;
    cin >> politicalImportance;

    cout << "Enter your preference regarding children (1 for yes, 2 for undecided, 3 for no): ";
    cin >> p.childrenPreference;

    cout << "On a scale from 0 to 10, how important is your children preference? ";
    double childrenImportance;
    cin >> childrenImportance;

    cout << "Enter your view on monogamy (1 for monogamous, 2 for polyamorous): ";
    cin >> p.monogamous;

    cout << "On a scale from 0 to 10, how important is your view on monogamy? ";
    double monogamousImportance;
    cin >> monogamousImportance;

    cout << "Enter your preference for serious dating (1 for yes, 2 for no): ";
    cin >> p.seriousDating;

    cout << "On a scale from 0 to 10, how important is your preference for serious dating? ";
    double seriousDatingImportance;
    cin >> seriousDatingImportance;

    if (p.politicalView > 3) {
        p.politicalView = 2 * 3 - p.politicalView; // Reflecting on the opposite side of 3
    } else
        p.politicalView += (1 - (politicalImportance / 10));

    if (p.childrenPreference > 3) {
        p.childrenPreference = 2 * 3 - p.childrenPreference; // Reflecting on the opposite side of 3
    } else
        p.childrenPreference += (1 - (childrenImportance / 10));

    if (p.monogamous > 2) {
        p.monogamous = 2 * 2 - p.monogamous; // Reflecting on the opposite side of 2
    } else
        p.monogamous += (1 - (monogamousImportance / 10));
    if (p.seriousDating > 2) {
        p.seriousDating = 2 * 2 - p.seriousDating; // Reflecting on the opposite side of 2
    } else
        p.seriousDating += (1 - (seriousDatingImportance / 10));
}

// Function to read names from a file into a vector
std::vector<std::string> readNamesFromFile(const std::string &filename) {
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

        // Randomly selecting a name from the common names list
        int randomIndex = rand() % commonNames.size();
        newProfile.name = commonNames[randomIndex];

        // Assigning random values based on constraints (now using decimals)
        newProfile.politicalView = (rand() % 21) / 10.0 + 1.0;
        newProfile.politicalImportance = 10 * (newProfile.politicalView - floor(newProfile.politicalView));
        newProfile.childrenPreference = ((rand() % 11) / 10.0 + 1.0);
        newProfile.childrenImportance = 10 * (newProfile.childrenPreference - floor(newProfile.childrenPreference));
        newProfile.monogamous = (rand() % 11) / 10.0 + 1.0;
        newProfile.monogamousImportance = 10 * (newProfile.monogamous - floor(newProfile.monogamous));
        newProfile.seriousDating = (rand() % 11) / 10.0 + 1.0;
        newProfile.seriousDatingImportance = 10 * (newProfile.seriousDating - floor(newProfile.seriousDating));

        // Adding the new profile to the vector
        profiles.push_back(newProfile);
    }

    return profiles;
}

// Function to calculate compatibility score as percentage
double calculateCompatibility(const Profile &user, const Profile &other) {
    double score = 0.0;

    // Calculate compatibility for each preference
    score += 100.0 * (1.0 - abs(user.politicalView - other.politicalView));
    score += 100.0 * (1.0 - abs(user.childrenPreference - other.childrenPreference));
    score += 100.0 * (1.0 - abs(user.monogamous - other.monogamous));
    score += 100.0 * (1.0 - abs(user.seriousDating - other.seriousDating));

    return score / 4.0; // Average the scores and return as percentage
}

void MR::Map(const Profile &user, const vector<Profile> &potentialMatches) {
    // Use the member variable pairs instead of a local variable
    for (const Profile &match : potentialMatches) {
        double score = calculateCompatibility(user, match);
        pairs.push_back(Pair(score, match.name, match.politicalView, match.politicalImportance,
                             match.childrenPreference, match.childrenImportance,
                             match.monogamous, match.monogamousImportance,
                             match.seriousDating, match.seriousDatingImportance));
    }
}
bool compare(const Pair &p1, const Pair &p2) {
    return p1.key > p2.key;
}

void MR::do_sort(vector<Pair> &vec) {
    sort(vec.begin(), vec.end(), compare);
}

int MR::Reduce(double key, const vector<Pair> &pairs, int index, string &values) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << pairs[index].key;

    while (index < pairs.size() && pairs[index].key == key) {
        values += pairs[index].val + "\n";
        values += "  Chose Political View: " + to_string(static_cast<int>(pairs[index].politicalView))
             + " (Importance: " + to_string(static_cast<int>(pairs[index].politicalViewImportance)) + ")\n";
        values += "  Chose Children Preference: " + to_string(static_cast<int>(pairs[index].childrenPreference))
             + " (Importance: " + to_string(static_cast<int>(pairs[index].childrenPreferenceImportance)) + ")\n";
        values += "  Chose Monogamous: " + to_string(static_cast<int>(pairs[index].monogamous))
             + " (Importance: " + to_string(static_cast<int>(pairs[index].monogamousImportance)) + ")\n";
        values += "  Chose Serious Dating: " + to_string(static_cast<int>(pairs[index].seriousDating))
             + " (Importance: " + to_string(static_cast<int>(pairs[index].seriousDatingImportance)) + ")\n";
        values += "  Compatibility Score: " + oss.str() + "%" + "\n";
        values += "------------------------------\n";  // Dotted line after each match
        index++;
    }

    return index;
}

const vector<Pair> &MR::run(const Profile &user) {
    Map(user, profiles);
    do_sort(pairs);

    int next = 0;
    double bestMatchScore = -1; // Variable to store the score of the best match

while (next < pairs.size()) {
    string values;
    double key = pairs[next].key;

    // Check if it's the best match so far
    if (bestMatchScore == -1 || key > bestMatchScore) {
        bestMatchScore = key;
        next = Reduce(key, pairs, next, values); // Gather all matches with the same score

        // Skip adding the result when bestMatchScore is -1
        if (bestMatchScore != -1) {
            Pair bestMatch(bestMatchScore, values,
                           pairs[next].politicalView, pairs[next].politicalViewImportance,
                           pairs[next].childrenPreference, pairs[next].childrenPreferenceImportance,
                           pairs[next].monogamous, pairs[next].monogamousImportance,
                           pairs[next].seriousDating, pairs[next].seriousDatingImportance);
            results.push_back(bestMatch);
        }
    }

    next = Reduce(key, pairs, next, values); // Move to the next set
}

    return results;
}
