//This program implements a decision tree learning algorithm for the car evaluation task in HW1 part 2 problem 2.
//Users can select information gain, majority error, or gini index, and optionally set a maximum depth from 1 to 6.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
| label values

unacc, acc, good, vgood             0,1,2,3

| attributes

buying:   vhigh, high, med, low.    0,1,2,3
maint:    vhigh, high, med, low.    0,1,2,3
doors:    2, 3, 4, 5more.           0,1,2,3
persons:  2, 4, more.               0,1,2
lug_boot: small, med, big.          0,1,2
safety:   low, med, high.           0,1,2

| columns
buying,maint,doors,persons,lug_boot,safety,label

*/


/* 
   | label values

    yes no             0,1

| attributes

outlook:        sunny, overcast, rainy      0,1,2
temperature:    hot, medium, cool           0,1,2
humidity:       high, normal, low           0,1,2
wind:           strong, weak                0,1

| columns
outlook,temperature,humidity,wind,play 
*/

struct instance {
    /*int buying;
    int maint;
    int doors;
    int persons;
    int lug_boot;
    int safety;
    int label;*/
    int outlook;
    int temperature;
    int humidity;
    int wind;
    int play;
};

int attributeToInt(char* attribute)
{
    if (!strcmp(attribute, "sunny") || !strcmp(attribute, "hot") || !strcmp(attribute, "high") || !strcmp(attribute, "strong") || !strcmp(attribute, "yes"))
        return 0;
    else if (!strcmp(attribute, "overcast") || !strcmp(attribute, "medium") || !strcmp(attribute, "normal") || !strcmp(attribute, "weak") || !strcmp(attribute, "no"))
        return 1;
    else if (!strcmp(attribute, "rainy") || !strcmp(attribute, "cold") || !strcmp(attribute, "low"))
        return 2;
    else
        return -1;
}

int main()
{
    int numInstances = 0;   //Number of instances in dataset
    
    //Open input file
    FILE *inputFile = fopen("tennis.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char row[50];
    char *token;

    //Count number of instances in input file
    while (feof(inputFile) != true)
    {
        fgets(row, 50, inputFile);
        numInstances++;
    }
    rewind(inputFile);
    
    //Create array of instance structs
    struct instance data[numInstances];
    //int data[numInstances][5];
    /*
        data[0] - outlook
        data[1] - temperature
        data[2] - humidity
        data[3] - wind
        data[4] - play
    */
    int dataIndex = 0;

    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        fgets(row, 50, inputFile);
        token = strtok(row, ",");
        data[dataIndex].outlook = attributeToInt(token);
        token = strtok(NULL, ",");
        data[dataIndex].temperature = attributeToInt(token);
        token = strtok(NULL, ",");
        data[dataIndex].humidity = attributeToInt(token);
        token = strtok(NULL, ",");
        data[dataIndex].wind = attributeToInt(token);
        token = strtok(NULL, ",\r\n");
        data[dataIndex].play = attributeToInt(token);
        dataIndex++;
    }

    fclose(inputFile);
    dataIndex = 0;

    //Ask user for method
    char userInput;
    bool userInputValid;
    int method;
    do 
    {
        printf("Select attribute split method:\n\tInformation gain (I)\n\tMajority error (M)\n\tGini index (G)\n\n");
        scanf("%s", &userInput);
        printf("\n");
        switch (userInput)
        {
            case 'I':
                userInputValid = true;
                method = 0;
                break;
            case 'M':
                userInputValid = true;
                method = 1;
                break;
            case 'G':
                userInputValid = true;
                method = 2;
                break;
            default:
                printf("Invalid selection\n\n");
                userInputValid = false;
        }
    } while (!userInputValid);

    //Ask user for max depth
    int depth;
    do 
    {
        printf("Select maximum tree depth (1-6)\n\n");
        scanf("%s", &userInput);
        printf("\n");
        if (userInput >= '1' && userInput <= '6')
        {
            userInputValid = true;
            depth = userInput-48;
        }
        else
        {
            printf("Invalid selection\n\n");
            userInputValid = false;
        }
    } while (!userInputValid);

    //Run algorithm to construct tree
    
}