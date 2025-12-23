#include "Document_store.hpp"
using namespace std;
int read_file(int *linecounter,int *maxlength, char *file_name){   
    FILE *file=fopen(file_name, "r");
    if(file==NULL){
        cout<<"Cannot open file: "<<file_name<<endl;
        return -1;
    }
    char *line=NULL;
    size_t len=0;
    int current_length;
    while((current_length=getline(&line, &len, file))!=-1){
        if(current_length>*maxlength){
            *maxlength=current_length;
        }
        (*linecounter)++;
    }
    fclose(file);
    free(line);
    
    if(*linecounter==0 || *maxlength==0){
        cout<<"File is empty: "<<file_name<<endl;
        return -1;
    }
    return 0;
}