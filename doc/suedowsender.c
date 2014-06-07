
#include "GS.h"

int main ()
{
  int		choice_number;
  TCP		dest_list;
  GSHandle_t	handle;
  TCB		tcb;

  GSContextRequirments_t context_requirments;
  GSContextInformation_t context_information;

  /* get the available destinations from the local CM */

  GS_inquire_destinations (&dest_list);

  /* display the destinations to the user and let him/her pick one */

  user_interface_selector (dest_list, &choice_number);
  
  /* did the user choose to cancel ? */

  if (choice_number == -1) exit (0);

  /* get the context requirements of the selected service */

  GS_inquire_context (dest_line[choice_number], &context_requirments);

  user_interface_context_filler (&context_requirments, &context_infomation);

  /* set up the handle for the communication channel */

  handle = GS_initilize (dest_tcp[choice_number], context_information);

  loop
    {
      /* load a data entity */

      get_some_data (data_description, &data_structure);

      /* set up a TCB for the transfer */

      strcpy (tcb.id, "TCB");
      tcb.BlockNum++;
      tcb.Lastblock = False;
      tcb.data      = &data_structure;

      /* send the data */

      GS_send (handle, &tcb, &result);

      /* check the result */

      if (result.status != SUCCESS) exit (1);
    }
  
  GS_term (handle);

  exit (0);
}
