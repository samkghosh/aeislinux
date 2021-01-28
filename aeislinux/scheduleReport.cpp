/***************************************************************************************
* File Name: ScheduleReport.cpp
*
* Purpose: Schedule the report as per tbl_schedule_report
*
* History:
*	[00]	[02-01-2013]	[SG]	Created
*
***************************************************************************************/




/*************************************************************************************
*
*
*************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"startSchedule"
void startSchedule(void *p)
{

	/* check the tbl_cron to check if any schedule is pending */
	/* tbl_cron will have the entry per each job and time */
	/* d_nextSchedDate > current time, then execute the job, 
	*  if it is done then delete and enter the next day's entry or next months entry
	*  if (no entry is there, then check for current entry from the master
	*
	*/

	





}
