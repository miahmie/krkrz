#include <windows.h>
#include "tp_stub.h"
#include "MenuItemIntf.h"
#include "resource.h"
#include <tchar.h>
#include <string.h>

const tjs_char* TVPSpecifyWindow = NULL;
const tjs_char* TVPSpecifyMenuItem = NULL;
const tjs_char* TVPInternalError = NULL;
const tjs_char* TVPNotChildMenuItem = NULL;
const tjs_char* TVPMenuIDOverflow = NULL;

static void LoadMessageFromResource() {
	static const int BUFF_SIZE = 1024;
	HINSTANCE hInstance = ::GetModuleHandle(_T("menu.dll"));
	TCHAR buffer[BUFF_SIZE];
	TCHAR* work;
	int len;

	len = ::LoadString( hInstance, IDS_SPECIFY_WINDOW, buffer, BUFF_SIZE );
	work = new TCHAR[len+1];
	_tcscpy_s( work, len+1, buffer );
	TVPSpecifyWindow = work;

	len = ::LoadString( hInstance, IDS_SPECIFY_MENU_ITEM, buffer, BUFF_SIZE );
	work = new TCHAR[len+1];
	_tcscpy_s( work, len+1, buffer );
	TVPSpecifyMenuItem = work;

	len = ::LoadString( hInstance, IDS_INTERNAL_ERROR, buffer, BUFF_SIZE );
	work = new TCHAR[len+1];
	_tcscpy_s( work, len+1, buffer );
	TVPInternalError = work;

	len = ::LoadString( hInstance, IDS_NOT_CHILD_MENU_ITEM, buffer, BUFF_SIZE );
	work = new TCHAR[len+1];
	_tcscpy_s( work, len+1, buffer );
	TVPNotChildMenuItem = work;
	
	len = ::LoadString( hInstance, IDS_MENU_ID_OVERFLOW, buffer, BUFF_SIZE );
	work = new TCHAR[len+1];
	_tcscpy_s( work, len+1, buffer );
	TVPMenuIDOverflow = work;
}
static void FreeMessage() {
	delete[] TVPSpecifyWindow;
	delete[] TVPSpecifyMenuItem;
	delete[] TVPInternalError;
	delete[] TVPNotChildMenuItem;
	delete[] TVPMenuIDOverflow;
	TVPSpecifyWindow = NULL;
	TVPSpecifyMenuItem = NULL;
	TVPInternalError = NULL;
	TVPNotChildMenuItem = NULL;
	TVPMenuIDOverflow = NULL;
}
static std::map<HWND,iTJSDispatch2*> MENU_LIST;
static void AddMenuDispatch( HWND hWnd, iTJSDispatch2* menu ) {
	MENU_LIST.insert( std::map<HWND, iTJSDispatch2*>::value_type( hWnd, menu ) );
}
static iTJSDispatch2* GetMenuDispatch( HWND hWnd ) {
	std::map<HWND, iTJSDispatch2*>::iterator i = MENU_LIST.find( hWnd );
	if( i != MENU_LIST.end() ) {
		return i->second;
	}
	return NULL;
}
static void DelMenuDispatch( HWND hWnd ) {
	MENU_LIST.erase(hWnd);
}
/**
 * ���j���[�̒�������ɑ��݂��Ȃ��Ȃ���Window�ɂ��Ă��郁�j���[�I�u�W�F�N�g���폜����
 */
