#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//=============================================================================
//please minimize everything to understand the code better
//Group_ID:15   MEMORY SIMULATOR
//=============================================================================

struct {//we will save the file content in this structure and the size of this structure array will be equal to the number of lines in file
	int processesId;
	char processState;//	B OR E
	int processAllocationSize;//size in KB
	int processFrames;//Ceil[processAllocationSize / 4]
}typedef struct_FileContent;

struct {//We save the offsets of the processes frames in this structure
	int startOffset;
	int finishOffset;
    int blockSize;
}typedef struct_BlockOffsets;

int checkPowerofTwo(int x) {
    //A function to check wether the provided memory size is to the power of 2
   if (x == 0)
      return 0;
   while( x != 1) {
      if(x % 2 != 0)
         return 0;
         x /= 2;
   }
   return 1;}

int sizeOfFile(FILE* fp) {
    //this function returns how many lines we have in the file
    //using this function we can create a dynamic struct_FileContent array
    rewind(fp);
	int i = 0, a, b;
	char c;
	while (fscanf(fp, " %c %d %d", &c, &a, &b) != EOF)
		i++;
	return i;}

void readFile(FILE* fp,struct_FileContent *fileLine ) {
    //this function reads the contnet of the file and store it in struct_FileContent dynamic array
    rewind(fp);
	int i = 0;
	while (fscanf(fp, " %c %d %d", &fileLine[i].processState, &fileLine[i].processesId, &fileLine[i].processAllocationSize) != EOF){
		if (fileLine[i].processState == 'E')
			fileLine[i].processAllocationSize = 0;
		fileLine[i].processFrames = ceil((double)(fileLine[i].processAllocationSize) / 4.0);
		i++;}}

void printEmptyHoles(int *memBlocks,int numOfBlocks){
    //this function loops over all the wholes in memory and print the empty ones with their start offset so we can pring the empty holes as requested
    int count =0;
    for (int i = 0; i < numOfBlocks; i++){
            if (memBlocks[i] == 0)
                count++;
            if (memBlocks[i] == 1)
                count =0;
            if (count == numOfBlocks && i - count > -1)
                printf("%d  %d\n",i - count - 1,count);
            else if ((memBlocks[i + 1] == 1 && memBlocks[i] == 0) || (i + 1 == numOfBlocks && memBlocks[i] != 1))
                printf("%d  %d\n",i - count + 1 ,count);
            }}

int firstFit(int *memBlocks /*takes array*/,int numOfBlocks ,int framesOfProcess,int *start /*returns start offset*/,int *end/*returns end offset*/){
    //this algorithm finds the first suitable hole size and allocate the process frames in that hole
    int holeSize =0;
    int totalEmptyBlocks = 0;//to calculate the external fragmentaion

    for (int i = 0; i < numOfBlocks; i++)
    {
        if (holeSize == framesOfProcess){//if we found enough hole size to fit the frames
            *start = i-holeSize;//return end & start offset in memory of this process
            *end = i;
            for (int j = i - holeSize; j < i; j++)
                memBlocks[j] = 1;//change memory blocks from 0 t0 1              
            return 1;//==========================succesful Allocation=========
        }
        if (memBlocks[i] == 0){//if block is empty
            holeSize++;//increase the hole size
            totalEmptyBlocks++;
        }
        if (memBlocks[i] == 1)// if we find a full block
            holeSize =0;//we reset the hole size
    }
    if (totalEmptyBlocks >= framesOfProcess)
        return 0;//====================external fragmentaion

    return -1;}//======================insufficient memory
int bestFit(int *memBlocks,int numOfBlocks ,int framesOfProcess,int *start,int *end){
    int count =0;//holeSize
    int totalEmptyBlocks = 0;
    int *hole = (int*)malloc(sizeof(int) * numOfBlocks);
    int *startPoint = (int*)malloc(sizeof(int) * numOfBlocks);
    int holeIndex =0;
    startPoint[0] = 0;

    // find all the holes in memory and store them in an array**********
    //================================================================== 
    for (int i = 0; i < numOfBlocks; i++){
        if (memBlocks[i] == 0){
            count++;
            totalEmptyBlocks++;
        }
        if ((memBlocks[i + 1] == 1 && memBlocks[i] == 0) || (i+1 == numOfBlocks && count != numOfBlocks && count >= framesOfProcess) ){
            hole[holeIndex] = count;
            startPoint[holeIndex] = i - count + 1;     
            holeIndex++;
            count =0;
        }
    }
    //================================================================== 
    //******************************************************************


    // if the memory is empty initially we fill from the start**********
    //================================================================== 
    if(count == numOfBlocks && count >= framesOfProcess){
        for (int i = 0; i < framesOfProcess; i++){
                memBlocks[i] = 1;
        }
        *start = 0;
        *end = framesOfProcess;
        return 1;//Allocation of initial process is succesful==========
    }
    //================================================================== 
    //******************************************************************

    // Loop through all holes to find the best fitting one**************
    //================================================================== 
    int bestHole = hole[0];
    int bestHoleIndex = 0;
    for (int i = 0; i < holeIndex; i++) {
        if ((bestHole > hole[i] && bestHole >= framesOfProcess) || bestHole < framesOfProcess){
            bestHole = hole[i];
            bestHoleIndex = i;
        }   
    }
    //================================================================== 
    //******************************************************************


    // if we find the best fitting hole that is suitable for the process
    //================================================================== 
    if (bestHole >= framesOfProcess) {
        *start = startPoint[bestHoleIndex];//return start offset
        *end = startPoint[bestHoleIndex] + framesOfProcess;//return end offset
        for (int j = startPoint[bestHoleIndex]; j < startPoint[bestHoleIndex] + framesOfProcess; j++)
                memBlocks[j] = 1;
        return 1;//succesful Allocation =====================
    }
    //================================================================== 
    //******************************************************************

    if (totalEmptyBlocks >= framesOfProcess)
        return 0;//external fragmentaion===============================

    return -1;}//insufficient memory===================================
