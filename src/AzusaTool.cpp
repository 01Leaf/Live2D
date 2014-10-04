#include "main.h"

extern int		AzusaPid;


void AzusaOutputCmdHelp()
{
	cout<<"RegisterAs(Output)"<<endl;
	cout<<"LinkRID(UI_Live2DAbort,false)"<<endl;
	cout<<"LinkRID(UI_AddModel,false)"<<endl;
	cout<<"LinkRID(UI_RemoveModels,false)"<<endl;
	cout<<"LinkRID(UI_GetModelPath,false)"<<endl;
	cout<<"LinkRID(UI_SetExpression,false)"<<endl;
	cout<<"LinkRID(UI_SetMotion,false)"<<endl;
	cout<<"LinkRID(UI_SetParameter,false)"<<endl;
	cout<<"LinkRID(UI_ClearParameter,false)"<<endl;
	cout<<"LinkRID(UI_SetMouthOpen,false)"<<endl;
	cout<<"LinkRID(UI_PlaySound,false)"<<endl;
	cout<<"LinkRID(UI_StopSound,false)"<<endl;
	cout<<"LinkRID(UI_SetEyeBalls,false)"<<endl;
	cout<<"LinkRID(UI_ShowMessage,false)"<<endl;
	cout<<"LinkRID(UI_SetBody,false)"<<endl;
	cout<<"LinkRID(UI_SetFace,false)"<<endl;
	cout<<"LinkRID(UI_LookAt,false)"<<endl;
	cout<<"LinkRID(UI_SetTrack,true)"<<endl;
	cout<<"GetAzusaPid()"<<endl;
	cout<<"��������μ�readme.txt\n����������"<<endl;
}



bool CheckAzusa()
{
	HANDLE myhProcess;
	PROCESSENTRY32 mype;
	BOOL mybRet;
	mype.dwSize=sizeof(mype);
	//���н��̿���
	myhProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); //TH32CS_SNAPPROCESS�������н���
	//��ʼ���̲���
	mybRet=Process32First(myhProcess,&mype);
	//ѭ���Ƚϣ��ó�ProcessID
	while(mybRet)
	{
		if(mype.th32ProcessID==AzusaPid)
			return true;
		else
			mybRet=Process32Next(myhProcess,&mype);
	}
	return false;
}


//��ȡ����--�������ʱʹ��
//strΪ�����ַ�����paraΪ���ܲ������ַ�����indexΪ�ڼ���������1��ʼ
void ReadParameter(char* arg,char* para,int index)
{
	int i,count=index,begin=0;

	for(i=0;count>0||i<strlen(arg);i++)
	{
		if(arg[i]==','||arg[i]=='\0')
		{
			count--;
			if(count>0)
				begin=i+1;
			else
				break;
		}
	}

	strncpy(para,arg+begin,i-begin);
	para[i-begin]='\0';
}

