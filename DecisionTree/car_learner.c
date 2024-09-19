//This program implements a decision tree learning algorithm for the car evaluation task in HW1 part 2 problem 2.
//Users can select information gain, majority error, or gini index, and optionally set a maximum depth from 1 to 6.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUM_LABELS 4
#define NUM_ATTRIBUTES 6
int numValues[NUM_ATTRIBUTES] = {4, 4, 4, 3, 3, 3};
//int numInstances = 0;
#define MAX_VAL 4 

int splitLeaf(int currentInstances[14], int data[][NUM_ATTRIBUTES+1], int numInstances, int method, bool parentAttribute[NUM_ATTRIBUTES], int branchIndex);
float ig_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances);
float ig_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute);
float me_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances);
float me_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute);
float gini_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances);
float gini_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute);

int countData(void);
int importData(int data[][NUM_ATTRIBUTES+1], int numInstances);
int valueToInt(char* value, int attribute);
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

void printTree(Branch tree[], int maxBranches);
void decodeAttribute(int attribute);
void decodeValue(int attribute, int value);
void decodeLabel(int label);

int getNextID(Branch tree[], int maxBranches);



int main()
{
    //Import data from CSV
    int numInstances = countData();
    if (numInstances == -1)
    {
        return 1;
    }
    printf("data counted %d\n", numInstances);
    int data[numInstances][NUM_ATTRIBUTES+1];
    printf("data array created\n");
    importData(data, numInstances);
    int method = getMethod();
    int maxDepth = getMaxDepth();
    printf("data imported\n");
    int maxBranches = 0;
    for (int i = 0; i < maxDepth+1; i++)
    {
        maxBranches += (int)pow(MAX_VAL, i);
    }
  
    Branch tree[maxBranches];
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
        tree[i].value = -1;
        tree[i].label = -1;
    }

    //Initialize head of tree
    int currentInstances[maxBranches][numInstances];
    for (int i = 0; i < maxBranches; i++)
    {
        for (int j = 0; j < numInstances; j++)
        {
            if (i == 0)
            {
                currentInstances[i][j] = j;
            }
            else
            {
                currentInstances[i][j] = -1;
            }
        }
    }
    int branchIndex = 0;
    tree[0].active = true;
    tree[0].level = 1;

    while(!allDone)
    {
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
            //find most common label for current value
            for (int i = 0; i < numInstances; i++)
            {
                if (data[i][tree[tree[branchIndex].parent].attribute] == tree[branchIndex].value)
                {
                    for (int j = 0; j < NUM_LABELS; j++)
                    {
                        if (data[i][NUM_ATTRIBUTES] == j)
                        {
                            labelCount[j]++;
                        }
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

            //Check if all leaves of parent branch are labelled
            for (int j = 0; j < numValues[tree[tree[branchIndex].parent].attribute]; j++)
            {
                //If not, set flag false
                if (tree[tree[tree[branchIndex].parent].leaf[j]].label == -1)
                {
                    allLeavesLabelled = false;
                    break;
                }
            }                 
            //Set index back to head if all labelled
            if (allLeavesLabelled)
            {
                tree[tree[branchIndex].parent].label = -2;  //mark current branch as having all leaves labelled
            }

            //set index to parent
            branchIndex = tree[branchIndex].parent;
        }
        else
        {
            //Check if all instances have same label
            for (int i = 0; i < numInstances; i++)
            {   
                //If current instance is in current set
                if (currentInstances[branchIndex][i] != -1)
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
                            break;
                        }
                        else
                        {
                            lastLabel = data[i][NUM_ATTRIBUTES];
                        }
                    }
                }
            }
            //If branch is ready to label
            if (readyToLabel)
            {
                //Label branch
                //If branch has no
                if (lastLabel == -1)
                {
                    //find most common label for current value
                    for (int i = 0; i < numInstances; i++)
                    {
                        if (data[i][tree[tree[tree[branchIndex].parent].parent].attribute] == tree[tree[branchIndex].parent].value)
                        {
                            for (int j = 0; j < NUM_LABELS; j++)
                            {
                                if (data[i][NUM_ATTRIBUTES] == j)
                                {
                                    labelCount[j]++;
                                }
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
                    //set lastLabel to maxLabel
                    lastLabel = maxLabel;
                }
                tree[branchIndex].label = lastLabel;
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
                    tree[branchIndex].label = -2;  //mark current branch as having all leaves labelled
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
                    int tempIndex = branchIndex;
                    while (tempIndex > -1)
                    {
                        if (tempIndex > 0)
                        {
                            parentValue[tree[tree[tempIndex].parent].attribute] = tree[tempIndex].value;
                        }
                        tempIndex = tree[tempIndex].parent;
                        parentAttribute[tree[tempIndex].attribute] = true;
                    }
                    //split
                    tree[branchIndex].attribute = splitLeaf(currentInstances[branchIndex], data, numInstances, method, parentAttribute, branchIndex);
                    //create leaves & assign values
                    for (int i = 0; i < numValues[tree[branchIndex].attribute]; i++)
                    {
                        tree[branchIndex].leaf[i] = getNextID(tree, maxBranches);
                        tree[tree[branchIndex].leaf[i]].active = true;
                        tree[tree[branchIndex].leaf[i]].value = i;
                        tree[tree[branchIndex].leaf[i]].parent = branchIndex;
                        tree[tree[branchIndex].leaf[i]].level = tree[branchIndex].level + 1;
                        //for each instance
                        for (int j = 0; j < numInstances; j++)
                        {
                            //if the instance value mathing the leaf value for the parent attribute is also present in parent subset
                            if (data[j][tree[branchIndex].attribute] == i && currentInstances[branchIndex][j] != -1)
                            {
                                currentInstances[tree[branchIndex].leaf[i]][j] = j;
                            }
                            else
                            {
                                currentInstances[tree[branchIndex].leaf[i]][j] = -1;
                            }
                        }
                    }
                    //set branch index to first leaf
                    branchIndex = tree[branchIndex].leaf[0];
                }
            }
        }
    }

    printTree(tree, maxBranches);

    return 0;
}