int worstFit(int *memBlocks,int numOfBlocks ,int framesOfProcess,int *start,int *end){
    int count =0;//holeSize
    int totalEmptyBlocks = 0;
    int *hole = (int*)malloc(sizeof(int) * numOfBlocks);
    int *startPoint = (int*)malloc(sizeof(int) * numOfBlocks);
    int holeIndex =0;
    startPoint[0] = 0;

    // find all the holes in memory and store them in an array**********
    //==================================================================
    for (int i = 0; i < numOfBlocks; i++) {
        if (memBlocks[i] == 0){
            count++;
            totalEmptyBlocks++;
        }
        if ((memBlocks[i + 1] == 1 && memBlocks[i] == 0) || (i+1 == numOfBlocks && count != numOfBlocks && count >= framesOfProcess) ){
            hole[holeIndex] = count;
            startPoint[holeIndex] = i - count + 1;     
            holeIndex++;
            count =0;
        }
    }
    //================================================================== 
    //******************************************************************

    // if the memory is empty initially we fill from the start**********
    //================================================================== 
    if(count == numOfBlocks && count >= framesOfProcess){
        for (int i = 0; i < framesOfProcess; i++){
                memBlocks[i] = 1;
        }
        *start = 0;
        *end = framesOfProcess;
        return 1;//Allocation of initial process is succesful==========
    }
    //================================================================== 
    //******************************************************************

    // Loop through all holes to find the worst fitting one**************
    //================================================================== 
    int worstHole = hole[0];
    int worstHoleIndex = 0;
    for (int i = 0; i < holeIndex; i++) {
        if (worstHole < hole[i]){
            worstHole = hole[i];
            worstHoleIndex = i;
        }   
    }
    //================================================================== 
    //******************************************************************

     // if we find the worst fitting hole that is suitable for the process
    //==================================================================
    if (worstHole >= framesOfProcess) {
        *start = startPoint[worstHoleIndex];//return start offset
        *end = startPoint[worstHoleIndex] + framesOfProcess;//return end offset
        for (int j = startPoint[worstHoleIndex]; j < startPoint[worstHoleIndex] + framesOfProcess; j++)
                memBlocks[j] = 1; 
        return 1;//succesful Allocation =====================
    }

    //================================================================== 
    //******************************************************************
    if (totalEmptyBlocks >= framesOfProcess)
        return 0;//external fragmentaion===============================
    return -1;}//insufficient memory===================================