static void UpdateMenuList() {
	std::map<HWND, iTJSDispatch2*>::iterator i = MENU_LIST.begin();
	for( ; i != MENU_LIST.end(); ) {
		HWND hWnd = i->first;
		BOOL exist = ::IsWindow( hWnd );
		if( exist == 0 ) {
			// ���ɂȂ��Ȃ���Window
			std::map<HWND, iTJSDispatch2*>::iterator target = i;
			i++;
			iTJSDispatch2* menu = target->second;
			MENU_LIST.erase( target );
			menu->Release();
			TVPDeleteAcceleratorKeyTable( hWnd );
		} else {
			i++;
		}
	}
}
class WindowMenuProperty : public tTJSDispatch {
	tjs_error TJS_INTF_METHOD PropGet( tjs_uint32 flag,	const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result,	iTJSDispatch2 *objthis ) {
		tTJSVariant var;
		if( TJS_FAILED(objthis->PropGet(0, TJS_W("HWND"), NULL, &var, objthis)) ) {
			return TJS_E_INVALIDOBJECT;
		}
		HWND hWnd = (HWND)(tjs_int64)var;
		iTJSDispatch2* menu = GetMenuDispatch( hWnd );
		if( menu == NULL ) {
			UpdateMenuList();
			menu = TVPCreateMenuItemObject(objthis);
			AddMenuDispatch( hWnd, menu );
		}
		*result = tTJSVariant(menu, menu);
		return TJS_S_OK;
	}
	tjs_error TJS_INTF_METHOD PropSet( tjs_uint32 flag, const tjs_char *membername,	tjs_uint32 *hint, const tTJSVariant *param,	iTJSDispatch2 *objthis ) {
		return TJS_E_ACCESSDENYED;
	}
} *gWindowMenuProperty;

/**
 * �L�[�R�[�h�����񎫏��^�z�񐶐�
 */
iTJSDispatch2* textToKeycodeMap = NULL;
iTJSDispatch2* keycodeToTextList = NULL;
static void ReleaseShortCutKeyCodeTable() {
	if( textToKeycodeMap ) textToKeycodeMap->Release();
	if( keycodeToTextList ) keycodeToTextList->Release();
	textToKeycodeMap = NULL;
	keycodeToTextList = NULL;
}
bool SetShortCutKeyCode(ttstr text, int key, bool force) {
	tTJSVariant vtext(text);
	tTJSVariant vkey(key);

	text.ToLowerCase();
	if( TJS_FAILED(textToKeycodeMap->PropSet(TJS_MEMBERENSURE, text.c_str(), NULL, &vkey, textToKeycodeMap)) )
		return false;
	if( force == false ) {
		tTJSVariant var;
		keycodeToTextList->PropGetByNum(0, key, &var, keycodeToTextList);
		if( var.Type() == tvtString ) return true;
	}
	return TJS_SUCCEEDED(keycodeToTextList->PropSetByNum(TJS_MEMBERENSURE, key, &vtext, keycodeToTextList));
}
static void CreateShortCutKeyCodeTable() {
	textToKeycodeMap = TJSCreateDictionaryObject();
	keycodeToTextList = TJSCreateArrayObject();
	if( textToKeycodeMap == NULL || keycodeToTextList == NULL ) return;

	TCHAR tempKeyText[32];
	for( int key = 8; key <= 255; key++ ) {
		int code = (::MapVirtualKey( key, 0 )<<16)|(1<<25);
		if( ::GetKeyNameText( code, tempKeyText, 32 ) > 0 ) {
			ttstr text(tempKeyText);
			// NumPad�L�[���ꏈ��
			if( TJS_strnicmp(text.c_str(), TJS_W("Num "), 4) == 0 ) {
				bool numpad = ( key >= VK_NUMPAD0 && key <= VK_DIVIDE );
				if( !numpad && ::GetKeyNameText( code|(1<<24), tempKeyText, 32 ) > 0 ) {
					text = tempKeyText;
				}
			}
			SetShortCutKeyCode(text, key, true);
		}
	}

	// �g���g���Q�݊��p�V���[�g�J�b�g������
	SetShortCutKeyCode(TJS_W("BkSp"), VK_BACK, false);
	SetShortCutKeyCode(TJS_W("PgUp"), VK_PRIOR, false);
	SetShortCutKeyCode(TJS_W("PgDn"), VK_NEXT, false);
}


