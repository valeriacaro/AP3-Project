/*********************************************************
File name: mh.cc
File function: develop a schedule with the fewest possible
days of screened films, taking into account the films that
cannot be projected in the same time and the number of
cinemas. It is solved by methauristics based on GRASP and
Simulated Annealing algorithms.
Authors: Valèria Caro & Esther Fanyanàs
Date: 03_01_2022
**********************************************************/

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
#include <math.h>
#include <stdlib.h>
#include <random>

using namespace std;

/***********************************************************
                 CONSTANTS AND VARIABLES
***********************************************************/

unsigned t0, t1; // Time variables
string input_file, output_file; // Files to read input and write output

int n_films; // |P|: Films number
int n_PairsFilms; // |L|: Pair of films that cannot be projected together
int n_CinRooms; // |S|: Cinema rooms number

int best_days; // Will take constance of the minimum number of days
// found to solve the problem

vector<string> films; // Vector with film names
vector<string> CinRooms; // Vector with cinema rooms names

map<string, int> film_code;// Map that assigns a film to a number

vector<vector<bool>> relations_graph; // Boolean matrix that
// indicates if a film can be projected with another one or not

using Organization = vector<vector<int>>; // Matrix with the festival
// organization; each row is a day and each column is a cinema room

/***********************************************************
                        FUNCTIONS
***********************************************************/

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
  string name;
  for (int i = 0; i < n_films; ++i){
    // Reads the film name
    in >> name;
    // Assigning the film to a number
    film_code.insert({name, i});
    // Saves the film names
    films[i] = name;
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
    // Marks the boxes corresponding to the two films as true, indicating
    // there is an incompatibility
    relations_graph[code1][code2] = true;
    relations_graph[code2][code1] = true;
  }

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
* Name: how_many_incompatibilities
* Function: Counts how many incompatibilities has a given
            film on the day it is being projected.
* Parameters: actual: Matrix with the schedule (rows are
              the days and, the columns, the cinema rooms).
              day: The day we want to check.
              code: Film number.
* Return: Number of incompatibilities of the film given
          on the day it is.
-------------------------------------------------------- */
int how_many_incompatibilities(const Organization& actual, int day, int code){
  int incompatibilities = 0;
  for (int room = 0; room < int(actual[day].size()); ++room) if (relations_graph[code][actual[day][room]]) incompatibilities += 1;
  return incompatibilities;
}

/* --------------------------------------------------------
* Name: generate_initial_solution
* Function: Generates a possible solution to solve the
            problem: a schedule for the festival with
            no incompatibilities between films. This
            solution is generated by a greedy randomized
            algorithm.
* Parameters: -
* Return: A schedule for the festival.
-------------------------------------------------------- */
Organization generate_initial_solution(){
  Organization actual;
  vector<int> p(n_films);
  // Fill the vector with ordered numbers
  for (int k = 0; k < n_films; ++k) p[k] = k;

  // Randomly rearrange elements in range using generator
  shuffle(p.begin(), p.end(), default_random_engine());

  for (int film_index = 0; film_index < n_films; ++film_index){
    // Projected will keep track of the film to decide if it has been placed
    bool projected = false;
    // Go through the days if the film has not been projected yet
    for (int day = 0; day < int(actual.size()) and not projected; ++day){
      // If the day has enough space and there are not incompatibilities, then we place the film
      if (int(actual[day].size()) < n_CinRooms and can_be_projected(actual, day, p[film_index])){
        actual[day].push_back(p[film_index]);
        projected = true;
      }
    }
    // If the place has not been placed, then place it in a new day
    if (not projected){
      actual.push_back({p[film_index]});
    }
  }
  return actual;
}

