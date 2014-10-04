/**
 *
 *  ���Υ��`����Live2D�v�B���ץ���_�k��;���ޤ�
 *  ���ɤ˸ĉ䤷�Ƥ�����픤��ޤ���
 *
 *  (c) CYBERNOIDS Co.,Ltd. All rights reserved.
 */

#include <string>


//Live2D Application
#include "LAppModel.h"
#include "LAppDefine.h"

#include "FileManager.h"
#include "ModelSettingJson.h"
#include "util\UtSystem.h"

using namespace live2d;

//D3D�ǥХ���
extern LPDIRECT3DDEVICE9		g_pD3DDevice ;




LAppModel::LAppModel()
	:L2DBaseModel(),modelSetting(NULL),live2DModel(NULL)
{
	eyeX=0;eyeY=0;
	bodyX=0;
	faceX=0;faceY=0;faceZ=0;
	num=0;
	isSpeaking=false;
	if (LAppDefine::DEBUG_LOG)
	{
		mainMotionMgr->setMotionDebugMode(true);
	}
}


LAppModel::~LAppModel(void)
{
	if(LAppDefine::DEBUG_LOG)UtDebug::print("delete model\n");
	delete modelSetting;
	delete live2DModel;

	for(unsigned int i=0;i<textures.size();i++)
	{
		textures[i]->Release() ;
	}
	textures.clear();
}


void LAppModel::load(const char* path)
{
	ModelPath=new char[strlen(path)+1];
	strcpy_s(ModelPath,MAX_PATH,path);
	if(LAppDefine::DEBUG_LOG) UtDebug::print( "load model : %s\n",path);	
    updating=true;
    initialized=false;
    
	int size ;

	char* data = FileManager::loadFile( path , &size ) ;
    modelSetting = new ModelSettingJson( data , size );

	FileManager::releaseBuffer(data);
	
	//JSON����äƤ���ե������modelHomeDir�˥��å�
	FileManager::getParentDir( path , &modelHomeDir ) ;

    if(LAppDefine::DEBUG_LOG) UtDebug::print( "create model : %s\n",modelSetting->getModelName());	
    updating=true;
    initialized=false;

   //��ǥ�Υ�`��
    if( strcmp( modelSetting->getModelFile() , "" ) != 0 )
    {        
        LDString modelFile=modelSetting->getModelFile();
        
		LDVector<LDString> texFiles;
		int len=modelSetting->getTextureNum();
		for (int i=0; i<len; i++)
		{
			texFiles.push_back(modelSetting->getTextureFile(i));
		}
		loadModelData(modelFile.c_str(),texFiles);
    }
	
	if (live2DModel==NULL) {

		return;
	}

    //����
	if (modelSetting->getExpressionNum() > 0)
	{
		LDVector<LDString> names;
		LDVector<LDString> files;
		int len=modelSetting->getExpressionNum();
		for (int i=0; i<len; i++)
		{
			names.push_back(modelSetting->getExpressionName(i));
			files.push_back(modelSetting->getExpressionFile(i));
		}
        loadExpressions(names,files);
	}
	
	//��������
	if( strcmp( modelSetting->getPhysicsFile(), "" ) != 0 )
    {        
        LDString path=modelSetting->getPhysicsFile();
        loadPhysics(path.c_str());
    }
	
	//�ݩ`��
	if( strcmp( modelSetting->getPoseFile() , "" ) != 0 )
    {
        LDString path=modelSetting->getPoseFile();
        loadPose(path.c_str());
    }
	//Ŀ�ѥ�
	if (eyeBlink==NULL)
	{
		eyeBlink=new L2DEyeBlink();
	}
	
	//�쥤������
	LDMap<LDString, float> layout;
	if (modelSetting->getLayout(layout) )
	{
		modelSetting->getLayout(layout);
		LDMap<LDString, float>::const_iterator ite;
		
		for (ite=layout.begin(); ite!=layout.end(); ite++) {
			LDString key=(*ite).first;
			float value=(*ite).second;
			if (key=="width")modelMatrix->setWidth(value);
			else if (key=="height" )	modelMatrix->setHeight(value);
		}
		
		for (ite=layout.begin(); ite!=layout.end(); ite++) {
			LDString key=(*ite).first;
			float value=(*ite).second;
			if (key=="x" )modelMatrix->setX(value);
			else if (key=="y" )modelMatrix->setY(value);
			else if (key=="center_x" )modelMatrix->centerX(value);
			else if (key=="center_y" )modelMatrix->centerY(value);
			else if (key=="top")modelMatrix->top(value);
			else if (key=="bottom" )modelMatrix->bottom(value);
			else if (key=="left" )modelMatrix->left(value);
			else if (key=="right" )modelMatrix->right(value);
		}
	}
	
	for ( int i = 0; i < modelSetting->getInitParamNum(); i++)
	{
		live2DModel->setParamFloat(modelSetting->getInitParamID(i), modelSetting->getInitParamValue(i));
	}

	for ( int i = 0; i < modelSetting->getInitPartsVisibleNum(); i++)
	{
		live2DModel->setPartsOpacity(modelSetting->getInitPartsVisibleID(i), modelSetting->getInitPartsVisibleValue(i));
	}
	
	live2DModel->saveParam();

	preloadMotionGroup(MOTION_GROUP_IDLE);
	
	mainMotionMgr->stopAllMotions();
	
    updating=false;//����״�B������
    initialized=true;//���ڻ�����
}


