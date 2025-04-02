#include <stdlib.h>
#include <string.h>

#define RANKS 13
#define SUITS 4

// Helper function to get the rank of a card
int get_rank(int card) {
    return card & 0xf;
}

// Helper function to get the suit of a card
int get_suit(int card) {
    return (card >> 4) & 0xf;
}

// Helper function to count occurrences of each rank
void count_ranks(int* hand, int* rank_counts) {
    memset(rank_counts, 0, RANKS * sizeof(int));
    for (int i = 0; i < 5; i++) {
        rank_counts[get_rank(hand[i]) - 1]++;
    }
}

// Helper function to check for flush
int is_flush(int* hand) {
    int suit = get_suit(hand[0]);
    for (int i = 1; i < 5; i++) {
        if (get_suit(hand[i]) != suit) return 0;
    }
    return 1;
}

// Helper function to check for straight
int is_straight(int* sorted_ranks) {
    // Check for A-2-3-4-5 straight
    if (sorted_ranks[0] == 12 && sorted_ranks[1] == 3 && sorted_ranks[2] == 2 && 
        sorted_ranks[3] == 1 && sorted_ranks[4] == 0) {
        return 1;
    }
    
    for (int i = 1; i < 5; i++) {
        if (sorted_ranks[i] != sorted_ranks[i-1] - 1) return 0;
    }
    return 1;
}

