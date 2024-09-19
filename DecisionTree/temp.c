//This program implements a decision tree learning algorithm for the car evaluation task in HW1 part 2 problem 2.
//Users can select information gain, majority error, or gini index, and optionally set a maximum depth from 1 to 6.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUM_LABELS 2
#define NUM_ATTRIBUTES 4
int numValues[NUM_ATTRIBUTES] = {3, 3, 3, 2};
#define MAX_VAL 3 

int splitLeaf(int currentInstances[], int data[][NUM_ATTRIBUTES+1], int numInstances, int method, bool parentAttribute[NUM_ATTRIBUTES]);
float ig_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances);
float ig_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute);


int countData(void);
int importData(int data[][NUM_ATTRIBUTES+1], int numInstances);
int valueToInt(char* value);
int getMethod(void);
int getMaxDepth(void);

typedef struct
{
   // int id;         //ID (also array index) of current branch
    bool active;    //Whether this branch slot is used by the tree
    int level;      //The depth in the tree this branch is on
    int parent;     //The ID of the parent branch
    int leaf[MAX_VAL];    //The IDs of the leaves of this branch
    int attribute;  //The attribute this branch is split on (if any)
    int value;      //The value a leaf represents when split from parent branch
    int label;      //The label for this branch (-1 no label, -2 all children labelled)
}Branch;

int getNextID(Branch tree[], int maxBranches);