void LAppModel::loadModelData( const char modelFile[],LDVector<LDString>& texFiles)
{
	//���Ǥ˥ǩ`�����Ф���Ϥ�����
	if (live2DModel!=NULL)
	{
		delete live2DModel;	//moc
		delete modelMatrix;	//��Q����

		for(unsigned int i=0;i<textures.size();i++)
		{
			textures[i]->Release() ;
		}
		textures.clear();
	}

	//�i���z���_ʼ
	LDString modelPath = modelHomeDir + modelFile ;
	if(LAppDefine::DEBUG_LOG)UtDebug::print("modelPath %s\n", modelPath.c_str());
	live2DModel = Live2DModelD3D::loadModel(modelPath);
	live2DModel->setDevice( g_pD3DDevice ) ;
	
	if (Live2D::getError()!=Live2D::L2D_NO_ERROR) {
		UtDebug::print("failed load\n");
		return;
	}
	
	if(LAppDefine::DEBUG_LOG)UtDebug::print("load model w:%.0f,h:%.0f\n",live2DModel->getCanvasWidth(),live2DModel->getCanvasHeight());
	
	int len=texFiles.size();
	for (int i=0; i<len; i++)
	{
		LPDIRECT3DTEXTURE9	texture ;
		textures.push_back(texture);

		LDString texPath = modelHomeDir + texFiles[i];

		FileManager::loadTexture( g_pD3DDevice , texPath.c_str(),&textures[i]) ;

		live2DModel->setTexture( i , textures[i] ) ;//�ƥ�������ȥ�ǥ��Y�ӤĤ���
	}
	
	//��ǥ��Q���Фγ����O��
	modelMatrix=new L2DModelMatrix(live2DModel->getCanvasWidth(),live2DModel->getCanvasHeight());
	modelMatrix->setWidth(2);
	modelMatrix->setCenterPosition(0, 0);
}


void LAppModel::loadPhysics(const char fileName[])
{
	//���Ǥ˥ǩ`�����Ф���Ϥ�����
	if (physics!=NULL)
	{
		delete physics;
	}
	
	LDString path = modelHomeDir + fileName ;

	//�i���z���_ʼ
	int size;
	void* data=FileManager::loadFile(path.c_str() ,&size);
	physics= L2DPhysics::load(data,size) ;
	FileManager::releaseBuffer(data);
}