/* --------------------------------------------------------
* Name: solve_incompatibilities
* Function: Solves incompatibilities among the days and
            returns if there persists incompatibilities
            following a Simulated Annealing algorithm
* Parameters: actual: Matrix with the schedule (rows are
              the days and, the columns, the cinema rooms).
              day_incomp: Vector with how many
              incompatibilities has each day.
              incompatibilities: Total number of
              incompatibilities.
* Return: true if actual ends up with no incompatibilities,
          false otherwise.
-------------------------------------------------------- */
bool solve_incompatibilities(Organization& actual, vector<int>& day_incomp, int& incompatibilities){
  // Set initial temperature needed for Simulated Annealing
  float T = 0.1;
  // While there are incompatibilities and T is bigger enough
  while (incompatibilities > 0 and T > 0.0000005){
    // Initialize a variable to check which day needs to be solved
    int day_to_solve;
    // At the beginning no incomaptibilities have been found
    bool incomp_found = false;
    // For each day and while no incomaptibilities have been found,
    for (int i = 0; i < int(day_incomp.size()) and not incomp_found; ++i){
      // Check if there are incompatibilities
      if (day_incomp[i] > 0){
        // Save the day as the one that needs to be solved
        day_to_solve = i;
        // And indicate we find an incompatibility to finish the for
        incomp_found = true;
      }
    }
    // Initialize incomp_found again for next iterations
    incomp_found = false;

    // Will need to save the changes on the number of total incompatibilities
    // and the ones on each day
    int old_incompatibilities2, old_incompatibilities1;
    int new_incompatibilities1, new_incompatibilities2;
    int new_incompatibilities, old_incompatibilities;

    // Search in the day with incompatibilities the first film generating conflicts
    for (int film_index = 0; film_index < int(actual[day_to_solve].size()); ++film_index){
      old_incompatibilities1 = how_many_incompatibilities(actual, day_to_solve, actual[day_to_solve][film_index]);
      // When found,
      if (old_incompatibilities1 != 0){
        int random_day;
        // choose a new different day
        do random_day = rand()%int(actual.size()); while (random_day == day_to_solve);
        // and a new film
        int random_film = rand()%int(actual[random_day].size());
        // Calculate the incomaptibilities that the film chosen at random generates on the day it is
        old_incompatibilities2 = how_many_incompatibilities(actual, random_day, actual[random_day][random_film]);
        // Change the position of the film chosen at random with the one found at the beginning
        int aux = actual[day_to_solve][film_index];
        actual[day_to_solve][film_index] = actual[random_day][random_film];
        actual[random_day][random_film] = aux;
        // Compute the new incompatibilities they generate on the days they are assigned now
        new_incompatibilities1 = how_many_incompatibilities(actual, day_to_solve, actual[day_to_solve][film_index]);
        new_incompatibilities2 = how_many_incompatibilities(actual, random_day, actual[random_day][random_film]);
        new_incompatibilities = new_incompatibilities1 + new_incompatibilities2;
        old_incompatibilities = old_incompatibilities1 + old_incompatibilities2;
        // If the previous incompatibilities were greater than the new ones
        if (old_incompatibilities > new_incompatibilities){
          // We update the incompatibilities numbers accepting the change done
          incompatibilities = incompatibilities - old_incompatibilities + new_incompatibilities;
          day_incomp[day_to_solve] += new_incompatibilities1 - old_incompatibilities1;
          day_incomp[random_day] += new_incompatibilities2 - old_incompatibilities2;
        }
        // Otherwise,
        else{
          // Calculate the probability of accepting the change although it is a worth solution
          // It will follow and exponencial law; the score functions are the number
          // of incompatibilities of the new and old parcial solution
          float p = exp(-(new_incompatibilities - old_incompatibilities)/T);
          // Generate a random value between 0 and 1
          float random_value = rand()/double(RAND_MAX);
          // If the random_value is lower or equal to the probability of acceptance,
          if (random_value <= p){
            // update incompatibilities and accept the solution; this avoids getting
            // stuck
            day_incomp[day_to_solve] += new_incompatibilities1 - old_incompatibilities1;
            day_incomp[random_day] += new_incompatibilities2 - old_incompatibilities2;
            incompatibilities = incompatibilities - old_incompatibilities + new_incompatibilities;
          }
          // Otherwise,
          else{
            // Undo the changes on the schedule
            actual[random_day][random_film]  = actual[day_to_solve][film_index];
            actual[day_to_solve][film_index] = aux;
          }
        }
        // Modify T making it lower in order to make p lower in the next iteration
        T *= 0.999;
      }
    }
  }
  // If there are no incompatibilities
  if (incompatibilities == 0){
    if (int(actual.size()) < best_days){
      best_days = int(actual.size());
      write(actual);
    }
    // We return true
    return true;
  }
  // Otherwise, we return false
  return false;
}


