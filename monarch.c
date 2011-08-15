#include "monarch.h"

struct egg *mBreakEgg(const char *fileName, struct egg *current) {

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
  
  return current;

}

struct egg *mParseEggHeader(struct egg *current) {

  mxml_node_t *tree;
  tree = mxmlLoadString(NULL,current->header,MXML_TEXT_CALLBACK);
  
  mxml_node_t *format;
  format = mxmlFindElement(tree,tree,"data_format",NULL,NULL,MXML_DESCEND);
  
  const char *value;
  value = mxmlElementGetAttr(format, "id");
  current->data = malloc(sizeof(struct event));
  current->data->frameID_size = atoi(value);

  value = mxmlElementGetAttr(format, "ts");
  current->data->timestamp_size = atoi(value);
 
  value = mxmlElementGetAttr(format, "data");
  current->data->data_size = atoi(value);

  current->data->event_size = current->data->frameID_size 
                             + current->data->timestamp_size 
                             + current->data->data_size;

  current->data->ts = malloc(current->data->timestamp_size);
  current->data->fID = malloc(current->data->frameID_size);
  current->data->record = malloc(current->data->data_size);

  return current;

}

int mHatchNextEvent(struct egg *current) {

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

  bytes_read  = fread(current->data->record,sizeof(char),current->data->data_size,
		      current->eggptr);

  if(bytes_read == 0)
    flag = 1;

  return flag;

 }

void mCleanUp(struct egg *current) {

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

  return;

}