void LAppModel::loadExpressions(LDVector<LDString>& names,LDVector<LDString>& files)
{
	//���Ǥ˥ǩ`�����Ф���Ϥ�����
	releaseExpressions();
	
	expressionMgr=new L2DMotionManager();
	//�i���z���_ʼ
	int len = names.size();
    for (int i = 0; i < len; i++)
	{
		LDString& name = names[i];

		LDString path = modelHomeDir + files[i] ;

		int size;
		void* data=FileManager::loadFile( path.c_str(),&size);
        
        L2DExpressionMotion* motion = L2DExpressionMotion::loadJson(data,size);
       
        expressions[name]= motion ;

		FileManager::releaseBuffer(data);
    }
}


void LAppModel::loadPose(const char fileName[])
{
	//���Ǥ˥ǩ`�����Ф���Ϥ�����
	if (pose!=NULL)
	{
		delete pose;
	}

	LDString path = modelHomeDir + fileName ;

	//�i���z���_ʼ
	int size;
	void* data=FileManager::loadFile(path.c_str() ,&size);

	pose= L2DPose::load(data,size) ;

	FileManager::releaseBuffer(data);
}


void LAppModel::preloadMotionGroup(const char name[])
{
    int len = modelSetting->getMotionNum( name );
    for (int i = 0; i < len; i++)
	{
		LDString motionName= modelSetting->getMotionFile(name,i);
		LDString path = modelHomeDir + motionName ;

		if(LAppDefine::DEBUG_LOG)UtDebug::print("load motion name:%s \n",path.c_str() );
        
		AMotion* motion = Live2DMotion::loadMotion(path.c_str());
        motion->setFadeIn(  modelSetting->getMotionFadeIn(name,i)  );
        motion->setFadeOut( modelSetting->getMotionFadeOut(name,i) );
        motions[motionName]= motion ;
    }
}


