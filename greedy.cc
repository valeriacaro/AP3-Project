/*********************************************************
File name: greedy.cc
File function: develop a schedule with the fewest possible
days of screened films, taking into account the films that
cannot be projected in the same time and the number of
cinemas. It is implemented with a greedy algorithm.
Authors: Valèria Caro & Esther Fanyanàs
Date: 07_12_2021
*********************************************************/

/*********************************************************
                        IMPORTS
*********************************************************/

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include <fstream>
#include <utility>

using namespace std;

/***********************************************************
                 CONSTANTS AND VARIABLES
***********************************************************/

unsigned t0, t1; // Time variables
string input_file, output_file; // Files to read input and write output

int n_films; // |P|: Films number
int n_PairsFilms; // |L|: Pair of films that cannot be projected together
int n_CinRooms; // |S|: Cinema rooms number

vector<string> films; // Vector with film names
vector<string> CinRooms; // Vector with cinema rooms names

map<string, int> film_code;// Map that assigns a film to a number

vector<vector<bool>> relations_graph; // Boolean matrix that
// indicates if a film can be projected with another one or not

using Organization = vector<vector<int>>; // Matrix with the festival
// organization; each row is a day and each column is a cinema room
using Pair = pair<int,int>; // Releates a film with the total incompatibilities
// it has

vector<Pair> restrictions; // Vector that stores with how many incompatible
// films has a film

/***********************************************************
                        FUNCTIONS
***********************************************************/


/* --------------------------------------------------------
* Name: comparator
* Function: Compares two Pair struct to determine which one
            it is bigger.
* Parameters: a and b: two Pair struct.
* Return: the Pair with the second biggest field.
-------------------------------------------------------- */
bool comparator (const Pair& a, const Pair& b){
  return a.second > b.second;
}

/* --------------------------------------------------------
* Name: read_data
* Function: Reads the input from a file and process it.
* Parameters: file: Name of the file from where we want to
             get the input.
* Return: -
-------------------------------------------------------- */
void read_data(){
  // Performing the input of a file
  ifstream in(input_file);

  // Reading films
  in >> n_films;
  films.resize(n_films);
  restrictions.resize(n_films);
  string name;
  for (int i = 0; i < n_films; ++i){
    // Reads the film name
    in >> name;
    // Assigning a number to the film
    film_code.insert({name, i});
    // Saves the film names
    films[i] = name;
    // At the moment the film has 0 incompatibilities
    restrictions[film_code[name]].first = film_code[name];
    restrictions[film_code[name]].second = 0;
  }

  // Reading films that cannot be projected at the same time
  in >> n_PairsFilms;
  // At the beginning there are not incompatibilities
  relations_graph.resize(n_films, vector<bool> (n_films, false));
  string film1, film2;
  for (int i = 0; i < n_PairsFilms; ++i){
    // Reads the films names
    in >> film1 >> film2;
    int code1 = film_code[film1];
    int code2 = film_code[film2];
    // Increases the number of restrictions each film has
    restrictions[code1].second += 1;
    restrictions[code2].second += 1;
    // Marks the boxes corresponding to the two films as true, indicating
    // there is an incompatibility
    relations_graph[code1][code2] = true;
    relations_graph[code2][code1] = true;
  }

  // Sorting films by restrictions
  sort(restrictions.begin(), restrictions.end(), comparator);

  // Reading cinema rooms
  in >> n_CinRooms;
  CinRooms.resize(n_CinRooms);
  // Saves the cinema names
  for (int i = 0; i < n_CinRooms; ++i) in >> CinRooms[i];

}

/* --------------------------------------------------------
* Name: write
* Function: Writes the output on "output.txt" file (time
            required, days the festival lasts and schedule).
* Parameters: best: matrix with the best film schedule
              where the rows are the days and the columns
              are the cinema rooms.
* Return: -
-------------------------------------------------------- */
void write(const Organization& best){
  // Calculates the time it has taken to know the schedule
  t1 = clock();
  double time = (double(t1-t0)/CLOCKS_PER_SEC);
  // Performing the output of a file
  ofstream file;
  // Set decimal precision with 1 decimal
  file.setf(ios::fixed);
  file.precision(1);
  // Creating or opening the file where we will write the output
  file.open(output_file);
  // Writes the time it has taken to compute the solution
  file << time << endl;
  // Writes how many days the festival lasts
  file << int(best.size()) << endl;
  // For each film writes on which day it will be performed and
  // at which cinema room
  for (int i = 0; i < int(best.size()); ++i){
    for (int j = 0; j < int(best[i].size()); ++j){
      file << films[best[i][j]] << " " << i+1 << " " << CinRooms[j] << endl;
    }
  }
  file.close();
}

/* --------------------------------------------------------
* Name: can_be_projected
* Function: Indicates if a film can be projected on a
            given a day depending on if it is incompatible
            with the thers films of that day or not.
* Parameters: actual: Matrix with the schedule (rows are
              the days and, the columns, the cinema rooms).
              day: The day we want to check.
              code: Film number.
* Return: True if a film can be projected that day or
          false otherwise.
-------------------------------------------------------- */
bool can_be_projected(const Organization& actual, int day, int code){
  for (int room = 0; room < int(actual[day].size()); ++room) if (relations_graph[code][actual[day][room]]) return false;
  return true;
}

/* --------------------------------------------------------
* Name: schedule_festival
* Function: Schedule films in a matrix of days and cinemas,
            respecting the films that cannot be projected
            at the same time.
* Parameters: actual: Matrix with the schedule (rows are
              the days and the columns the cinemas).
* Return: -
-------------------------------------------------------- */
void schedule_festival(Organization& actual){
  // Go through the films
  for (int film_index = 0; film_index < n_films; ++film_index){
    // projected will keep track of the film to decide if it has been placed
    bool projected = false;
    // Go through the days if the film has not been projected yet
    for (int day = 0; day < int(actual.size()) and not projected; ++day){
      // If the day has enough space and there are not incompatibilities, then we place the film
      if (int(actual[day].size()) < n_CinRooms and can_be_projected(actual, day, restrictions[film_index].first)){
        actual[day].push_back(restrictions[film_index].first);
        projected = true;
      }
    }
    // If the place has not been placed, then place it in a new day
    if (not projected) actual.push_back({restrictions[film_index].first});
  }
  // Finish when all films are placed
  write(actual);
}

/***********************************************************
                          MAIN
***********************************************************/

/* --------------------------------------------------------
* Name: main
* Function: main function
* Parameters: argc: number of arguments passed when
              launching the code
              argv: arguments given
* Return: 0
-------------------------------------------------------- */

int main(int argc, char** argv){
  // Set the intput and output files
  input_file = string(argv[1]);
  output_file = string(argv[2]);
  // Read data from the file
  read_data();
  // Create the schedul
  Organization actual;
  // Start counting time
  t0 = clock();
  // Schedule the festival
  schedule_festival(actual);
}
