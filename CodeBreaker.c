#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <Windows.h>
#define p printf_s
#define s scanf_s

/*
 NOTE: I use the Arrow Operator to dereference a pointer, otherwise I would use (*pass).member
 https://stackoverflow.com/questions/1580070/in-c-how-to-set-a-pointer-to-a-structure-member-that-is-an-array
 https://accuweb.cloud/resource/articles/arrow-operator-in-c
*/

struct Code {
    char answer[9];
    int charcount;
    //contain the char range within the struct incase I want multiple players, or a save state
    char max_char;
    int code_matching_flag[9];
    int guess_matching_flag[9];
};

//Globals
int win = 0, turn_count = 0;
//Prototypes
void initialize_code(struct Code* pass);
void get_code_length(struct Code* pass);
void get_letter_range(struct Code* pass);
void generate_secret_code(struct Code* pass);
int generate_random_char(struct Code* pass);
void print_code_debug(struct Code* pass);
void guess_the_password(struct Code* pass);
void wait(int seconds);
void greetings();

int main() {
    //seed random value
    srand((unsigned)time(NULL));

    // Build a single struct for the code/password
    struct Code password;
    //play the game
    greetings();
    initialize_code(&password);
    get_code_length(&password);
    get_letter_range(&password);
    p("**********************\n");
    p("Selected range: A-%c\n", password.max_char);
    p("Selected Length: %d characters\n", password.charcount);
    p("**********************\n");
    generate_secret_code(&password);
    guess_the_password(&password);
    return 0;
}

void greetings() {
    p("*************************************\n");
    p("        Welcome to Code Breaker!       \n");
    p("*************************************\n");
    wait(1);
    p("Your goal is to guess the secret code!\n");
    wait(1);
    p("\nHere's how it works:\n");
    p("1. Choose Difficulty:\n");
    p("   - Select the length of the code (3 to 8 characters).\n");
    p("   - Select the range of characters (A to F).\n");
    wait(2);
    p("\n2. Hints After Each Guess:\n");
    p("   - You'll be told:\n");
    p("     * How many letters are in the correct position.\n");
    p("     * How many letters are right, but in the wrong position.\n");
    wait(2);
    p("\n3. Win or Lose:\n");
    p("   - Guess the code before you run out of attempts to win!\n");
    p("   - If you fail, the code will be revealed.\n");
    wait(1);
    p("\nReady? Lets break this code!!\n");
    p("***************************************\n");
    wait(1);
}

void initialize_code(struct Code* pass) {
    //Fill the array with "" like we did with Hand.c
    strcpy_s(pass->answer, sizeof(pass->answer), "");
    pass->charcount = 0;
}
void get_letter_range(struct Code* pass) {
    int range = 0;
    p("Enter the maximum range size (from 4 to 8, starting with 'A'): ");
    do {
        s("%d", &range);
        if (range < 4||range > 8) {
            p("Try entering a value between 4 and 8.\n");}
    } while (range < 4||range > 8);

    //Place the ASCII number of the final character in the max_char
    pass->max_char = 'A' + range - 1;
}

void print_code_debug(struct Code* pass) {
    for (int i = 0; i < pass->charcount; i++) {
        p("Character %d: %c (ASCII: %d)\n", i, pass->answer[i], pass->answer[i]);
    }
}
void get_code_length(struct Code* pass) {
    int choice = 0;
    p("Select your level of difficulty.\n");
    p("Enter a value from 3 to 8: ");
    do {
        s("%d", &choice);  
        //Get the player's choice, and set the length of the struct
        if (choice < 3 || choice > 8) {
            p("Invalid choice. Please enter a value between 3 and 8.\n");
        }
    } while (choice < 3 || choice > 8);

    pass->charcount = choice;  
    //charcount references the length of the password
}
int generate_random_char(struct Code* pass) {
    //return a random char where the maximum value is divisible by the char ASCII value - the max char the player selects.
    return 'A' + (rand() % (pass->max_char - 'A' + 1));
}
void generate_secret_code(struct Code* pass) {
    for (int indexi = 0; indexi < pass->charcount; indexi++) {
        pass->answer[indexi] = (char)generate_random_char(pass);}
    pass->answer[pass->charcount] = '\0'; //consume the carriage
}
void guess_the_password(struct Code* pass) {
    char guess[9]; // Assuming max password length is 8 (+1 for null terminator)
    int attempts = 0;

    int k = pass->max_char - 'A' + 1; // Symbol range
    int max_attempts = 1 + (int)ceil(pass->charcount * log2(k));

    p("BREAK!\n");
    wait(1);
    p("THAT!\n");
    wait(1);
    p("CODE!\n");
    wait(1);

    while (attempts < max_attempts) {
        p("Enter your guess:\n");
        s("%8s", guess, (unsigned)sizeof(guess));

        // Check if the guess has the correct length
        if (strlen(guess) != pass->charcount) {
            p("The password is %d characters!\n", pass->charcount);
            continue;
        }
        // Check for invalid characters
        int invalid_input = 0;
        for (int i = 0; i < strlen(guess); i++) {
            if (guess[i] > pass->max_char || guess[i] < 'A') {
                p("Only use the characters A-%c\n", pass->max_char);
                invalid_input = 1;
                break;
            }
        }
        // If input is invalid, skip this iteration
        if (invalid_input) {
            continue;
        }

        /*
        Reset flags and frequency array   
        No nested for loops!https://coderanch.com/t/673693/java/Counting-occurrences-array-loop
        https://stackoverflow.com/questions/17205439/count-frequency-of-items-in-array-without-two-for-loops
        */
        int frequency[26] = { 0 }; // For 'A'-'Z', tracks unmatched occurrences in the code
        for (int i = 0; i < pass->charcount; i++) {
            pass->code_matching_flag[i] = 0;
            pass->guess_matching_flag[i] = 0;
        }

        int correct_position = 0;
        int wrong_position = 0;

        //Identify correct positions and build frequency array
        for (int i = 0; i < pass->charcount; i++) {
            if (guess[i] == pass->answer[i]) {
                correct_position++;
                pass->code_matching_flag[i] = 1;
                pass->guess_matching_flag[i] = 1;
            }
            else {
                frequency[pass->answer[i] - 'A']++;
                //Check the answer and add a counter to that index of the frequency array if it is unmatched.
            }
        }

        //Count correct characters in the wrong position
        for (int i = 0; i < pass->charcount; i++) {
            if (pass->guess_matching_flag[i]) continue; // Skip already matched characters

            int index = guess[i] - 'A';
            if (index >= 0 && index < 26 && frequency[index] > 0) {
                wrong_position++;
                frequency[index]--; // Decrement frequency for this character
            }
        }
        if (correct_position == pass->charcount) {
            p("You broke the code!\n My credit card number is...");
            wait(1);
            p(".");
            wait(1);
            p(".");
            wait(1);
            p("\nJust kidding, but you did it! Congratulations!\n");
            wait(1);
            return;
        }
        if (correct_position == 1) {
           p("%d letter is in the correct place!\n", correct_position);
        }
        if (correct_position > 1)
        {
            p("%d letters are in the correct place!\n", correct_position);
        }
        
        if (wrong_position == 1) 
        {
            p("%d letter is correct, but in the wrong position.\n", wrong_position);
        }
        if (wrong_position > 1)
        {
            p("%d letters are correct, but in the wrong position.\n", wrong_position);
        }
        
        attempts++;
        p("You have %d attempts remaining.\n", max_attempts - attempts);
    }

    p("You've run out of attempts. The password was: %s\n", pass->answer);
}


void wait(int seconds) {
    Sleep(seconds * 1000);
}


