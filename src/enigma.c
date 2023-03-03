#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>


// DEFINING ERROR CODES/CONSTANTS
// GENERAL
#define NOT_SET 0
#define SET 1
// PLUGBOARD
#define PLUGBOARD_TOO_MANY_CONNECTIONS 1
#define PLUGBOARD_SINGLE_PLUG 2
#define PLUGBOARD_MULTIPLE_MAPPINGS 3
// ROTOR
#define TOO_FEW_ROTORS 2
#define TOO_MANY_ROTORS 3
#define UNKNOWN_ROTOR 4
// SETTINGS
#define INVALID_SETTING 2
#define WRONG_NUM_SETTINGS 3
// ENIGMA


// DEFINING CONSTANTS
#define MINIMUM_ROTORS 3
#define MAXIMUM_ROTORS 8
#define TERM_WIDTH 80
#define MSGLENGTH 2048
#define BUFFSZ 1024


typedef struct{
	char name[BUFFSZ];
	int mapping[26];
} reflector_t;

typedef struct{
	int number;
	int position;
	int turnover;
	int mapping[26];
	int rev_mapping[26];
} rotor_t;

typedef struct{
	int pairings[26];
} plugboard_t;

typedef struct{
	// rotors[0] on right, rotors[length-1] on left, visually
	int num_rotors;
	rotor_t* rotors;
	reflector_t reflector;
	plugboard_t plugboard;
} enigma_t;


void apply_mapping(int* map, char* mapping) {
	char A = 'A';
	for (int i=0; i< 26; i++) {
		map[i] = mapping[i]-A;
	}
	return;
}


void reverse_mapping(int* rev_map, int* forward_map) {
	for (int i=0; i<26; i++) {
		rev_map[forward_map[i]] = i;
	}
	return;
}


void rotor_setup(rotor_t** rotors, int* num_rotors) {
	/*
	Setups up the necessary rotors, add more if you want... or make your own.
	*/

	char A = 'A'; // Helpful for char math
	// Rotor Setup
	//// Rotor I
	rotor_t I;
	I.number = 1;
	I.position = 0;
	I.turnover = 'R'-A;
	char I_mapping[] = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
	apply_mapping(I.mapping,I_mapping);
	reverse_mapping(I.rev_mapping,I.mapping);

	//// Rotor II
	rotor_t II;
	II.number = 2;
	II.position = 0;
	II.turnover = 'F'-A;
	char II_mapping[] = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
	apply_mapping(II.mapping,II_mapping);
	reverse_mapping(II.rev_mapping,II.mapping);

	//// Rotor III
	rotor_t III;
	III.number = 3;
	III.position = 0;
	III.turnover = 'W'-A;
	char III_mapping[] = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
	apply_mapping(III.mapping,III_mapping);
	reverse_mapping(III.rev_mapping,III.mapping);

	//// Rotor IV
	rotor_t IV;
	IV.number = 4;
	IV.position = 0;
	IV.turnover = 'K'-A;
	char IV_mapping[] = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
	apply_mapping(IV.mapping,IV_mapping);
	reverse_mapping(IV.rev_mapping,IV.mapping);

	//// Rotor V
	rotor_t V;
	V.number = 5;
	V.position = 0;
	V.turnover = 'A'-A;
	char V_mapping[] = "VZBRGITYUPSDNHLXAWMJQOFECK";
	apply_mapping(V.mapping,V_mapping);
	reverse_mapping(V.rev_mapping,V.mapping);



	// However many rotors I've actually bothered to fill out at this point
	*num_rotors = 5;

	// Pack Up Rotors
	*rotors = realloc(*rotors,*num_rotors*sizeof(rotor_t));
	(*rotors)[0] = I;
	(*rotors)[1] = II;
	(*rotors)[2] = III;
	(*rotors)[3] = IV;
	(*rotors)[4] = V;

	return;
}


void reflector_setup(reflector_t** reflectors, int* num_reflectors) {

	// Reflector A
	reflector_t RefA;
	strcpy(RefA.name,"A");
	char RefA_mapping[] = "EJMZALYXVBWFCRQUONTSPIKHGD";
	apply_mapping(RefA.mapping,RefA_mapping);

	// Reflector B
	reflector_t RefB;
	strcpy(RefB.name,"B");
	char RefB_mapping[] = "YRUHQSLDPXNGOKMIEBFZCWVJAT";
	apply_mapping(RefB.mapping,RefB_mapping);

	// Reflector C
	reflector_t RefC;
	strcpy(RefC.name,"C");
	char RefC_mapping[] = "FVPJIAOYEDRZXWGCTKUQSBNMHL";
	apply_mapping(RefC.mapping,RefC_mapping);

	// Reflector UKW
	reflector_t RefUKW;
	strcpy(RefUKW.name,"UKW");
	char RefUKW_mapping[] = "QYHOGNECVPUZTFDJAXWMKISRBL";
	apply_mapping(RefUKW.mapping,RefUKW_mapping);


	*num_reflectors = 4;

	*reflectors = realloc(*reflectors,*num_reflectors*sizeof(reflector_t));
	(*reflectors)[0] = RefA;
	(*reflectors)[1] = RefB;
	(*reflectors)[2] = RefC;
	(*reflectors)[3] = RefUKW;

	return;
}


