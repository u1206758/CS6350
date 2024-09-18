//This program implements a decision tree learning algorithm for the car evaluation task in HW1 part 2 problem 2.
//Users can select information gain, majority error, or gini index, and optionally set a maximum depth from 1 to 6.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUM_ATTRIBUTES 4
int numValues[NUM_ATTRIBUTES] = {3, 3, 3, 2};
#define MAX_VAL 3 

int splitLeaf(int currentInstances[], int data[][NUM_ATTRIBUTES+1], int numInstances, int method);
float ig_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances);
float ig_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute);


int countData(void);
int importData(int data[][NUM_ATTRIBUTES+1], int numInstances);
int attributeToInt(char* attribute);
int getMethod(void);
int getMaxDepth(void);

struct Branch
{
    bool active;    //Whether this branch slot is used by the tree
    int level;      //The depth in the tree this branch is on
    int parent;     //The ID of the parent branch
    int leaf[MAX_VAL];    //The IDs of the leaves of this branch
    int attribute;  //The attribute this branch is split on (if any)
    int label;      //The label for this branch (-1 no label, -2 all children labelled)
};

int main()
{
    //Import data from CSV
    int numInstances = countData();
    if (numInstances == -1)
        return 1;
    int data[numInstances][NUM_ATTRIBUTES+1];
    importData(data, numInstances);
/*
    for (int i = 0; i < numInstances; i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
        {
            printf("%d, ", data[i][j]);
        }
        printf("\n");
    }
*/

    int maxDepth = 6;
    int method = 0;

    int maxBranches = (int)pow(MAX_VAL, maxDepth);
    struct Branch tree[maxBranches];
    int currentLevel = 1;
    bool allDone = false;

    //Initialize all possible branches
    for (int i = 0; i < maxBranches; i++)
    {
        tree[i].active = false;
        tree[i].level = -1;
        tree[i].parent = -1;
        for (int j = 0; j < MAX_VAL; j++)
            tree[i].leaf[j] = -1;
        tree[i].attribute = -1;
        tree[i].label = -1;
    }
/* dumb stuff probably just delete
    for (int i = 0; i < 20; i++)
    {
        if (i > 10)
        {
            if (i > 12)
                printf("dbif: %d\n",i);
            printf("if: %d\n",i) ;
        }
        printf("loop: %d\n",i);
    }
*/
    //Initialize head of tree
    int currentInstances[numInstances];
    for (int i = 0; i < numInstances; i++)
        currentInstances[i] = i;
    int branchIndex = 0;
    tree[0].active = true;
    tree[0].level = 1;

    while(!allDone)
    {
        if (currentLevel > maxDepth)
        {
            
        }
        else
        {

        }
    }
    /*
    start at branch 0
    
    loop
        if current level > max level
            assign all branches on current level a label based on most common
            done
        else
            if ready to label
                label
                set branch index to parent
                if all leaves are labelled
                    set branch index back to parent (if not head)
                else
                    set branch index to next unlabelled leaf
            else
                if leaves on this node exist
                    if all leaves are labelled (or all children labelled)
                        if on head
                            done
                        else
                            set branch index back to parent
                    else
                        set branch index to next unlabeled leaf
                else
                    find new current instances
                    split level based on attribute other than parent
                    create leaves
                    set branch index to first leaf
    */  

    tree[0].attribute = splitLeaf(currentInstances, data, numInstances, method);

    //Flags to check if each value exists for the chosen attribute
    bool valueExists[MAX_VAL];
    
    //Initialize all flags to false
    for (int i = 0; i < MAX_VAL; i++)
        valueExists[i] = false;

    //Check every instance at selected attribute to check for value existence
    for (int i = 0; i < numInstances; i++)
    {
        for (int j = 0; j < numValues[tree[branchIndex].attribute]; j++)
        {
            if (data[i][tree[branchIndex].attribute] == j)
                {
                    valueExists[j] = true;
                    tree[branchIndex].leaf[j] = branchIndex + j;
                    tree[branchIndex+j+1].parent = branchIndex;
                    tree[branchIndex+j+1].active = true;
                    tree[branchIndex+j+1].level = tree[branchIndex].level+1;
                    break;
                }
        }
    }
    
    for (int i = 0; i < 10; i++)
    {
        printf("branch[%d] - active: %d, parent: %d, level: %d, attribute: %d\n", i, tree[i].active, tree[i].parent, tree[i].level, tree[i].attribute);
    }

    //printf("%d  %d  %d\n", valueExists[0], valueExists[1], valueExists[2]);



    return 0;
}