int splitLeaf(int currentInstances[14], int data[][NUM_ATTRIBUTES+1], int numInstances, int method, bool parentAttribute[NUM_ATTRIBUTES], int branchIndex)
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
            initialInformation = me_initial(currentInstances, data, numInstances);
            break;
        //Gini
        case 2:
            initialInformation = gini_initial(currentInstances, data, numInstances);
            break;
    }
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
                attributeGain[i] = me_gain(currentInstances, data, numInstances, i);
                break;
            //Gini
            case 2:
                attributeGain[i] = gini_gain(currentInstances, data, numInstances, i);
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
    float labelCount[NUM_LABELS];
    float init = 0;

    for (int i = 0; i < numInstances; i++)
    {
        if (subset[i] != -1)
        {
            totalCount++;
            for (int j = 0; j < NUM_LABELS; j++)
            {
                if (dataset[i][NUM_ATTRIBUTES] == j)
                {
                    labelCount[j]++;
                }       
            }
        }
    }

    for (int i = 0; i < NUM_LABELS; i++)
    {
        init -= ((labelCount[i]/totalCount)*log2(labelCount[i]/totalCount));
    }

    return init;
}

//Calculate weighted entropy gain for each attribute in current instance set
float ig_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute)
{
    float totalCount = 0;
    float valueCount[numValues[attribute]];
    float labelCount[numValues[attribute]][NUM_LABELS];
    float entropy[numValues[attribute]];
    float weightedEntropy = 0;

    //Initialize values to zero
    for (int j = 0; j < numValues[attribute]; j++)
    {
        valueCount[j] = 0;
        for (int k = 0; k < NUM_LABELS; k++)
        {
            labelCount[j][k] = 0;
        }
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
                    for (int k = 0; k < NUM_LABELS; k++)
                    {
                        if (dataset[i][NUM_ATTRIBUTES] == labelCount[j][k])
                        {
                            labelCount[j][k]++;
                        }
                    }
                }
            }
        }
    }
    


    for (int j = 0; j < numValues[attribute]; j++)
    {
        for (int k = 0; k < NUM_LABELS; k++)
        {
            if (labelCount[j][k] == 0)
            {
                entropy[j] = 0;
            }
            else
            {
                entropy[j] -= ((labelCount[j][k]/valueCount[j])*log2(labelCount[j][k]/valueCount[j]));
                //entropy[j] = -(yesCount[j]/valueCount[j])*log2(yesCount[j]/valueCount[j])-(noCount[j]/valueCount[j])*log2(noCount[j]/valueCount[j]);
            }
        }
        weightedEntropy += valueCount[j]/totalCount*entropy[j];
    }
    return weightedEntropy;
}

//Calculate majority error on labels for current set of instances
float me_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances)
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
            {
                yesCount++;
            }
            else
            {
                noCount++;
            }
        }
    }

    if (yesCount >= noCount)
    {
        return 1 - (yesCount/totalCount);
    }
    else
    {
        return 1 - (noCount/totalCount);
    }
}