void plugboard_initialize(plugboard_t* plugboard, int mapping[26]) {
	for (int i=0; i<26; i++) {
		plugboard->pairings[i] = mapping[i];
	}
	return;
}


int get_char(enigma_t* enigma, int input) {
	// plugboard BS
	input = enigma->plugboard.pairings[input];
	// Rotor BS
	int advance = 1;
	for (int i=0; i<enigma->num_rotors; i++) {
		if (advance == 1) {
			enigma->rotors[i].position++;
			enigma->rotors[i].position = enigma->rotors[i].position%26;
			if (enigma->rotors[i].turnover != enigma->rotors[i].position) {
				advance = 0;
			}
		}
	}
	// Now for Transmission
	for (int i=0; i<enigma->num_rotors; i++) {
		input = (input + enigma->rotors[i].position)%26;
		input = enigma->rotors[i].mapping[input];
		input = (26 + input - enigma->rotors[i].position)%26;
	}
	input = enigma->reflector.mapping[input];
	for (int i=enigma->num_rotors; i>0; i--) {
		input = (input + enigma->rotors[i-1].position)%26;
		input = enigma->rotors[i-1].rev_mapping[input];
		input = (26 + input - enigma->rotors[i-1].position)%26;
	}
	// plugboard again
	input = enigma->plugboard.pairings[input];
	return input;
}


int enter_plug_input(plugboard_t* plugboard, char* buffer) {
	int pair[2];
	char buffcpy[BUFFSZ];
	int found_chars;
	char* pair_str;
	char A = 'A';
	char pairings[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25}; // Keeps from affecting plugboard on failure

	strcpy(buffcpy,buffer);
	pair_str = strtok(buffcpy,",");
	// Check for completeness
	while (pair_str != NULL) {
		found_chars = 0;
		for (int i=0; i<strlen(pair_str); i++) {
			if (isalpha(pair_str[i])) {
				if (found_chars == 2) {
					return PLUGBOARD_TOO_MANY_CONNECTIONS;
				}
				if (islower(pair_str[i])) {
					pair_str[i] = toupper(pair_str[i]);
				}
				pair[found_chars] = pair_str[i]-A;
				found_chars++;
			}
		}
		if (found_chars == 2) {
			if (pair[0] == pair[1]) return PLUGBOARD_SINGLE_PLUG;
			for (int i=0; i<2; i++) {
				if (pairings[pair[i]] != pair[i]) {
					return PLUGBOARD_MULTIPLE_MAPPINGS;
				}
			}
			pairings[pair[0]] = pair[1];
			pairings[pair[1]] = pair[0];
		} else {
			return PLUGBOARD_SINGLE_PLUG;
		}
		pair_str = strtok(NULL,",");
	}
	// If we got here, we should succeed
	pair_str = strtok(buffer,",");
	while (pair_str != NULL) {
		found_chars = 0;
		for (int i=0; i<strlen(pair_str); i++) {
			if (isalpha(pair_str[i])) {
				if (islower(pair_str[i])) {
					pair_str[i] = toupper(pair_str[i]);
				}
				pair[found_chars] = pair_str[i]-A;
				found_chars++;
			}
		}
		plugboard->pairings[pair[0]] = pair[1];
		plugboard->pairings[pair[1]] = pair[0];
		pair_str = strtok(NULL,",");
	}

	return SET;
}


int read_plug_input(plugboard_t* plugboard, int PLUGBOARD) {
	/*
	Reads Pairs from the User and Modified Plugboard accordingly
	*/
	char buffer[BUFFSZ];
	while (PLUGBOARD != SET) {
		printf("ENTER ALL PLUGBOARD PAIRINGS, SEPARATED BY COMMAS:\n");
		fgets(buffer,BUFFSZ,stdin);
		PLUGBOARD = enter_plug_input(plugboard,buffer); 
	}
	
	return EXIT_SUCCESS;
}