/* --------------------------------------------------------
* Name: improve
* Function: Tries to remove a day from the schedule
            placing the films on the last day in empty
            cinema rooms of previous days.
* Parameters: actual: Matrix with the schedule (rows are
              the days and, the columns, the cinema rooms).
              day_incomp: Vector with how many
              incompatibilities has each day.
              incompatibilities: Total number of
              incompatibilities.
* Return: -
-------------------------------------------------------- */
void improve(Organization& actual, vector<int>& day_incomp, int& incompatibilities){
  // Set the last day as the one to being removed
  int day_to_remove = int(actual.size())-1;
  int day_to_complete;
  int film_to_remove;
  // Set an initial big number of incompatibilities in order to, later, find
  // the minimum value for the variables counting them
  int incompatibilities_generated, new_incompatibilities = 1e6;
  // While there are films in the day to remove
  while (int(actual[day_to_remove].size()) > 0){
    // Get the film last film as the one to being removed
    film_to_remove = actual[day_to_remove][int(actual[day_to_remove].size())-1];
    // At the beginning, not empty spaces have been found
    bool empty_spaces = false;
    // Look for an empty space on the previous days
    for (int i = 0; i < day_to_remove; ++i){
      // If there are empty cinema rooms,
      if (int(actual[i].size()) < n_CinRooms){
        // check the number of incompatibilities it would generate the film to
        // remove in that spot
        incompatibilities_generated = how_many_incompatibilities(actual, i, film_to_remove);
        // If the incompatibilities generated are less than the minimum found at the moment,
        if (incompatibilities_generated < new_incompatibilities){
          // Update the new_incompatibilities: now the minimum is the ones just found
          new_incompatibilities = incompatibilities_generated;
          // And indicate on which day we can place the film in order to complete it
          day_to_complete = i;
          // We have found an empty space
          empty_spaces = true;
        }
      }
    }
    // In case there are empty spaces
    if (empty_spaces){
      // Update incompatibilities of the day we are placing the film
      day_incomp[day_to_complete] += new_incompatibilities;
      // Update total incompatibilities
      incompatibilities += new_incompatibilities;
      // We pop the film from the day to remove
      actual[day_to_remove].pop_back();
      // And add the film to remove to the day to complete
      actual[day_to_complete].push_back(film_to_remove);
      // Intialize empty_spaces and new_incompatibilities again to remove the
      // next film on the day to remove
      empty_spaces = false;
      new_incompatibilities = 1e6;
    }
    // Otherwise
    else{
      // In case the number of actual days are less than the best days,
      if (int(actual.size()) < best_days){
        // Update best days
        best_days = int(actual.size());
        // And write the result in the file; this is the best solution by far
        // as there are not empty cinema rooms and all films are placed
        write(actual);
      }
    }
  }
  // If the day to remove is empty
  if ((actual[day_to_remove].size()) == 0){
    // We remove it from the schedule
    actual.pop_back();
    day_incomp.pop_back();
  }
}

/* --------------------------------------------------------
* Name: GRASP
* Function: Greedy Randomized Adaptive Search Procedure
* Parameters: -
* Return: -
-------------------------------------------------------- */
void GRASP(){
  while(true){
    // Creates a first solution
    Organization actual = generate_initial_solution();
    int days = int(actual.size());
    // If the number of days of the solution is lower than the one on the best
    // solution,
    if (days < best_days){
      // Write the time required, days the festival lasts and schedule
      write(actual);
      // Update the minimum number of days found
      best_days = days;
    }
    // Try to remove a day from the solution and, once this happens, try to
    // solve the incompatibilities generated
    int incompatibilities = 0;
    vector<int> day_incomp(days, 0);
    do improve(actual, day_incomp, incompatibilities); while (solve_incompatibilities(actual, day_incomp, incompatibilities));
  }
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
  // We will want the code to compute completely random the variables
  // that need to be randomized
  srand(time(NULL));
  // Set the intput and output files
  input_file = string(argv[1]);
  output_file = string(argv[2]);
  // Read data
  read_data();
  // Start counting time
  t0 = clock();
  // In the worst case, there will be as many days as films
  best_days = n_films;
  // Schedule the festival
  GRASP();
}
