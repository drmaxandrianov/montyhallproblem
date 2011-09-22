#include <stdlib.h>
#include <stdio.h>

#define NUMBER_OF_ITER 9999999
#define CHANGE_CHOICE 1

/*
 * Участники задачи: игрок и ведущий.
 * Описание задачи: ведущий предлагает игроку выбрать одну из трех дверей.
 * За одной из них находится автомобиль, за другими - самокаты. Игрок
 * выбирает какую-то дверь. После чего ведущий открывает другую дверь и
 * за ней оказывается самокат. Ведущий предлагает игроку изменить свой
 * выбор. Игрок его меняет, если CHANGE_CHOICE == 1, иначе настаивает на
 * прежнем выборе.
 * Задача: определить вероятность выиграша в зависимости от CHANGE_CHOICE.
 * В фильме "21" говориться, что, если поменять свой выбор, то вероятность
 * победы будет 0.66, иначе 0.33.
 * Проверим это!
 */

enum DoorState {OPEN, CLOSE};
enum Prize {CAR, SCOOTER};

struct Door {
	enum DoorState doorState;
	enum Prize prize;
};

struct Iteration {
	struct Door *door_1;
	struct Door *door_2;
	struct Door *door_3;
	struct Door *selected_door;
	struct Door *compromised_door;
};

struct Iteration experiment[NUMBER_OF_ITER];

void fillExperiment(struct Iteration[]);
void fillRandomSelectedDoor(struct Iteration[]);
void fillCompromisedDoor(struct Iteration[]);
int finalDoorSelection(struct Iteration[]);
int getRandomDoorState();
struct Door* getRandomDoor();
void printExperiment(struct Iteration[]);

int main() {
	printf("--- Experiment settings ---\n");
	printf("Number of iterations: %i\n", NUMBER_OF_ITER);
	printf("Will guy change his choice: %s\n", (CHANGE_CHOICE == 1) ? "Yes" : "No");
	printf("\n");
	printf("--- Starting ---\n");
	printf("* Filling random data in empty experiment...\n");
	fillExperiment(experiment);
	printf("* Selecting random doors...\n");
	fillRandomSelectedDoor(experiment);
	printf("* Compromising random door from 2 left...\n");
	fillCompromisedDoor(experiment);
	printf("* Making the final door selection...\n");
	int totalLucks = finalDoorSelection(experiment);
	printf("--- Finishing ---\n\n");
	printf("Total percentage of lucks: %f\n", totalLucks / (double) NUMBER_OF_ITER);
	
	// Uncomment the folowing line to see the experiment details
	// printExperiment(experiment);
	
	return 0;
}

void fillExperiment(struct Iteration experiment[]) {
	int i;
	for (i = 0; i < NUMBER_OF_ITER; i++) {
		experiment[i].door_1 = getRandomDoor();
		experiment[i].door_1->prize = SCOOTER;
		experiment[i].door_2 = getRandomDoor();
		experiment[i].door_2->prize = SCOOTER;
		experiment[i].door_3 = getRandomDoor();
		experiment[i].door_3->prize = SCOOTER;
		switch (rand() % 3) {
			case 0:
				experiment[i].door_1->prize = CAR;
				break;
			case 1:
				experiment[i].door_2->prize = CAR;
				break;
			case 2:
				experiment[i].door_3->prize = CAR;
				break;
		}
	}
}

void fillRandomSelectedDoor(struct Iteration experiment[]) {
	int i;
	for (i = 0; i < NUMBER_OF_ITER; i++) {
		switch (rand() % 3) {
			case 0:
				experiment[i].selected_door = experiment[i].door_1;
				experiment[i].door_1->doorState = OPEN;
				break;
			case 1:
				experiment[i].selected_door = experiment[i].door_2;
				experiment[i].door_2->doorState = OPEN;
				break;
			case 2:
				experiment[i].selected_door = experiment[i].door_3;
				experiment[i].door_3->doorState = OPEN;
				break;
		}
	}
}

void fillCompromisedDoor(struct Iteration experiment[]) {
	int i, j;
	for (i = 0; i < NUMBER_OF_ITER; i++) {
		// Define the array for easy interation with doors
		struct Door **doors = malloc(sizeof(struct Door));
		doors[0] = experiment[i].door_1;
		doors[1] = experiment[i].door_2;
		doors[2] = experiment[i].door_3;
		
		// Now select the compromised door
		// Select door with CAR
		struct Door *doorWithCar;
		for (j = 0; j < 3; j++) {
			if (doors[j]->prize == CAR)
				doorWithCar = doors[j];
		}
		
		// Select chousen by hand door
		struct Door *doorChousenByHand;
		for (j = 0; j < 3; j++) {
			if (doors[j]->doorState == OPEN)
				doorChousenByHand = doors[j];
		}
		
		// Open the left door
		struct Door* comprDoor;
		if (doorChousenByHand == doorWithCar) {
			if (doors[0] == doorChousenByHand)
				comprDoor = doors[1];
			else
				comprDoor = doors[0];
		} else {
			for (j = 0; j < 3; j++) {
				if (doors[j] != doorChousenByHand &&
					doors[j] != doorWithCar)
					comprDoor = doors[j];
			}
		}
		
		// Set compromised state to this door
		comprDoor->doorState = OPEN;
		experiment[i].compromised_door = comprDoor;
	}
}

int finalDoorSelection(struct Iteration experiment[]) {
	int i, lucks = 0;
	for (i = 0; i < NUMBER_OF_ITER; i++) {
		if (CHANGE_CHOICE == 1) {
			// We will change the door selection
			if (experiment[i].door_1->doorState == CLOSE) {
				if (experiment[i].door_1->prize == CAR)
					lucks++;
			} else if (experiment[i].door_2->doorState == CLOSE) {
				if (experiment[i].door_2->prize == CAR)
					lucks++;
			} else {
				if (experiment[i].door_3->prize == CAR)
					lucks++;
			}
		} else {
			// We will not change the door selection
			if (experiment[i].selected_door->prize == CAR)
				lucks++;
		}
	}
	return lucks;
}

int getRandomDoorState() {
	int rnd = rand() % 2;
	if (rnd == 0) return OPEN;
	else return CLOSE;
}

struct Door* getRandomDoor() {
	struct Door *door = malloc(sizeof(struct Door));
	door->doorState = CLOSE;
	return door;
}

void printExperiment(struct Iteration experiment[]) {
	int i;
	for (i = 0; i < NUMBER_OF_ITER; i++) {
		printf("\nIteration #%i\n", i);
		printf("Door 1 - %s - %s\n", (experiment[i].door_1->prize == CAR) ? "CAR" : "SCOOTER",
			(experiment[i].door_1->doorState == CLOSE) ? "CLOSED" : 
				(experiment[i].door_1 == experiment[i].compromised_door) ? "COMPROMISED" : "OPEN BY HAND");
		printf("Door 2 - %s - %s\n", (experiment[i].door_2->prize == CAR) ? "CAR" : "SCOOTER",
			(experiment[i].door_2->doorState == CLOSE) ? "CLOSED" : 
				(experiment[i].door_2 == experiment[i].compromised_door) ? "COMPROMISED" : "OPEN BY HAND");
		printf("Door 3 - %s - %s\n", (experiment[i].door_3->prize == CAR) ? "CAR" : "SCOOTER",
			(experiment[i].door_3->doorState == CLOSE) ? "CLOSED" : 
				(experiment[i].door_3 == experiment[i].compromised_door) ? "COMPROMISED" : "OPEN BY HAND");
	}
}