int enter_rotor_input(enigma_t* enigma, rotor_t* rotors, int num_rotors, char* buffer) {
	char* rotor_str;
	int num_selected, rotor_select, rotor_found;
	int chosen_rotors[MAXIMUM_ROTORS];
	rotor_str = strtok(buffer,",");
	num_selected = 0;
	while (rotor_str != NULL) {
		rotor_found = 0;
		rotor_select = atoi(rotor_str);
		for (int i=0; i<num_rotors; i++) {
			if (rotor_select == rotors[i].number) {
				if (num_selected > MAXIMUM_ROTORS) {
					printf("TOO MANY ROTORS SELECTED: PLEASE ENTER FEWER THAN %d ROTORS\n.",MAXIMUM_ROTORS);
					return TOO_MANY_ROTORS;
				}
				for (int j=MAXIMUM_ROTORS-1; j>0; j--) {
					chosen_rotors[j] = chosen_rotors[j-1];
				}
				chosen_rotors[0] = i;
				rotor_found = 1;
				break;
			}
		}
		if (rotor_found != 1) {
			printf("ONE OF THE ENTERED ROTORS WAS NOT FOUND, TRY AGAIN.\n");
			return UNKNOWN_ROTOR;
		}
		num_selected++;
		rotor_str = strtok(NULL,",");
	}
	if (num_selected<MINIMUM_ROTORS) {
		return TOO_FEW_ROTORS;
	}
	enigma->num_rotors = num_selected;
	enigma->rotors = realloc(enigma->rotors,num_selected*sizeof(rotor_t));
	for (int i=0; i<num_selected; i++) {
		enigma->rotors[i] = rotors[chosen_rotors[i]];
		#ifdef DEBUG
			printf("ROTOR %d: %d\n",i,enigma->rotors[i].number);
		#endif
	}

	return SET;
}

int enter_setting_input(enigma_t* enigma, rotor_t* rotors, int num_rotors, char* buffer) {
	char* rotor_str;
	int num_selected,setting_found;
	rotor_str = strtok(buffer,",");
	num_selected = 0;
	while (rotor_str != NULL) {
		setting_found = 0;
		for (int i=0; i<strlen(rotor_str); i++) {
			if isalpha(rotor_str[i]) {
				if (setting_found == 1) {
					return INVALID_SETTING;
				}
				enigma->rotors[enigma->num_rotors-1-num_selected].position = toupper(rotor_str[i]) - 'A';
				setting_found++;
			}
		}
		if (setting_found == 0) {
			return INVALID_SETTING;
		}
		rotor_str = strtok(NULL,",");
		num_selected++;
	}
	if (num_selected != enigma->num_rotors) {
		return WRONG_NUM_SETTINGS;
	}
	return SET;
}


void read_rotor_input(enigma_t* enigma, rotor_t* rotors, int num_rotors, int ROTOR, int SETTINGS) {
	char buffer[BUFFSZ];

	while (!(ROTOR == SET)) {
		printf("AVAILABLE ROTORS: ");
		for (int i=0; i<num_rotors; i++) {
			printf("%d ",rotors[i].number);
		}
		printf("\nENTER DESIRED ROTORS FROM LEFT TO RIGHT, SEPARATED BY COMMAS:\n");
		fgets(buffer,BUFFSZ,stdin);
		ROTOR= enter_rotor_input(enigma,rotors,num_rotors,buffer);
		if (ROTOR == SET) {continue;}
		// PRINT ERRORS
	}
	
	while (!(SETTINGS == SET)) {
		printf("FOR EACH OF THE %d ROTORS, SELECT THE STARTING LETTER, SEPARATED BY COMMAS:\n",enigma->num_rotors);
		fgets(buffer,BUFFSZ,stdin);
		SETTINGS = enter_setting_input(enigma,rotors,num_rotors,buffer);
		if (SETTINGS == SET) {continue;}
		// PRINT ERRORS
	}

	return;
}


int enter_reflector_input(enigma_t* enigma, reflector_t* reflectors, int num_reflectors, char* buffer) {
	int found = 0;
	int chosen;
	for (int i=0; i<BUFFSZ; i++) {
		buffer[i] = toupper(buffer[i]);
		if (buffer[i] == '\0') break;
	}
	for (int i=0; i<num_reflectors; i++) {
		if (strncmp(reflectors[i].name,buffer,strlen(reflectors[i].name)) == 0) {
			found = 1;
			chosen = i;
			break;
		}
	}
	if (found == 0) return 0;
	enigma->reflector = reflectors[chosen];
	return 1;
}


void read_reflector_input(enigma_t* enigma, reflector_t* reflectors, int num_reflectors) {
	char buffer[BUFFSZ];
	int found;
	printf("AVAILABLE REFLECTORS:");
	for (int i=0; i<num_reflectors; i++) {
		printf(" %s",reflectors[i].name);
	}
	printf("\n");
	found = 0;
	while (found == 0) {
		printf("CHOOSE ONE (1) REFLECTOR: ");
		fgets(buffer,BUFFSZ,stdin);
		found = enter_reflector_input(enigma,reflectors,num_reflectors,buffer);
		if (found == 0) {
			printf("ERROR: SELECTION NOT RECOGNIZED.\n");
		}
	}

	return;
}