void LAppModel::update()
{
	//-----------------------------------------------------------------
	live2DModel->loadParam();//ǰ�إ��`�֤��줿״�B���`��
	if(mainMotionMgr->isFinished())
	{
		//��`�������������ʤ����ϡ����C��`�������Ф�����������������
		startRandomMotion(MOTION_GROUP_IDLE, PRIORITY_IDLE);
	}
	else
	{
		bool update = mainMotionMgr->updateParam(live2DModel);//��`���������
		
		if( ! update){
			//�ᥤ���`�����θ��¤��ʤ��Ȥ�
			eyeBlink->setParam(live2DModel);//Ŀ�ѥ�
		}
	}

	live2DModel->saveParam();//״�B�򱣴�
	//-----------------------------------------------------------------
	
	
		if(expressionMgr!=NULL)expressionMgr->updateParam(live2DModel);// ����ǥѥ��`�����£������仯��
	
	live2DModel->addToParamFloat( PARAM_ANGLE_X, dragX *  30 , 1 );
	live2DModel->addToParamFloat( PARAM_ANGLE_Y, dragY *  30 , 1 );
	live2DModel->addToParamFloat( PARAM_ANGLE_Z, (dragX*dragY) * -30 , 1 );	
	//�ɥ�å��ˤ��仯
	//�ɥ�å��ˤ����򤭤��{��
	//live2DModel->addToParamFloat( PARAM_ANGLE_X, faceX *  30 , 1 );//-30����30�΂���Ӥ���
	//live2DModel->addToParamFloat( PARAM_ANGLE_Y, faceY *  30 , 1 );
	//live2DModel->addToParamFloat( PARAM_ANGLE_Z, (faceX*faceY) * -30 , 1 );
	//live2DModel->addToParamFloat( PARAM_ANGLE_Z, faceZ* 30 , 1 );
	//�ɥ�å��ˤ������򤭤��{��
	live2DModel->addToParamFloat( PARAM_BODY_X    , bodyX * 10 , 1 );//-1����1�΂���Ӥ���
	
	//�ɥ�å��ˤ��Ŀ���򤭤��{��
	//live2DModel->addToParamFloat( PARAM_EYE_BALL_X, eyeX  , 1 );//-1����1�΂���Ӥ���
	//live2DModel->addToParamFloat( PARAM_EYE_BALL_Y, eyeY  , 1 );
	live2DModel->addToParamFloat( PARAM_EYE_BALL_X, dragX  , 1 );
	live2DModel->addToParamFloat( PARAM_EYE_BALL_Y, dragY  , 1 );

	//�����ʤ�
	LDint64	 timeMSec = UtSystem::getUserTimeMSec() - startTimeMSec  ;
	double t = (timeMSec / 1000.0) * 2 * 3.14159  ;//2��t
	
	live2DModel->addToParamFloat( PARAM_ANGLE_X,	(float) (15 * sin( t/ 6.5345 )) , 0.5f);//-15 ~ +15 �ޤ����ڵĤ˼��㡣���ڤ����Ȥ��餹��
	live2DModel->addToParamFloat( PARAM_ANGLE_Y,	(float) ( 8 * sin( t/ 3.5345 )) , 0.5f);
	live2DModel->addToParamFloat( PARAM_ANGLE_Z,	(float) (10 * sin( t/ 5.5345 )) , 0.5f);
	live2DModel->addToParamFloat( PARAM_BODY_X,	(float) ( 4 * sin( t/15.5345 )) , 0.5f);
	live2DModel->setParamFloat  ( PARAM_BREATH,	(float) (0.5f + 0.5f * sin( t/3.2345 )),1);//0~1 �ޤ����ڵĤ��O������`�������ϕ�����
	//live2DModel->setParamFloat(PARAM_MOUTH_OPEN_Y,mouthY,1);

	//����ָ������
	for(int i=0;i<10;i++)
	{
		live2DModel->setParamFloat(paraname[i],paraval[i],paraweight[i]);
	}

	if(physics!=NULL)physics->updateParam(live2DModel);//��������ǥѥ��`������

	//��åץ��󥯤��O��
	if(isSpeaking)
	{
		//srand(unsigned( UtSystem::getUserTimeMSec()));
		float value = 0;//�ꥢ�륿����ǥ�åץ��󥯤��Ф����ϡ������ƥफ��������ȡ�ä���0��1�ι�����������Ƥ���������
		live2DModel->setParamFloat(PARAM_MOUTH_OPEN_Y, mouthY,0.8f);
	}
	
	//�ݩ`�����O��
	if(pose!=NULL)pose->updateParam(live2DModel);

	live2DModel->update();
}


int LAppModel::startMotion(const char name[],int no,int priority)
{
	if (! mainMotionMgr->reserveMotion(priority))
	{
		if(LAppDefine::DEBUG_LOG)UtDebug::print("can't start motion.\n");
		return -1;
	}
	LDString motionFile = modelSetting->getMotionFile(name, no);
	AMotion* motion = motions[motionFile];
	bool autoDelete = false;
	if ( motion == NULL )
	{
		//�i���z��
		LDString path = modelHomeDir + motionFile ;
		motion = Live2DMotion::loadMotion(path.c_str());
		
		if(strlen(motionFile.c_str())>0)
		{
			motion->setFadeIn(  modelSetting->getMotionFadeIn(name,no)  );
			motion->setFadeOut( modelSetting->getMotionFadeOut(name,no) );
		}
		
		autoDelete = true;//�K�˕r�˥��꤫������
	}
	
    if(LAppDefine::DEBUG_LOG)UtDebug::print("start motion ( %s : %d )\n",name,no);

	return mainMotionMgr->startMotionPrio(motion,autoDelete,priority);
}


