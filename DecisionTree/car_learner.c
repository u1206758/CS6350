//This program implements a decision tree learning algorithm for the car evaluation task in HW1 part 2 problem 2.
//Users can select information gain, majority error, or gini index, and optionally set a maximum depth from 1 to 6.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUM_ATTRIBUTES 4

int numInstances = 0;   //Number of instances in dataset
int numValues[4] = {3, 3, 3, 2};  //Number of values per attribute
int method;

int splitLeaf(int currentInstances[numInstances], int data[numInstances][NUM_ATTRIBUTES+1]);
float ig_initial(int subset[numInstances], int dataset[numInstances][NUM_ATTRIBUTES+1]);
float ig_gain(int subset[numInstances], int dataset[numInstances][NUM_ATTRIBUTES+1], int attribute);
int attributeToInt(char* attribute);

struct Branch
{
    int attribute;
    struct Branch *parent;
    int numLeaves;
    struct Branch *leaf[3];
    int totalCount;
    int yesCount;
    int noCount;
    int label;
};

int main()
{   
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
    //struct instance data[numInstances];
    int data[numInstances][NUM_ATTRIBUTES+1];
    /*
        data[inst][0] - outlook
            sunny - 0
            overcast - 1
            rainy - 2
        data[]inst[1] - temperature
            hot - 0
            medium - 1
            cool - 2
        data[inst][2] - humidity
            high - 0
            normal - 1
            low - 2
        data[inst][3] - wind
            strong - 0
            weak - 1
        data[inst][4] - play
            no - 0
            yes - 1
    */
    int dataIndex = 0;

    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        fgets(row, 50, inputFile);
        token = strtok(row, ",");
        for (int i = 0; i < NUM_ATTRIBUTES+1; i++)
        {
            data[dataIndex][i] = attributeToInt(token);
            token = strtok(NULL, ",\r\n");
        }
        dataIndex++;
    }

    fclose(inputFile);
    dataIndex = 0;

    //Ask user for method
    char userInput;
    bool userInputValid;
    //int method;
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
    int maxDepth;
    do 
    {
        printf("Select maximum tree depth (1-6)\n\n");
        scanf("%s", &userInput);
        printf("\n");
        if (userInput >= '1' && userInput <= '6')
        {
            userInputValid = true;
            maxDepth = userInput-48;
        }
        else
        {
            printf("Invalid selection\n\n");
            userInputValid = false;
        }
    } while (!userInputValid);

    //Run algorithm to construct tree

    //Initialize instance sets
    int currentInstances[numInstances];  //This tracks the subset of instances at each evaluation
    for (int i = 0; i < numInstances; i++)
    {
        currentInstances[i] = i;
    }


    //In some set of instances which starts as all
        //Find original rating of labels
        //for each attribute
            //for each value of that attribute
                //calculate intermediate for all values
            //calculate weighted attribute value
        //determine which attribute is best to split
        //track which attribute was split on
        //determine subset of instances for each leaf of attribute split
        //if they all have same label, next leaf
            //if all leaves are one label, next waiting attribute split
        //repeat

    



//THIS WILL PRINT THE READ IN DATASET CONVERTED TO INTEGERS
    /*
    for (int i = 0; i < numInstances; i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
            printf("%d, ", data[i][j]);
        printf("\n");
    }
    */

}


//Calculate entropy on labels for current set of instances
float ig_initial(int subset[numInstances], int dataset[numInstances][NUM_ATTRIBUTES+1])
{
    float totalCount = 0;
    float yesCount = 0;
    float noCount = 0;
    for (int i = 0; i < numInstances; i++)
    {
        if (subset[i] != -1)
        {
            totalCount++;
            if (dataset[i][NUM_ATTRIBUTES] == 1)
                yesCount++;
            else
                noCount++;
        }
    }

    return -(yesCount/totalCount)*log2(yesCount/totalCount)-(noCount/totalCount)*log2(noCount/totalCount);
}