int main(int argc,char *argv[]) {

    //*******************************************Validations*****************************************************
    //===========================================================================================================

    if(argc != 4 ){
         printf("enter 4 arguments only eg.\" memsim 1024 processes.txt 1\""); 
        return 0; }
    int memorySize = atoi(argv[1]);
    FILE* fp = fopen(argv[2], "r");
    int algorithm = atoi(argv[3]);
    if (fp == NULL){
        printf("The File could not be opened \n");return 0;}
    if(checkPowerofTwo(memorySize) == 0)
        printf("\nThe memory size is not of power 2 it would be better to enter a size with power of 2\n\n");
    if(algorithm > 3 || algorithm < 1){
        printf("Wrong algorithm number you should pick\n (1) for first-fit\n (2) for best-fit\n (3) for worst-fit \n");
        return 0;}

    //===========================================================================================================
    //******************************************Validations are done*********************************************


    //******************************************Initializing variables*******************************************
    //===========================================================================================================

    char algorithmNames[3][20] = {"First-Fit","Best-Fit","Worst-Fit"};
	int numOfFileLines = sizeOfFile(fp);
	struct_FileContent *fileLine = (struct_FileContent*)malloc(sizeof(struct_FileContent) * numOfFileLines);
	struct_BlockOffsets* blockOffsets = (struct_BlockOffsets*)malloc(sizeof(struct_BlockOffsets) * numOfFileLines);
	readFile(fp,fileLine);//read file and store it into array of FileContent structure
	int blocks = memorySize / 4;

	int* MemoryBlocks =(int*)malloc(sizeof(int) * blocks);//array of 4kb memory blocks ex : 000000000000111111111110101111111000000001111
    //0 means empty block and 1 means full block

	int internalFragmentaion = 0;
	int externalFragmentaionCount = 0;
	int insufProcessRejectionCount = 0;
    //initialize all memory blocks to empty at first
    for (int i = 0; i < blocks; i++)
        MemoryBlocks[i] = 0;
    //initialize all offsets to -1
    for (int i = 0; i < numOfFileLines; i++) {
        blockOffsets[i].finishOffset = -1;
        blockOffsets[i].blockSize = -1;
        blockOffsets[i].startOffset = -1;}

    int rtn = -2;
    //return -1 for Insuffiecient memory
    //return 0 for external frahmentaion
    //retutn 1 for successful allocation

    //===========================================================================================================
    //****************************************Initializing variables is done*************************************


    //****************************************Main program lunches here******************************************
    //===========================================================================================================
    printf("\nProgram Lunched\nAlgorithm : %s\n\n",algorithmNames[algorithm-1]);
	for (int i = 0; i < numOfFileLines; i++) {
		if (fileLine[i].processState == 'B' || fileLine[i].processState == 'b') {// line starts with a letter B
            printf("%c   %d   %d\t-->\t", fileLine[i].processState, fileLine[i].processesId, fileLine[i].processAllocationSize);
            if (algorithm == 1){//==============First-Fit======================First-Fit====================First-Fit================
                rtn = firstFit(MemoryBlocks,memorySize/4,fileLine[i].processFrames,&blockOffsets[fileLine[i].processesId].startOffset
                ,&blockOffsets[fileLine[i].processesId].finishOffset); 
            }
            else if (algorithm == 2){//==============Best-Fit======================Best-Fit====================Best-Fit==============
                rtn = bestFit(MemoryBlocks,memorySize/4,fileLine[i].processFrames,&blockOffsets[fileLine[i].processesId].startOffset
                ,&blockOffsets[fileLine[i].processesId].finishOffset);
            }
            else if(algorithm == 3) {//==============Worst-Fit======================Worst-Fit====================Worst-Fit============
                rtn = worstFit(MemoryBlocks,memorySize/4,fileLine[i].processFrames,&blockOffsets[fileLine[i].processesId].startOffset
                ,&blockOffsets[fileLine[i].processesId].finishOffset); 
            }
            if (rtn == 1){//==============Succesful Allocation
				internalFragmentaion += fileLine[i].processFrames * 4 - fileLine[i].processAllocationSize;
                blocks -= fileLine[i].processFrames;
                printf("%d frames will be used, remaining #frames: %d\n",fileLine[i].processFrames,blocks);
            }
            else if (rtn == 0){//===========External fragmentation
                printf("%d frames will be used, ERROR! External fragmentation\n",fileLine[i].processFrames);
                externalFragmentaionCount++;
            }
            else if (rtn == -1){//===========Insufficient memory
                printf("ERROR! Insufficient memory\n");
                insufProcessRejectionCount++;
            }
		}
		else if (fileLine[i].processState == 'E' || fileLine[i].processState == 'e') {// line starts with a letter E
            printf("%c   %d\t   \t-->\t", fileLine[i].processState, fileLine[i].processesId);
            int start = blockOffsets[fileLine[i].processesId].startOffset;
            int end = blockOffsets[fileLine[i].processesId].finishOffset;
            int tempBlockSize = end - start;
            blocks +=tempBlockSize;
            for (start; start < end; start++)
                MemoryBlocks[start] = 0;//Deallocate blocks to empty
            if (tempBlockSize == 0)
                printf("Deallocation failure: this process has not been allocated\n");
            else
                printf("%d frames are deallocated, available #frames: %d\n", fileLine[fileLine[i].processesId].processFrames, blocks);		}

            //-----------------------------------------------------------------------------------------------------------------------------
            //uncomment the code below to visualize the memory and to see whats happening after each process allocation or deallocation

            /*printf("*************************************************************************************************\n");
            printf("Memory blocks represented with (0 = empty) & (1 = full) & (each line is 10 blocks) ==============\n\n");
            printf("Memory after process_id : %d, with the state : %c\n\n",fileLine[i].processesId,fileLine[i].processState);
            for (int m = 0; m < memorySize / 4; m++) {
                if ( m % 10 == 0)
                    printf("\n"); 
                printf("%d ",MemoryBlocks[m]);
            }
            printf("\n\n||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
            printf("\nvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n\n\n");*/

            //uncomment the code above to visualize the memory and to see whats happening after each process allocation or deallocation
            //-----------------------------------------------------------------------------------------------------------------------------            
	}
    //===========================================================================================================
    //****************************************Main program finishes here*****************************************

	printf("\nTotal free memory in holes: %d frames, %d KB\n", blocks, blocks * 4);
	printf("Total memory wasted as an internal fragmentation: %d KB\n", internalFragmentaion);
	printf("Total number of rejected processes due to external fragmentation: %d\n", externalFragmentaionCount);
	printf("Total number of rejected processes due to insufficient memory: %d\n", insufProcessRejectionCount);
    printf("\nHoles:\n");
    printEmptyHoles(MemoryBlocks,memorySize/4);
	return 0;
    }//End of main