enigma_t enigma_initialize() {
	enigma_t enigma;
	enigma.rotors = calloc(1,sizeof(rotor_t));
	return enigma;
}


void display_enigma(enigma_t enigma) {
	return;
}


void help() {
	printf("Usage: enigma (OPTIONAL)[FLAG,FLAG SETTING]\n\n");
	printf("\t-P\t\tenter plugboard pairs in CSL\n");
	printf("\t-R\t\tenter rotors(L->R) in CSL\n");
	printf("\t-L\t\tenter reflector choice\n");

	printf("\n");
}


int main(int argc, char** argv) {
	// SETUP
	int ROTOR, PLUGBOARD, REFLECTOR, SETTINGS, VERBOSE;
	int num_rotors, num_reflectors;
	//char buffer[1024];
	char msg[2048];
	reflector_t* reflectors = calloc(1,sizeof(reflector_t));
	rotor_t* rotors = calloc(1,sizeof(rotor_t));
	int alphabet[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
	char A = 'A';
	VERBOSE=NOT_SET;
	ROTOR=NOT_SET;
	PLUGBOARD=NOT_SET;
	REFLECTOR=NOT_SET;
	SETTINGS=NOT_SET;
	enigma_t enigma = enigma_initialize();
	rotor_setup(&rotors, &num_rotors);
	plugboard_initialize(&(enigma.plugboard),alphabet);
	reflector_setup(&reflectors,&num_reflectors);

	// ARGS
	for (int i=1; i<argc; i++) {
		if (strcmp("v",argv[i])==0) {
			printf("VERBOSE\n");
			VERBOSE=SET;
		} else if (strcmp("-P",argv[i])==0) {
			i++;
			PLUGBOARD = enter_plug_input((&enigma.plugboard), argv[i]);
			if (PLUGBOARD == SET) {
				printf("PLUGBOARD ACCEPTED\n");
			} else {
				printf("PLUGBOARD ERROR: MANUAL ENTRY REQUIRED.\n");
			}
		} else if (strcmp("-R",argv[i])==0) {
			i++;
			ROTOR = enter_rotor_input(&enigma,rotors,num_rotors,argv[i]);
			if (ROTOR == SET) {
				printf("ROTORS ACCEPTED\n");
			} else {
				printf("ROTOR ERROR: MANUAL ENTRY REQUIRED.\n");
			}
		} else if (strcmp("-S",argv[i])==0) {
			i++;

			SETTINGS = enter_setting_input(&enigma,rotors,num_rotors,argv[i]);
			if (SETTINGS == SET) {
				printf("SETTINGS ACCEPTED\n");
			} else {
				printf("SETTINGS ERROR: MANUAL ENTRY REQUIRED.\n");
			}
		} else if (strcmp("-L",argv[i])==0) {
			i++;
			REFLECTOR = enter_reflector_input(&enigma,reflectors,num_reflectors,argv[i]);
			if (REFLECTOR == SET) {
				printf("REFLECTOR ACCEPTED\n");
			} else {
				printf("REFLECTOR ERROR: MANUAL ENTRY REQUIRED.\n");
			}
		}else {
			help();
			free(rotors);
			free(reflectors);
			free(enigma.rotors);
			exit(0);
		}
	}
	
	// MANUAL ENTRY
	if (!(ROTOR==SET) || !(SETTINGS==SET)) {
		read_rotor_input(&enigma,rotors, num_rotors,ROTOR,SETTINGS);
	}
	if (!(PLUGBOARD==SET)) {
		read_plug_input(&(enigma.plugboard),PLUGBOARD);
	}
	if (!(REFLECTOR==SET)) {
		read_reflector_input(&enigma,reflectors,num_reflectors);
	}

	#ifdef DEBUG
		// TEST -- SUCCESS
		for (int i=0; i<26; i++) {
			printf("%c -> %c\n",A+i,enigma.plugboard.pairings[i]+A);
		}
		printf("Selected: %s\n",enigma.reflector.name);
	#endif

	if (VERBOSE) {
		for (int i=0; i<enigma.num_rotors; i++) {
			printf("ROTOR %d: NUMBER: %d, POSITION: %c (%d)\n",i,enigma.rotors[i].number,enigma.rotors[i].position+A,enigma.rotors[i].position);
		}
	}
	
	// ENTER MSG
	printf("ENTER MESSAGE: ");
	fgets(msg,MSGLENGTH,stdin);
	printf("ENCODED MESSAGE: ");
	for (int i=0; i<strlen(msg); i++) {
		if (isalpha(msg[i])) {
			printf("%c",get_char(&enigma,toupper(msg[i])-A)+A);
		}
	}
	printf("\n");

	free(enigma.rotors);
	free(rotors);
	free(reflectors);

	return EXIT_SUCCESS;
}