int main()
{
    //Import data from CSV
    int numInstances = countData();
    if (numInstances == -1)
        return 1;
    int data[numInstances][NUM_ATTRIBUTES+1];
    importData(data, numInstances);

    for (int i = 0; i < numInstances; i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
        {
            printf("%d, ", data[i][j]);
        }
        printf("\n");
    }


    int maxDepth = 6;
    int method = 0;

    int maxBranches = (int)pow(MAX_VAL, maxDepth);
    Branch tree[maxBranches];
    int currentLevel = 1;
    bool allDone = false;

    //Initialize all possible branches
    for (int i = 0; i < maxBranches; i++)
    {
        //tree[i].id = i;
        tree[i].active = false;
        tree[i].level = -1;
        tree[i].parent = -1;
        for (int j = 0; j < MAX_VAL; j++)
            tree[i].leaf[j] = -1;
        tree[i].attribute = -1;
        tree[i].value = -1;
        tree[i].label = -1;
    }

    //Initialize head of tree
    int currentInstances[numInstances];
    for (int i = 0; i < numInstances; i++)
        currentInstances[i] = i;
    int branchIndex = 0;
    tree[0].active = true;
    tree[0].level = 1;

    while(!allDone)
    {
        printf("entering loop at branchIndex: %d, parent: %d, attribute: %d\n",branchIndex, tree[branchIndex].parent, tree[branchIndex].attribute);
     if (branchIndex == 10)
     {
        for (int i = 0; i < 10; i++)
        {
            printf("branch[%d] - active: %d, parent: %d, level: %d, attribute: %d, value: %d, label: %d\n", i, tree[i].active, tree[i].parent, tree[i].level, tree[i].attribute, tree[i].value, tree[i].label);
        }
        return 0;
     }
        int lastLabel = -1;
        bool readyToLabel = true;
        bool allLeavesLabelled = true;
        bool hasLeaves = false;
        int labelCount[NUM_LABELS];
        for (int i = 0; i < NUM_LABELS; i++)
        {
            labelCount[i] = 0;
        }
        int maxLabelCount = 0;
        int maxLabel = -1;
        bool parentAttribute[NUM_ATTRIBUTES];
        int parentValue[NUM_ATTRIBUTES];
        for (int i = 0; i < NUM_ATTRIBUTES; i++)
        {
            parentAttribute[i] = false;
            parentValue[i] = -1;
        }

        //If current branch/leaf is at max level
        if (tree[branchIndex].level > maxDepth)
        {
            //find most common label overall
            for (int i = 0; i < numInstances; i++)
            {
                for (int j = 0; j < NUM_LABELS; j++)
                {
                    if (data[i][NUM_ATTRIBUTES] == j)
                    {
                        labelCount[j]++;
                    }
                }
            }
            for (int i = 0; i < NUM_LABELS; i++)
            {
                if (labelCount[i] > maxLabelCount)
                {
                    maxLabelCount = labelCount[i];
                    maxLabel = i;
                }
            }
            //Assign most common label to leaf
            tree[branchIndex].label = maxLabel;
            //set index to parent
            branchIndex = tree[branchIndex].parent;
        }
        else
        {
            //Check if all instances have same label
            for (int i = 0; i < numInstances; i++)
            {   
                //If current instance is in current set
                if (currentInstances[i] != -1)
                {
                    //Check if all labels are the same
                    if (lastLabel == -1)
                    {
                        lastLabel = data[i][NUM_ATTRIBUTES];
                    }
                    else
                    {
                        if (lastLabel != data[i][NUM_ATTRIBUTES])
                        {
                            readyToLabel = false;
                            printf("broke because %d != %d\tdata[%d]\n", lastLabel, data[i][NUM_ATTRIBUTES], i);
                            break;
                        }
                        else
                        {
                            lastLabel = data[i][NUM_ATTRIBUTES];
                        }
                    }
                    printf("cI: %d, lL: %d, cL: %d, rTL: %d\n", i, lastLabel, data[i][NUM_ATTRIBUTES], readyToLabel);
                }
            }
            //If branch is ready to label
            if (readyToLabel)
            {
                //Label branch
                tree[branchIndex].label = lastLabel;
                printf("labelling branch %d with label %d\n", branchIndex, lastLabel);
                //If index at head, all done, else set index to parent
                if (branchIndex == 0)
                {
                    allDone = true;
                }
                else
                {
                    branchIndex = tree[branchIndex].parent;       
                }
                //Check if all leaves of current branch are labelled
                for (int j = 0; j < numValues[tree[branchIndex].attribute]; j++)
                {
                    //If not, set index to next unlabelled leaf
                    if (tree[tree[branchIndex].leaf[j]].label == -1)
                    {
                        allLeavesLabelled = false;
                        branchIndex = tree[branchIndex].leaf[j];
                        break;
                    }
                }                 
                //Set index back to head if all labelled
                if (allLeavesLabelled)
                {
                    tree[tree[branchIndex].parent].label = -2;  //mark parent as having all leaves labelled
                    branchIndex = tree[branchIndex].parent;
                }
            }   
            else //branch is not ready to label
            {
                //Check if leaves exist on this branch
                for (int i = 0; i < numValues[tree[branchIndex].attribute]; i++)
                {
                    if (tree[branchIndex].leaf[i != -1])
                        {
                            hasLeaves = true;
                            break;
                        }
                }
                if (hasLeaves)
                {
                    //Check if all leaves are labelled
                    for (int i = 0; i < numValues[tree[branchIndex].attribute]; i++)
                    {
                        //if all are not labelled, set index to next unlabeleld
                        if (tree[tree[branchIndex].leaf[i]].label == -1)
                        {
                            allLeavesLabelled = false;
                            branchIndex = tree[branchIndex].leaf[i];
                            break;
                        }
                    }
                    if (allLeavesLabelled)
                    {
                        //if all leaves on all branches under head are labelled, done
                        if (branchIndex == 0)
                        {
                            allDone = true;
                        }
                        else // set index back to parent
                        {
                            branchIndex = tree[branchIndex].parent;
                        }
                    }
                }
                else //no leaves, not ready to label, need to split
                {
                    //Find all attributes already split in the current path
                    int tempIndex = branchIndex;//tree[branchIndex].parent;
                    while (tempIndex > -1)
                    {
                        if (tempIndex > 0)
                        {
                            parentValue[tree[tree[tempIndex].parent].attribute] = tree[tempIndex].value;
                        }
                        tempIndex = tree[tempIndex].parent;
                        parentAttribute[tree[tempIndex].attribute] = true;
                    }
                    printf("pA: %d %d %d %d\n", parentAttribute[0], parentAttribute[1], parentAttribute[2], parentAttribute[3]);
                    printf("pv: %d %d %d %d\n", parentValue[0], parentValue[1], parentValue[2], parentValue[3]);
                    //If not on the head
                    if (branchIndex != 0)
                    {
                        //set new subset of current instances
                        for (int i = 0; i < numInstances; i++)
                        {
                            /* the new instance space should include every instance where the value of the instance for the parent attribute
                            matches the value of the current leaf and the value of the parent attribute in the instance
                            matches the value of the parent leaf all the way up the chain

                            every instance where parentAttribute == leaf value AND
                            parentParentAttribute == leafParent value etc
                            */
                            if (data[i][tree[tree[branchIndex].parent].attribute] == tree[branchIndex].value)
                            {
                                currentInstances[i] = i;
                            }
                            else
                            {
                                currentInstances[i] = -1;
                            }
                            printf("is: %d\n", currentInstances[i]);
                        }
                    }
                    //split
                    tree[branchIndex].attribute = splitLeaf(currentInstances, data, numInstances, method, parentAttribute);
                    //create leaves & assign values
                    for (int i = 0; i < numValues[tree[branchIndex].attribute]; i++)
                    {
                        tree[branchIndex].leaf[i] = getNextID(tree, maxBranches);
                        tree[tree[branchIndex].leaf[i]].active = true;
                        tree[tree[branchIndex].leaf[i]].value = i;
                        tree[tree[branchIndex].leaf[i]].parent = branchIndex;
                        tree[tree[branchIndex].leaf[i]].level = tree[branchIndex].level + 1;
                    }
                    //set branch index to first leaf
                    branchIndex = tree[branchIndex].leaf[0];
                }
            }
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
                label -
                set branch index to parent (if not head) -
                if all leaves are labelled
                    set parent as all leaves labelled
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

    
    for (int i = 0; i < 10; i++)
    {
        printf("branch[%d] - active: %d, parent: %d, level: %d, attribute: %d\n", i, tree[i].active, tree[i].parent, tree[i].level, tree[i].attribute);
    }

    //printf("%d  %d  %d\n", valueExists[0], valueExists[1], valueExists[2]);



    return 0;
}

int splitLeaf(int currentInstances[], int data[][NUM_ATTRIBUTES+1], int numInstances, int method, bool parentAttribute[NUM_ATTRIBUTES])
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
    //Calculate gain of splitting on each attribute, other than parent branch attribute
    for (int i = 0; i < NUM_ATTRIBUTES; i++)
    {
        if (!parentAttribute[i])
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
        else
        {
            attributeGain[i] = 1;
        }
    }
    
    //Find max gain attribute
    for (int i = 0; i < NUM_ATTRIBUTES; i++)
    {
        printf("gain of splitting on %d: %f\n", i, initialInformation-attributeGain[i]);
        if (initialInformation - attributeGain[i] > bestGain)
        {
            bestGain = initialInformation - attributeGain[i];
            bestAttribute = i;
        }
    }
    printf("splitting on: %d\n", bestAttribute);
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

    //Initialize values to zero
    for (int j = 0; j < numValues[attribute]; j++)
    {
        valueCount[j] = 0;
        yesCount[j] = 0;
        noCount[j] = 0;
        entropy[j] = 0;
    }

    //for each instance that is in the current subset
    for (int i = 0; i < numInstances; i++)
    {
        if (subset[i] != -1)
        {
            //count values for each attribute and their label
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
            //printf("att: %d, tc: %f, val: %d, vc: %f, yc: %f, nc: %f\n", attribute, totalCount, dataset[i][attribute], valueCount[dataset[i][attribute]], yesCount[dataset[i][attribute]], noCount[dataset[i][attribute]]);
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
                data[i][j] = valueToInt(token);
                token = strtok(NULL, ",\r\n");
            }
        }
    }
    fclose(inputFile);
    return 0;
}

//Convert value strings from input dataset to integers
int valueToInt(char* value)
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

    if (!strcmp(value, "sunny") || !strcmp(value, "hot") || !strcmp(value, "high") || !strcmp(value, "strong") || !strcmp(value, "no"))
        return 0;
    else if (!strcmp(value, "overcast") || !strcmp(value, "medium") || !strcmp(value, "normal") || !strcmp(value, "weak") || !strcmp(value, "yes"))
        return 1;
    else if (!strcmp(value, "rainy") || !strcmp(value, "cool") || !strcmp(value, "low"))
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

int getNextID(Branch tree[], int maxBranches)
{
    for (int i = 0; i < maxBranches; i++)
    {
        if (tree[i].active == false)
            return i;
    }
    return -1;
}