//Calculate weighted majority error gain for each attribute in current instance set
float me_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute)
{
    float totalCount = 0;
    float valueCount[numValues[attribute]];
    float yesCount[numValues[attribute]];
    float noCount[numValues[attribute]];
    float me[numValues[attribute]];
    float weightedme = 0;

    //Initialize values to zero
    for (int j = 0; j < numValues[attribute]; j++)
    {
        valueCount[j] = 0;
        yesCount[j] = 0;
        noCount[j] = 0;
        me[j] = 0;
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
                    {
                        yesCount[j]++;
                    }
                    else
                    {
                        noCount[j]++;
                    }
                }
            }
        }
    }
    


    for (int j = 0; j < numValues[attribute]; j++)
    {
        if (valueCount[j] == 0)
        {
            me[j] = 1;
        }
        else
        {
            if (yesCount[j] >= noCount[j])
            {
                me[j] = 1 - (yesCount[j]/valueCount[j]);
            }
            else
            {
                me[j] = 1 - (noCount[j]/valueCount[j]);
            }
        }
        weightedme += valueCount[j]/totalCount*me[j];
    }
    return weightedme;
}

//Calculate gini on labels for current set of instances
float gini_initial(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances)
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

    return 1 - ((yesCount/totalCount)*(yesCount/totalCount) + (noCount/totalCount)*(noCount/totalCount));
}

//Calculate weighted gini gain for each attribute in current instance set
float gini_gain(int subset[], int dataset[][NUM_ATTRIBUTES+1], int numInstances, int attribute)
{
    float totalCount = 0;
    float valueCount[numValues[attribute]];
    float yesCount[numValues[attribute]];
    float noCount[numValues[attribute]];
    float gini[numValues[attribute]];
    float weightedGini = 0;

    //Initialize values to zero
    for (int j = 0; j < numValues[attribute]; j++)
    {
        valueCount[j] = 0;
        yesCount[j] = 0;
        noCount[j] = 0;
        gini[j] = 0;
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
        }
    }
    


    for (int j = 0; j < numValues[attribute]; j++)
    {
        if (valueCount[j] == 0)
        {
            gini[j] = 1;
        }
        else
        {
            gini[j] = 1 - ((yesCount[j]/valueCount[j])*(yesCount[j]/valueCount[j]) + (noCount[j]/valueCount[j])*(noCount[j]/valueCount[j]));
        }
        weightedGini += valueCount[j]/totalCount*gini[j];
    }
    return weightedGini;
}

