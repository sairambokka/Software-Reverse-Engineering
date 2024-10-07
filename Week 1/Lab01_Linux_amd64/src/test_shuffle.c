#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "poker.h"  // Assuming DECK is defined in this header

// Declare the shuffle function implemented in assembly
void shuffle(DECK deck);

void init_deck(DECK deck) {
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 1; rank <= 13; rank++) {
            deck[suit * 13 + rank - 1] = suit << 4 | rank;
        }
    }
}

int find_card(DECK deck) {
    for (int i = 0; i < 52; i++) {
        if (deck[i] == 0x31) {  
            return i;
        }
    }
    abort();  // If the card is not found, something went wrong
    return -1;
}

int main() {
    int dist[52] = {0};

    // Seed the random number generator with the current time
    srand(time(NULL));

    for(int i = 0; i < 1000; i++) {
        DECK deck;  // Use the DECK type as defined in poker.h
        init_deck(deck);  // Initialize the deck
        shuffle(deck);    // Shuffle the deck using the assembly function
        int card = find_card(deck);  // Find the 2 of Hearts (0x32)
        dist[card]++;  // Track where the card appears in the shuffled deck
    }

    for (int i = 0; i < 52; i++) {
        printf("dist[%d] =  %d\n", i, dist[i]);
    }

    return 0;
}
