#include "monarch.h"

struct egg *mBreakEgg(const char *fileName, struct egg *current) {

  printf("Breaking egg..\n");

  current->eggptr = fopen(fileName,"r");
  current->prelude = malloc(prelude_size);

  int bytes_read;
  int header_size;
  bytes_read = fread(current->prelude,sizeof(char),prelude_size,
		 	 current->eggptr);
    
  if(bytes_read != prelude_size) {

    //fix me

  }

  bytes_read = sscanf(current->prelude,"%08x",&header_size);
  
  if(!1) {

    //fix me

  }
  
  current->header = malloc(header_size);
  bytes_read = fread(current->header,sizeof(char),header_size,current->eggptr);

  printf("Break successful.\n");

  return current;

}

struct egg *mParseEggHeader(struct egg *current) {

  printf("Parsing header...\n");

  mxml_node_t *tree;
  tree = mxmlLoadString(NULL,current->header,MXML_TEXT_CALLBACK);
  
  mxml_node_t *data_format;
  data_format = mxmlFindElement(tree,tree,"data_format",NULL,NULL,MXML_DESCEND);
  
  const char *value;
  value = mxmlElementGetAttr(data_format, "id");
  current->data = malloc(sizeof(struct event));
  current->data->frameID_size = atoi(value);

  value = mxmlElementGetAttr(data_format, "ts");
  current->data->timestamp_size = atoi(value);
 
  value = mxmlElementGetAttr(data_format, "data");
  current->data->record_size = atoi(value);

  current->data->event_size = current->data->frameID_size 
                             + current->data->timestamp_size 
                             + current->data->record_size;

  current->data->ts = malloc(current->data->timestamp_size);
  current->data->fID = malloc(current->data->frameID_size);
  current->data->record = malloc(current->data->record_size);

  mxml_node_t *digitizer;
  digitizer = mxmlFindElement(tree,tree,"digitizer",NULL,NULL,MXML_DESCEND);
  value = mxmlElementGetAttr(digitizer, "rate");
  current->data->sample_rate = atoi(value);

  mxml_node_t *run;
  run = mxmlFindElement(tree,tree,"run",NULL,NULL,MXML_DESCEND);
  value = mxmlElementGetAttr(run, "length");
  current->data->sample_length = atoi(value);

  printf("Parse successful.\n");

  return current;

}

int mHatchNextEvent(struct egg *current) {

  printf("Hatching egg...\n");

  int flag = 0;

  int bytes_read;
  bytes_read  = fread(current->data->ts,sizeof(char),current->data->timestamp_size,
		      current->eggptr);

  if(bytes_read == 0)
    flag = 1;

  bytes_read  = fread(current->data->fID,sizeof(char),current->data->frameID_size,
		      current->eggptr);

  if(bytes_read == 0)
    flag = 1;

  bytes_read  = fread(current->data->record,sizeof(char),current->data->record_size,
		      current->eggptr);

  if(bytes_read == 0)
    flag = 1;

  printf("Egg hatched.\n");

  return flag;

 }

void mCleanUp(struct egg *current) {

  printf("Cleaning up.\n");

  if(current->prelude)
    free(current->prelude);

  if(current->header)
    free(current->header);

  if(current->data->ts)
    free(current->data->ts);

  if(current->data->fID)
    free(current->data->fID);

  if(current->data->record)
    free(current->data->record);

  if(current->data)
    free(current->data);

  fclose(current->eggptr);

  printf("Clean up successful.\n");

  return;

}