//Calculate weighted entropy gain for each attribute in current instance set
float ig_gain(int subset[numInstances], int dataset[numInstances][NUM_ATTRIBUTES+1], int attribute)
{
    float totalCount = 0;
    float valueCount[numValues[attribute]];
    float yesCount[numValues[attribute]];
    float noCount[numValues[attribute]];
    float entropy[numValues[attribute]];
    float weightedEntropy = 0;

    for (int j = 0; j < numValues[attribute]; j++)
    {
        valueCount[j] = 0;
        yesCount[j] = 0;
        noCount[j] = 0;
        entropy[j] = 0;
    }

    //printf("vc[0]: %f, vc[1]: %f, vc[2]: %f\n", valueCount[0], valueCount[1], valueCount[2]);

    for (int i = 0; i < numInstances; i++)
    {
        if (subset[i] != -1)
        {
            for (int j = 0; j < numValues[attribute]; j++)
            {
                //printf("inst: %d, attr: %d, val: %d, j: %d\t", i, attribute, dataset[i][attribute], j);
                if (dataset[i][attribute] == j)
                {
                    //printf("incrementing vc[%d]\n",j);
                    valueCount[j]++;
                    totalCount++;
                    if (dataset[i][NUM_ATTRIBUTES] == 1)
                        yesCount[j]++;
                    else
                        noCount[j]++;
                }
                //else
                    //printf("\n");            
            }
        }
    }
    
    for (int j = 0; j < numValues[attribute]; j++)
    {
        //printf("val: %d, vc: %f, yc: %f, nc: %f\n", j, valueCount[j], yesCount[j], noCount[j]);
        if (yesCount[j] == 0 || noCount[j] == 0)
            entropy[j] = 0;
        else
            entropy[j] = -(yesCount[j]/valueCount[j])*log2(yesCount[j]/valueCount[j])-(noCount[j]/valueCount[j])*log2(noCount[j]/valueCount[j]);
        //printf("ent: %f\n", entropy[j]);
        weightedEntropy += valueCount[j]/totalCount*entropy[j];
    }

    //printf("went: %f\n",weightedEntropy);

    return weightedEntropy;
    //for each attribute
        //for each value of that attribute
            //calculate intermediate for all values
        //calculate weighted attribute value
}

int splitLeaf(int currentInstances[numInstances], int data[numInstances][NUM_ATTRIBUTES+1])
{
    //Main algorithm loop
    float initialInformation;
    float attributeGain[4];
    float bestGain;
    int bestAttribute;

    //do
    //{
        //Reset best gain and best attribute
        bestGain = -1;
        bestAttribute = -1;

        //Calculate initial label information
        switch (method)
        {
            //IG
            case 0:
                initialInformation = ig_initial(currentInstances, data);
                printf("INITINF: %f\n", initialInformation);
                break;
            //ME
            case 1:
                //initialInformation = me_initial(currentInstances[currentDepth], data);
                break;
            //Gini
            case 2:
                //initialInformation = gini_initial(currentInstances[currentDepth], data);
                break;
        }

        //Calculate gain of splitting on each attribute
        for (int i = 0; i < NUM_ATTRIBUTES; i++)
        {
            switch (method)
            {
            //IG
            case 0:
                attributeGain[i] = ig_gain(currentInstances, data, i);
                break;
            //ME
            case 1:
                //attributeGain[i] = me_gain(currentInstances[currentDepth], data);
                break;
            //Gini
            case 2:
                //attributeGain[i] = gini_gain(currentInstances[currentDepth], data);
                break;
            }
        }

        //Find max gain attribute
        for (int i = 0; i < NUM_ATTRIBUTES; i++)
        {
            printf("gain: %f\n", initialInformation-attributeGain[i]);
            if (initialInformation - attributeGain[i] > bestGain)
            {
                bestGain = initialInformation - attributeGain[i];
                bestAttribute = i;
            }
        }

        printf("Splitting on attribute: %d\n", bestAttribute);
        return bestAttribute;
        //numLeaves[currentDepth] = numValues[bestAttribute];
        //printf("leaves below: %d", numLeaves[currentDepth]);
        //Check for other splits to decide on this level
        //if (currentDepth == 0)
        //    currentDepth++;
        //else
        //{
        //    if (numLeavesSplit[currentDepth-1] == numLeaves[currentDepth-1])
        //        currentDepth++;
        //    else

        //}

}

//Convert attribute strings from input dataset to integers
int attributeToInt(char* attribute)
{
    if (!strcmp(attribute, "sunny") || !strcmp(attribute, "hot") || !strcmp(attribute, "high") || !strcmp(attribute, "strong") || !strcmp(attribute, "no"))
        return 0;
    else if (!strcmp(attribute, "overcast") || !strcmp(attribute, "medium") || !strcmp(attribute, "normal") || !strcmp(attribute, "weak") || !strcmp(attribute, "yes"))
        return 1;
    else if (!strcmp(attribute, "rainy") || !strcmp(attribute, "cool") || !strcmp(attribute, "low"))
        return 2;
    else
        return -1;
}