int countData(void)
{
     int count = 0;

    //Open input file
    FILE *inputFile = fopen("test.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[100];

    //Count number of instances in input file
    while (feof(inputFile) != true)
    {
        fgets(row, 100, inputFile);
        count++;
    }
    fclose(inputFile);
    return count;
}

int importData(int data[][NUM_ATTRIBUTES+1], int numInstances)
{
    FILE *inputFile = fopen("test.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[100];
    char *token;

    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        for (int i = 0; i < numInstances; i++)
        {
            if (fgets(row, 100, inputFile) == NULL)
                return 0;
            else
            {
            token = strtok(row, ",");
            for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
            {
                data[i][j] = valueToInt(token, j);
                token = strtok(NULL, ",\r\n");
            }
            }
        }
    }
    fclose(inputFile);
    return 0;
}

//Convert value strings from input dataset to integers
int valueToInt(char* value, int attribute)
{
    /*
        data[inst][0] - buying
            vhigh - 0
            high - 1
            med - 2
            low - 3
        data[]inst[1] - maint
            vhigh - 0
            high - 1
            med - 2
            low - 3
        data[inst][2] - doors
            2 - 0
            3 - 1
            4 - 2
            5more - 3
        data[inst][3] - persons
            2 - 0
            4 - 1
            more - 2
        data[inst][4] - lug_boot
            small - 0
            med - 1
            big - 2
        data[inst][5] - safety
            low - 0
            med - 1
            high - 2
        data[inst][6] - label
            unacc - 0
            acc - 1
            good - 2
            vgood - 3
    */

   switch (attribute)
   {
        case 0:
            if (!strcmp(value, "vhigh"))
                return 0;
            else if (!strcmp(value, "high"))
                return 1;
            else if (!strcmp(value, "med"))
                return 2;
            else if (!strcmp(value, "low"))
                return 3;
            break;
        case 1:
            if (!strcmp(value, "vhigh"))
                return 0;
            else if (!strcmp(value, "high"))
                return 1;
            else if (!strcmp(value, "med"))
                return 2;
            else if (!strcmp(value, "low"))
                return 3;
            break;
        case 2:
            if (!strcmp(value, "2"))
                return 0;
            else if (!strcmp(value, "3"))
                return 1;
            else if (!strcmp(value, "4"))
                return 2;
            else if (!strcmp(value, "5more"))
                return 3;
            break;
        case 3:
            if (!strcmp(value, "2"))
                return 0;
            else if (!strcmp(value, "4"))
                return 1;
            else if (!strcmp(value, "more"))
                return 2;
            break;
        case 4:
            if (!strcmp(value, "small"))
                return 0;
            else if (!strcmp(value, "med"))
                return 1;
            else if (!strcmp(value, "big"))
                return 2;
            break;
        case 5:
            if (!strcmp(value, "low"))
                return 0;
            else if (!strcmp(value, "med"))
                return 1;
            else if (!strcmp(value, "high"))
                return 2;
            break;
        case 6:
            if (!strcmp(value, "unacc"))
                return 0;
            else if (!strcmp(value, "acc"))
                return 1;
            else if (!strcmp(value, "good"))
                return 2;
            else if (!strcmp(value, "vgood"))
                return 3;
            break;
   }
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
        scanf(" %c", &userInput);
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
        scanf(" %c", &userInput);
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

void printTree(Branch tree[], int maxBranches)
{
    int numBranches = 0;
    for (int i = 0; i < maxBranches; i++)
    {
        if (tree[i].active)
        {
            numBranches++;
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < numBranches; i++)
    {
        if (i == 0)
        {
            printf("Branch node 0 is the beginning of the tree and splits on attribute ");
            decodeAttribute(tree[i].attribute);
            printf("\n");
        }
        else
        {
            if (tree[i].label < 0)
            {
                printf("Branch node %d has value ", i);
                decodeValue(tree[tree[i].parent].attribute, tree[i].value);
                printf(" from parent branch %d and splits on attribute ", tree[i].parent);
                decodeAttribute(tree[i].attribute);
                printf("\n");
            }
            else
            {
                printf("Branch node %d has value ", i);
                decodeValue(tree[tree[i].parent].attribute, tree[i].value);
                printf(" from parent branch %d and label ", tree[i].parent);
                decodeLabel(tree[i].label);
                printf("\n");
            }
        }
    }
}

void decodeAttribute(int attribute)
{
    switch (attribute)
    {
        case 0:
            printf("'buying'");
            break;
        case 1:
            printf("'maint'");
            break;
        case 2:
            printf("'doors'");
            break;
        case 3:
            printf("'persons'");
            break;
        case 4:
            printf("'lug_boot'");
            break;
        case 5:
            printf("'safety'");
            break;
    }
}

void decodeValue(int attribute, int value)
{
    switch (attribute)
    {
        case 0:
            switch (value)
            {
                case 0:
                    printf("'vhigh'");
                    break;
                case 1:
                    printf("'high'");
                    break;
                case 2:
                    printf("'med'");
                    break;
                case 3:
                    printf("'low'");
                    break;
            }
            break;
        case 1:
            switch (value)
            {
                case 0:
                    printf("'vhigh'");
                    break;
                case 1:
                    printf("'high'");
                    break;
                case 2:
                    printf("'med'");
                    break;
                case 3:
                    printf("'low'");
                    break;
            }
            break;
        case 2:
            switch (value)
            {
                case 0:
                    printf("'2'");
                    break;
                case 1:
                    printf("'3'");
                    break;
                case 2:
                    printf("'4'");
                    break;
                case 3:
                    printf("'5more'");
                    break;
            }
            break;
        case 3:
            switch (value)
            {
                case 0:
                    printf("'2'");
                    break;
                case 1:
                    printf("'4'");
                    break;
                case 2:
                    printf("'more'");
                    break;
            }
            break;
        case 4:
            switch (value)
            {
                case 0:
                    printf("'small'");
                    break;
                case 1:
                    printf("'med'");
                    break;
                case 2:
                    printf("'big'");
                    break;
            }
            break;
        case 5:
            switch (value)
            {
                case 0:
                    printf("'low'");
                    break;
                case 1:
                    printf("'med'");
                    break;
                case 2:
                    printf("'high'");
                    break;
            }
            break;
    }
}

void decodeLabel(int label)
{
    switch (label)
    {
        case 0:
            printf("'unacc'");
            break;
        case 1:
            printf("'acc'");
            break;
        case 2:
            printf("'good'");
            break;
        case 3:
            printf("'vgood'");
            break;
    }
}
