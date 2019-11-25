/**
  ******************************************************************************
  * @file    cx_sch.c
  * @author  CX
  * @version V1.0.0.1
  * @date    2016-7-26
  * @brief   1.0.0.1
	     �޸��������ݽṹ,����һ����ռ����
	     ����RunNumԤ����ѡ������Ƿ�������ִ�д���
	     1.0.0.0 
	     ��ɻ����ܹ��
  ******************************************************************************
  * @attention
  *
  * ��Ŀ   ��None
  * ����   : None
  * ʵ���� ��None
  ******************************************************************************
  */


#include "cx_sch.h"



sTask_Typedef SCH_tasks_G[SCH_MAX_TASKS];



/**
  * @brief   ��������
  * @param   pFunction ����ָ��, Delay �ӳ�ʱ��, Peroid ����ִ������, RunNum ����ִ�д���, ModeEnum ����ģʽ
  * @retval  �����������
  * @notice  None
*/
uint8_t SCH_Add_Task(void(*pFunction)(), uint32_t Delay, uint32_t Peroid, uint8_t RunNum, TaskMode_Enum ModeEnum)
{
	uint8_t Index = 0;
	while((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))	
	{
		Index++;                                                             
	}
	if(Index == SCH_MAX_TASKS)
	{
		return SCH_MAX_TASKS;
	}
	SCH_tasks_G[Index].pTask = pFunction;
	SCH_tasks_G[Index].Delay = Delay;
	SCH_tasks_G[Index].Peroid = Peroid;
	SCH_tasks_G[Index].RunMe = 0;
#if RunNum_ON
	SCH_tasks_G[Index].RunNum = RunNum;
#endif
	SCH_tasks_G[Index].ModeEnum = ModeEnum;
	return Index;
}



/**
  * @brief   ɾ������
  * @param   Index, �����������
  * @retval  None
  * @notice  None
*/
void SCH_Delete_Tasks(uint8_t Index)
{
	SCH_tasks_G[Index].pTask = 0;
	SCH_tasks_G[Index].Delay = 0;
	SCH_tasks_G[Index].Peroid = 0;
	SCH_tasks_G[Index].RunMe = 0;
#if RunNum_ON
	SCH_tasks_G[Index].RunNum = 0;
#endif
}



/**
  * @brief   ��������
  * @param   None
  * @retval  None
  * @notice  ��Ҫ����֧��, ��ռ����ִ��ʱ������С�������������ʱ��
*/
void SCH_Update_Tasks(void)
{
	u8 Index;
	for(Index = 0;Index < SCH_MAX_TASKS;Index++)
	{
		if(SCH_tasks_G[Index].pTask)
		{
			if(SCH_tasks_G[Index].Delay == 0)   
			{
				switch(SCH_tasks_G[Index].ModeEnum)
				{
					case SEIZ_Enum:                         //��ռ������������
						SCH_tasks_G[Index].pTask();
						if(SCH_tasks_G[Index].RunMe > 0)
						{
							SCH_tasks_G[Index].RunMe--;
#if RunNum_ON
							if(SCH_tasks_G[Index].RunNum > 0)
							{
								SCH_tasks_G[Index].RunNum--;
								if(SCH_tasks_G[Index].RunNum == 0)
								{
									SCH_Delete_Tasks(Index);
								}	
							}
#endif
						}
						break;
					case COOP_Enum:
						SCH_tasks_G[Index].RunMe++;     
						break;
					default:break;
				}				
				if(SCH_tasks_G[Index].Peroid)
				{
					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Peroid;
				}
			}
			else
			{
				SCH_tasks_G[Index].Delay--;     
			}
		}
	}
}



/**
  * @brief   ���������
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Dispatch_Tasks(void)
{
	u8 Index;
	while(1)
	{
		for(Index = 0;Index < SCH_MAX_TASKS;Index++)
		{
			if(SCH_tasks_G[Index].RunMe > 0)
			{
				SCH_tasks_G[Index].pTask();
				SCH_tasks_G[Index].RunMe--;
#if RunNum_ON
				if(SCH_tasks_G[Index].RunNum > 0)
				{
					SCH_tasks_G[Index].RunNum--;
					if(SCH_tasks_G[Index].RunNum == 0)
					{
						SCH_Delete_Tasks(Index);
					}	
				}
#endif
				if(SCH_tasks_G[Index].Peroid == 0)
				{
					SCH_Delete_Tasks(Index);
				}
			}
		}
	}
}



/**
  * @brief   �����ʼ��
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Init(void)
{
	TIM2Base_Config(10);
}


/**
  * @brief   �����������
  * @param   None
  * @retval  None
  * @notice  None
*/
void SCH_Start(void)
{
	TIM_Cmd(TIM2, ENABLE);	
}