int splitLeaf(int currentInstances[], int data[][NUM_ATTRIBUTES+1], int numInstances, int method)
{
    //Main algorithm loop
    float initialInformation;
    float attributeGain[4];
    float bestGain = -1;
    int bestAttribute = -1;
    
    //Calculate initial label information
    switch (method)
    {
        //IG
        case 0:
            initialInformation = ig_initial(currentInstances, data, numInstances);
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
    printf("init: %f\n", initialInformation);
    //Calculate gain of splitting on each attribute
    for (int i = 0; i < NUM_ATTRIBUTES; i++)
    {
        switch (method)
        {
        //IG
        case 0:
            attributeGain[i] = ig_gain(currentInstances, data, numInstances, i);
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
        if (initialInformation - attributeGain[i] > bestGain)
        {
            bestGain = initialInformation - attributeGain[i];
            bestAttribute = i;
        }
    }
    return bestAttribute;
}

//Calculate entropy on labels for current set of instances
float ig_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances)
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
float ig_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute)
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

    for (int i = 0; i < numInstances; i++)
    {
        if (subset[i] != -1)
        {
            for (int j = 0; j < numValues[attribute]; j++)
            {
                if (dataset[i][attribute] == j)
                {
                    valueCount[j]++;
                    totalCount++;
                    if (dataset[i][NUM_ATTRIBUTES] == 1)
                        yesCount[j]++;
                    else
                        noCount[j]++;
                }
            }
        }
    }
    
    for (int j = 0; j < numValues[attribute]; j++)
    {
        if (yesCount[j] == 0 || noCount[j] == 0)
            entropy[j] = 0;
        else
            entropy[j] = -(yesCount[j]/valueCount[j])*log2(yesCount[j]/valueCount[j])-(noCount[j]/valueCount[j])*log2(noCount[j]/valueCount[j]);
        weightedEntropy += valueCount[j]/totalCount*entropy[j];
    }
    return weightedEntropy;
}

int countData(void)
{
     int count = 0;

    //Open input file
    FILE *inputFile = fopen("tennis.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[50];

    //Count number of instances in input file
    while (feof(inputFile) != true)
    {
        fgets(row, 50, inputFile);
        count++;
    }
    fclose(inputFile);
    return count;
}

int importData(int data[][NUM_ATTRIBUTES+1], int numInstances)
{
    FILE *inputFile = fopen("tennis.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[50];
    char *token;

    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        for (int i = 0; i < numInstances; i++)
        {
            fgets(row, 50, inputFile);
            token = strtok(row, ",");
            for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
            {
                data[i][j] = attributeToInt(token);
                token = strtok(NULL, ",\r\n");
            }
        }
    }
    fclose(inputFile);
    return 0;
}

//Convert attribute strings from input dataset to integers
int attributeToInt(char* attribute)
{
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

    if (!strcmp(attribute, "sunny") || !strcmp(attribute, "hot") || !strcmp(attribute, "high") || !strcmp(attribute, "strong") || !strcmp(attribute, "no"))
        return 0;
    else if (!strcmp(attribute, "overcast") || !strcmp(attribute, "medium") || !strcmp(attribute, "normal") || !strcmp(attribute, "weak") || !strcmp(attribute, "yes"))
        return 1;
    else if (!strcmp(attribute, "rainy") || !strcmp(attribute, "cool") || !strcmp(attribute, "low"))
        return 2;
    else
        return -1;
}

int getMethod(void)
{
    char userInput;
    bool userInputValid;
    int method;
    do 
    {
        printf("Select attribute split method:\n\tInformation gain (I)\n\tMajority error (M)\n\tGini index (G)\n\n");
        scanf("%c", &userInput);
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
    return method;
}

int getMaxDepth(void)
{
    char userInput;
    bool userInputValid;
    int depth;

    do
    {
        printf("Select maximum tree depth (1-6)\n\n");
        scanf("%c", &userInput);
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
    return depth;
}