//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved) {
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" __declspec(dllexport) HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	LoadMessageFromResource();

	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	CreateShortCutKeyCodeTable();

	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	{
		gWindowMenuProperty = new WindowMenuProperty();
		val = tTJSVariant(gWindowMenuProperty);
		gWindowMenuProperty->Release();
		tTJSVariant win;
		if( TJS_SUCCEEDED(global->PropGet(0,TJS_W("Window"),NULL,&win,global)) ) {
			iTJSDispatch2* obj = win.AsObjectNoAddRef();
			obj->PropSet(TJS_MEMBERENSURE,TJS_W("menu"),NULL,&val,obj);
			win.Clear();
		}
		val.Clear();

		//-----------------------------------------------------------------------
		iTJSDispatch2 * tjsclass = TVPCreateNativeClass_MenuItem();
		val = tTJSVariant(tjsclass);
		tjsclass->Release();
		global->PropSet( TJS_MEMBERENSURE, TJS_W("MenuItem"), NULL, &val, global );
		//-----------------------------------------------------------------------
		
	}

	// - global �� Release ����
	global->Release();

	// val ���N���A����B
	// ����͕K���s���B�������Ȃ��� val ���ێ����Ă���I�u�W�F�N�g
	// �� Release ���ꂸ�A���Ɏg�� TVPPluginGlobalRefCount �����m�ɂȂ�Ȃ��B
	val.Clear();


	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// �Ƃ��čT���Ă����BTVPPluginGlobalRefCount �͂��̃v���O�C������
	// �Ǘ�����Ă��� tTJSDispatch �h���I�u�W�F�N�g�̎Q�ƃJ�E���^�̑��v�ŁA
	// ������ɂ͂���Ɠ������A����������Ȃ��Ȃ��ĂȂ��ƂȂ�Ȃ��B
	// �����Ȃ��ĂȂ���΁A�ǂ����ʂ̂Ƃ���Ŋ֐��Ȃǂ��Q�Ƃ���Ă��āA
	// �v���O�C���͉���ł��Ȃ��ƌ������ƂɂȂ�B

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) HRESULT _stdcall V2Unlink()
{
	// �g���g��������A�v���O�C����������悤�Ƃ���Ƃ��ɌĂ΂��֐��B

	// �������炩�̏����Ńv���O�C��������ł��Ȃ��ꍇ��
	// ���̎��_�� E_FAIL ��Ԃ��悤�ɂ���B
	// �����ł́ATVPPluginGlobalRefCount �� GlobalRefCountAtInit ����
	// �傫���Ȃ��Ă���Ύ��s�Ƃ������Ƃɂ���B
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL ���A��ƁAPlugins.unlink ���\�b�h�͋U��Ԃ�

	/*
		�������A�N���X�̏ꍇ�A�����Ɂu�I�u�W�F�N�g���g�p���ł���v�Ƃ������Ƃ�
		�m�邷�ׂ�����܂���B��{�I�ɂ́APlugins.unlink �ɂ��v���O�C���̉����
		�댯�ł���ƍl���Ă������� (�������� Plugins.link �Ń����N������A�Ō��
		�Ńv���O�C������������A�v���O�����I���Ɠ����Ɏ����I�ɉ��������̂��g)�B
	*/

	// �v���p�e�B�J��
	// - �܂��ATJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// ���j���[�͉������Ȃ��͂��Ȃ̂ŁA�����I�ɂ͉�����Ȃ�

	// - global �� DeleteMember ���\�b�h��p���A�I�u�W�F�N�g���폜����
	if(global)
	{
		// TJS ���̂����ɉ������Ă����Ƃ��Ȃǂ�
		// global �� NULL �ɂȂ蓾��̂� global �� NULL �łȂ�
		// ���Ƃ��`�F�b�N����

		global->DeleteMember( 0, TJS_W("MenuItem"), NULL, global );
	}

	// - global �� Release ����
	if(global) global->Release();

	ReleaseShortCutKeyCodeTable();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	FreeMessage();
	return S_OK;
}
//---------------------------------------------------------------------------