int LAppModel::startRandomMotion(const char name[],int priority)
{
	if(modelSetting->getMotionNum(name)==0)return -1;
    int no = rand() % modelSetting->getMotionNum(name); 
    
    return startMotion(name,no,priority);
}


/**
 * ��ǥ���軭���롣
 * �ץ�åȥե��`�ऴ�Ȥι����O�����Ф���
 * ��ǥ뤬�O������Ƥʤ����ϤϺΤ⤷�ʤ���
 * @param gl
 */
void LAppModel::draw()
{
    if (live2DModel == NULL)return;

	// ���ˉ�Q�˱�
	D3DXMATRIXA16 buf ;
	g_pD3DDevice->GetTransform(D3DTS_WORLD, &buf);//World���ˤ�ȡ��

	// ��ǥ�Ή�Q���m��
	float* tr = modelMatrix->getArray() ;//float[16]
	g_pD3DDevice->MultiplyTransform( D3DTS_WORLD , (D3DXMATRIXA16*)tr ) ;

	// Live2D���軭
	live2DModel->draw();

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &buf);//��Q���Ԫ
}


/**
 * �������ж��Ȥκ��ץƥ��ȡ�
 * ָ��ID��픵�ꥹ�Ȥ��餽���򺬤����ξ��Τ�Ӌ�㤷���㤬�����˺��ޤ�뤫�ж�
 *
 * @param id
 * @param testX
 * @param testY
 * @return
 */
bool LAppModel::hitTest(const char pid[],float testX,float testY)
{
	if(alpha<1)return false;//͸���r�ϵ������ж��ʤ���
	int len=modelSetting->getHitAreasNum();
	for (int i = 0; i < len; i++)
	{
		if( strcmp( modelSetting->getHitAreaName(i) ,pid) == 0 )
		{
			const char* drawID=modelSetting->getHitAreaID(i);
			int drawIndex=live2DModel->getDrawDataIndex(drawID);
			if(drawIndex<0)return false;//���ڤ��ʤ����Ϥ�false
			int count=0;
			float* points=live2DModel->getTransformedPoints(drawIndex,&count);
			
			float left=live2DModel->getCanvasWidth();
			float right=0;
			float top=live2DModel->getCanvasHeight();
			float bottom=0;
			
			for (int j = 0; j < count; j++)
			{
				float x = points[DEF::VERTEX_OFFSET+j*DEF::VERTEX_STEP];
				float y = points[DEF::VERTEX_OFFSET+j*DEF::VERTEX_STEP+1];
				if(x<left)left=x;	// ��С��x
				if(x>right)right=x;	// ����x
				if(y<top)top=y;		// ��С��y
				if(y>bottom)bottom=y;// ����y
			}
			float tx=modelMatrix->invertTransformX(testX);
			float ty=modelMatrix->invertTransformY(testY);
			
			return ( left <= tx && tx <= right && top <= ty && ty <= bottom ) ;
		}
	}
	return false;//���ڤ��ʤ����Ϥ�false
}

//��������
  void LAppModel::setExpression(const char expressionID[])
{
	AMotion* motion = expressions[expressionID] ;
	if(LAppDefine::DEBUG_LOG)UtDebug::print( "expression[%s]\n" , expressionID ) ;
	if( motion != NULL )
	{
		expressionMgr->startMotion(motion, false) ;
	}
	else
	{
		if(LAppDefine::DEBUG_LOG)UtDebug::print( "expression[%s] is null \n" , expressionID ) ;
	}
}


void LAppModel::setRandomExpression()
{
	int no=rand()%expressions.size();
	LDMap<LDString,AMotion* >::const_iterator map_ite;
	int i=0;
	for(map_ite=expressions.begin();map_ite!=expressions.end();map_ite++)
	{
		if (i==no)
		{
			LDString name=(*map_ite).first;
			setExpression(name.c_str());
			return;
		}
		i++;
	}
}


void LAppModel::deviceLost() {
	live2DModel->deviceLostD3D() ;
}