// Main function to compare hands
int compare_hands(int* hand0, int* hand1) {
    int rank_counts0[RANKS], rank_counts1[RANKS];
    int sorted_ranks0[5], sorted_ranks1[5];
    
    count_ranks(hand0, rank_counts0);
    count_ranks(hand1, rank_counts1);
    
    // Sort ranks in descending order
    for (int i = 0; i < 5; i++) {
        sorted_ranks0[i] = get_rank(hand0[i]) - 1;
        sorted_ranks1[i] = get_rank(hand1[i]) - 1;
    }
    qsort(sorted_ranks0, 5, sizeof(int), (int (*)(const void*, const void*))strcmp);
    qsort(sorted_ranks1, 5, sizeof(int), (int (*)(const void*, const void*))strcmp);
    
    int flush0 = is_flush(hand0);
    int flush1 = is_flush(hand1);
    int straight0 = is_straight(sorted_ranks0);
    int straight1 = is_straight(sorted_ranks1);
    
    // Check for royal flush
    if (flush0 && straight0 && sorted_ranks0[0] == 12) {
        if (!(flush1 && straight1 && sorted_ranks1[0] == 12)) return 0;  // hand0 wins
    }
    if (flush1 && straight1 && sorted_ranks1[0] == 12) return 1;  // hand1 wins
    
    // Check for straight flush
    if (flush0 && straight0) {
        if (!(flush1 && straight1)) return 0;  // hand0 wins
        // Both are straight flushes, compare high cards
        return sorted_ranks1[0] > sorted_ranks0[0] ? 1 : 0;
    }
    if (flush1 && straight1) return 1;  // hand1 wins
    
    // Check for four of a kind
    for (int i = 0; i < RANKS; i++) {
        if (rank_counts0[i] == 4) {
            if (rank_counts1[i] == 4) {
                // Both have four of a kind, compare kickers
                return sorted_ranks1[4] > sorted_ranks0[4] ? 1 : 0;
            }
            return 0;  // hand0 wins
        }
        if (rank_counts1[i] == 4) return 1;  // hand1 wins
    }
    
    // Check for full house
    int three0 = -1, three1 = -1, two0 = -1, two1 = -1;
    for (int i = 0; i < RANKS; i++) {
        if (rank_counts0[i] == 3) three0 = i;
        if (rank_counts1[i] == 3) three1 = i;
        if (rank_counts0[i] == 2) two0 = i;
        if (rank_counts1[i] == 2) two1 = i;
    }
    if (three0 != -1 && two0 != -1) {
        if (three1 != -1 && two1 != -1) {
            // Both have full house, compare three of a kind
            return three1 > three0 ? 1 : 0;
        }
        return 0;  // hand0 wins
    }
    if (three1 != -1 && two1 != -1) return 1;  // hand1 wins
    
    // Check for flush
    if (flush0) {
        if (!flush1) return 0;  // hand0 wins
        // Both are flushes, compare high cards
        for (int i = 0; i < 5; i++) {
            if (sorted_ranks1[i] > sorted_ranks0[i]) return 1;
            if (sorted_ranks0[i] > sorted_ranks1[i]) return 0;
        }
        // If we get here, the hands are identical
        return compare_suits(hand0, hand1);
    }
    if (flush1) return 1;  // hand1 wins
    
    // Check for straight
    if (straight0) {
        if (!straight1) return 0;  // hand0 wins
        // Both are straights, compare high cards
        return sorted_ranks1[0] > sorted_ranks0[0] ? 1 : 0;
    }
    if (straight1) return 1;  // hand1 wins
    
    // Check for three of a kind
    if (three0 != -1) {
        if (three1 == -1) return 0;  // hand0 wins
        // Both have three of a kind, compare ranks
        if (three1 > three0) return 1;
        if (three0 > three1) return 0;
        // If three of a kind are equal, compare remaining cards
        for (int i = 3; i < 5; i++) {
            if (sorted_ranks1[i] > sorted_ranks0[i]) return 1;
            if (sorted_ranks0[i] > sorted_ranks1[i]) return 0;
        }
        // If we get here, the hands are identical
        return compare_suits(hand0, hand1);
    }
    if (three1 != -1) return 1;  // hand1 wins
    
    // Check for two pair
    if (two0 != -1) {
        int second_pair0 = -1, second_pair1 = -1;
        for (int i = 0; i < RANKS; i++) {
            if (i != two0 && rank_counts0[i] == 2) second_pair0 = i;
            if (i != two1 && rank_counts1[i] == 2) second_pair1 = i;
        }
        if (second_pair0 != -1) {
            if (second_pair1 == -1) return 0;  // hand0 wins
            // Both have two pair, compare higher pairs
            int high_pair0 = two0 > second_pair0 ? two0 : second_pair0;
            int high_pair1 = two1 > second_pair1 ? two1 : second_pair1;
            if (high_pair1 > high_pair0) return 1;
            if (high_pair0 > high_pair1) return 0;
            // If higher pairs are equal, compare lower pairs
            int low_pair0 = two0 < second_pair0 ? two0 : second_pair0;
            int low_pair1 = two1 < second_pair1 ? two1 : second_pair1;
            if (low_pair1 > low_pair0) return 1;
            if (low_pair0 > low_pair1) return 0;
            // If both pairs are equal, compare kickers
            for (int i = 4; i < 5; i++) {
                if (sorted_ranks1[i] > sorted_ranks0[i]) return 1;
                if (sorted_ranks0[i] > sorted_ranks1[i]) return 0;
            }
            // If we get here, the hands are identical
            return compare_suits(hand0, hand1);
        }
        // hand0 has one pair
        if (two1 != -1) {
            // hand1 also has one pair, compare pair ranks
            if (two1 > two0) return 1;
            if (two0 > two1) return 0;
            // If pairs are equal, compare remaining cards
            for (int i = 2; i < 5; i++) {
                if (sorted_ranks1[i] > sorted_ranks0[i]) return 1;
                if (sorted_ranks0[i] > sorted_ranks1[i]) return 0;
            }
            // If we get here, the hands are identical
            return compare_suits(hand0, hand1);
        }
        return 0;  // hand0 wins (one pair vs high card)
    }
    if (two1 != -1) return 1;  // hand1 wins (one pair vs high card)
    
    // High card
    for (int i = 0; i < 5; i++) {
        if (sorted_ranks1[i] > sorted_ranks0[i]) return 1;
        if (sorted_ranks0[i] > sorted_ranks1[i]) return 0;
    }
    
    // If we get here, the hands are identical
    return compare_suits(hand0, hand1);
}

// Helper function to compare suits when ranks are identical
int compare_suits(int* hand0, int* hand1) {
    int max_suit0 = 0, max_suit1 = 0;
    for (int i = 0; i < 5; i++) {
        int suit0 = get_suit(hand0[i]);
        int suit1 = get_suit(hand1[i]);
        if (suit0 > max_suit0) max_suit0 = suit0;
        if (suit1 > max_suit1) max_suit1 = suit1;
    }
    return max_suit1 > max_suit0 ? 1 : 0;
}