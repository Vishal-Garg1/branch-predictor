This is a simulator which tells about the accuracy of branch prediction in various forms such as always taken , always not taken , 1-bit dynamic branch predictor , 2-bit dynamic branch predictor.
We also maintain branch target buffer , branch history table for 1-bit as well as 2-bit dynamic branch predictors.
We are given the trace in "trace.txt"  file. Based on trace we predict whether branch will be taken or not taken and compare with the trace whether our prediction is correct or not.

Always taken branch predictor always predicts that branch will be taken.

Always not taken branch predictor always predicts that branch will be not taken.

1-bit dynamic branch predictor starts from assuming not taken and then dynamically changing its assumption based on previous that branch was taken or not.
If previously it was taken then predict taken and vice-versa and the state changes to actual response.

2-bit dynamic branch predictor starts from assuming weakly not taken and then dynamically changing its assumption based on previous two instances of whether that branch was taken or not.
If its in state of weakly not taken or strongly not taken then it predicts not taken and if its in state of weakly taken or strongly taken then it predicts taken.
The state changes as following:
Current state           Actual response     state changes to
Strongly not taken      Not taken           Strongly not taken
Weakly not taken        Not taken           Strongly not taken
Weakly taken            Not taken           Weakly not taken
Strongly taken          Not taken           Weakly taken 
Strongly not taken      Taken               Weakly not taken
Weakly not taken        Taken               Weakly taken 
Weakly taken            Taken               Strongly taken
Strongly taken          Taken               Strongly taken

Due to memory constraints this works on a trace of maximum 10,000 lines (approx.) .

To compile : g++ branchPredictor.cpp
To run : ./a.exe or ./a.out
Having trace.txt file is necessary.